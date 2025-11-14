#ifndef GAME_H
#define GAME_H

//*******************************************************************************
//                                                                              
//       Game Unit for SOLDAT                                                   
//                                                                              
//       Copyright (c) 2012 Gregor A. Cieslak          
//                                                                              
//*******************************************************************************

#include "Vector.h"
#include "Constants.h"
#include "PolyMap.h"
#include "Parts.h"
#include "Sprites.h"
#include "Bullets.h"
#include "Things.h"
#include "Waypoints.h"
#include "Anims.h"
#include "Weapons.h"
#include "Sha1.h"
#include "Util.h"
#include <vector>
#include <string>
#include <memory>

// Structures
struct TKillSort {
    int Kills;
    int Deaths;
    uint8_t Flags;
    int PlayerNum;
    uint32_t Color;
};

// Global variables
extern int Ticks;
extern int TicksPerSecond;
extern int Frames;
extern int FramesPerSecond;
extern int TickTime;
extern int TickTimeLast;
extern int GOALTICKS;

extern int BulletTimeTimer;

// Particle systems
extern ParticleSystem SpriteParts;
extern ParticleSystem BulletParts;
extern ParticleSystem SparkParts;
extern ParticleSystem GostekSkeleton;
extern ParticleSystem BoxSkeleton;
extern ParticleSystem FlagSkeleton;
extern ParticleSystem ParaSkeleton;
extern ParticleSystem StatSkeleton;
extern ParticleSystem RifleSkeleton10;
extern ParticleSystem RifleSkeleton11;
extern ParticleSystem RifleSkeleton18;
extern ParticleSystem RifleSkeleton22;
extern ParticleSystem RifleSkeleton28;
extern ParticleSystem RifleSkeleton36;
extern ParticleSystem RifleSkeleton37;
extern ParticleSystem RifleSkeleton39;
extern ParticleSystem RifleSkeleton43;
extern ParticleSystem RifleSkeleton50;
extern ParticleSystem RifleSkeleton55;

// Animations
extern TAnimation Run;
extern TAnimation Stand;
extern TAnimation RunBack;
extern TAnimation Jump;
extern TAnimation JumpSide;
extern TAnimation Roll;
extern TAnimation RollBack;
extern TAnimation Fall;
extern TAnimation Crouch;
extern TAnimation CrouchRun;
extern TAnimation CrouchRunBack;
extern TAnimation Reload;
extern TAnimation Throw;
extern TAnimation Recoil;
extern TAnimation Shotgun;
extern TAnimation Barret;
extern TAnimation SmallRecoil;
extern TAnimation AimRecoil;
extern TAnimation HandsUpRecoil;
extern TAnimation ClipIn;
extern TAnimation ClipOut;
extern TAnimation SlideBack;
extern TAnimation Change;
extern TAnimation ThrowWeapon;
extern TAnimation WeaponNone;
extern TAnimation Punch;
extern TAnimation ReloadBow;
extern TAnimation Melee;
extern TAnimation Cigar;
extern TAnimation Match;
extern TAnimation Smoke;
extern TAnimation Wipe;
extern TAnimation Groin;
extern TAnimation TakeOff;
extern TAnimation Victory;
extern TAnimation Piss;
extern TAnimation Mercy;
extern TAnimation Mercy2;
extern TAnimation Own;
extern TAnimation Prone;
extern TAnimation GetUp;
extern TAnimation ProneMove;
extern TAnimation Aim;
extern TAnimation HandsUpAim;

#ifndef SERVER_CODE
extern int GameWidth;
extern int GameHeight;
extern float GameWidthHalf;
extern float GameHeightHalf;
#endif

// Ping improvement variables
extern TVector2 OldSpritePos[MAX_SPRITES + 1][MAX_OLDPOS + 1];  // Pascal arrays start from 1

// Survival variables
extern uint8_t AliveNum;
extern uint8_t TeamAliveNum[6];
extern uint8_t TeamPlayersNum[5];
extern bool SurvivalEndRound;
extern bool WeaponsCleaned;

extern int CeaseFireTime;
extern int MapChangeTime;
extern int MapChangeCounter;
extern std::string MapChangeName;
extern TMapInfo MapChange;
extern uint64_t MapChangeItemID;
extern TSHA1Digest MapChangeChecksum;
extern int TimeLimitCounter;
extern int StartHealth;
extern int TimeLeftSec;
extern int TimeLeftMin;
extern uint8_t WeaponSel[MAX_SPRITES + 1][MAIN_WEAPONS + 1];  // Pascal arrays start from 1

extern int TeamScore[6];
extern int TeamFlag[5];

extern float SinusCounter;

extern TPolyMap Map;

extern TSHA1Digest GameModChecksum;
extern TSHA1Digest CustomModChecksum;
extern TSHA1Digest MapCheckSum;

extern int MapIndex;

extern TWaypoints BotPath;

extern TKillSort SortedPlayers[MAX_SPRITES + 1];  // Pascal arrays start from 1
#ifndef SERVER_CODE
extern TKillSort SortedTeamScore[MAX_SPRITES + 1];
extern int HeartbeatTime;
extern int HeartbeatTimeWarnings;
#endif

// Game entities
extern TSprite Sprite[MAX_SPRITES + 1];  // Pascal arrays start from 1
extern TBullet Bullet[MAX_BULLETS + 1];  // Pascal arrays start from 1
#ifndef SERVER_CODE
extern TSpark Spark[MAX_SPARKS + 1];  // Pascal arrays start from 1
#endif
extern TThing Thing[MAX_THINGS + 1];  // Pascal arrays start from 1

// Voting
extern bool VoteActive;
extern uint8_t VoteType;
extern std::string VoteTarget;
extern std::string VoteStarter;
extern std::string VoteReason;
extern int VoteTimeRemaining;
extern uint8_t VoteNumVotes;
extern uint8_t VoteMaxVotes;
extern bool VoteHasVoted[MAX_SPRITES + 1];  // Pascal arrays start from 1
extern int VoteCooldown[MAX_SPRITES + 1];  // Pascal arrays start from 1
extern bool VoteKickReasonType;

// Function declarations
void Number27Timing();
void ToggleBulletTime(bool TurnOn, int Duration = 30);
void UpdateGameStats();
bool PointVisible(float X, float Y, int i);
bool PointVisible2(float X, float Y, int i);
void StartVote(uint8_t StarterVote, uint8_t TypeVote, const std::string& TargetVote, const std::string& ReasonVote);
void StopVote();
void TimerVote();
#ifdef SERVER_CODE
void CountVote(uint8_t Voter);
#endif
void ShowMapChangeScoreboard(); // overload
void ShowMapChangeScoreboard(const std::string& NextMap); // overload
bool IsTeamGame();
#ifndef SERVER_CODE
bool IsPointOnScreen(TVector2 Point);
#endif
void ChangeMap();
void SortPlayers();

namespace GameImpl {
    // Timing variables
    inline uint64_t TimeInMilLast = 0;
    inline uint64_t TimeInMil = 0;
    inline uint32_t TimePassed = 0;
    inline int Seconds = 0;
    inline int SecondsLast = 0;

    inline void Number27Timing() {
        TimeInMilLast = TimeInMil;
        TimeInMil = GetTickCount64();  // Assuming this function exists or is replaced with std::chrono equivalent
        if ((TimeInMil - TimeInMilLast) > 2000) {
            TimeInMilLast = TimeInMil;  // safety precaution
        }

        TimePassed += static_cast<uint32_t>(TimeInMil - TimeInMilLast);
        SecondsLast = Seconds;
        Seconds = static_cast<int>(TimePassed / 1000);

        if (Seconds != SecondsLast) {  // new Second
            TicksPerSecond = Ticks;
            Ticks = 0;

            FramesPerSecond = Frames;
            Frames = 0;
        }

        Frames++;

        TickTimeLast = TickTime;

        double divisor = (1000.0 / GOALTICKS);
        if (divisor != 0) {
            TickTime = static_cast<int>(TimePassed / divisor);
        } else {
            TickTime = 0;
        }
    }

    inline void UpdateGameStats() {
        // Game Stats save
        if (log_enable.Value()) {
            // This would create and save statistics to a file
            // For now, just a placeholder implementation
            // TStringList s;
            // s.Add("In-Game Statistics");
            // s.Add("Players: " + IntToStr(PlayersNum));
            // etc...
            // s.SaveToFile(UserDirectory + "logs/gamestat.txt");
        }
    }

    inline void ToggleBulletTime(bool TurnOn, int Duration) {
#ifdef SERVER_CODE
        // Trace('ToggleBulletTime');
#endif

        if (TurnOn) {
            BulletTimeTimer = Duration;
            GOALTICKS = DEFAULT_GOALTICKS / 3;
        } else {
            GOALTICKS = DEFAULT_GOALTICKS;
        }

        Number27Timing();
    }

#ifndef SERVER_CODE
    inline bool IsPointOnScreen(TVector2 Point) {
        bool result = true;
        float P1 = GameWidthHalf - (CameraX - Point.x);
        float P2 = GameHeightHalf - (CameraY - Point.y);
        if ((P1 < 0) || (P1 > GameWidth)) {
            result = false;
        }
        if ((P2 < 0) || (P2 > GameHeight)) {
            result = false;
        }
        return result;
    }
#endif

    inline bool PointVisible(float X, float Y, int i) {
#ifdef SERVER_CODE
        // TODO: check why numbers differ on server and client
        const int GAME_WIDTH = MAX_GAME_WIDTH;
        const int GAME_HEIGHT = 480;
#else
        // workaround because of variables instead of constants
        int GAME_WIDTH = GameWidth;
        int GAME_HEIGHT = GameHeight;
#endif

        bool result = false;

        if ((i > MAX_PLAYERS) || (i < 1)) {
            return false;
        }

        float SX = SpriteParts.Pos[i].x - ((SpriteParts.Pos[i].x - Sprite[i].Control.MouseAimX) / 2);
        float SY = SpriteParts.Pos[i].y - ((SpriteParts.Pos[i].y - Sprite[i].Control.MouseAimY) / 2);

        if ((X > (SX - GAME_WIDTH)) && (X < (SX + GAME_WIDTH)) &&
            (Y > (SY - GAME_HEIGHT)) && (Y < (SY + GAME_HEIGHT))) {
            result = true;
        }
        return result;
    }

    inline bool PointVisible2(float X, float Y, int i) {
#ifdef SERVER_CODE
        // TODO: check why numbers differ on server and client
        const int GAME_WIDTH = MAX_GAME_WIDTH;
        const int GAME_HEIGHT = 480;
#else
        const int GAME_WIDTH = 600;
        const int GAME_HEIGHT = 440;
#endif

        bool result = false;

        float SX = SpriteParts.Pos[i].x;
        float SY = SpriteParts.Pos[i].y;

        if ((X > (SX - GAME_WIDTH)) && (X < (SX + GAME_WIDTH)) &&
            (Y > (SY - GAME_HEIGHT)) && (Y < (SY + GAME_HEIGHT))) {
            result = true;
        }
        return result;
    }

    inline void StartVote(uint8_t StarterVote, uint8_t TypeVote, const std::string& TargetVote, const std::string& ReasonVote) {
        VoteActive = true;
        if ((StarterVote < 1) || (StarterVote > MAX_PLAYERS)) {
            VoteStarter = "Server";
        } else {
            VoteStarter = std::string(Sprite[StarterVote].Player.Name.begin(), Sprite[StarterVote].Player.Name.end());
            // VoteCooldown[StarterVote] = DEFAULT_VOTE_TIME;
#ifndef SERVER_CODE
            if (StarterVote == MySprite) {
                if (VoteType == VOTE_KICK) {
                    // MainConsole.Console(_("You have voted to kick") + " " +
                    //     WideString(Sprite[KickMenuIndex].Player.Name) + " " + _("from the game"),
                    //     VOTE_MESSAGE_COLOR);
                    VoteActive = false;
                    // ClientVoteKick(StrToInt(TargetVote), True, "");
                }
            }
#endif
        }
        VoteType = TypeVote;
        VoteTarget = TargetVote;
        VoteReason = ReasonVote;
        VoteTimeRemaining = DEFAULT_VOTING_TIME;
        VoteNumVotes = 0;
        VoteMaxVotes = 0;
        for (int i = 1; i <= MAX_PLAYERS; i++) {
            if (Sprite[i].Active) {
                if (Sprite[i].Player.ControlMethod == HUMAN) {
                    VoteMaxVotes++;
                }
            }
        }
    }

    inline void StopVote() {
        VoteActive = false;
        VoteNumVotes = 0;
        VoteMaxVotes = 0;
        VoteType = 0;
        VoteTarget = "";
        VoteStarter = "";
        VoteReason = "";
        VoteTimeRemaining = -1;
        for (int i = 1; i <= MAX_PLAYERS; i++) {
            VoteHasVoted[i] = false;
        }
    }

    inline void TimerVote() {
#ifndef SERVER_CODE
        if (VoteActive) {
#endif
            if (VoteTimeRemaining > -1) {
                VoteTimeRemaining--;
            }

            if (VoteTimeRemaining == 0) {
                if (VoteType == VOTE_MAP) {
                    // MainConsole.Console(_("No map has been voted"), VOTE_MESSAGE_COLOR);
                }
                StopVote();
            }
#ifndef SERVER_CODE
        }
#endif
    }

#ifdef SERVER_CODE
    inline void CountVote(uint8_t Voter) {
        if (VoteActive && !VoteHasVoted[Voter]) {
            VoteNumVotes++;
            VoteHasVoted[Voter] = true;
            float Edge = static_cast<float>(VoteNumVotes) / VoteMaxVotes;
            if (Edge >= (sv_votepercent.Value() / 100.0f)) {
                if (VoteType == VOTE_KICK) {
                    int i = stoi(VoteTarget);
                    // There should be no permanent bans by votes. Reduced to 1 day.
                    // if (CheatTag[i] == 0)
                    //     KickPlayer(i, True, KICK_VOTED, HOUR, "Vote Kicked")
                    // else
                    //     KickPlayer(i, True, KICK_VOTED, DAY, "Vote Kicked by Server");
                    // DoBalanceBots(1, Sprite[i].Player.Team);
                } else if (VoteType == VOTE_MAP) {
                    // if (!PrepareMapChange(VoteTarget)) {
                    //     MainConsole.Console("Map not found (" + VoteTarget + ")", WARNING_MESSAGE_COLOR);
                    //     MainConsole.Console("No map has been voted", VOTE_MESSAGE_COLOR);
                    // }
                }
                StopVote();
                // ServerSendVoteOff();
            }
        }
    }
#endif

    inline void ShowMapChangeScoreboard() {
        ShowMapChangeScoreboard("EXIT*!*");
    }

    inline void ShowMapChangeScoreboard(const std::string& NextMap) {
        MapChangeName = NextMap;
        MapChangeCounter = MapChangeTime;
#ifndef SERVER_CODE
        // GameMenuShow(LimboMenu, False);
        // FragsMenuShow := True;
        // StatsMenuShow := False;
        // for (int i = 1; i <= MAX_PLAYERS; i++) {
        //     if (Sprite[i].Active) {
        //         StopSound(Sprite[i].ReloadSoundChannel);
        //         StopSound(Sprite[i].JetsSoundChannel);
        //         StopSound(Sprite[i].GattlingSoundChannel);
        //         StopSound(Sprite[i].GattlingSoundChannel2);
        //     }
        // }
#endif
    }

    inline bool IsTeamGame() {
        switch (sv_gamemode.Value()) {
            case GAMESTYLE_TEAMMATCH:
            case GAMESTYLE_CTF:
            case GAMESTYLE_INF:
            case GAMESTYLE_HTF:
                return true;
            default:
                return false;
        }
    }

    inline void ChangeMap() {
        // This is a complex function that would handle map changes
        // It includes loading new maps, resetting player states, spawning items, etc.
        // For now I'll provide a skeleton implementation

        // Reset bullets and things
        for (int i = 1; i <= MAX_BULLETS; i++) {
            Bullet[i].Kill();
        }
        for (int i = 1; i <= MAX_THINGS; i++) {
            Thing[i].Kill();
        }
#ifndef SERVER_CODE
        for (int i = 1; i <= MAX_SPARKS; i++) {
            Spark[i].Kill();
        }
#endif

        // Reset sprites
        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i].Active && Sprite[i].IsNotSpectator()) {
                // RandomizeStart(SpriteParts.Pos[i], Sprite[i].Player.Team);
                Sprite[i].Respawn();
                Sprite[i].Player.Kills = 0;
                Sprite[i].Player.Deaths = 0;
                Sprite[i].Player.Flags = 0;
                Sprite[i].BonusTime = 0;
                Sprite[i].BonusStyle = BONUS_NONE;
#ifndef SERVER_CODE
                Sprite[i].SelWeapon = 0;
#endif
                Sprite[i].FreeControls();
                Sprite[i].Weapon = Guns[NOWEAPON];

                int SecWep = Sprite[i].Player.SecWep + 1;

                if ((SecWep >= 1) && (SecWep <= SECONDARY_WEAPONS) &&
                    (WeaponActive[PRIMARY_WEAPONS + SecWep] == 1)) {
                    Sprite[i].SecondaryWeapon = Guns[PRIMARY_WEAPONS + SecWep];
                } else {
                    Sprite[i].SecondaryWeapon = Guns[NOWEAPON];
                }

                Sprite[i].RespawnCounter = 0;
            }
        }

#ifndef SERVER_CODE
        for (int j = 1; j <= MAX_SPRITES; j++) {
            for (int i = 1; i <= PRIMARY_WEAPONS; i++) {
                WeaponSel[j][i] = 1;
            }
        }
#endif

        if (sv_advancemode.Value()) {
#ifndef SERVER_CODE
            for (int j = 1; j <= MAX_SPRITES; j++) {
                for (int i = 1; i <= PRIMARY_WEAPONS; i++) {
                    WeaponSel[j][i] = 0;
                }
            }

            if (MySprite > 0) {
                for (int i = 1; i <= MAIN_WEAPONS; i++) {
                    // LimboMenu.Button[i - 1].Active = Boolean(WeaponSel[MySprite][i]);
                }
            }
#endif
        }

        for (int i = 1; i <= 4; i++) {
            TeamScore[i] = 0;
        }

        for (int i = 1; i <= 2; i++) {
            TeamFlag[i] = 0;
        }

#ifndef SERVER_CODE
        // FragsMenuShow = False;
        // StatsMenuShow = False;

        // if (MySprite > 0) {
        //     GameMenuShow(LimboMenu);
        // }
#endif

        // Reset team alive counts
        for (int i = 1; i <= 4; i++) {
            TeamAliveNum[i] = 0;
            TeamPlayersNum[i] = 0;
        }

        // Sort players
        SortPlayers();

        MapChangeCounter = -60;

        TimeLimitCounter = sv_timelimit.Value();

        // This would also handle server-side specific code for spawning flags/guns, etc.
    }

    inline void SortPlayers() {
        PlayersNum = 0;
        BotsNum = 0;
        SpectatorsNum = 0;
        for (int i = 1; i <= 4; i++) {
            PlayersTeamNum[i] = 0;
        }

        for (int i = 1; i <= MAX_SPRITES; i++) {
            SortedPlayers[i].Kills = 0;
            SortedPlayers[i].Deaths = 0;
            SortedPlayers[i].Flags = 0;
            SortedPlayers[i].PlayerNum = 0;
        }

        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i].Active && (!Sprite[i].Player.DemoPlayer)) {
                PlayersNum++;
                if (Sprite[i].Player.ControlMethod == BOT) {
                    BotsNum++;
                }

                if (Sprite[i].IsSpectator()) {
                    SpectatorsNum++;
                }

                if (Sprite[i].IsNotSolo() && Sprite[i].IsNotSpectator()) {
                    PlayersTeamNum[Sprite[i].Player.Team]++;
                }

                if (Sprite[i].IsNotSpectator()) {
                    SortedPlayers[PlayersNum].Kills = Sprite[i].Player.Kills;
                    SortedPlayers[PlayersNum].Deaths = Sprite[i].Player.Deaths;
                    SortedPlayers[PlayersNum].Flags = Sprite[i].Player.Flags;
                    SortedPlayers[PlayersNum].PlayerNum = i;
                } else {
                    SortedPlayers[PlayersNum].Kills = 0;
                    SortedPlayers[PlayersNum].Deaths = INT_MAX;  // High value for sorting
                    SortedPlayers[PlayersNum].Flags = 0;
                    SortedPlayers[PlayersNum].PlayerNum = i;
                }

                // Kill Limit
                if (MapChangeCounter < 1) {
                    if (!IsTeamGame()) {
                        if (Sprite[i].Player.Kills >= sv_killlimit.Value()) {
#ifndef SERVER_CODE
                            // CameraFollowSprite = i;
                            // if not EscMenu.Active then
                            // begin
                            //   mx := GameWidthHalf;
                            //   my := GameHeightHalf;
                            //   MousePrev.x := mx;
                            //   MousePrev.y := my;
                            // end;
#else
                            // NextMap();
#endif
                        }
                    }
                }
            }
        }

        // sort by flags first if new score board
        if (PlayersNum > 0) {
            for (int i = 1; i <= PlayersNum; i++) {
                for (int j = i + 1; j <= PlayersNum; j++) {
                    if (SortedPlayers[j].Flags > SortedPlayers[i].Flags) {
                        TKillSort temp = SortedPlayers[i];
                        SortedPlayers[i] = SortedPlayers[j];
                        SortedPlayers[j] = temp;
                    }
                }
            }
        }

        // sort by kills
        if (PlayersNum > 0) {
            for (int i = 1; i <= PlayersNum; i++) {
                for (int j = i + 1; j <= PlayersNum; j++) {
                    if (SortedPlayers[j].Flags == SortedPlayers[i].Flags) {
                        if (SortedPlayers[j].Kills > SortedPlayers[i].Kills) {
                            TKillSort temp = SortedPlayers[i];
                            SortedPlayers[i] = SortedPlayers[j];
                            SortedPlayers[j] = temp;
                        }
                    }
                }
            }
        }

        // final sort by deaths
        if (PlayersNum > 0) {
            for (int i = 1; i <= PlayersNum; i++) {
                for (int j = i + 1; j <= PlayersNum; j++) {
                    if (SortedPlayers[j].Flags == SortedPlayers[i].Flags) {
                        if (SortedPlayers[j].Kills == SortedPlayers[i].Kills) {
                            if (SortedPlayers[j].Deaths < SortedPlayers[i].Deaths) {
                                TKillSort temp = SortedPlayers[i];
                                SortedPlayers[i] = SortedPlayers[j];
                                SortedPlayers[j] = temp;
                            }
                        }
                    }
                }
            }
        }

#ifndef SERVER_CODE
        // Sort Team Score
        for (int i = 1; i <= 4; i++) {
            SortedTeamScore[i].Kills = TeamScore[i];
            SortedTeamScore[i].PlayerNum = i;
            // SortedTeamScore[i].Color would be set based on ui_status_transparency
        }

        for (int i = 1; i <= 4; i++) {
            for (int j = i + 1; j <= 4; j++) {
                if (SortedTeamScore[j].Kills > SortedTeamScore[i].Kills) {
                    TKillSort temp = SortedTeamScore[i];
                    SortedTeamScore[i] = SortedTeamScore[j];
                    SortedTeamScore[j] = temp;
                }
            }
        }
#endif

#ifdef SERVER_CODE
        // Team - Kill Limit
        if (MapChangeCounter < 1) {
            for (int i = 1; i <= 4; i++) {
                if (TeamScore[i] >= sv_killlimit.Value()) {
                    // NextMap();
                    break;
                }
            }
        }
        // UpdateWaveRespawnTime();  // Assuming this function exists
#endif

        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i].Active) {
#ifdef SERVER_CODE
                if (Sprite[i].Active && (Sprite[i].Player.Team == TEAM_ALPHA)) {
                    TeamAliveNum[TEAM_ALPHA]++;
                }
                if (Sprite[i].Active && (Sprite[i].Player.Team == TEAM_BRAVO)) {
                    TeamAliveNum[TEAM_BRAVO]++;
                }
                if (Sprite[i].Active && (Sprite[i].Player.Team == TEAM_CHARLIE)) {
                    TeamAliveNum[TEAM_CHARLIE]++;
                }
                if (Sprite[i].Active && (Sprite[i].Player.Team == TEAM_DELTA)) {
                    TeamAliveNum[TEAM_DELTA]++;
                }
#else
                if (Sprite[i].Player.Team == TEAM_NONE) {
                    TeamPlayersNum[TEAM_NONE]++;
                }
                if (Sprite[i].Player.Team == TEAM_ALPHA) {
                    TeamPlayersNum[TEAM_ALPHA]++;
                }
                if (Sprite[i].Player.Team == TEAM_BRAVO) {
                    TeamPlayersNum[TEAM_BRAVO]++;
                }
                if (Sprite[i].Player.Team == TEAM_CHARLIE) {
                    TeamPlayersNum[TEAM_CHARLIE]++;
                }
                if (Sprite[i].Player.Team == TEAM_DELTA) {
                    TeamPlayersNum[TEAM_DELTA]++;
                }
#endif
            }
        }
    }
}

// Using declarations to bring into global namespace
using GameImpl::TKillSort;
using GameImpl::Ticks;
using GameImpl::TicksPerSecond;
using GameImpl::Frames;
using GameImpl::FramesPerSecond;
using GameImpl::TickTime;
using GameImpl::TickTimeLast;
using GameImpl::GOALTICKS;
using GameImpl::BulletTimeTimer;
using GameImpl::SpriteParts;
using GameImpl::BulletParts;
using GameImpl::SparkParts;
using GameImpl::GostekSkeleton;
using GameImpl::BoxSkeleton;
using GameImpl::FlagSkeleton;
using GameImpl::ParaSkeleton;
using GameImpl::StatSkeleton;
using GameImpl::RifleSkeleton10;
using GameImpl::RifleSkeleton11;
using GameImpl::RifleSkeleton18;
using GameImpl::RifleSkeleton22;
using GameImpl::RifleSkeleton28;
using GameImpl::RifleSkeleton36;
using GameImpl::RifleSkeleton37;
using GameImpl::RifleSkeleton39;
using GameImpl::RifleSkeleton43;
using GameImpl::RifleSkeleton50;
using GameImpl::RifleSkeleton55;
using GameImpl::Run;
using GameImpl::Stand;
using GameImpl::RunBack;
using GameImpl::Jump;
using GameImpl::JumpSide;
using GameImpl::Roll;
using GameImpl::RollBack;
using GameImpl::Fall;
using GameImpl::Crouch;
using GameImpl::CrouchRun;
using GameImpl::CrouchRunBack;
using GameImpl::Reload;
using GameImpl::Throw;
using GameImpl::Recoil;
using GameImpl::Shotgun;
using GameImpl::Barret;
using GameImpl::SmallRecoil;
using GameImpl::AimRecoil;
using GameImpl::HandsUpRecoil;
using GameImpl::ClipIn;
using GameImpl::ClipOut;
using GameImpl::SlideBack;
using GameImpl::Change;
using GameImpl::ThrowWeapon;
using GameImpl::WeaponNone;
using GameImpl::Punch;
using GameImpl::ReloadBow;
using GameImpl::Melee;
using GameImpl::Cigar;
using GameImpl::Match;
using GameImpl::Smoke;
using GameImpl::Wipe;
using GameImpl::Groin;
using GameImpl::TakeOff;
using GameImpl::Victory;
using GameImpl::Piss;
using GameImpl::Mercy;
using GameImpl::Mercy2;
using GameImpl::Own;
using GameImpl::Prone;
using GameImpl::GetUp;
using GameImpl::ProneMove;
using GameImpl::Aim;
using GameImpl::HandsUpAim;
using GameImpl::GameWidth;
using GameImpl::GameHeight;
using GameImpl::GameWidthHalf;
using GameImpl::GameHeightHalf;
using GameImpl::OldSpritePos;
using GameImpl::AliveNum;
using GameImpl::TeamAliveNum;
using GameImpl::TeamPlayersNum;
using GameImpl::SurvivalEndRound;
using GameImpl::WeaponsCleaned;
using GameImpl::CeaseFireTime;
using GameImpl::MapChangeTime;
using GameImpl::MapChangeCounter;
using GameImpl::MapChangeName;
using GameImpl::MapChange;
using GameImpl::MapChangeItemID;
using GameImpl::MapChangeChecksum;
using GameImpl::TimeLimitCounter;
using GameImpl::StartHealth;
using GameImpl::TimeLeftSec;
using GameImpl::TimeLeftMin;
using GameImpl::WeaponSel;
using GameImpl::TeamScore;
using GameImpl::TeamFlag;
using GameImpl::SinusCounter;
using GameImpl::Map;
using GameImpl::GameModChecksum;
using GameImpl::CustomModChecksum;
using GameImpl::MapCheckSum;
using GameImpl::MapIndex;
using GameImpl::BotPath;
using GameImpl::SortedPlayers;
using GameImpl::SortedTeamScore;
using GameImpl::HeartbeatTime;
using GameImpl::HeartbeatTimeWarnings;
using GameImpl::Sprite;
using GameImpl::Bullet;
using GameImpl::Spark;
using GameImpl::Thing;
using GameImpl::VoteActive;
using GameImpl::VoteType;
using GameImpl::VoteTarget;
using GameImpl::VoteStarter;
using GameImpl::VoteReason;
using GameImpl::VoteTimeRemaining;
using GameImpl::VoteNumVotes;
using GameImpl::VoteMaxVotes;
using GameImpl::VoteHasVoted;
using GameImpl::VoteCooldown;
using GameImpl::VoteKickReasonType;
using GameImpl::Number27Timing;
using GameImpl::ToggleBulletTime;
using GameImpl::UpdateGameStats;
using GameImpl::PointVisible;
using GameImpl::PointVisible2;
using GameImpl::StartVote;
using GameImpl::StopVote;
using GameImpl::TimerVote;
using GameImpl::CountVote;
using GameImpl::ShowMapChangeScoreboard;
using GameImpl::IsTeamGame;
using GameImpl::IsPointOnScreen;
using GameImpl::ChangeMap;
using GameImpl::SortPlayers;

// Initialize global variables
namespace GameImpl {
    inline int Ticks = 0;
    inline int TicksPerSecond = 0;
    inline int Frames = 0;
    inline int FramesPerSecond = 0;
    inline int TickTime = 0;
    inline int TickTimeLast = 0;
    inline int GOALTICKS = DEFAULT_GOALTICKS;

    inline int BulletTimeTimer = 0;

    // Initialize arrays and structures
    inline uint8_t AliveNum = 0;
    inline uint8_t TeamAliveNum[6] = {0};
    inline uint8_t TeamPlayersNum[5] = {0};  // Team 0-4
    inline bool SurvivalEndRound = false;
    inline bool WeaponsCleaned = false;

    inline int CeaseFireTime = DEFAULT_CEASEFIRE_TIME;
    inline int MapChangeTime = DEFAULT_MAPCHANGE_TIME;
    inline int MapChangeCounter = 0;
    inline std::string MapChangeName = "";
    inline TMapInfo MapChange = {};  // Default initialization
    inline uint64_t MapChangeItemID = 0;
    inline TSHA1Digest MapChangeChecksum = {};  // Default initialization
    inline int TimeLimitCounter = 3600;
    inline int StartHealth = 150;
    inline int TimeLeftSec = 0;
    inline int TimeLeftMin = 0;
    inline uint8_t WeaponSel[MAX_SPRITES + 1][MAIN_WEAPONS + 1] = {{0}};  // Init to 0

    inline int TeamScore[6] = {0};  // Teams 0-5
    inline int TeamFlag[5] = {0};   // Teams 0-4

    inline float SinusCounter = 0.0f;

    // Initialize game objects
    inline TPolyMap Map;  // Assuming default constructor

    inline TSHA1Digest GameModChecksum = {};      // Default initialization
    inline TSHA1Digest CustomModChecksum = {};    // Default initialization
    inline TSHA1Digest MapCheckSum = {};          // Default initialization

    inline int MapIndex = 0;

    inline TWaypoints BotPath;  // Assuming default constructor

    inline TKillSort SortedPlayers[MAX_SPRITES + 1] = {};  // Init to 0

#ifndef SERVER_CODE
    inline TKillSort SortedTeamScore[MAX_SPRITES + 1] = {};  // Init to 0
    inline int HeartbeatTime = 0;
    inline int HeartbeatTimeWarnings = 0;
#endif

    // Initialize game entities
    inline TSprite Sprite[MAX_SPRITES + 1] = {};  // Init to default values
    inline TBullet Bullet[MAX_BULLETS + 1] = {};  // Init to default values
#ifndef SERVER_CODE
    inline TSpark Spark[MAX_SPARKS + 1] = {};     // Init to default values
#endif
    inline TThing Thing[MAX_THINGS + 1] = {};    // Init to default values

    // Voting system
    inline bool VoteActive = false;
    inline uint8_t VoteType = 0;  // VOTE_MAP or VOTE_KICK
    inline std::string VoteTarget = "";
    inline std::string VoteStarter = "";
    inline std::string VoteReason = "";
    inline int VoteTimeRemaining = -1;
    inline uint8_t VoteNumVotes = 0;
    inline uint8_t VoteMaxVotes = 0;
    inline bool VoteHasVoted[MAX_SPRITES + 1] = {false};  // Init to false
    inline int VoteCooldown[MAX_SPRITES + 1] = {0};       // Init to 0
    inline bool VoteKickReasonType = false;

    // Animation objects - would need proper initialization
    inline TAnimation Run;
    inline TAnimation Stand;
    inline TAnimation RunBack;
    inline TAnimation Jump;
    inline TAnimation JumpSide;
    inline TAnimation Roll;
    inline TAnimation RollBack;
    inline TAnimation Fall;
    inline TAnimation Crouch;
    inline TAnimation CrouchRun;
    inline TAnimation CrouchRunBack;
    inline TAnimation Reload;
    inline TAnimation Throw;
    inline TAnimation Recoil;
    inline TAnimation Shotgun;
    inline TAnimation Barret;
    inline TAnimation SmallRecoil;
    inline TAnimation AimRecoil;
    inline TAnimation HandsUpRecoil;
    inline TAnimation ClipIn;
    inline TAnimation ClipOut;
    inline TAnimation SlideBack;
    inline TAnimation Change;
    inline TAnimation ThrowWeapon;
    inline TAnimation WeaponNone;
    inline TAnimation Punch;
    inline TAnimation ReloadBow;
    inline TAnimation Melee;
    inline TAnimation Cigar;
    inline TAnimation Match;
    inline TAnimation Smoke;
    inline TAnimation Wipe;
    inline TAnimation Groin;
    inline TAnimation TakeOff;
    inline TAnimation Victory;
    inline TAnimation Piss;
    inline TAnimation Mercy;
    inline TAnimation Mercy2;
    inline TAnimation Own;
    inline TAnimation Prone;
    inline TAnimation GetUp;
    inline TAnimation ProneMove;
    inline TAnimation Aim;
    inline TAnimation HandsUpAim;

    // Particle systems - would need proper initialization
    inline ParticleSystem SpriteParts;
    inline ParticleSystem BulletParts;
    inline ParticleSystem SparkParts;
    inline ParticleSystem GostekSkeleton;
    inline ParticleSystem BoxSkeleton;
    inline ParticleSystem FlagSkeleton;
    inline ParticleSystem ParaSkeleton;
    inline ParticleSystem StatSkeleton;
    inline ParticleSystem RifleSkeleton10;
    inline ParticleSystem RifleSkeleton11;
    inline ParticleSystem RifleSkeleton18;
    inline ParticleSystem RifleSkeleton22;
    inline ParticleSystem RifleSkeleton28;
    inline ParticleSystem RifleSkeleton36;
    inline ParticleSystem RifleSkeleton37;
    inline ParticleSystem RifleSkeleton39;
    inline ParticleSystem RifleSkeleton43;
    inline ParticleSystem RifleSkeleton50;
    inline ParticleSystem RifleSkeleton55;

#ifndef SERVER_CODE
    inline int GameWidth = DEFAULT_WIDTH;
    inline int GameHeight = DEFAULT_HEIGHT;
    inline float GameWidthHalf = DEFAULT_WIDTH / 2.0f;
    inline float GameHeightHalf = DEFAULT_WIDTH / 2.0f;
#endif

    // Initialize the old sprite position array
    inline TVector2 OldSpritePos[MAX_SPRITES + 1][MAX_OLDPOS + 1] = {{{0, 0}}};  // Init to 0
}

#endif // GAME_H