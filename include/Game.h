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
#include "Cvar.h"  // For cvar access like log_enable, sv_gamemode, etc.
#include <vector>
#include <string>
#include <memory>
#include <climits>  // for INT_MAX
#include <chrono>   // for time functions

// Structures
struct TKillSort {
    int Kills;
    int Deaths;
    uint8_t Flags;
    int PlayerNum;
    uint32_t Color;
};

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
extern TParticleSystem SpriteParts;
extern TParticleSystem BulletParts;
extern TParticleSystem SparkParts;
extern TParticleSystem GostekSkeleton;
extern TParticleSystem BoxSkeleton;
extern TParticleSystem FlagSkeleton;
extern TParticleSystem ParaSkeleton;
extern TParticleSystem StatSkeleton;
extern TParticleSystem RifleSkeleton10;
extern TParticleSystem RifleSkeleton11;
extern TParticleSystem RifleSkeleton18;
extern TParticleSystem RifleSkeleton22;
extern TParticleSystem RifleSkeleton28;
extern TParticleSystem RifleSkeleton36;
extern TParticleSystem RifleSkeleton37;
extern TParticleSystem RifleSkeleton39;
extern TParticleSystem RifleSkeleton43;
extern TParticleSystem RifleSkeleton50;
extern TParticleSystem RifleSkeleton55;

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

namespace GameImpl {
    using ::Sprite;  // Make global Sprite vector accessible in this namespace
    using ::SpriteParts;  // Make global SpriteParts accessible in this namespace
    using ::sv_votepercent;  // Make cvar accessible
    using ::sv_killlimit;  // Make cvar accessible
    using ::sv_timelimit;  // Make cvar accessible
    using ::sv_advancemode;  // Make cvar accessible
    using ::sv_gamemode;  // Make cvar accessible
    using ::PlayersNum;  // Make global variable accessible
    using ::BotsNum;  // Make global variable accessible
    using ::SpectatorsNum;  // Make global variable accessible
    using ::PlayersTeamNum;  // Make global array accessible
    using ::MySprite;  // Make global variable accessible
    using ::TeamScore;  // Make global array accessible
    using ::WeaponSel;  // Make global 2D array accessible
    using ::MapChangeName;  // Make global variable accessible
    using ::MapChangeCounter;  // Make global variable accessible
    using ::MapChangeTime;  // Make global variable accessible
    using ::TimeLimitCounter;  // Make global variable accessible
    using ::TeamAliveNum;  // Make global array accessible
    using ::TeamPlayersNum;  // Make global array accessible
    using ::VoteActive;  // Make global variable accessible
    using ::VoteType;  // Make global variable accessible
    using ::VoteTarget;  // Make global variable accessible
    using ::VoteStarter;  // Make global variable accessible
    using ::VoteReason;  // Make global variable accessible
    using ::VoteTimeRemaining;  // Make global variable accessible
    using ::VoteNumVotes;  // Make global variable accessible
    using ::VoteMaxVotes;  // Make global variable accessible
    using ::VoteHasVoted;  // Make global array accessible
    using ::Map;  // Make global variable accessible
    using ::SortedPlayers;  // Make global array accessible
    using ::SortedTeamScore;  // Make global array accessible
    using ::GameWidth;  // Make global variable accessible
    using ::GameHeight;  // Make global variable accessible
    using ::GameWidthHalf;  // Make global variable accessible
    using ::GameHeightHalf;  // Make global variable accessible
    using ::DEFAULT_VOTING_TIME;  // Make constant accessible
    using ::DEFAULT_GOALTICKS;  // Make constant accessible
    using ::MAX_PLAYERS;  // Make constant accessible
    using ::MAX_SPRITES;  // Make constant accessible
    using ::MAX_BULLETS;  // Make constant accessible
    using ::MAX_THINGS;  // Make constant accessible
    using ::MAX_SPARKS;  // Make constant accessible
    using ::DEFAULT_CEASEFIRE_TIME;  // Make constant accessible
    using ::DEFAULT_MAPCHANGE_TIME;  // Make constant accessible
    using ::MAIN_WEAPONS;  // Make constant accessible
    using ::PRIMARY_WEAPONS;  // Make constant accessible
    using ::SECONDARY_WEAPONS;  // Make constant accessible
    using ::DEFAULT_WIDTH;  // Make constant accessible
    using ::DEFAULT_HEIGHT;  // Make constant accessible
    using ::MAX_GAME_WIDTH;  // Make constant accessible
    using ::TEAM_ALPHA;  // Make constant accessible
    using ::TEAM_BRAVO;  // Make constant accessible
    using ::TEAM_CHARLIE;  // Make constant accessible
    using ::TEAM_DELTA;  // Make constant accessible
    using ::TEAM_NONE;  // Make constant accessible
    using ::HUMAN;  // Make constant accessible
    using ::BOT;  // Make constant accessible
    using ::VOTE_KICK;  // Make constant accessible
    using ::VOTE_MAP;  // Make constant accessible
    using ::BONUS_NONE;  // Make constant accessible
    using ::NOWEAPON;  // Make constant accessible
    using ::DEFAULT_VOTE_TIME;  // Make constant accessible
    using ::GAMESTYLE_TEAMMATCH;  // Make constant accessible
    using ::GAMESTYLE_CTF;  // Make constant accessible
    using ::GAMESTYLE_INF;  // Make constant accessible
    using ::GAMESTYLE_HTF;  // Make constant accessible
    using ::TICKS;  // Make constant accessible
    using ::SECOND;  // Make constant accessible
    using ::DEFAULT_VOTE_TIME;  // Make constant accessible
    using ::HOUR;  // Make constant accessible
    using ::DAY;  // Make constant accessible
    using ::INT_MAX;  // Make constant accessible

    // Timing variables
    inline void Number27Timing() {
        static uint64_t TimeInMilLast = 0;
        static uint64_t TimeInMil = 0;
        static uint32_t TimePassed = 0;
        static int Seconds = 0;
        static int SecondsLast = 0;

        TimeInMilLast = TimeInMil;
        TimeInMil = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
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
        // Note: log_enable cvar needs to be defined elsewhere
        // For now, just a placeholder implementation
        // This would create and save statistics to a file
        // TStringList s;
        // s.Add("In-Game Statistics");
        // s.Add("Players: " + IntToStr(PlayersNum));
        // etc...
        // s.SaveToFile(UserDirectory + "logs/gamestat.txt");
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
        // Assuming CameraX and CameraY are global variables that need to be defined
        // For now using placeholders - these should be defined elsewhere in the actual code
        extern float CameraX, CameraY;  // These should be declared elsewhere in the actual code
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

        float SX = SpriteParts.Pos[i].x - ((SpriteParts.Pos[i].x - Sprite[i]->Control.MouseAimX) / 2);
        float SY = SpriteParts.Pos[i].y - ((SpriteParts.Pos[i].y - Sprite[i]->Control.MouseAimY) / 2);

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
            VoteStarter = std::string(Sprite[StarterVote]->Player->Name.begin(), Sprite[StarterVote]->Player->Name.end());
            // VoteCooldown[StarterVote] = DEFAULT_VOTE_TIME;
#ifndef SERVER_CODE
            extern uint8_t MySprite;  // This should be defined elsewhere
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
            if (Sprite[i] && Sprite[i]->Active) {
                if (Sprite[i]->Player->ControlMethod == HUMAN) {
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

    inline void ShowMapChangeScoreboard() {
        ShowMapChangeScoreboard("EXIT*!*");
    }

    inline bool IsTeamGame() {
        // Assuming sv_gamemode is defined elsewhere as a cvar
        // For now, returning false as a placeholder
        // switch (sv_gamemode.Value()) {
        //     case GAMESTYLE_TEAMMATCH:
        //     case GAMESTYLE_CTF:
        //     case GAMESTYLE_INF:
        //     case GAMESTYLE_HTF:
        //         return true;
        //     default:
        //         return false;
        // }
        return false;  // Placeholder
    }

    inline void ChangeMap() {
        // This is a complex function that would handle map changes
        // It includes loading new maps, resetting player states, spawning items, etc.
        // For now I'll provide a skeleton implementation

        // Reset bullets and things
        for (int i = 1; i <= MAX_BULLETS; i++) {
            // Bullet[i].Kill();  // Kill method should be defined in TBullet
        }
        for (int i = 1; i <= MAX_THINGS; i++) {
            // Thing[i].Kill();  // Kill method should be defined in TThing
        }
#ifndef SERVER_CODE
        for (int i = 1; i <= MAX_SPARKS; i++) {
            // Spark[i].Kill();  // Kill method should be defined in TSpark
        }
#endif

        // Reset sprites
        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i] && Sprite[i]->Active && Sprite[i]->IsNotSpectator()) {
                // RandomizeStart(SpriteParts.Pos[i], Sprite[i].Player.Team);
                // Sprite[i].Respawn();
                Sprite[i]->Player->Kills = 0;
                Sprite[i]->Player->Deaths = 0;
                Sprite[i]->Player->Flags = 0;
                Sprite[i]->BonusTime = 0;
                Sprite[i]->BonusStyle = BONUS_NONE;
#ifndef SERVER_CODE
                // Sprite[i].SelWeapon = 0;
#endif
                // Sprite[i].FreeControls();
                // Sprite[i].Weapon = Guns[NOWEAPON];

                // int SecWep = Sprite[i].Player.SecWep + 1;

                // if ((SecWep >= 1) && (SecWep <= SECONDARY_WEAPONS) &&
                //     (WeaponActive[PRIMARY_WEAPONS + SecWep] == 1)) {
                //     Sprite[i].SecondaryWeapon = Guns[PRIMARY_WEAPONS + SecWep];
                // } else {
                //     Sprite[i].SecondaryWeapon = Guns[NOWEAPON];
                // }

                // Sprite[i].RespawnCounter = 0;
            }
        }

#ifndef SERVER_CODE
        for (int j = 1; j <= MAX_SPRITES; j++) {
            for (int i = 1; i <= PRIMARY_WEAPONS; i++) {
                WeaponSel[j][i] = 1;
            }
        }
#endif

        // extern bool sv_advancemode;  // This should be defined elsewhere
        // if (sv_advancemode.Value()) {
#ifndef SERVER_CODE
            for (int j = 1; j <= MAX_SPRITES; j++) {
                for (int i = 1; i <= PRIMARY_WEAPONS; i++) {
                    WeaponSel[j][i] = 0;
                }
            }

            extern uint8_t MySprite;  // This should be defined elsewhere
            if (MySprite > 0) {
                for (int i = 1; i <= MAIN_WEAPONS; i++) {
                    // LimboMenu.Button[i - 1].Active = Boolean(WeaponSel[MySprite][i]);
                }
            }
#endif
        // }

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

        // extern auto sv_timelimit;  // This should be defined elsewhere
        // TimeLimitCounter = sv_timelimit.Value();

        // This would also handle server-side specific code for spawning flags/guns, etc.
    }

    inline void SortPlayers() {
        extern int PlayersNum;  // These should be declared elsewhere
        extern int BotsNum;
        extern int SpectatorsNum;
        extern int PlayersTeamNum[5];  // 0-4 teams

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
            if (Sprite[i] && Sprite[i]->Active && (!Sprite[i]->Player->DemoPlayer)) {
                PlayersNum++;
                if (Sprite[i]->Player->ControlMethod == BOT) {
                    BotsNum++;
                }

                if (Sprite[i]->IsSpectator()) {
                    SpectatorsNum++;
                }

                if (Sprite[i]->IsNotSolo() && Sprite[i]->IsNotSpectator()) {
                    PlayersTeamNum[Sprite[i]->Player->Team]++;
                }

                if (Sprite[i]->IsNotSpectator()) {
                    SortedPlayers[PlayersNum].Kills = Sprite[i]->Player->Kills;
                    SortedPlayers[PlayersNum].Deaths = Sprite[i]->Player->Deaths;
                    SortedPlayers[PlayersNum].Flags = Sprite[i]->Player->Flags;
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
                        // extern auto sv_killlimit;  // This should be defined elsewhere
                        // if (Sprite[i].Player.Kills >= sv_killlimit.Value()) {
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
                        // }
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
                // extern auto sv_killlimit;  // This should be defined elsewhere
                // if (TeamScore[i] >= sv_killlimit.Value()) {
                    // NextMap();
                    break;
                // }
            }
        }
        // UpdateWaveRespawnTime();  // Assuming this function exists
#endif

        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i] && Sprite[i]->Active) {
#ifdef SERVER_CODE
                if (Sprite[i]->Active && (Sprite[i]->Player->Team == TEAM_ALPHA)) {
                    TeamAliveNum[TEAM_ALPHA]++;
                }
                if (Sprite[i]->Active && (Sprite[i]->Player->Team == TEAM_BRAVO)) {
                    TeamAliveNum[TEAM_BRAVO]++;
                }
                if (Sprite[i]->Active && (Sprite[i]->Player->Team == TEAM_CHARLIE)) {
                    TeamAliveNum[TEAM_CHARLIE]++;
                }
                if (Sprite[i]->Active && (Sprite[i]->Player->Team == TEAM_DELTA)) {
                    TeamAliveNum[TEAM_DELTA]++;
                }
#else
                if (Sprite[i]->Player->Team == TEAM_NONE) {
                    TeamPlayersNum[TEAM_NONE]++;
                }
                if (Sprite[i]->Player->Team == TEAM_ALPHA) {
                    TeamPlayersNum[TEAM_ALPHA]++;
                }
                if (Sprite[i]->Player->Team == TEAM_BRAVO) {
                    TeamPlayersNum[TEAM_BRAVO]++;
                }
                if (Sprite[i]->Player->Team == TEAM_CHARLIE) {
                    TeamPlayersNum[TEAM_CHARLIE]++;
                }
                if (Sprite[i]->Player->Team == TEAM_DELTA) {
                    TeamPlayersNum[TEAM_DELTA]++;
                }
#endif
            }
        }
    }
}



#endif // GAME_H