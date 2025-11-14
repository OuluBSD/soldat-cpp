#ifndef GAME_MENUS_H
#define GAME_MENUS_H

//*******************************************************************************
//                                                                              
//       Game Menus Unit for SOLDAT                                               
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <vector>
#include <string>
#include <memory>
#include "Vector.h"
#include "Constants.h"
#include "Weapons.h"
#include "Client.h"

// Forward declarations
struct TGameButton;
struct TGameMenu;

// Structures
struct TGameButton {
    bool Active;
    int x1, y1, x2, y2;
    std::wstring Caption;
    
    TGameButton() : Active(false), x1(0), y1(0), x2(0), y2(0), Caption(L"") {}
};

struct TGameMenu {
    bool Active;
    int x, y;
    int w, h;
    std::vector<TGameButton> Button;
    
    TGameMenu() : Active(false), x(0), y(0), w(0), h(0) {}
};

// Global variables
extern std::vector<TGameMenu> GameMenu;
extern TGameMenu* HoveredMenu;
extern TGameButton* HoveredButton;
extern int HoveredButtonIndex;
extern TGameMenu* EscMenu;
extern TGameMenu* TeamMenu;
extern TGameMenu* LimboMenu;
extern TGameMenu* KickMenu;
extern TGameMenu* MapMenu;
extern int KickMenuIndex;
extern int MapMenuIndex;

// Function declarations
void InitGameMenus();
void GameMenuShow(TGameMenu* Menu, bool Show = true);
bool GameMenuAction(TGameMenu* Menu, int ButtonIndex);
void GameMenuMouseMove();
bool GameMenuClick();

namespace GameMenusImpl {
    inline void InitButton(TGameMenu* Menu, int ButtonIndex, const std::wstring& Caption,
                          int x, int y, int w, int h, bool Active = true) {
        if (ButtonIndex >= 0 && ButtonIndex < static_cast<int>(Menu->Button.size())) {
            Menu->Button[ButtonIndex].Active = Active;
            Menu->Button[ButtonIndex].x1 = Menu->x + x;
            Menu->Button[ButtonIndex].y1 = Menu->y + y;
            Menu->Button[ButtonIndex].x2 = Menu->x + x + w;
            Menu->Button[ButtonIndex].y2 = Menu->y + y + h;
            Menu->Button[ButtonIndex].Caption = Caption;
        }
    }

    inline void InitGameMenus() {
        HoveredMenu = nullptr;
        HoveredButton = nullptr;
        HoveredButtonIndex = 0;

        GameMenu.resize(5);
        EscMenu   = &GameMenu[0];
        TeamMenu  = &GameMenu[1];
        LimboMenu = &GameMenu[2];
        KickMenu  = &GameMenu[3];
        MapMenu   = &GameMenu[4];

        // esc menu
        EscMenu->w = 300;
        EscMenu->h = 200;

        if (r_scaleinterface.Value()) {
            EscMenu->x = static_cast<int>((GameWidth - EscMenu->w) / 2);
            EscMenu->y = static_cast<int>((GameHeight - EscMenu->h) / 2);
        } else {
            EscMenu->x = static_cast<int>((RenderWidth - EscMenu->w) / 2);
            EscMenu->y = static_cast<int>((RenderHeight - EscMenu->h) / 2);
        }

#ifdef STEAM_CODE
        EscMenu->Button.resize(5);
        InitButton(EscMenu, 0, L"1 " + _(L"Exit to menu"), 5, 1 * 25, 240, 25);
        InitButton(EscMenu, 1, L"2 " + _(L"Change map"), 5, 2 * 25, 240, 25);
        InitButton(EscMenu, 2, L"3 " + _(L"Kick player"), 5, 3 * 25, 240, 25);
        InitButton(EscMenu, 3, L"4 " + _(L"Change team"), 5, 4 * 25, 240, 25);
        InitButton(EscMenu, 4, _(L"Server Website"), 5, 7 * 25, 240, 15);
#else
        EscMenu->Button.resize(4);
        InitButton(EscMenu, 0, L"1 " + _(L"Exit to menu"), 5, 1 * 25, 240, 25);
        InitButton(EscMenu, 1, L"2 " + _(L"Change map"), 5, 2 * 25, 240, 25);
        InitButton(EscMenu, 2, L"3 " + _(L"Kick player"), 5, 3 * 25, 240, 25);
        InitButton(EscMenu, 3, L"4 " + _(L"Change team"), 5, 4 * 25, 240, 25);
#endif

        // team menu
        TeamMenu->w = 0;
        TeamMenu->h = 0;
        TeamMenu->x = 0;
        TeamMenu->y = 0;

        TeamMenu->Button.resize(6);
        InitButton(TeamMenu, 0, L"0 " + _(L"0 Player"), 40, 140 + 40 * 1, 215, 35);
        InitButton(TeamMenu, 1, L"1 " + _(L"Alpha Team"), 40, 140 + 40 * 1, 215, 35);
        InitButton(TeamMenu, 2, L"2 " + _(L"Bravo Team"), 40, 140 + 40 * 2, 215, 35);
        InitButton(TeamMenu, 3, L"3 " + _(L"Charlie Team"), 40, 140 + 40 * 3, 215, 35);
        InitButton(TeamMenu, 4, L"4 " + _(L"Delta Team"), 40, 140 + 40 * 4, 215, 35);
        InitButton(TeamMenu, 5, L"5 " + _(L"Spectator"), 40, 140 + 40 * 5, 215, 35);

        // limbo menu
        LimboMenu->w = 0;
        LimboMenu->h = 0;
        LimboMenu->x = 0;
        LimboMenu->y = 0;

        LimboMenu->Button.resize(MAIN_WEAPONS);

        for (int i = 0; i < MAIN_WEAPONS; i++) {
            std::wstring s;
            if (i < PRIMARY_WEAPONS) {
                s = std::to_wstring((i + 1) % 10) + L" " + std::wstring(GunDisplayName[Guns[i + 1].Num].begin(), GunDisplayName[Guns[i + 1].Num].end());
            } else {
                s = std::wstring(GunDisplayName[Guns[i + 1].Num].begin(), GunDisplayName[Guns[i + 1].Num].end());
            }

            int yPos = 154 + 18 * (i + (i >= PRIMARY_WEAPONS ? 1 : 0));
            InitButton(LimboMenu, i, s, 35, yPos, 235, 16);
        }

        // kick menu
        KickMenu->w = 370;
        KickMenu->h = 90;
        KickMenu->x = 125;
        KickMenu->y = 355;

        KickMenu->Button.resize(4);
        InitButton(KickMenu, 0, L"<<<<",  15, 35, 90, 25);
        InitButton(KickMenu, 1, L">>>>", 265, 35, 90, 25);
        InitButton(KickMenu, 2, _(L"Kick"), 105, 55, 90, 25);
        InitButton(KickMenu, 3, _(L"Ban"), 195, 55, 80, 25);

        KickMenu->Button[3].Active = false;  // TODO: ban not supported for now

        // map menu
        MapMenu->w = 370;
        MapMenu->h = 90;
        MapMenu->x = 125;
        MapMenu->y = 355;

        MapMenu->Button.resize(3);
        InitButton(MapMenu, 0, L"<<<<",  15, 35, 90, 25);
        InitButton(MapMenu, 1, L">>>>", 265, 35, 90, 25);
        InitButton(MapMenu, 2, _(L"Select"), 120, 55, 90, 25);
    }

    inline void HideAll() {
        for (size_t i = 0; i < GameMenu.size(); i++) {
            GameMenu[i].Active = false;
        }
    }

    inline void GameMenuShow(TGameMenu* Menu, bool Show) {
        static bool LimboWasActive = false;
        int i;

        if (Menu == EscMenu) {
            if (Show) {
                if (LimboMenu->Active) {
                    LimboWasActive = true;
                }

                HideAll();
                FragsMenuShow = false;
                StatsMenuShow = false;

                for (i = 1; i <= MAX_PLAYERS; i++) {
                    if (Sprite[i].Active) {
                        StopSound(Sprite[i].ReloadSoundChannel);
                        StopSound(Sprite[i].JetsSoundChannel);
                        StopSound(Sprite[i].GattlingSoundChannel);
                        StopSound(Sprite[i].GattlingSoundChannel2);
                    }
                }

                if (cl_runs.Value() < 3) {
                    NoobShow = true;
                }
            } else {
                HideAll();
                NoobShow = false;
                if (LimboWasActive) {
                    LimboMenu->Active = true;
                }
            }
        } else if ((Menu == TeamMenu) && Show) {
            HideAll();

            if (Show) {
                switch (sv_gamemode.Value()) {
                    case GAMESTYLE_CTF:
                    case GAMESTYLE_INF:
                    case GAMESTYLE_HTF:
                        Menu->Button[0].Active = false;
                        Menu->Button[1].Active = true;
                        Menu->Button[2].Active = true;
                        Menu->Button[3].Active = false;
                        Menu->Button[4].Active = false;
                        break;
                    case GAMESTYLE_TEAMMATCH:
                        Menu->Button[0].Active = false;
                        Menu->Button[1].Active = true;
                        Menu->Button[2].Active = true;
                        Menu->Button[3].Active = true;
                        Menu->Button[4].Active = true;
                        break;
                    default:
                        Menu->Button[0].Active = true;
                        Menu->Button[1].Active = false;
                        Menu->Button[2].Active = false;
                        Menu->Button[3].Active = false;
                        Menu->Button[4].Active = false;
                        break;
                }
            }
        } else if ((Menu == MapMenu) && Show) {
            // ClientVoteMap(MapMenuIndex);
            KickMenu->Active = false;
        } else if ((Menu == KickMenu) && Show) {
            KickMenuIndex = 1;
            MapMenu->Active = false;

            if (PlayersNum < 1) {
                Menu = nullptr;
            }
        } else if (Menu == LimboMenu) {
            Menu->Active = false;

            if (!Show) {
                LimboWasActive = false;
            } else if (WeaponsInGame == 0) {
                Menu = nullptr;
            }
        }

        if (Menu != nullptr) {
            Menu->Active = Show;
        }

        GameMenuMouseMove();
    }

    inline bool GameMenuAction(TGameMenu* Menu, int ButtonIndex) {
        bool result = false;
        int i, count;

        if ((ButtonIndex >= 0) && (ButtonIndex < static_cast<int>(Menu->Button.size())) && 
            Menu->Button[ButtonIndex].Active) {
            
            if (Menu == EscMenu) {
                result = true;

                switch (ButtonIndex) {
                    case 0:  // Exit to menu
                        // ClientDisconnect();
                        // exit(0);  // Equivalent to Halt(0) in Pascal
                        break;
                    case 1:  // Change map
                        GameMenuShow(MapMenu, !MapMenu->Active);
                        break;
                    case 2:  // Kick player
                        GameMenuShow(KickMenu, !KickMenu->Active);
                        break;
                    case 3:  // Change team
                        result = (MySprite > 0) && (MapChangeCounter < 0);

                        if (result) {
                            GameMenuShow(TeamMenu, true);
                            MapChangeCounter = -60;
                            SelTeam = 0;
                        } else if ((MySprite == 0) && IsTeamGame()) {
                            result = true;
                            GameMenuShow(TeamMenu, true);
                        }
                        break;
#ifdef STEAM_CODE
                    case 4:  // Server Website (Steam)
                        // if (sv_website.Value() != "") {
                        //     // SteamAPI.Friends.ActivateGameOverlayToWebPage(PChar(sv_website.Value()), k_EActivateGameOverlayToWebPageMode_Default);
                        // }
                        break;
#endif
                }
            } else if (Menu == TeamMenu) {
                result = true;
                GameMenuShow(TeamMenu, false);
                SelTeam = static_cast<uint8_t>(ButtonIndex);

                if ((MySprite == 0) || (ButtonIndex != Sprite[MySprite].Player.Team)) {
                    // NOTE this actually sends a change team request
                    // ClientSendPlayerInfo();  // Assuming this function exists
                }
            }
            else if (Menu == KickMenu) {
                i = KickMenuIndex;

                if (PlayersNum < 1) {
                    GameMenuShow(KickMenu, false);
                } else {
                    switch (ButtonIndex) {
                        case 0:  // prev
                            do {
                                KickMenuIndex = ((MAX_SPRITES + KickMenuIndex - 2) % MAX_SPRITES) + 1;
                            } while (!(Sprite[KickMenuIndex].Active || Sprite[KickMenuIndex].Player.DemoPlayer));

                            result = (KickMenuIndex != i);
                            break;

                        case 1:  // next
                            do {
                                KickMenuIndex = (KickMenuIndex % MAX_SPRITES) + 1;
                            } while (!(Sprite[KickMenuIndex].Active || Sprite[KickMenuIndex].Player.DemoPlayer));

                            result = (KickMenuIndex != i);
                            break;

                        case 2:  // kick
                            result = (KickMenuIndex != MySprite);

                            if (result) {
                                GameMenuShow(EscMenu, false);
                                ChatText = L" ";
                                ChatChanged = true;
                                VoteKickReasonType = true;
                                // SDL_StartTextInput();  // Assuming SDL functionality exists
                            }
                            break;
                    }
                }
            }
            else if (Menu == MapMenu) {
                if (PlayersNum < 1) {
                    GameMenuShow(KickMenu, false);
                } else {
                    switch (ButtonIndex) {
                        case 0:  // prev
                            if (MapMenuIndex > 0) {
                                MapMenuIndex--;
                                // ClientVoteMap(MapMenuIndex);  // Assuming this function exists
                            }

                            result = (KickMenuIndex != 0);
                            break;

                        case 1:  // next
                            if (MapMenuIndex < VoteMapCount - 1) {
                                MapMenuIndex++;
                                // ClientVoteMap(MapMenuIndex);  // Assuming this function exists
                            }

                            result = (MapMenuIndex <= VoteMapCount - 1);
                            break;

                        case 2:  // vote map
                            GameMenuShow(EscMenu, false);
                            // ClientSendStringMessage('votemap ' + WideString(VoteMapName), MSGTYPE_CMD);  // Assuming this function exists
                            break;
                    }
                }
            }
            else if ((Menu == LimboMenu) && (MySprite > 0)) {
                result = true;
                i = ButtonIndex + 1;

                if ((WeaponActive[i] == 1) && (WeaponSel[MySprite][i] == 1)) {
                    if (i <= 10) {
                        if ((WeaponActive[i] == 1) && (WeaponSel[MySprite][i] == 1)) {
                            Sprite[MySprite].SelWeapon = Guns[i].Num;
                        }

                        if (Sprite[MySprite].SelWeapon > 0) {
                            GameMenuShow(LimboMenu, false);

                            if (!Sprite[MySprite].DeadMeat &&
                                !(Sprite[MySprite].Weapon.Num == Guns[BOW].Num || 
                                  Sprite[MySprite].Weapon.Num == Guns[BOW2].Num)) {
                                // Sprite[MySprite].ApplyWeaponByNum(Sprite[MySprite].SelWeapon, 1);  // Assuming method exists
                                // ClientSpriteSnapshot();  // Assuming this function exists
                            }
                        }
                    } else {
                        // cl_player_secwep.SetValue(i - 11);
                        Sprite[MySprite].Player.SecWep = i - 11;
                        // Sprite[MySprite].ApplyWeaponByNum(Guns[i].Num, 2);  // Assuming method exists

                        count = 0;
                        for (int j = 1; j <= PRIMARY_WEAPONS; j++) {
                            count += WeaponActive[j];
                        }

                        if (count == 0) {
                            GameMenuShow(LimboMenu, false);
                            Sprite[MySprite].Weapon = Sprite[MySprite].SecondaryWeapon;
                            Sprite[MySprite].SecondaryWeapon = Guns[NOWEAPON];
                        }

                        if (!Sprite[MySprite].DeadMeat) {
                            // ClientSpriteSnapshot();  // Assuming this function exists
                        }
                    }
                }
            }

            if (result) {
                // PlaySound(SFX_MENUCLICK);  // Assuming this function exists
            }
        }

        return result;
    }

    inline void GameMenuMouseMove() {
        HoveredMenu = nullptr;
        HoveredButton = nullptr;
        HoveredButtonIndex = 0;

        float x = mx * _rscala.x;
        float y = my * _rscala.y;

        for (size_t i = 0; i < GameMenu.size(); i++) {
            if (GameMenu[i].Active) {
                for (size_t j = 0; j < GameMenu[i].Button.size(); j++) {
                    TGameButton* btn = &GameMenu[i].Button[j];

                    if (btn->Active && (x > btn->x1) && (x < btn->x2) &&
                        (y > btn->y1) && (y < btn->y2)) {
                        HoveredMenu = &GameMenu[i];
                        HoveredButton = btn;
                        HoveredButtonIndex = static_cast<int>(j);
                        return;
                    }
                }
            }
        }
    }

    inline bool GameMenuClick() {
        if (HoveredButton != nullptr) {
            return GameMenuAction(HoveredMenu, HoveredButtonIndex);
        }
        return false;
    }
}

// Using declarations to bring into global namespace
using GameMenusImpl::TGameButton;
using GameMenusImpl::TGameMenu;
using GameMenusImpl::GameMenu;
using GameMenusImpl::HoveredMenu;
using GameMenusImpl::HoveredButton;
using GameMenusImpl::HoveredButtonIndex;
using GameMenusImpl::EscMenu;
using GameMenusImpl::TeamMenu;
using GameMenusImpl::LimboMenu;
using GameMenusImpl::KickMenu;
using GameMenusImpl::MapMenu;
using GameMenusImpl::KickMenuIndex;
using GameMenusImpl::MapMenuIndex;
using GameMenusImpl::InitGameMenus;
using GameMenusImpl::GameMenuShow;
using GameMenusImpl::GameMenuAction;
using GameMenusImpl::GameMenuMouseMove;
using GameMenusImpl::GameMenuClick;

// Global variable definitions
extern std::vector<TGameMenu> GameMenu;
extern TGameMenu* HoveredMenu = nullptr;
extern TGameButton* HoveredButton = nullptr;
extern int HoveredButtonIndex = 0;
extern TGameMenu* EscMenu = nullptr;
extern TGameMenu* TeamMenu = nullptr;
extern TGameMenu* LimboMenu = nullptr;
extern TGameMenu* KickMenu = nullptr;
extern TGameMenu* MapMenu = nullptr;
extern int KickMenuIndex = 0;
extern int MapMenuIndex = 0;

#endif // GAME_MENUS_H