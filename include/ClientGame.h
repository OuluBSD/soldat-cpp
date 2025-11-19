#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

//*******************************************************************************
//                                                                              
//       Client Game Unit for SOLDAT                                             
//                                                                              
//       Copyright (c) 2012 Daniel Forssten          
//                                                                              
//*******************************************************************************

#include "Constants.h"
#include "Vector.h"
#include "Sprites.h"
#include "Net.h"
#include "UpdateFrame.h"
#include "Demo.h"
#include "Cvar.h"  // For cvar declarations
#include "Console.h"  // For MainConsole
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

// Forward declarations (these would be defined in other headers)
struct TFrameTiming;

// Forward declarations for types used but defined elsewhere
struct TWeaponStat;  // Defined in Client.h
struct TConsole;     // Defined in Console.h
struct TBooleanCvar; // Defined in Cvar.h
struct TIntCvar;     // Defined in Cvar.h
struct TFloatCvar;   // Defined in Cvar.h
struct TColorCvar;   // Defined in Cvar.h

// Function forward declarations needed
void ClientDisconnect();
void ClientSpriteSnapshot();
void ClientSpriteSnapshotMov();
void ClientSpriteSnapshotDead();
void ShowMapChangeScoreboard();
void ExitToMenu();
void GameMenuShow(void* Menu, bool Show); // Using void* as a placeholder






struct TFrameTiming {
    int64_t Frequency;
    int64_t StartTime;
    double PrevTime;
    double PrevRenderTime;
    double Accumulator;
    double MinDeltaTime;
    double Elapsed;
    int Counter;
    int Fps;
    double FpsAccum;

    TFrameTiming() : Frequency(0), StartTime(0), PrevTime(0), PrevRenderTime(0),
                     Accumulator(0), MinDeltaTime(0), Elapsed(0), Counter(0),
                     Fps(0), FpsAccum(0) {}
};

namespace ClientGameImpl {
    // FrameTiming variable declaration
    extern TFrameTiming FrameTiming;
    
    // Global variables
    inline TVector2 MousePrev = {0.0f, 0.0f};
    inline float mx = 0.0f;
    inline float my = 0.0f;
    inline bool MapChanged = false;
    inline bool ChatChanged = true;  // used for blinking chat input
    inline bool ShouldRenderFrames = true;  // false during game request phase

    // Demo recorder and player
    inline std::unique_ptr<TDemoRecorder> DemoRecorder = nullptr;
    inline std::unique_ptr<TDemoPlayer> DemoPlayer = nullptr;

    // used for action snap
    inline uint8_t ActionSnap = 1;
    inline bool ActionSnapTaken = false;
    inline int CapScreen = 255;
    inline bool ShowScreen = false;
    inline uint8_t ScreenCounter = 255;
    
    // Additional variables that were missing
    inline bool IsFullscreen = false;
    inline int ScreenWidth = 800;
    inline int ScreenHeight = 600;
    inline int RenderWidth = 800;
    inline int RenderHeight = 600;
    inline int WindowWidth = 800;
    inline int WindowHeight = 600;
    inline std::wstring ChatText = L"";
    inline std::wstring LastChatText = L"";
    inline std::wstring FireChatText = L"";
    inline uint8_t ChatType = 0;
    inline std::string CompletionBase = "";
    inline int CompletionBaseSeparator = 0;
    inline uint8_t CurrentTabCompletePlayer = 0;
    inline uint8_t CursorPosition = 0;  // For chat cursor position
    inline bool TabCompletePressed = false;  // For tab completion
    inline uint8_t ChatTimeCounter = 0;  // For chat blinking
    inline int ClientStopMovingCounter = 0;  // For connection issues
    inline bool ForceClientSpriteSnapshotMov = false;
    inline int LastForceClientSpriteSnapshotMovTick = 0;
    inline int MenuTimer = 0;
    
    inline double GetCurrentTime() {
        int64_t x = SDL_GetPerformanceCounter();
        return static_cast<double>(x - FrameTiming.StartTime) / FrameTiming.Frequency;
    }
    
    inline void ResetFrameTiming() {
        FrameTiming.Frequency = SDL_GetPerformanceFrequency();
        FrameTiming.StartTime = SDL_GetPerformanceCounter();

        FrameTiming.PrevTime = GetCurrentTime();
        FrameTiming.PrevRenderTime = FrameTiming.PrevTime;
        FrameTiming.Accumulator = 0;
        FrameTiming.MinDeltaTime = 0;
        FrameTiming.Elapsed = 0;

        FrameTiming.Counter = 0;
        FrameTiming.Fps = 0;
        FrameTiming.FpsAccum = 0;

        if (r_fpslimit.Value()) {
            FrameTiming.MinDeltaTime = 1.0 / r_maxfps.Value();
        }

        TickTime = 0;
        TickTimeLast = 0;
    }

    inline void BigMessage(const std::wstring& Text, int Delay, uint32_t Col) {
        // GfxTextPixelRatio(Vector2(1, 1));
        // SetFontStyle(FONT_BIG);

        // float w = RectWidth(GfxTextMetrics(Text));
        // float s = 4.8 * (RenderHeight / 480);

        // BigX[1] = 0;
        // BigText[1] = Text;
        // BigDelay[1] = Delay;
        // BigScale[1] = Min(1 / 4.8, (0.7 * RenderWidth / w) / s);
        // BigColor[1] = Col;
        // BigPosX[1] = (RenderWidth - s * w * BigScale[1]) / 2;
        // BigPosY[1] = 420 * _iscala.y;

        // if (r_scaleinterface.Value()) {
        //     BigPosX[1] = BigPosX[1] * (GameWidth / RenderWidth);
        // }
        
        // Implement the actual function based on the graphics system
        // This is a simplified version that would need to be integrated with the game rendering system
    }

    // Helper function to check if a string contains a substring (case-insensitive)
    inline bool ContainsText(const std::string& str, const std::string& substr) {
        auto it = std::search(
            str.begin(), str.end(),
            substr.begin(), substr.end(),
            [](char c1, char c2) {
                return std::tolower(c1) == std::tolower(c2);
            }
        );
        return (it != str.end());
    }

    // In-game nickname tab completion
    inline void TabComplete() {
        int ChatTextLen = ChatText.length();
        int Offset = 0;

        if (MySprite < 1) {
            return;
        }

        if ((ChatTextLen > 1) && (ChatText[1] == L'^')) {  // Pascal strings are 1-indexed
            Offset = 1;
        }

        // If not already tab-completing, save and use this base text for tab completion
        if (CurrentTabCompletePlayer == 0) {
            // Find where the current word starts
            size_t LastSeparator = ChatText.find_last_of(L' ');
            if (LastSeparator == std::wstring::npos) {
                LastSeparator = Offset;
            } else if (LastSeparator < Offset) {
                LastSeparator = Offset;
            }

            int CompletionBaseLen = ChatTextLen - LastSeparator;
            CompletionBase = std::string(ChatText.begin() + LastSeparator, ChatText.end());
            CompletionBaseSeparator = LastSeparator;
        }

        // Next potential match
        uint8_t ContinuedTabCompletePlayer = (CurrentTabCompletePlayer + 1) % MAX_PLAYERS;

        if (ChatTextLen > Offset) {  // Don't complete if chat is empty
            for (int i = ContinuedTabCompletePlayer; i < (ContinuedTabCompletePlayer + MAX_PLAYERS); i++) {
                int Next = ((i - 1) % MAX_PLAYERS) + 1;
                if (Sprite[Next] && Sprite[Next]->Active && (!Sprite[Next]->Player->DemoPlayer) && (Next != MySprite)) {
                    std::string playerName(Sprite[Next]->Player->Name.begin(), Sprite[Next]->Player->Name.end());
                    std::string completionBase(CompletionBase.begin(), CompletionBase.end());
                    if ((CompletionBase.empty()) || ContainsText(playerName, completionBase)) {
                        int AvailableChatSpace = MAXCHATTEXT - CompletionBaseSeparator;
                        std::wstring SpaceFittedName = std::wstring(Sprite[Next]->Player->Name.begin(), Sprite[Next]->Player->Name.end());
                        if (SpaceFittedName.length() > AvailableChatSpace) {
                            SpaceFittedName = SpaceFittedName.substr(0, AvailableChatSpace);
                        }
                        std::wstring prefix = ChatText.substr(0, CompletionBaseSeparator);
                        ChatText = prefix + SpaceFittedName;
                        CurrentTabCompletePlayer = Next;
                        CursorPosition = static_cast<uint8_t>(ChatText.length());
                        TabCompletePressed = true;
                        return;
                    }
                }
            }
        }
    }

    // Resets the stats of all weapons
    inline void ResetWeaponStats() {
        for (uint8_t i = 0; i <= 20; i++) {
            WepStats[i].Shots = 0;
            WepStats[i].Hits = 0;
            WepStats[i].Kills = 0;
            WepStats[i].Headshots = 0;
            WepStats[i].Accuracy = 0;
        }
    }

    inline int GetGameFps() {
        return FrameTiming.Fps;
    }

    inline void GameLoop() {
        bool GamePaused = (MapChangeCounter >= 0);

        double CurrentTime = GetCurrentTime();

        double FrameTime = CurrentTime - FrameTiming.PrevTime;

        FrameTiming.FpsAccum += FrameTime;

        FrameTiming.PrevTime = CurrentTime;
        TickTimeLast = TickTime;

        if (FrameTime > 2) {
            FrameTime = 0;
        }

        double dt = 1.0 / GOALTICKS;

        FrameTiming.Accumulator += FrameTime;
        TickTime = TickTime + static_cast<int>(FrameTiming.Accumulator / dt);

        double SimTime = (TickTime - TickTimeLast) * dt;
        FrameTiming.Accumulator = FrameTiming.Accumulator - SimTime;
        double FramePercent = std::min(1.0, std::max(0.0, FrameTiming.Accumulator / dt));

        for (int MainControl = 1; MainControl <= (TickTime - TickTimeLast); MainControl++) {
            // frame rate independent code
            if (!GamePaused) {
                FrameTiming.Elapsed = FrameTiming.Elapsed + (1.0 / DEFAULT_GOALTICKS);
            }

            Ticks++;
            ClientTickCount++;
            // Update main tick counter
            MainTickCounter++;

            if (MenuTimer > -1) {
                MenuTimer--;
            }

#ifdef STEAM
            // SteamAPI_RunCallbacks();
#endif

            // General game updating
            Update_Frame();

            if (DemoRecorder && DemoRecorder->Active() && (MainTickCounter % demo_rate.Value() == 0)) {
                DemoRecorder->SavePosition();
            }

            if ((MapChangeCounter < 0) && (!EscMenu->Active)) {
                // DEMO
                if (DemoRecorder && DemoRecorder->Active()) {
                    DemoRecorder->SaveNextFrame();
                }
                if (DemoPlayer && DemoPlayer->Active()) {
                    DemoPlayer->ProcessDemo();
                }
            }

            // Radio Cooldown
            if ((MainTickCounter % SECOND == 0) &&
                (RadioCooldown > 0) && (sv_radio.Value())) {
                RadioCooldown--;
            }

            // Packet rate send adjusting
            float Adjust = 1.0f;
            if (PacketAdjusting == 1) {
                int HeavySendersNum = PlayersNum - SpectatorsNum;

                if (HeavySendersNum < 5) {
                    Adjust = 0.75f;
                } else if (HeavySendersNum < 9) {
                    Adjust = 0.87f;
                } else {
                    Adjust = 1.0f;
                }
            }

            if ((MySprite > 0) && (!DemoPlayer->Active())) {
                // connection problems
                if ((MapChangeCounter < 0) && !EscMenu->Active) {
                    NoHeartbeatTime++;
                }

                if (NoHeartbeatTime > CONNECTIONPROBLEM_TIME) {
                    if (MainTickCounter % 120 == 0) {
                        if (NoHeartbeatTime > DISCONNECTION_TIME) {
                            MainConsole.Console(static_cast<WideString>(L"Connection timeout"), static_cast<uint32_t>(WARNING_MESSAGE_COLOR));
                        } else {
                            MainConsole.Console(static_cast<WideString>(L"Connection problem"), static_cast<uint32_t>(WARNING_MESSAGE_COLOR));
                        }
                    }

                    ClientStopMovingCounter = 0;
                }

                if (NoHeartbeatTime == DISCONNECTION_TIME) {
                    ShowMapChangeScoreboard();

                    GameMenuShow(TeamMenu, false);

                    MainConsole.Console(static_cast<WideString>(L"Connection timeout"), static_cast<uint32_t>(WARNING_MESSAGE_COLOR));

                    ClientDisconnect();
                }

                if (NoHeartbeatTime < 0) {
                    NoHeartbeatTime = 0;
                }

                ClientStopMovingCounter--;

                if (Connection == INTERNET) {
                    if (Sprite[MySprite]->Active) {
                        if (!Sprite[MySprite]->DeadMeat) {
                            if ((MainTickCounter % static_cast<int>(std::round(7 * Adjust)) == 1) &&
                               (MainTickCounter % static_cast<int>(std::round(5 * Adjust)) != 0)) {
                                ClientSpriteSnapshot();
                            }
                            if ((MainTickCounter % static_cast<int>(std::round(5 * Adjust)) == 0) ||
                               ForceClientSpriteSnapshotMov) {
                                ClientSpriteSnapshotMov();
                            }
                        } else {
                            if (MainTickCounter % static_cast<int>(std::round(30 * Adjust)) == 0) {
                                ClientSpriteSnapshotDead();
                            }
                        }
                    }
                }
                else if (Connection == LAN) {
                    if (!Sprite[MySprite]->DeadMeat) {
                        if (MainTickCounter % static_cast<int>(std::round(4 * Adjust)) == 0) {
                            ClientSpriteSnapshot();
                        }
                        if ((MainTickCounter % static_cast<int>(std::round(3 * Adjust)) == 0) ||
                           ForceClientSpriteSnapshotMov) {
                            ClientSpriteSnapshotMov();
                        }
                    } else {
                        if (MainTickCounter % static_cast<int>(std::round(15 * Adjust)) == 0) {
                            ClientSpriteSnapshotDead();
                        }
                    }
                }

                ForceClientSpriteSnapshotMov = false;
            }  // playing
        }  // Client

        // this shouldn't happen but still done for safety
        if (FrameTiming.PrevRenderTime > CurrentTime) {
            FrameTiming.PrevRenderTime = CurrentTime - FrameTiming.MinDeltaTime;
        }

        if (ShouldRenderFrames &&
            ((CurrentTime - FrameTiming.PrevRenderTime) >= FrameTiming.MinDeltaTime)) {
            FrameTiming.PrevRenderTime = CurrentTime;
            FrameTiming.Counter++;

            if (FrameTiming.Counter >= 30) {
                FrameTiming.Fps = static_cast<int>(std::round(FrameTiming.Counter / FrameTiming.FpsAccum));
                FrameTiming.Counter = 0;
                FrameTiming.FpsAccum = 0;
            }

            if (GamePaused) {
                RenderFrame(FrameTiming.Elapsed, FramePercent, true);
            } else {
                RenderFrame(FrameTiming.Elapsed - dt * (1 - FramePercent), FramePercent, false);
            }
        }

        if ((MapChangeCounter < 0) && (MapChangeCounter > -59)) {
            if (MapChangeName == "EXIT*!*") {
                ExitToMenu();
            }
        }

        if (MapChanged) {
            MapChanged = false;
            ResetFrameTiming();
        }

        if (r_sleeptime.Value() > 0) {
            SDL_Delay(r_sleeptime.Value());
        }
    }

    inline uint8_t GetCameraTarget(bool Backwards = false) {
        uint8_t NewCam = CameraFollowSprite;
        uint8_t NumLoops = 0;
        bool ValidCam = false;

        do {
            NumLoops++;
            if (NumLoops == 33) {  // Shit, way too many loops...
                NewCam = 0;
                ValidCam = true;
                break;
            }

            if (!Backwards) {
                NewCam++;
            } else {
                if (NewCam > 0) {
                    NewCam--;
                } else {
                    NewCam = MAX_SPRITES;
                }
            }
            
            if (NewCam > MAX_SPRITES) {
                NewCam = 1;
            } else if (NewCam < 1) {
                NewCam = MAX_SPRITES;
            }

            if (!Sprite[NewCam] || !Sprite[NewCam]->Active) {
                continue;  // Sprite slot empty
            }
            if (Sprite[NewCam]->DeadMeat) {
                continue;  // Sprite is dead
            }
            if (Sprite[NewCam]->IsSpectator()) {
                continue;  // Sprite is a spectator
            }

            if (Sprite[MySprite] && Sprite[MySprite]->Control.Up && (!sv_realisticmode.Value()) &&
               Sprite[MySprite]->IsNotSpectator()) {
                NewCam = 0;
                ValidCam = true;
                break;
            }  // Freecam if not Realistic

            if (Sprite[MySprite] && Sprite[MySprite]->IsSpectator()) {
                if (Sprite[MySprite] && Sprite[MySprite]->Control.Up) {
                    NewCam = 0;
                    ValidCam = true;
                    break;
                } else {
                    ValidCam = true;
                    break;  // Allow spectators to go into Free Cam
                }  // Let spectator view all players
            }

            if (Sprite[NewCam] && Sprite[MySprite] && Sprite[NewCam]->IsNotInSameTeam(Sprite[MySprite].get())) {
                continue;  // Don't swap camera to a player not on my team
            }

            ValidCam = true;
        } while (!ValidCam);

        return ValidCam ? NewCam : CameraFollowSprite;
    }

#ifdef STEAM_CODE
    inline void GetMicData() {
        // This would interface with Steam voice API
        // EVoiceResult availableVoice;
        // Cardinal availableVoiceBytes;
        // std::vector<uint8_t> voiceData;
        
        // availableVoice = SteamAPI.User.GetAvailableVoice(&availableVoiceBytes, nullptr, 0);

        // if ((availableVoice == k_EVoiceResultOK) && (availableVoiceBytes > 0)) {
        //     voiceData.resize(availableVoiceBytes);
        //     availableVoice = SteamAPI.User.GetVoice(true, voiceData.data(), availableVoiceBytes, &availableVoiceBytes, false, nullptr, 0, nullptr, 0);

        //     if ((availableVoice == k_EVoiceResultOK) && (availableVoiceBytes > 0)) {
        //         ClientSendVoiceData(voiceData, availableVoiceBytes);
        //     }
        // }
    }
#endif
}  // closes namespace ClientGameImpl
    
    

// Using declarations to bring into global namespace
using ClientGameImpl::MousePrev;
using ClientGameImpl::mx;
using ClientGameImpl::my;
using ClientGameImpl::MapChanged;
using ClientGameImpl::ChatChanged;
using ClientGameImpl::ShouldRenderFrames;
using ClientGameImpl::ActionSnap;
using ClientGameImpl::ActionSnapTaken;
using ClientGameImpl::CapScreen;
using ClientGameImpl::ShowScreen;
using ClientGameImpl::ScreenCounter;
using ClientGameImpl::IsFullscreen;
using ClientGameImpl::ScreenWidth;
using ClientGameImpl::ScreenHeight;
using ClientGameImpl::RenderWidth;
using ClientGameImpl::RenderHeight;
using ClientGameImpl::WindowWidth;
using ClientGameImpl::WindowHeight;
using ClientGameImpl::ChatText;
using ClientGameImpl::LastChatText;
using ClientGameImpl::FireChatText;
using ClientGameImpl::ChatType;
using ClientGameImpl::CompletionBase;
using ClientGameImpl::CompletionBaseSeparator;
using ClientGameImpl::CurrentTabCompletePlayer;
using ClientGameImpl::CursorPosition;
using ClientGameImpl::TabCompletePressed;
using ClientGameImpl::ChatTimeCounter;
using ClientGameImpl::ClientStopMovingCounter;
using ClientGameImpl::ForceClientSpriteSnapshotMov;
using ClientGameImpl::LastForceClientSpriteSnapshotMovTick;
using ClientGameImpl::MenuTimer;
using ClientGameImpl::ResetFrameTiming;
using ClientGameImpl::GameLoop;
using ClientGameImpl::GetGameFps;
using ClientGameImpl::GetCurrentTime;
using ClientGameImpl::TabComplete;
using ClientGameImpl::ResetWeaponStats;
using ClientGameImpl::BigMessage;
using ClientGameImpl::GetCameraTarget;
using ClientGameImpl::DemoRecorder;
using ClientGameImpl::DemoPlayer;
#ifdef STEAM_CODE
using ClientGameImpl::GetMicData;
#endif






#endif // CLIENT_GAME_H