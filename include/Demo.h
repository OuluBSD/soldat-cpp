#ifndef DEMO_H
#define DEMO_H

//*******************************************************************************
//
//       Demo Unit for SOLDAT
//
//       Copyright (c) 2002 Michal Marcinkowski
//
//*******************************************************************************

#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Net.h"
#include "Console.h"
#include "Util.h"
#include "Cvar.h"
#include "Game.h"
#include "SysUtils.h"  // Assuming this contains DateTimeToUnix and other utilities

#ifdef SERVER_CODE
#include "Server.h"
#else
#include "Client.h"
#include "GameStrings.h"
#endif

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <cctype>

const char DEMO_MAGIC[6] = {'S', 'O', 'L', 'D', 'E', 'M'};

// Add utility functions if not already available
#ifndef EXTRACT_FILE_NAME_DEFINED
#define EXTRACT_FILE_NAME_DEFINED

namespace Util {
    inline std::string ExtractFileName(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos) {
            return path.substr(pos + 1);
        }
        return path;
    }
    
    inline void StringToArray(char* dest, const std::string& src) {
        size_t len = std::min(src.length(), static_cast<size_t>(160)); // Assuming 160 byte array limit
        std::copy(src.begin(), src.begin() + len, dest);
        dest[len] = '\0'; // Ensure null termination
    }
}
using Util::ExtractFileName;
using Util::StringToArray;

#endif

#pragma pack(push, 1)  // Pascal records are tightly packed
struct TDemoHeader {
    char Header[6];
    uint16_t Version;
    char MapName[161];  // 0-indexed array of 161 elements
    int32_t StartDate;
    int32_t TicksNum;

    TDemoHeader() {
        std::copy(DEMO_MAGIC, DEMO_MAGIC + 6, Header);
        Version = 0;
        std::fill(MapName, MapName + 161, 0);
        StartDate = 0;
        TicksNum = 0;
    }
};
#pragma pack(pop)

class TDemo {
protected:
    std::shared_ptr<std::vector<uint8_t>> FDemoFile;
    TDemoHeader FDemoHeader;
    bool FActive;
    std::string FName;
    uint8_t FOldCam;

public:
    TDemo() : FActive(false), FName(""), FOldCam(0) {
        FDemoFile = std::make_shared<std::vector<uint8_t>>();
        FDemoHeader = TDemoHeader();
    }

    bool GetActive() const { return FActive; }
    void SetActive(bool value) { FActive = value; }
    bool Active() const { return FActive; }  // Convenience method to match Pascal interface
    const std::string& GetName() const { return FName; }
    const TDemoHeader& GetHeader() const { return FDemoHeader; }
};

class TDemoRecorder : public TDemo {
private:
    int32_t FTicksNum;

public:
    TDemoRecorder() : FTicksNum(0) {}
    
    int CreateDemoPlayer() {
        if (Sprite[MAX_SPRITES] && Sprite[MAX_SPRITES]->Active) {
            MainConsole.Console(L"Failed to create Demo Recorder player. Demos can be recorded with up to 31 players", INFO_MESSAGE_COLOR);
            StopRecord();
            return -1;
        }

        TPlayer* player = new TPlayer();  // Assuming TPlayer has a constructor
        player->DemoPlayer = true;
        player->Name.resize(PLAYERNAME_CHARS);
        std::string nameStr = "Demo Recorder";
        if (nameStr.length() > PLAYERNAME_CHARS) nameStr.resize(PLAYERNAME_CHARS);
        std::copy(nameStr.begin(), nameStr.end(), player->Name.begin());
        player->Team = TEAM_SPECTATOR;
        player->ControlMethod = HUMAN;

#ifdef SERVER_CODE
        player->Peer = UINT32_MAX;  // Equivalent to High(LongWord)
#endif

        TVector2 a;
        a.x = MIN_SECTORZ * Map.SectorsDivision * 0.7f;
        a.y = MIN_SECTORZ * Map.SectorsDivision * 0.7f;

        // First, get a free sprite number
        uint8_t freeSpriteNum = GetNextFreeSpriteNum();
        if (freeSpriteNum == 0) {
            MainConsole.Console(L"Failed to create Demo Recorder player. No free sprite slots", INFO_MESSAGE_COLOR);
            StopRecord();
            return -1;
        }

        // Create the sprite with the demo player
        CreateSprite(freeSpriteNum, TEAM_SPECTATOR, a, player);
        
        // CreateSprite doesn't update the sprite array directly, so if needed, we assign player
        // Since CreateSprite should already handle assigning the player, we just verify
        int p = freeSpriteNum;
        if ((p > 0) && (p < MAX_SPRITES + 1)) {
#ifdef SERVER_CODE
            ServerSyncCvars(p, player->Peer, true);
            ServerSendPlayList(player->Peer);
            ServerVars(p);
            ServerSendNewPlayerInfo(p, JOIN_NORMAL);
            ServerThingMustSnapshotOnConnect(p);
            Sprite[p]->Player->DemoPlayer = true;
#else
            // Client-side functions would go here if needed
            Sprite[p]->Player->DemoPlayer = true;
#endif
            SpriteParts.Pos[p] = Vector2(0, 0);
            return p;
        }

        return -1;
    }

#ifndef SERVER_CODE
    void SaveCamera() {
        // TMsg_ClientSpriteSnapshot_Dead msg;  // Assuming this struct exists
        // msg.Header.ID = MsgID_ClientSpriteSnapshot_Dead;
        // msg.CameraFocus = CameraFollowSprite;
        // SaveRecord(msg, sizeof(msg));
    }

    void SavePosition() {
        // TMsg_ServerSpriteDelta_Movement movementMsg;  // Assuming this struct exists
        // movementMsg.Header.ID = MsgID_Delta_Movement;
        //
        // movementMsg.Num = MySprite;
        // movementMsg.Velocity = SpriteParts.Velocity[MySprite];
        // movementMsg.Pos = SpriteParts.Pos[MySprite];
        // movementMsg.ServerTick = MainTickCounter;
        //
        // EncodeKeys(Sprite[MySprite], movementMsg.Keys16);
        //
        // movementMsg.MouseAimX = Sprite[MySprite].Control.MouseAimX;
        // movementMsg.MouseAimY = Sprite[MySprite].Control.MouseAimY;
        //
        // SaveRecord(movementMsg, sizeof(movementMsg));
    }
#endif

    bool StartRecord(const std::string& Filename) {
        bool result = false;

#ifndef SERVER_CODE
        if (DemoPlayer && DemoPlayer->GetActive()) {
            return false;
        }
#endif

        FDemoFile = std::make_shared<std::vector<uint8_t>>();

        std::string filenameOnly = ExtractFileName(Filename);

        std::wstring wideStr(filenameOnly.begin(), filenameOnly.end());
        MainConsole.Console(wideStr.c_str(), INFO_MESSAGE_COLOR);

        FName = filenameOnly;
        FActive = true;
        FOldCam = 255;
        FTicksNum = 0;

        // Set up header
        std::copy(DEMO_MAGIC, DEMO_MAGIC + 6, FDemoHeader.Header);
        FDemoHeader.TicksNum = 0;
        FDemoHeader.StartDate = DateTimeToUnix(Now());  // Assuming these functions exist
        FDemoHeader.Version = DEMO_VERSION;

        std::fill(FDemoHeader.MapName, FDemoHeader.MapName + 161, 0);
        StringToArray(FDemoHeader.MapName, Map.Name);

        // Write header to file
        // FDemoFile->WriteBuffer(FDemoHeader, sizeof(FDemoHeader));  // Would need implementation

        int spriteID = CreateDemoPlayer();

        if (spriteID == MAX_SPRITES) {
            result = true;
        }

        return result;
    }

    void StopRecord() {
        if (!Active()) {
            return;
        }

        std::wstring wideStr(FName.begin(), FName.end());
        MainConsole.Console((L"Demo stopped (" + wideStr + L")").c_str(), INFO_MESSAGE_COLOR);

        KillSprite(MAX_SPRITES, 0, 0, 0);  // Provide all required arguments for KillSprite

        // Go back to beginning of file to update header
        // FDemoFile->Position = 0;  // Would need implementation

        FDemoHeader.Version = DEMO_VERSION;
        FDemoHeader.TicksNum = FTicksNum;

        std::fill(FDemoHeader.MapName, FDemoHeader.MapName + 161, 0);
        StringToArray(FDemoHeader.MapName, Map.Name);

        // FDemoFile->WriteBuffer(FDemoHeader, sizeof(FDemoHeader));  // Would need implementation

        try {
            // FDemoFile->SaveToFile(UserDirectory + "demos/" + FName);  // Would need implementation
        } catch (const std::exception& e) {
            std::string errMsg = "Failed to save demo file: ";
            errMsg += e.what();
            std::wstring wideErr(errMsg.begin(), errMsg.end());
            MainConsole.Console(wideErr, INFO_MESSAGE_COLOR);
        }

        FActive = false;
        FName = "";
        FDemoFile.reset();  // Equivalent to Free() in Pascal
    }

    void SaveRecord(const void* R, int Size) {
        if (Size == 0) {
            return;
        }

        if (!FActive) {
            return;
        }

        // FDemoFile->Write(Size, sizeof(RSize));  // Would need implementation
        // FDemoFile->Write(R, Size);  // Would need implementation
    }

    void SaveNextFrame() {
        if (!FActive) {
            return;
        }

        // Save record type
        RSize = 1;

        // FDemoFile->Write(RSize, sizeof(RSize));  // Would need implementation

        // Save camera change
#ifndef SERVER_CODE
        if (FOldCam != CameraFollowSprite) {
            SaveCamera();
            FOldCam = CameraFollowSprite;
        }
#endif

        FTicksNum++;
    }

    int GetTicksNum() const { return FTicksNum; }
};

#ifndef SERVER_CODE
class TDemoPlayer : public TDemo {
private:
    int32_t FSkipTo;

public:
    TDemoPlayer() : FSkipTo(0) {}

    bool OpenDemo(const std::string& Filename) {
        bool result = false;
        FDemoFile = std::make_shared<std::vector<uint8_t>>();

        std::ifstream file(Filename, std::ios::binary);
        if (!file.is_open()) {
            std::string errMsg = "Failed to load demo file: Could not open file";
            std::wstring wideErrMsg(errMsg.begin(), errMsg.end());
            MainConsole.Console(wideErrMsg, INFO_MESSAGE_COLOR);
            return false;
        }

        // Read header
        file.read(reinterpret_cast<char*>(&FDemoHeader), sizeof(FDemoHeader));

        if (std::string(FDemoHeader.Header, 6) != std::string(DEMO_MAGIC, 6)) {
            std::string errMsg = "The provided file is not valid: " + FName;
            std::wstring wideErrMsg(errMsg.begin(), errMsg.end());
            MainConsole.Console(wideErrMsg, INFO_MESSAGE_COLOR);
            return false;
        } else if (FDemoHeader.Version != DEMO_VERSION) {
            std::string errMsg = "Wrong demo version: " + std::to_string(DEMO_VERSION) + " - " +
                                std::to_string(FDemoHeader.Version);
            std::wstring wideErrMsg(errMsg.begin(), errMsg.end());
            MainConsole.Console(wideErrMsg, INFO_MESSAGE_COLOR);
            return false;
        } else {
            FName = ExtractFileName(Filename);
            std::wstring wideFName(FName.begin(), FName.end());
            MainConsole.Console((L"Playing demo " + wideFName).c_str(), INFO_MESSAGE_COLOR);
            Spectator = 1;  // Assuming this is a global variable
            FActive = true;
            result = true;
        }

        return result;
    }

    void StopDemo() {
        if (!FActive) {
            return;
        }

        MainConsole.Console(L"Demo stopped", INFO_MESSAGE_COLOR);

        FDemoFile.reset();  // Equivalent to Free()

        FActive = false;
    }

    void ProcessDemo() {
        char readBuf[16384];  // Array[0..16383] of Char

        while (FActive) {
            // Check if we've reached the end of the demo file
            // This would need actual file position checking implementation
            // if (FDemoFile->Position == FDemoFile->Size) {
            //     StopDemo();
            //     ExitToMenu();  // Assuming this function exists
            //     return;
            // }

            // Would need actual file reading implementation
            // try
            //     FDemoFile->Read(RSize, sizeof(RSize));
            // except
            //     return;
            // end;

            if ((FSkipTo > 0) && (MainTickCounter >= FSkipTo)) {
                FSkipTo = -1;
                ShouldRenderFrames = true;  // Assuming this is a global variable
                GOALTICKS = static_cast<int>(demo_speed.Value() * DEFAULT_GOALTICKS);  // Assuming demo_speed exists
            }

            if (RSize == 0) {
                // FDemoFile->Position := FDemoFile->Position + 2;  // Would need implementation
                return;
            }

            if (RSize == 1) {  // next frame
                return;
            }

            // Would need actual file reading implementation
            // try
            //     FDemoFile->Read(ReadBuf, RSize);
            // except
            //     return;
            // end;

            // For now, just simulating the network message handling:
            // packet := UDP.NetworkingUtil.AllocateMessage(RSize);
            // packet.m_pData := @ReadBuf;
            // UDP.HandleMessages(packet);
            // packet^.m_pData := nil;
            // packet.m_pfnRelease(packet);
        }
    }

    void Position(int Ticks) {
        FSkipTo = Ticks;
        ShouldRenderFrames = false;

        if (FSkipTo < MainTickCounter) {
            // FDemoFile->Seek(sizeof(FDemoHeader), std::ios::beg);  // Would need implementation
            // Assuming Seek from beginning of file

            MainTickCounter = 0;

            for (int i = 1; i <= MAX_SPRITES; i++) {
                if (Sprite[i] && Sprite[i]->Active) {
                    KillSprite(i, 0, 0, 0);  // Provide all required arguments for KillSprite
                }
            }
            for (int i = 1; i <= MAX_BULLETS; i++) {
                if (Bullet[i]) {
                    // Assuming there's a function to kill bullets, or use a different approach
                    // If Kill() method doesn't exist, we may need to reset instead
                    // ResetBullet(i);  // Assuming such function exists
                }
            }
            for (int i = 1; i <= MAX_SPARKS; i++) {
                if (Spark[i]) {
                    // Similar for sparks if needed
                }
            }
            for (int i = 1; i <= MAX_THINGS; i++) {
                if (Thing[i]) {
                    KillThing(i);  // Assuming there's a function to kill things
                }
            }

            // Reset World and Big Texts
            for (int i = 0; i <= MAX_BIG_MESSAGES; i++) {
                // Big Text
                BigText[i] = L"";
                BigDelay[i] = 0;
                BigScale[i] = 0;
                BigColor[i] = 0;
                BigPosX[i] = 0;
                BigPosY[i] = 0;
                BigX[i] = 0;
                // World Text
                WorldText[i] = L"";
                WorldDelay[i] = 0;
                WorldScale[i] = 0;
                WorldColor[i] = 0;
                WorldPosX[i] = 0;
                WorldPosY[i] = 0;
                WorldX[i] = 0;
            }

            // Reset ABOVE CHAT MESSAGE
            for (int i = 1; i <= MAX_SPRITES; i++) {
                ChatDelay[i] = 0;
                ChatMessage[i] = L"";
                ChatTeam[i] = false;
            }

            MainConsole.Count = 0;
            BigConsole.Count = 0;
        }

        GOALTICKS = DEFAULT_GOALTICKS * 20;
    }

    int GetSkipTo() const { return FSkipTo; }
};
#endif

// Global variables
extern std::unique_ptr<TDemoRecorder> DemoRecorder;
#ifndef SERVER_CODE
extern std::unique_ptr<TDemoPlayer> DemoPlayer;
#endif
extern uint16_t RSize;
extern uint8_t FreeCam;
extern uint8_t NoTexts;

// Initialize global objects
inline void DemoInit() {
    DemoRecorder = std::make_unique<TDemoRecorder>();
#ifndef SERVER_CODE
    DemoPlayer = std::make_unique<TDemoPlayer>();
#endif
}

#endif // DEMO_H