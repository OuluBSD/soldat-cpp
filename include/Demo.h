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
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "SysUtils.h"  // Assuming this contains DateTimeToUnix and other utilities

const char DEMO_MAGIC[6] = {'S', 'O', 'L', 'D', 'E', 'M'};

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
    const std::string& GetName() const { return FName; }
    const TDemoHeader& GetHeader() const { return FDemoHeader; }
};

class TDemoRecorder : public TDemo {
private:
    int32_t FTicksNum;
    
    int CreateDemoPlayer();
#ifndef SERVER_CODE
    void SaveCamera();
    void SavePosition();
#endif

public:
    TDemoRecorder() : FTicksNum(0) {}
    
    bool StartRecord(const std::string& Filename);
    void StopRecord();
    void SaveRecord(const void* R, int Size);
    void SaveNextFrame();
    
    int GetTicksNum() const { return FTicksNum; }
};

#ifndef SERVER_CODE
class TDemoPlayer : public TDemo {
private:
    int32_t FSkipTo;

public:
    TDemoPlayer() : FSkipTo(0) {}
    
    bool OpenDemo(const std::string& Filename);
    void StopDemo();
    void ProcessDemo();
    void Position(int Ticks);
    
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

namespace DemoImpl {
    inline int TDemoRecorder::CreateDemoPlayer() {
        if (Sprite[MAX_SPRITES].Active) {
            MainConsole.Console("Failed to create Demo Recorder player. Demos can be recorded with up to 31 players", INFO_MESSAGE_COLOR);
            StopRecord();
            return -1;
        }

        TPlayer* player = new TPlayer();  // Assuming TPlayer has a constructor
        player->DemoPlayer = true;
        player->Name = "Demo Recorder";
        player->Team = TEAM_SPECTATOR;
        player->ControlMethod = HUMAN;

#ifdef SERVER_CODE
        player->Peer = UINT32_MAX;  // Equivalent to High(LongWord)
#endif

        TVector2 a;
        a.x = MIN_SECTORZ * Map.SectorsDivision * 0.7f;
        a.y = MIN_SECTORZ * Map.SectorsDivision * 0.7f;

        int p = CreateSprite(a, Vector2(0, 0), 1, MAX_SPRITES, player, true);
        if ((p > 0) && (p < MAX_SPRITES + 1)) {
#ifdef SERVER_CODE
            ServerSyncCvars(p, player->Peer, true);
            ServerSendPlayList(player->Peer);
#else
            ServerSyncCvars(p, 0, true);
            ServerSendPlayList(0);
#endif
            ServerVars(p);
            ServerSendNewPlayerInfo(p, JOIN_NORMAL);
            ServerThingMustSnapshotOnConnect(p);
            Sprite[p].Player.DemoPlayer = true;
            SpriteParts.Pos[p] = Vector2(0, 0);
            return p;
        }

        return -1;
    }

#ifndef SERVER_CODE
    inline void TDemoRecorder::SaveCamera() {
        // TMsg_ClientSpriteSnapshot_Dead msg;  // Assuming this struct exists
        // msg.Header.ID = MsgID_ClientSpriteSnapshot_Dead;
        // msg.CameraFocus = CameraFollowSprite;
        // SaveRecord(msg, sizeof(msg));
    }

    inline void TDemoRecorder::SavePosition() {
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

    inline bool TDemoRecorder::StartRecord(const std::string& Filename) {
        bool result = false;

#ifndef SERVER_CODE
        if (DemoPlayer && DemoPlayer->GetActive()) {
            return false;
        }
#endif

        FDemoFile = std::make_shared<std::vector<uint8_t>>();

        std::string filenameOnly = ExtractFileName(Filename);
        
        std::wstring wideStr(filenameOnly.begin(), filenameOnly.end());
        MainConsole.Console(L"Recording demo: " + wideStr, INFO_MESSAGE_COLOR);

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

    inline void TDemoRecorder::StopRecord() {
        if (!Active) {
            return;
        }

        std::wstring wideStr(FName.begin(), FName.end());
        MainConsole.Console(L"Demo stopped (" + wideStr + L")", INFO_MESSAGE_COLOR);

        Sprite[MAX_SPRITES].Kill();

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

    inline void TDemoRecorder::SaveRecord(const void* R, int Size) {
        if (Size == 0) {
            return;
        }

        if (!FActive) {
            return;
        }

        // FDemoFile->Write(Size, sizeof(RSize));  // Would need implementation
        // FDemoFile->Write(R, Size);  // Would need implementation
    }

    inline void TDemoRecorder::SaveNextFrame() {
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

#ifndef SERVER_CODE
    inline bool TDemoPlayer::OpenDemo(const std::string& Filename) {
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
            MainConsole.Console(L"Playing demo " + wideFName, INFO_MESSAGE_COLOR);
            Spectator = 1;  // Assuming this is a global variable
            FActive = true;
            result = true;
        }

        return result;
    }

    inline void TDemoPlayer::StopDemo() {
        if (!FActive) {
            return;
        }

        MainConsole.Console(L"Demo stopped", INFO_MESSAGE_COLOR);

        FDemoFile.reset();  // Equivalent to Free()

        FActive = false;
    }

    inline void TDemoPlayer::ProcessDemo() {
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

    inline void TDemoPlayer::Position(int Ticks) {
        FSkipTo = Ticks;
        ShouldRenderFrames = false;

        if (FSkipTo < MainTickCounter) {
            // FDemoFile->Seek(sizeof(FDemoHeader), std::ios::beg);  // Would need implementation
            // Assuming Seek from beginning of file

            MainTickCounter = 0;

            for (int i = 1; i <= MAX_SPRITES; i++) {
                Sprite[i].Kill();
            }
            for (int i = 1; i <= MAX_BULLETS; i++) {
                Bullet[i].Kill();
            }
            for (int i = 1; i <= MAX_SPARKS; i++) {
                Spark[i].Kill();
            }
            for (int i = 1; i <= MAX_THINGS; i++) {
                Thing[i].Kill();
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
#endif
}

// Using declarations to bring into global namespace
using DemoImpl::TDemo;
using DemoImpl::TDemoRecorder;
#ifndef SERVER_CODE
using DemoImpl::TDemoPlayer;
#endif

// Global variables
extern std::unique_ptr<TDemoRecorder> DemoRecorder;
#ifndef SERVER_CODE
extern std::unique_ptr<TDemoPlayer> DemoPlayer;
#endif
extern uint16_t RSize;
extern uint8_t FreeCam;
extern uint8_t NoTexts = 0;

// Initialize global objects
namespace DemoImpl {
    inline void DemoInit() {
        DemoRecorder = std::make_unique<TDemoRecorder>();
#ifndef SERVER_CODE
        DemoPlayer = std::make_unique<TDemoPlayer>();
#endif
    }
}

#endif // DEMO_H