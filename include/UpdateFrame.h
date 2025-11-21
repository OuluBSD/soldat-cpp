#ifndef UPDATEFRAME_H
#define UPDATEFRAME_H

//*******************************************************************************
//                                                                              
//       UpdateFrame Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2003 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Sound.h"
#include "Demo.h"
#include "GameStrings.h"
#include "GameRendering.h"
#include "Sprites.h"
#include "Vector.h"
#include "Weapons.h"
#include "Net.h"
#include "NetworkClientConnection.h"
#include "Constants.h"

#include "PolyMap.h"
#include "Game.h"
#include "Client.h"
#include "Util.h"
#include "Calc.h"
#include "LogFile.h"
#include "WeatherEffects.h"
#include "Sparks.h"
#include "ClientGame.h"
#include "InterfaceGraphics.h"
#include "GameMenus.h"
#include "Input.h"
#include <string>

// Function declaration
void Update_Frame();

// External variable declarations
extern int SparksCount;
extern int ShotDistanceShow;
extern int ChatTimeCounter;
extern int IdleCounter;
extern int OldMouseX;
extern int ScreenCounter;
extern uint8_t CameraFollowSprite;
extern float CameraX, CameraY;
extern float mx, my;
extern TVector2 CameraPrev;
extern TVector2 MousePrev;
extern int MainTickCounter;
extern uint8_t MapChangeCounter;
extern std::string MapChangeName;
extern int TimeLimitCounter;
extern float ShotDistance;
extern uint16_t HitSprayCounter;
extern bool ScreenTaken;
extern bool TargetMode;
extern bool SurvivalEndRound;
extern bool DemoPlayer;
extern bool DemoRecorder;
extern int PlayersNum;
extern int MySprite;
extern int MapChangeTime;
extern std::vector<std::unique_ptr<struct TSprite>> Sprite;
extern std::vector<std::unique_ptr<struct TBullet>> Bullet;
extern std::vector<std::unique_ptr<struct TSpark>> Spark;
extern std::vector<std::unique_ptr<struct TThing>> Thing;
extern TMapInfo Map;
extern int GrenadeEffectTimer;
extern bool MuteAll;
extern TGameMenu* EscMenu;
extern TConsole MainConsole;
extern TConsole KillConsole;
extern std::wstring ChatMessage[MAX_SPRITES + 1];
extern bool ChatTeam[MAX_SPRITES + 1];
extern int ChatDelay[MAX_SPRITES + 1];
extern int BigDelay[MAX_BIG_MESSAGES + 1];
extern int WorldDelay[MAX_BIG_MESSAGES + 1];
extern bool ScreenTaken;
extern std::string ConsoleLogFileName;
extern std::string UserDirectory;

namespace UpdateFrameImpl {

    // Internal variables
    extern int IdleCounter;
    extern int OldMouseX;

    inline void Update_Frame() {
        CheckSynchronize();

        CameraPrev.x = CameraX;
        CameraPrev.y = CameraY;
        MousePrev.x = mx;
        MousePrev.y = my;

        if (MapChangeCounter < 0) {
            if (DemoPlayer && DemoPlayer->Active() && EscMenu && EscMenu->Active) {
                return;
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j] && Sprite[j]->Active) {
                    if (Sprite[j]->IsNotSpectator()) {
                        if (ClientStopMovingCounter > 0) {
                            SpriteParts.DoEulerTimeStepFor(j);  // integrate sprite particles
                        }
                    }
                }
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j] && Sprite[j]->Active) {
                    Sprite[j]->Update();  // update sprite
                }
            }

            // Bullets update
            for (int j = 1; j <= MAX_BULLETS; j++) {
                if (Bullet[j] && Bullet[j]->Active) {
                    Bullet[j]->Update();
                }

                if (Bullet[j] && Bullet[j]->PingAdd > 0) {
                    Bullet[j]->PingAdd -= 4;
                }
            }

            BulletParts.DoEulerTimeStep();

            SparksCount = 0;
            for (int j = 1; j <= MAX_SPARKS; j++) {
                if (Spark[j] && Spark[j]->Active) {
                    Spark[j]->Update();
                    SparksCount++;
                }
            }

            // update Things
            for (int j = 1; j <= MAX_THINGS; j++) {
                if (Thing[j] && Thing[j]->Active) {
                    Thing[j]->Update();
                }
            }

            if (MainTickCounter % SECOND == 0) {
                if (ScreenCounter != 255) {
                    // TODO: don't rely on underflow
                    ScreenCounter = 0xFF & (ScreenCounter - 1);
                }
            }

            // Change spectate target away from dead player
            if (MainTickCounter % (SECOND * 5) == 0) {
                if ((CameraFollowSprite > 0) && Sprite[CameraFollowSprite] && 
                    Sprite[CameraFollowSprite]->DeadMeat &&
                    (sv_realisticmode.Value()) && (sv_survivalmode.Value()) && !SurvivalEndRound) {
                    CameraFollowSprite = GetCameraTarget();
                }
            }

            // Weather effects
            if (r_weathereffects.Value()) {
                switch (Map.Weather) {
                    case 1: MakeRain(); break;
                    case 2: MakeSandStorm(); break;
                    case 3: MakeSnow(); break;
                }
            }
        }  // mapchangecounter < 0
        else {
            // allow camera switching in demos while paused
            //if DemoPlay then
            //  for j := 1 to MAX_SPRITES do
            //   if Sprite[j] && Sprite[j]->Active then
            //    ControlSprite(Sprite[j]);
        }

        // >> cursor on player <<
        CursorText = "";
        CursorFriendly = false;

        // TODO(helloer): While watching demos this code needs to use SpectNumber instead of MySprite
        if ((MySprite > 0) && (!DemoPlayer || !DemoPlayer->Active())) {
            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j] && Sprite[j]->Active && Sprite[j]->IsNotSpectator() &&
                    (j != MySprite) && (Sprite[j]->BonusStyle != BONUS_PREDATOR) &&
                    ((Sprite[j]->Position == POS_STAND) ||
                    (Sprite[j]->IsNotSolo() && Sprite[j]->IsInSameTeam(Sprite[MySprite].get())) ||
                    (Sprite[MySprite] && Sprite[MySprite]->DeadMeat) || (Sprite[j] && Sprite[j]->DeadMeat)) &&
                    ((Sprite[j]->Visible > 40) || (!sv_realisticmode.Value()))) {
                    
                    if (Distance(-GameWidthHalf + CameraX + mx, -GameHeightHalf + CameraY + my,
                        SpriteParts.Pos[j].x, SpriteParts.Pos[j].y) <
                        CURSORSPRITE_DISTANCE) {
                        
                        CursorText = Sprite[j]->Player->Name;
                        if (IsTeamGame()) {
                            if (Sprite[j]->IsInSameTeam(Sprite[MySprite].get())) {
                                CursorText = CursorText + " " +
                                    std::to_string(static_cast<int>(round((Sprite[j]->Health / STARTHEALTH) * 100))) + "%";
                                CursorFriendly = true;
                            }
                        }

                        break;
                    }
                }
            }
        }
        CursorTextLength = static_cast<int>(CursorText.length());

        // bullet timer
        if (BulletTimeTimer > -1) {
            BulletTimeTimer--;
        }

        if (BulletTimeTimer == 0) {
            ToggleBulletTime(false);
            BulletTimeTimer = -1;
        }
        else if (BulletTimeTimer < 1) {
            // MapChange counter update
            if ((MapChangeCounter > -60) && (MapChangeCounter < 99999999)) {
                MapChangeCounter = MapChangeCounter - 1;
            }
            if ((MapChangeCounter < 0) && (MapChangeCounter > -59)) {
                if (MapChangeName != "EXIT*!*") {
                    ChangeMap();
                    ResetWeaponStats();
                }
            }

            // Game Stats save
            if (MainTickCounter % log_filesupdate.Value == 0) {
                if (log_enable.Value) {
                    if (CheckFileSize(ConsoleLogFileName) > MAX_LOGFILESIZE) {
                        NewLogFiles();
                    }

                    WriteLogFile(GameLog, ConsoleLogFileName);
                }
            }

            if (MainTickCounter % (SECOND * 6) == 0) {
                if (PlayersNum == 0) {
                    if (MapChangeCounter > 99999999) {
                        MapChangeCounter = -60;
                    }
                }
            }

            SinusCounter += ILUMINATESPEED;

            if (GrenadeEffectTimer > -1) {
                GrenadeEffectTimer--;
            }

            // Spray counter
            if (HitSprayCounter > 0) {
                HitSprayCounter--;
            }

            // Idle counter
            if (MySprite > 0) {
                if (MapChangeCounter < 99999999) {
                    if (Sprite[MySprite] && Sprite[MySprite]->IsNotSpectator() &&
                            (!Sprite[MySprite]->Player || !Sprite[MySprite]->Player->DemoPlayer)) {
                        if (OldMouseX - static_cast<int>(round(mx)) == 0) {
                            IdleCounter++;
                        } else {
                            IdleCounter = 0;
                        }

                        if (IdleCounter > IDLE_KICK) {
                            ClientDisconnect();
                            ExitToMenu();
                        }

                        OldMouseX = static_cast<int>(round(mx));
                    }
                }
            }

            // Time Limit decrease
            if ((MapChangeCounter < 99999999)) {
                if (TimeLimitCounter > 0) {
                    TimeLimitCounter--;
                }
            }

            TimeLeftMin = TimeLimitCounter / 3600;
            TimeLeftSec = (TimeLimitCounter - TimeLeftMin * 3600) / 60;

            if (TimeLimitCounter > 0) {
                if (TimeLimitCounter < 601) {
                    if (TimeLimitCounter % 60 == 0) {
                        if (MapChangeCounter == -60) {
                            MainConsole.Console(_("Time Left:") + " " +
                                WideString(std::to_string(TimeLimitCounter / 60)) + " " + _("seconds"),
                                    GAME_MESSAGE_COLOR);
                            PlaySound(SFX_SIGNAL);
                        }
                    }
                }
                else if (TimeLimitCounter < 3601) {
                    if (TimeLimitCounter % 600 == 0) {
                        MainConsole.Console(_("Time Left:") + " " +
                            WideString(std::to_string(TimeLimitCounter / 60)) + " " + _("seconds"),
                                GAME_MESSAGE_COLOR);
                        PlaySound(SFX_SIGNAL);
                    }
                }
                else if (TimeLimitCounter < 18001) {
                    if (TimeLimitCounter % 3600 == 0) {
                        MainConsole.Console(_("Time Left:") + " " +
                            WideString(std::to_string(TimeLimitCounter / 3600)) + " " + _("minutes"),
                                GAME_MESSAGE_COLOR);
                        PlaySound(SFX_SIGNAL);
                    }
                }
                else if (TimeLimitCounter % 18000 == 0) {
                    MainConsole.Console(_("Time Left:") + " " +
                        WideString(std::to_string(TimeLimitCounter / 3600)) + " " + _("minutes"),
                            GAME_MESSAGE_COLOR);
                    PlaySound(SFX_SIGNAL);
                }
            }

            // Map voting timer
            TimerVote();

            // Chat Update
            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (ChatDelay[j] > 0) {
                    ChatDelay[j]--;
                }
            }

            // Big and World Message update
            for (int j = 0; j <= MAX_BIG_MESSAGES; j++) {
                if (BigDelay[j] > 0) {
                    BigDelay[j]--;
                }
                if (WorldDelay[j] > 0) {
                    WorldDelay[j]--;
                }
            }

            // Shot dist update
            if (ShotDistanceShow > 0) {
                ShotDistanceShow--;
            }

            // Consoles Update
            MainConsole.ScrollTick++;
            if (MainConsole.ScrollTick == MainConsole.ScrollTickMax) {
                MainConsole.ScrollConsole();
            }

            if (MainConsole.AlphaCount > 0) {
                MainConsole.AlphaCount--;
            }

            KillConsole.ScrollTick++;
            if (KillConsole.ScrollTick == KillConsole.ScrollTickMax) {
                KillConsole.ScrollConsole();
                if ((KillConsole.Count > 0) &&
                    (KillConsole.NumMessage[KillConsole.Count] == -255)) {
                    KillConsole.ScrollConsole();
                }
            }

            if (ChatTimeCounter > 0) {
                ChatTimeCounter--;
            }
        }  // bullettime off

        // MOVE -=CAMERA=-
        if ((CameraFollowSprite > 0) && (CameraFollowSprite < MAX_SPRITES + 1)) {
            if (Sprite[CameraFollowSprite] && Sprite[CameraFollowSprite]->Active && 
                Sprite[CameraFollowSprite]->IsNotSpectator()) {
                // FIXME(skoskav): Scope zoom and non-default resolution makes this a bit complicated. Why
                // does the magic number ~6.8 work so well?

                TVector2 M;
                M.X = expf(r_zoom.Value()) * ((mx - GameWidthHalf) / Sprite[CameraFollowSprite]->AimDistCoef *
                    ((2 * 640 / GameWidth - 1) +
                    (GameWidth - 640) / GameWidth * (DEFAULTAIMDIST - Sprite[CameraFollowSprite]->AimDistCoef) / 6.8f));

                M.Y = expf(r_zoom.Value()) * ((my - GameHeightHalf) / Sprite[CameraFollowSprite]->AimDistCoef);
                
                TVector2 CamV = {CameraX, CameraY};
                TVector2 P = {SpriteParts.Pos[CameraFollowSprite].X, SpriteParts.Pos[CameraFollowSprite].Y};
                TVector2 Norm = Vec2Subtract(P, CamV);
                TVector2 S;
                Vec2Scale(S, Norm, CAMSPEED);
                CamV = Vec2Add(CamV, S);
                CamV = Vec2Add(CamV, M);
                CameraX = CamV.X;
                CameraY = CamV.Y;
            }
            else {
                CameraFollowSprite = 0;
            }
        }
        else if (CameraFollowSprite == 0) {
            float DisplayRatio = static_cast<float>(GameWidth) / 640;

            TVector2 M;
            if ((mx > 310 * DisplayRatio) && (mx < 330 * DisplayRatio) &&
                (my > 230) && (my < 250)) {
                M.X = 0;
                M.Y = 0;
            }
            else {
                M.X = (mx - GameWidthHalf) / SPECTATORAIMDIST;
                M.Y = (my - GameHeightHalf) / SPECTATORAIMDIST;
            }
            TVector2 CamV = {CameraX, CameraY};
            CamV = Vec2Add(CamV, M);
            CameraX = CamV.X;
            CameraY = CamV.Y;
        }

        // safety
        if ((MySprite > 0) && (Sprite[MySprite] && Sprite[MySprite]->IsSpectator())) {
            if ((CameraX > MAX_SECTORZ * Map.SectorsDivision) ||
                (CameraX < MIN_SECTORZ * Map.SectorsDivision) ||
                (CameraY > MAX_SECTORZ * Map.SectorsDivision) ||
                (CameraY < MIN_SECTORZ * Map.SectorsDivision)) {
                CameraX = 0;
                CameraY = 0;
                TargetMode = false;
            }
        }

        // end game screen
        if (ScreenTaken) {
            if (MapChangeCounter < (DEFAULT_MAPCHANGE_TIME / 3)) {
                ScreenTaken = false;
                
                std::string ScreenFile = UserDirectory + "screens/" +
                    FormatDateTime("yyyy-mm-dd_hh-nn-ss_", Now()) + Map.Name +
                    "_endgame.png";
                TakeScreenShot(ScreenFile);
            }
        }

        if ((demo_autorecord.Value()) && (!DemoRecorder || !DemoRecorder->Active()) && (Map.Name != "")) {
            if (DemoRecorder) {
                DemoRecorder->StartRecord(UserDirectory + "demos/" +
                    FormatDateTime("yyyy-mm-dd_hh-nn-ss_", Now()) + Map.Name + ".sdm");
            }
        }
    }

} // namespace UpdateFrameImpl

using UpdateFrameImpl::Update_Frame;

#endif // UPDATEFRAME_H