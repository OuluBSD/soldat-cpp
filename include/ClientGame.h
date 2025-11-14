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
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

// Forward declarations (these would be defined in other headers)
struct TSprite;
struct TFrameTiming;

// Global variables
extern TVector2 MousePrev;
extern float mx, my;
extern bool MapChanged;
extern bool ChatChanged;  // used for blinking chat input
extern bool ShouldRenderFrames;  // false during game request phase

// used for action snap
extern uint8_t ActionSnap;
extern bool ActionSnapTaken;
extern int CapScreen;
extern bool ShowScreen;
extern uint8_t ScreenCounter;

// resolution
extern bool IsFullscreen;
extern int ScreenWidth;
extern int ScreenHeight;
extern int RenderWidth;
extern int RenderHeight;
extern int WindowWidth;
extern int WindowHeight;

// chat stuff
extern std::wstring ChatText;
extern std::wstring LastChatText;
extern std::wstring FireChatText;
extern uint8_t ChatType;
extern std::string CompletionBase;
extern int CompletionBaseSeparator;
extern uint8_t CurrentTabCompletePlayer;
extern uint8_t CursorPosition;
extern bool TabCompletePressed;
extern int ChatTimeCounter;

extern int ClientStopMovingCounter;
extern bool ForceClientSpriteSnapshotMov;
extern int LastForceClientSpriteSnapshotMovTick;
extern int MenuTimer;

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

    inline double GetCurrentTime() {
        int64_t x = SDL_GetPerformanceCounter();
        return static_cast<double>(x - FrameTiming.StartTime) / FrameTiming.Frequency;
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
                if (Sprite[Next].Active && (!Sprite[Next].Player.DemoPlayer) && (Next != MySprite)) {
                    std::string playerName(Sprite[Next].Player.Name.begin(), Sprite[Next].Player.Name.end());
                    std::string completionBase(CompletionBase.begin(), CompletionBase.end());
                    if ((CompletionBase.empty()) || ContainsText(playerName, completionBase)) {
                        int AvailableChatSpace = MAXCHATTEXT - CompletionBaseSeparator;
                        std::wstring SpaceFittedName = std::wstring(Sprite[Next].Player.Name.begin(), Sprite[Next].Player.Name.end());
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

        for (int MainControl = 1; MainControl <= (Ticktime - ticktimeLast); MainControl++) {
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

            if (DemoRecorder.Active && (MainTickCounter % demo_rate.Value() == 0)) {
                DemoRecorder.SavePosition();
            }

            if ((MapChangeCounter < 0) && (!EscMenu.Active)) {
                // DEMO
                if (DemoRecorder.Active) {
                    DemoRecorder.SaveNextFrame();
                }
                if (DemoPlayer.Active) {
                    DemoPlayer.ProcessDemo();
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

            if ((MySprite > 0) && (!DemoPlayer.Active)) {
                // connection problems
                if ((MapChangeCounter < 0) && !EscMenu.Active) {
                    NoHeartbeatTime++;
                }

                if (NoHeartbeatTime > CONNECTIONPROBLEM_TIME) {
                    if (MainTickCounter % 120 == 0) {
                        if (NoHeartbeatTime > DISCONNECTION_TIME) {
                            MainConsole.Console(L"Connection timeout", WARNING_MESSAGE_COLOR);
                        } else {
                            MainConsole.Console(L"Connection problem", WARNING_MESSAGE_COLOR);
                        }
                    }

                    ClientStopMovingCounter = 0;
                }

                if (NoHeartbeatTime == DISCONNECTION_TIME) {
                    ShowMapChangeScoreboard();

                    GameMenuShow(TeamMenu, false);

                    MainConsole.Console(L"Connection timeout", WARNING_MESSAGE_COLOR);

                    ClientDisconnect();
                }

                if (NoHeartbeatTime < 0) {
                    NoHeartbeatTime = 0;
                }

                ClientStopMovingCounter--;

                if (Connection == INTERNET) {
                    if (Sprite[MySprite].Active) {
                        if (!Sprite[MySprite].DeadMeat) {
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
                    if (!Sprite[MySprite].DeadMeat) {
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

            if (!Sprite[NewCam].Active) {
                continue;  // Sprite slot empty
            }
            if (Sprite[NewCam].DeadMeat) {
                continue;  // Sprite is dead
            }
            if (Sprite[NewCam].IsSpectator()) {
                continue;  // Sprite is a spectator
            }

            if (Sprite[MySprite].Control.Up && (!sv_realisticmode.Value()) &&
               Sprite[MySprite].IsNotSpectator()) {
                NewCam = 0;
                ValidCam = true;
                break;
            }  // Freecam if not Realistic

            if (Sprite[MySprite].IsSpectator()) {
                if (Sprite[MySprite].Control.Up) {
                    NewCam = 0;
                    ValidCam = true;
                    break;
                } else {
                    ValidCam = true;
                    break;  // Allow spectators to go into Free Cam
                }  // Let spectator view all players
            }

            if (Sprite[NewCam].IsNotInSameTeam(Sprite[MySprite])) {
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
}

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
using ClientGameImpl::GetMicData;

// Global variables
extern TVector2 MousePrev = {0.0f, 0.0f};
extern float mx = 0.0f;
extern float my = 0.0f;
extern bool MapChanged = false;
extern bool ChatChanged = true;  // used for blinking chat input
extern bool ShouldRenderFrames = true;  // false during game request phase

// used for action snap
extern uint8_t ActionSnap = 1;
extern bool ActionSnapTaken = false;
extern int CapScreen = 255;
extern bool ShowScreen = false;
extern uint8_t ScreenCounter = 255;

// resolution
extern bool IsFullscreen = false;
extern int ScreenWidth = DEFAULT_WIDTH;
extern int ScreenHeight = DEFAULT_HEIGHT;
extern int RenderWidth = 0;
extern int RenderHeight = 0;
extern int WindowWidth = 0;
extern int WindowHeight = 0;

// chat stuff
extern std::wstring ChatText = L"";
extern std::wstring LastChatText = L"";
extern std::wstring FireChatText = L"";
extern uint8_t ChatType = 0;
extern std::string CompletionBase = "";
extern int CompletionBaseSeparator = 0;
extern uint8_t CurrentTabCompletePlayer = 0;
extern uint8_t CursorPosition = 0;
extern bool TabCompletePressed = false;
extern int ChatTimeCounter = 0;

extern int ClientStopMovingCounter = 99999;
extern bool ForceClientSpriteSnapshotMov = false;
extern int LastForceClientSpriteSnapshotMovTick = 0;
extern int MenuTimer = 0;

// Frame timing variable
extern TFrameTiming FrameTiming;

#endif // CLIENT_GAME_H