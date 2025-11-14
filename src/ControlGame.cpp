//*******************************************************************************
//                                                                              
//       ControlGame Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2016 Paweł Drzazga               
//                                                                              
//*******************************************************************************

#include "ControlGame.h"
#include "Client.h"
#include "ClientGame.h"
#include "Game.h"
#include "Sprites.h"
#include "Input.h"
#include "Util.h"
#include "Demo.h"
#include "GameMenus.h"
#include "Command.h"
#include "Cvar.h"
#include "NetworkClientConnection.h"
#include "NetworkClientGame.h"
#include "NetworkClientMessages.h"
#include "Sound.h"
#include "GameStrings.h"
#include <cwctype>
#include <algorithm>
#include <cmath>

// Helper functions implementations
void ClearChatText() {
    LastChatText = ChatText;
    FireChatText = L"";
    CompletionBase = "";
    CurrentTabCompletePlayer = 0;
    CursorPosition = 1;
    VoteKickReasonType = false;
    ChatText = L"";
    SDL_StopTextInput();
}

std::wstring FilterChatText(const std::wstring& Str) {
    std::wstring result = L"";
    for (size_t i = 0; i < Str.length(); i++) {
        if (Str[i] >= 32 && Str[i] != 127) {
            result += Str[i];
        }
    }
    return result;
}

bool ChatKeyDown(Uint8 KeyMods, SDL_Keycode KeyCode) {
    bool result = false;

    if (ChatText.length() > 0) {
        if (KeyMods == KM_CTRL && KeyCode == SDLK_v) {
            // Get clipboard text
            char* clipboard_text = SDL_GetClipboardText();
            if (clipboard_text != nullptr) {
                std::string utf8_str = clipboard_text;
                std::wstring wide_str = utf8_to_wide(utf8_str);
                std::wstring filtered_str = FilterChatText(wide_str);
                
                int len = static_cast<int>(ChatText.length());
                int insert_pos = std::min(static_cast<int>(CursorPosition), len);
                ChatText.insert(insert_pos, filtered_str);
                int new_len = static_cast<int>(ChatText.length());
                CursorPosition += (new_len - len);

                int max_len = VoteKickReasonType ? REASON_CHARS - 1 : MAXCHATTEXT;
                if (static_cast<int>(ChatText.length()) > max_len) {
                    ChatText = ChatText.substr(0, max_len);
                    CursorPosition = std::min(CursorPosition, max_len);
                }

                CurrentTabCompletePlayer = 0;
                ChatChanged = true;
                result = true;
            }
        } else if (KeyMods == KM_NONE) {
            result = true;

            switch (KeyCode) {
                case SDLK_ESCAPE:
                    ChatText = LastChatText;
                    ClearChatText();
                    break;

                case SDLK_BACKSPACE:
                    ChatChanged = true;
                    if ((CursorPosition > 1) || (ChatText.length() == 1)) {
                        CurrentTabCompletePlayer = 0;
                        if (CursorPosition > 0) {
                            ChatText.erase(ChatText.begin() + CursorPosition - 1);
                            CursorPosition--;
                        }
                        if (ChatText.empty()) {
                            ChatText = LastChatText;
                            ClearChatText();
                        }
                    }
                    break;

                case SDLK_DELETE:
                    ChatChanged = true;
                    if (ChatText.length() > CursorPosition) {
                        ChatText.erase(ChatText.begin() + CursorPosition);
                        CurrentTabCompletePlayer = 0;
                    }
                    break;

                case SDLK_HOME:
                    ChatChanged = true;
                    CursorPosition = 1;
                    break;

                case SDLK_END:
                    ChatChanged = true;
                    CursorPosition = static_cast<int>(ChatText.length());
                    break;

                case SDLK_RIGHT:
                    ChatChanged = true;
                    if (ChatText.length() > static_cast<size_t>(CursorPosition)) {
                        CursorPosition++;
                    }
                    break;

                case SDLK_LEFT:
                    ChatChanged = true;
                    if (CursorPosition > 1) {
                        CursorPosition--;
                    }
                    break;

                case SDLK_TAB:
                    TabComplete();
                    break;

                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (!ChatText.empty() && ChatText[0] == '/') {
                        ChatType = MSGTYPE_CMD;
                        std::string console_str = wide_to_utf8(ChatText.substr(1));
                        if (ParseInput(console_str)) {
                            ClearChatText();
                            return true;
                        }
                    }
                    if (MySprite > 0) {
                        if (VoteKickReasonType) {
                            if (ChatText.length() > 3) {
                                ClientVoteKick(KickMenuIndex, false, wide_to_utf8(ChatText));
                                VoteKickReasonType = false;
                            }
                        } else {
                            std::wstring chat_copy = ChatText.length() > 1 ? ChatText.substr(1) : L"";
                            ClientSendStringMessage(chat_copy, ChatType);
                        }
                    }

                    ClearChatText();
                    break;

                default:
                    result = false;
                    break;
            }
        }
    }

    return result;
}

bool MenuKeyDown(Uint8 KeyMods, SDL_Scancode KeyCode) {
    bool result = false;

    if (KeyMods == KM_NONE && KeyCode == SDL_SCANCODE_ESCAPE) {
        result = true;

        if (ShowRadioMenu) {
            ShowRadioMenu = false;
            RMenuState = "  ";
        } else if (KickMenu.Active || MapMenu.Active) {
            GameMenuShow(EscMenu);
        } else {
            GameMenuShow(EscMenu, !EscMenu.Active);
        }
    } else if (KeyCode >= SDL_SCANCODE_1 && KeyCode <= SDL_SCANCODE_0) {
        if (TeamMenu.Active) {
            if (KeyMods == KM_NONE) {
                int menu_index = (KeyCode - SDL_SCANCODE_1 + 1) % 10;
                result = GameMenuAction(TeamMenu, menu_index);
            }
        } else if (EscMenu.Active) {
            if (KeyMods == KM_NONE) {
                int menu_index = KeyCode - SDL_SCANCODE_1;
                result = GameMenuAction(EscMenu, menu_index);
            }
        } else if (LimboMenu.Active) {
            if (KeyMods == KM_NONE) {
                int menu_index = KeyCode - SDL_SCANCODE_1;
                result = GameMenuAction(LimboMenu, menu_index);
            } else if (KeyMods == KM_CTRL) {
                int menu_index = KeyCode - SDL_SCANCODE_1 + 10;
                result = GameMenuAction(LimboMenu, menu_index);
            }
        }
    }

    return result;
}

bool KeyDown(SDL_KeyboardEvent& KeyEvent) {
    bool result = true;
    SDL_Scancode KeyCode = KeyEvent.keysym.scancode;

    Uint8 KeyMods = 
        ((KeyEvent.keysym.mod & KMOD_ALT) != 0 ? 1 : 0) << 0 |
        ((KeyEvent.keysym.mod & KMOD_CTRL) != 0 ? 1 : 0) << 1 |
        ((KeyEvent.keysym.mod & KMOD_SHIFT) != 0 ? 1 : 0) << 2;

    if (ChatKeyDown(KeyMods, KeyEvent.keysym.sym)) {
        return true;
    }

    if (KeyEvent.repeat != 0) {
        result = false;
        return result;
    }

    if (MenuKeyDown(KeyMods, KeyCode)) {
        return true;
    }

    // other hard coded key bindings
    if (KeyMods == KM_NONE) {
        switch (KeyCode) {
            case SDL_SCANCODE_PAGEDOWN:
                if (FragsMenuShow) {
                    FragsScrollLev = std::min(FragsScrollLev + 1, FragsScrollMax);
                }
                break;

            case SDL_SCANCODE_PAGEUP:
                if (FragsMenuShow) {
                    FragsScrollLev = std::max(FragsScrollLev - 1, 0);
                }
                break;

            case SDL_SCANCODE_F11:
                result = VoteActive;
                VoteActive = false;
                break;

            case SDL_SCANCODE_F12:
                result = VoteActive;

                if (VoteActive) {
                    VoteActive = false;

                    if (VoteType == VOTE_MAP) {
                        std::wstring target_wstr = utf8_to_wide(VoteTarget);
                        ClientSendStringMessage(L"votemap " + target_wstr, MSGTYPE_CMD);
                        std::string msg = "You have voted on " + VoteTarget;
                        MainConsole.Console(utf8_to_wide(msg), VOTE_MESSAGE_COLOR);
                    } else if (VoteType == VOTE_KICK) {
                        int i = std::stoi(VoteTarget);
                        ClientVoteKick(i, true, "");
                        std::string player_name = wide_to_utf8(Sprite[i].Player.Name);
                        std::string msg = "You have voted to kick " + player_name;
                        MainConsole.Console(utf8_to_wide(msg), VOTE_MESSAGE_COLOR);
                    }
                }
                break;

            case SDL_SCANCODE_F9:
                SDL_MinimizeWindow(GameWindow);
                result = true;
                break;

            case SDL_SCANCODE_F8:
                result = false;

                if (DemoPlayer.Active) {
                    result = true;
                    GoalTicks = (GoalTicks == DEFAULT_GOALTICKS) ? 8.0 : 1.0;
                }
                break;

            case SDL_SCANCODE_F10:
                result = false;

                if (DemoPlayer.Active) {
                    result = true;
                    if ((MapChangeCounter < 0) || (MapChangeCounter > 99999999)) {
                        if (MapChangeCounter < 0) {
                            MapChangeCounter = 999999999;
                        } else {
                            MapChangeCounter = -60;
                        }
                    }
                }
                break;

            case SDL_SCANCODE_1:
            case SDL_SCANCODE_2:
            case SDL_SCANCODE_3:
            {
                result = false;

                if (ChatText.empty() && sv_radio.Value && ShowRadioMenu) {
                    result = true;
                    int i = (RMenuState[0] != ' ') ? 1 : 0;

                    switch (KeyCode) {
                        case SDL_SCANCODE_1: RMenuState[i] = '1'; break;
                        case SDL_SCANCODE_2: RMenuState[i] = '2'; break;
                        case SDL_SCANCODE_3: RMenuState[i] = '3'; break;
                    }

                    if (i == 1) {
                        // Radio menu implementation would go here based on the Pascal code
                        // For now, just send a placeholder message
                        ChatText = L"Radio message";
                        ClientSendStringMessage(ChatText, MSGTYPE_RADIO);
                        ChatText = L"";
                        ShowRadioMenu = false;
                        RMenuState = "  ";
                    }
                }
                break;
            }

            default:
                result = false;
                break;
        }
    } else if (KeyMods == KM_ALT) {
        switch (KeyCode) {
            case SDL_SCANCODE_F4:
                ExitToMenu();
                break;

            case SDL_SCANCODE_F9:
                ExitToMenu();
                break;

            default:
                result = false;
                break;
        }
    } else if (KeyMods == KM_CTRL || KeyMods == KM_SHIFT) {
        result = false;
    }

    if (result) {
        return result;
    }

    // bindings
    PBind bind = FindKeyBind(KeyMods, KeyCode);
    result = (bind != nullptr);

    if (!result) {
        return result;
    }

    TAction action = bind->Action;

    if (action == TAction::SniperLine) {
        if (!sv_sniperline.Value) {
            SniperLine = !SniperLine;
        }
    } else if (action == TAction::StatsMenu) {
        if (!EscMenu.Active) {
            StatsMenuShow = !StatsMenuShow;
            if (StatsMenuShow) {
                FragsMenuShow = false;
            }
        }
    } else if (action == TAction::GameStats) {
        ConInfoShow = !ConInfoShow;
    } else if (action == TAction::MiniMap) {
        MiniMapShow = !MiniMapShow;
    } else if (action == TAction::PlayerName) {
        PlayerNamesShow = !PlayerNamesShow;
    } else if (action == TAction::FragsList) {
        if (!EscMenu.Active) {
            FragsScrollLev = 0;
            FragsMenuShow = !FragsMenuShow;
            if (FragsMenuShow) {
                StatsMenuShow = false;
            }
        }
    } else if (action == TAction::Radio) {
        if (ChatText.empty() && sv_radio.Value && MySprite > 0 && 
           Sprite[MySprite].IsNotSpectator()) {
            ShowRadioMenu = !ShowRadioMenu;
            RMenuState = "  ";
        }
    } else if (action == TAction::RecordDemo) {
        if (!DemoPlayer.Active) {
            if (demo_autorecord.Value) {
                DemoRecorder.StopRecord();
                std::string filename = UserDirectory + "demos/" + 
                    GenerateTimestamp() + Map.Name + ".sdm";
                DemoRecorder.StartRecord(filename);
            } else if (DemoRecorder.Active) {
                DemoRecorder.StopRecord();
            } else {
                std::string filename = UserDirectory + "demos/" + 
                    GenerateTimestamp() + Map.Name + ".sdm";
                DemoRecorder.StartRecord(filename);
            }
        }
    } else if (action == TAction::VolumeUp || action == TAction::VolumeDown) {
        if (ChatText.empty() && !EscMenu.Active) {
            int old_volume = snd_volume.Value;

            if (action == TAction::VolumeUp) {
                snd_volume.SetValue(std::min(snd_volume.Value + 10, 100));
            }

            if (action == TAction::VolumeDown) {
                snd_volume.SetValue(std::max(snd_volume.Value - 10, 0));
            }

            if (snd_volume.Value != old_volume) {
                VolumeInternal = ScaleVolumeSetting(snd_volume.Value);
                SetVolume(-1, VolumeInternal);
                std::string msg = "Volume: " + std::to_string(snd_volume.Value) + "%";
                MainConsole.Console(utf8_to_wide(msg), MUSIC_MESSAGE_COLOR);
            }
        }
    } else if (action == TAction::MouseSensitivityUp || action == TAction::MouseSensitivityDown) {
        if (ChatText.empty() && !EscMenu.Active) {
            int increment = (action == TAction::MouseSensitivityDown) ? -5 : 5;
            cl_sensitivity.SetValue(std::max(0.0f, 
                static_cast<float>(increment + std::floor(100 * cl_sensitivity.Value)) / 100));
            std::string msg = "Sensitivity: " + std::to_string(static_cast<int>(std::floor(100 * cl_sensitivity.Value))) + "%";
            MainConsole.Console(utf8_to_wide(msg), MUSIC_MESSAGE_COLOR);
        }
    } else if (action == TAction::Cmd) {
        if (ChatText.empty()) {
            ChatText = L"/";
            ChatType = MSGTYPE_CMD;
            ChatChanged = true;
            CursorPosition = 1;
            VoteKickReasonType = false;
            SDL_StartTextInput();
        }
    } else if (action == TAction::Chat) {
        if (ChatText.empty()) {
            SDL_StartTextInput();
            ChatChanged = true;
            ChatText = L" ";
            ChatType = MSGTYPE_PUB;

            if (!FireChatText.empty()) {
                ChatText = FireChatText;
            }

            // force spectator chat to teamchat in survival mode when Round hasn't ended
            if (sv_survivalmode.Value && Sprite[MySprite].IsSpectator() &&
                !SurvivalEndRound && sv_survivalmode_antispy.Value) {
                ChatType = MSGTYPE_TEAM;
            }

            CursorPosition = static_cast<int>(ChatText.length());
        }
    } else if (action == TAction::TeamChat) {
        if (ChatText.empty() && MySprite > 0 && 
           (Sprite[MySprite].IsSpectator() || IsTeamGame())) {
            SDL_StartTextInput();
            ChatText = L" ";
            ChatType = MSGTYPE_TEAM;
            ChatChanged = true;
            CursorPosition = static_cast<int>(ChatText.length());
        }
    } else if (action == TAction::Snap) {
        if (cl_actionsnap.Value && ScreenCounter < 255 && ActionSnapTaken) {
            ShowScreen = !ShowScreen;

            if (!ShowScreen) {
                ScreenCounter = 255;
            } else {
                PlaySound(SFX_SNAPSHOT);
            }
        } else {
            ScreenCounter = 255;
            ShowScreen = false;
        }
    } else if (action == TAction::Weapons) {
        if (ChatText.empty() && MySprite > 0 && !EscMenu.Active && 
           !Sprite[MySprite].IsSpectator()) {
            if (Sprite[MySprite].DeadMeat) {
                GameMenuShow(LimboMenu, !LimboMenu.Active);
                LimboLock = !LimboMenu.Active;
                std::string msg = LimboLock ? "Weapons menu disabled" : "Weapons menu active";
                MainConsole.Console(utf8_to_wide(msg), GAME_MESSAGE_COLOR);
            } else {
                int PriCount = 0;
                int SecCount = 0;
                int PriNum = Sprite[MySprite].Weapon.Num;
                int SecNum = Sprite[MySprite].SecondaryWeapon.Num;

                for (int i = 1; i <= PRIMARY_WEAPONS; i++) {
                    PriCount += WeaponActive[i];
                }

                for (int i = 1; i <= SECONDARY_WEAPONS; i++) {
                    SecCount += WeaponActive[i + PRIMARY_WEAPONS];
                }

                if (!LimboMenu.Active ||
                    (((PriNum != Guns[NOWEAPON].Num) || (PriCount == 0)) &&
                     ((SecNum != Guns[NOWEAPON].Num) || (SecCount == 0)))) {
                    GameMenuShow(LimboMenu, false);
                    LimboLock = !LimboLock;
                    std::string msg = LimboLock ? "Weapons menu disabled" : "Weapons menu active";
                    MainConsole.Console(utf8_to_wide(msg), GAME_MESSAGE_COLOR);
                }
            }
        }
    } else if (action == TAction::Bind) {
        if (ChatTimeCounter == 0 && !bind->Command.empty()) {
            if (ChatText.empty() && !EscMenu.Active) {
                if (!ParseInput(wide_to_utf8(bind->Command))) {
                    ClientSendStringMessage(bind->Command, MSGTYPE_CMD);
                }
            }
        }
    } else if (action == TAction::VoiceChat) {
        // Steam voice chat implementation would go here
        // For now, just add a placeholder
        result = false;
    } else {
        result = false;
    }

    return result;
}

bool KeyUp(SDL_KeyboardEvent& KeyEvent) {
    bool result = true;
    SDL_Scancode KeyCode = KeyEvent.keysym.scancode;

    Uint8 KeyMods = 
        ((KeyEvent.keysym.mod & KMOD_ALT) != 0 ? 1 : 0) << 0 |
        ((KeyEvent.keysym.mod & KMOD_CTRL) != 0 ? 1 : 0) << 1 |
        ((KeyEvent.keysym.mod & KMOD_SHIFT) != 0 ? 1 : 0) << 2;

    if (KeyEvent.repeat != 0) {
        result = false;
        return result;
    }

    // bindings
    PBind bind = FindKeyBind(KeyMods, KeyCode);
    result = (bind != nullptr);

    if (!result) {
        return result;
    }

    TAction action = bind->Action;

    if (action == TAction::VoiceChat) {
        // Steam voice chat implementation would go here
        // For now, just add a placeholder
        result = false;
    } else {
        result = false;
    }

    return result;
}

void GameInput() {
    SDL_Event event;
    std::wstring str;
    bool ChatEnabled = !ChatText.empty();

    while (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
            case SDL_QUIT:
                ClientDisconnect();
                exit(0);
                break;

            case SDL_KEYDOWN:
                if (!KeyDown(event.key)) {
                    KeyStatus[event.key.keysym.scancode] = true;
                }
                break;

            case SDL_KEYUP:
                KeyStatus[event.key.keysym.scancode] = false;
                KeyUp(event.key);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (!GameMenuClick()) {
                    KeyStatus[event.button.button + 300] = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                KeyStatus[event.button.button + 300] = false;
                break;

            case SDL_TEXTINPUT:
                if (ChatEnabled) {
                    std::string utf8_str = event.text.text;
                    std::wstring wide_str = utf8_to_wide(utf8_str);
                    std::wstring filtered_str = FilterChatText(wide_str);

                    if (ChatText == L"/" && filtered_str == L"/" && LastChatText.length() > 1) {
                        ChatChanged = true;
                        CurrentTabCompletePlayer = 0;
                        ChatText = LastChatText;
                        CursorPosition = static_cast<int>(ChatText.length());
                    } else if (!ChatText.empty()) {
                        if (static_cast<int>(ChatText.length()) < (VoteKickReasonType ? REASON_CHARS - 1 : MAXCHATTEXT)) {
                            ChatChanged = true;
                            CurrentTabCompletePlayer = 0;
                            int insert_pos = std::min(static_cast<int>(CursorPosition), static_cast<int>(ChatText.length()));
                            ChatText.insert(insert_pos, filtered_str);
                            CursorPosition += static_cast<int>(filtered_str.length());
                        }
                    }
                }
                break;

            case SDL_MOUSEMOTION:
                if (SDL_GetWindowFlags(GameWindow) & SDL_WINDOW_INPUT_FOCUS) {
                    mx = std::max(0.0f, std::min(static_cast<float>(GameWidth), 
                        mx + event.motion.xrel * cl_sensitivity.Value));
                    my = std::max(0.0f, std::min(static_cast<float>(GameHeight), 
                        my + event.motion.yrel * cl_sensitivity.Value));

                    GameMenuMouseMove();
                }
                break;
        }
    }
}