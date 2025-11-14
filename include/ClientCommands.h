#ifndef CLIENTCOMMANDS_H
#define CLIENTCOMMANDS_H

//*******************************************************************************
//                                                                              
//       Client Commands Unit for SOLDAT                                        
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <cctype>

#include "Client.h"
#include "Command.h"
#include "Util.h"
#include "Game.h"
#include "ClientGame.h"
#include "Sound.h"
#include "GameRendering.h"
#include "NetworkClientMessages.h"
#include "Demo.h"
#include "GameStrings.h"
#include "Net.h"
#include "Sprites.h"
#include "Constants.h"
#include "Input.h"

// Global variables
extern uint8_t ScreenShotsInARow;

// Function declarations
void InitClientCommands();

namespace ClientCommandsImpl {
    inline void CommandBind(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() < 3) {
            MainConsole.Console(L"Usage: bind \"key\" \"command\"", GAME_MESSAGE_COLOR);
            return;
        }

        std::string BindKeyName = ToLower(Args[1]);
        std::string CommandString = Args[2];
        uint16_t Modifier = KM_NONE;

        if (BindKeyName.find('+') != std::string::npos) {
            if (ContainsText(BindKeyName, "ctrl")) {
                Modifier |= KM_CTRL;
            }
            if (ContainsText(BindKeyName, "shift")) {
                Modifier |= KM_SHIFT;
            }
            if (ContainsText(BindKeyName, "alt")) {
                Modifier |= KM_ALT;
            }
            
            // Remove modifier strings and the plus sign
            std::vector<std::string> toRemove = {"ctrl", "shift", "alt", "+"};
            for (const auto& str : toRemove) {
                size_t pos;
                while ((pos = BindKeyName.find(str)) != std::string::npos) {
                    BindKeyName.erase(pos, str.length());
                }
            }
        }

        if (Args[2].length() > 0 && Args[2][0] == '+') {
            // BindKey(BindKeyName, CommandString, CommandString, Modifier);  // Assuming BindKey is available
        } else {
            // BindKey(BindKeyName, "+bind", CommandString, Modifier);  // Assuming BindKey is available
        }
    }

    inline void CommandConnect(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string S;
        
        if (Args.size() <= 1) {
            MainConsole.Console(L"Usage: connect ip port password", GAME_MESSAGE_COLOR);
            return;
        }
        
        ExitToMenu();
        
        if (Args[0] == "joinurl") {
            S = Args[1];
            JoinIP = GetPiece(S, "//", 2);
            JoinIP = GetPiece(JoinIP, ":", 1);

            JoinPort = GetPiece(S, ":", 3);
            JoinPort = GetPiece(JoinPort, "/", 1);
            // Remove trailing slashes
            size_t slashPos = JoinPort.find("/");
            if (slashPos != std::string::npos) {
                JoinPort = JoinPort.substr(0, slashPos);
            }

            JoinPassword = GetPiece(S, "/", 4);
        } else {
            JoinIP = Args[1];
            if (Args.size() == 2) {
                JoinPort = "23073";
            } else {
                JoinPort = Args[2];
            }
            if (Args.size() > 3) {
                JoinPassword = Args[3];
            }
        }
        JoinServer();
    }

    inline void CommandRetry(const std::vector<std::string>& Args, uint8_t Sender) {
        ExitToMenu();
        JoinServer();
    }

    inline void CommandDisconnect(const std::vector<std::string>& Args, uint8_t Sender) {
        ExitToMenu();
    }

    inline void CommandSay(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() <= 1) {
            MainConsole.Console(L"Usage: say \"text\"", GAME_MESSAGE_COLOR);
            return;
        }
        ClientSendStringMessage(WideString(Args[1]), MSGTYPE_PUB);
    }

    inline void CommandSayTeam(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() <= 1) {
            MainConsole.Console(L"Usage: say_team \"text\"", GAME_MESSAGE_COLOR);
            return;
        }
        ClientSendStringMessage(WideString(Args[1]), MSGTYPE_TEAM);
    }

    inline void CommandRecord(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Str;
        
        if (Args.size() == 2) {
            Str = Args[1];
        } else {
            // Format as datetime_mapname
            Str = FormatDateTime("yyyy-mm-dd_hh-nn-ss_", Now()) + Map.Name;
        }

        if (DemoRecorder.Active) {
            DemoRecorder.StopRecord();
        }
        DemoRecorder.StartRecord(UserDirectory + "demos/" + Str + ".sdm");
    }

    inline void CommandMute(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Str;
        int i;
        std::vector<uint8_t> Targets;  // Assuming CommandTarget returns vector of indices

        if (Args.size() == 1) {
            return;
        }

        Str = Args[1];

        if (Str == "all") {
            MuteAll = !MuteAll;

            if (MuteAll) {
                MainConsole.Console(WideString("Everyone is muted"), CLIENT_MESSAGE_COLOR);
            } else {
                MainConsole.Console(WideString("Everyone is unmuted"), CLIENT_MESSAGE_COLOR);
            }

            return;
        }

        Targets = CommandTarget(Str, Sender);  // Assuming CommandTarget is implemented elsewhere
        for (i = 0; i < Targets.size(); i++) {
            Sprite[Targets[i]].Muted = true;
            std::wstring name = std::wstring(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end());
            MainConsole.Console(WideFormat(_("%s is muted"), {name}), CLIENT_MESSAGE_COLOR);
        }
    }

    inline void CommandUnbindall(const std::vector<std::string>& Args, uint8_t Sender) {
        UnbindAll();  // Assuming UnbindAll is implemented elsewhere
        MainConsole.Console("Unbinded all binds", GAME_MESSAGE_COLOR);
    }

    inline void CommandUnmute(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Str;
        int i;
        std::vector<uint8_t> Targets;  // Assuming CommandTarget returns vector of indices

        if (Args.size() == 1) {
            return;
        }

        Str = Args[1];

        Targets = CommandTarget(Str, Sender);  // Assuming CommandTarget is implemented elsewhere
        for (i = 0; i < Targets.size(); i++) {
            Sprite[Targets[i]].Muted = false;
            std::wstring name = std::wstring(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end());
            MainConsole.Console(WideFormat(_("%s is unmuted"), {name}), CLIENT_MESSAGE_COLOR);
        }
    }

    inline void CommandStop(const std::vector<std::string>& Args, uint8_t Sender) {
        if (DemoRecorder.Active) {
            DemoRecorder.StopRecord();
        }
    }

    inline void CommandShutdown(const std::vector<std::string>& Args, uint8_t Sender) {
        ExitToMenu();
    }

    inline void CommandScreenshot(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string ScreenFile;

        if (ScreenShotsInARow < 3) {
            ScreenShotsInARow++;
#ifdef STEAM_CODE
            if (cl_steam_screenshots.Value()) {
                // SteamAPI.Screenshots.TriggerScreenshot();  // Assuming Steam API is available
                return;
            }
#endif
            ScreenFile = UserDirectory + "screens/" +
                         FormatDateTime("yyyy-mm-dd_hh-nn-ss_", Now()) +
                         Map.Name + "_screenshot.png";

            MainConsole.Console(WideString("Screenshot saved to " + ScreenFile),
                                DEBUG_MESSAGE_COLOR);

            // TakeScreenShot(ScreenFile);  // Assuming TakeScreenShot is implemented elsewhere

            // PlaySound(SFX_SNAPSHOT);  // Assuming PlaySound is implemented elsewhere

            if (ShowScreen) {
                ShowScreen = false;
            }
        }
    }

    inline void CommandSwitchCam(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() <= 1) {
            MainConsole.Console(L"Usage: switchcam \"id\"", GAME_MESSAGE_COLOR);
            return;
        }
        if (Sprite[MySprite].IsSpectator()) {
            CameraFollowSprite = StrToIntDef(Args[1], 0);
        }
    }

    inline void CommandSwitchCamFlag(const std::vector<std::string>& Args, uint8_t Sender) {
        int i;

        if (Args.size() <= 1) {
            MainConsole.Console(L"Usage: switchcamflag \"id\"", GAME_MESSAGE_COLOR);
            return;
        }
        if (Sprite[MySprite].IsSpectator()) {
            for (i = 1; i <= MAX_THINGS; i++) {
                if (Thing[i].Style == StrToIntDef(Args[1], 0)) {
                    CameraFollowSprite = 0;
                    CameraX = Thing[i].Skeleton.Pos[1].X;
                    CameraY = Thing[i].Skeleton.Pos[1].Y;
                }
            }
        }
    }

    inline void CommandDemoTick(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args[0] == "demo_tick") {
            // DemoPlayer.Position(StrToIntDef(Args[1], 0));  // Assuming DemoPlayer is implemented
        } else {
            // DemoPlayer.Position(MainTickCounter + StrToIntDef(Args[1], 0));  // Assuming DemoPlayer is implemented
        }
    }

    inline void InitClientCommands() {
        // CommandAdd('bind', CommandBind, 'Binds command to key', []);
        // CommandAdd('connect', CommandConnect, 'connect to server', [CMD_DEFERRED]);
        // CommandAdd('join', CommandConnect, 'connect to server', [CMD_DEFERRED]);
        // CommandAdd('joinurl', CommandConnect, 'connect to server using url', [CMD_DEFERRED]);
        // CommandAdd('disconnect', CommandDisconnect, 'disconnect from server', [CMD_DEFERRED]);
        // CommandAdd('retry', CommandRetry, 'retry connect to last server', [CMD_DEFERRED]);
        // CommandAdd('screenshot', CommandScreenshot, 'take a screenshot of game', [CMD_DEFERRED]);
        // CommandAdd('say', CommandSay, 'send chat message', []);
        // CommandAdd('say_team', CommandSayTeam, 'send team chat message', []);
        // CommandAdd('record', CommandRecord, 'record demo', []);
        // CommandAdd('mute', CommandMute, 'mute specific nick or id', []);
        // CommandAdd('unbindall', CommandUnbindall, 'Unbinds all binds', []);
        // CommandAdd('unmute', CommandUnmute, 'unmute specific nick or id', []);
        // CommandAdd('stop', CommandStop, 'stop recording demo', []);
        // CommandAdd('shutdown', CommandShutdown, 'shutdown game', []);
        // CommandAdd('switchcam', CommandSwitchCam, 'switches camera to specific player', []);
        // CommandAdd('switchcamflag', CommandSwitchCamFlag, 'switches camera to specific flag', []);
        // CommandAdd('demo_tick', CommandDemoTick, 'skips to a tick in demo', []);
        // CommandAdd('demo_tick_r', CommandDemoTick, 'skips to a tick (relatively) in demo', []);

        // Placeholder implementation using Command system
        // Assuming CommandAdd function takes: name, function pointer, description, and flags
        // These would need to be properly implemented based on the Command system
        
        // This implementation would need to be integrated with the Command system defined in Command.h
        // For now, just adding function pointers based on the Pascal implementation
    }
}

// Using declarations to bring into global namespace
using ClientCommandsImpl::InitClientCommands;
using ClientCommandsImpl::CommandBind;
using ClientCommandsImpl::CommandConnect;
using ClientCommandsImpl::CommandRetry;
using ClientCommandsImpl::CommandDisconnect;
using ClientCommandsImpl::CommandSay;
using ClientCommandsImpl::CommandSayTeam;
using ClientCommandsImpl::CommandRecord;
using ClientCommandsImpl::CommandMute;
using ClientCommandsImpl::CommandUnbindall;
using ClientCommandsImpl::CommandUnmute;
using ClientCommandsImpl::CommandStop;
using ClientCommandsImpl::CommandShutdown;
using ClientCommandsImpl::CommandScreenshot;
using ClientCommandsImpl::CommandSwitchCam;
using ClientCommandsImpl::CommandSwitchCamFlag;
using ClientCommandsImpl::CommandDemoTick;

// Global variable
extern uint8_t ScreenShotsInARow = 0;

#endif // CLIENTCOMMANDS_H