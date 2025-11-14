#ifndef SERVER_LOOP_H
#define SERVER_LOOP_H

//*******************************************************************************
//                                                                              
//       Server Loop Unit for SOLDAT                                               
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include "Constants.h"
#include "Server.h"
#include "Game.h"
#include "Sprites.h"
#include "Net.h"
#include "Things.h"
#include "Vector.h"
#include "ServerHelper.h"
#include "Demo.h"
#include "Weapons.h"
#include "Cvar.h"
#include "LobbyClient.h"

// Function declarations
void UpdateFrame();
void AppOnIdle();

namespace ServerLoopImpl {
    inline void AppOnIdle() {
        // Trace('AppOnIdle');  // Assuming Trace function exists elsewhere
        
        Number27Timing();  // Makes the program go and do the timing calculations

        // NET RECEIVE
        if (UDP) {
            UDP->ProcessLoop();
        }

#ifdef RCON_CODE
        if (AdminServer != nullptr) {
            // AdminServer.ProcessCommands();  // Assuming this method exists
        }
#endif

        // Process ticks between TickTime and TickTimeLast
        for (int mainControl = 1; mainControl <= (TickTime - TickTimeLast); mainControl++) {
            // Frame rate independent code
            Ticks++;

            ServerTickCounter++;
            // Update main tick counter
            MainTickCounter++;
            if (MainTickCounter == 2147483640) {
                MainTickCounter = 0;
            }

#ifdef SCRIPT_CODE
            // ScrptDispatcher.OnClockTick();  // Assuming this method exists
#endif

#ifdef STEAM_CODE
            // RunManualCallbacks();  // This function would be defined elsewhere
#endif
            // Flood Numbers Cancel
            if (MainTickCounter % 1000 == 0) {
                for (int j = 1; j <= MAX_FLOODIPS; j++) {
                    FloodNum[j] = 0;
                }
            }

            // Clear last admin connect flood list every 3 seconds
            if (MainTickCounter % (SECOND * 3) == 0) {
                for (int j = 0; j <= MAX_LAST_ADMIN_IPS; j++) {
                    LastAdminIPs[j] = "";
                }
            }

            // Warnings Cancel
            if (MainTickCounter % (MINUTE * 5) == 0) {
                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    if (PingWarnings[j] > 0) {
                        PingWarnings[j]--;
                    }

                    if (FloodWarnings[j] > 0) {
                        FloodWarnings[j]--;
                    }
                }
            }

            if (MainTickCounter % 1000 == 0) {
                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    Sprite[j].Player.KnifeWarnings = 0;
                }
            }

            // Sync changed cvars to all players
            if (CvarsNeedSyncing) {
                ServerSyncCvars(0, 0, false);
            }

            // General game updating
            UpdateFrame();

            if (MapChangeCounter < 0) {
                if (DemoRecorder.Active()) {
                    DemoRecorder.SaveNextFrame();
                }
            }

            // Trace('AppOnIdle 2');  // Assuming Trace function exists elsewhere

            // Network updating
            if (MainTickCounter % SECOND == 0) {
                // Player Ping Warning
                if (MapChangeCounter < 0) {
                    if (MainTickCounter % (SECOND * 6) == 0) {
                        for (int j = 1; j <= MAX_PLAYERS; j++) {
                            if (Sprite[j].Active && 
                                (Sprite[j].Player.ControlMethod == HUMAN) &&
                                ((Sprite[j].Player.RealPing > sv_maxping.Value()) ||
                                ((Sprite[j].Player.RealPing < sv_minping.Value()) &&
                                (Sprite[j].Player.PingTime > 0)))) {
                                MainConsole.Console(std::string(Sprite[j].Player.Name.begin(), Sprite[j].Player.Name.end()) + 
                                                  " gets a ping warning", WARNING_MESSAGE_COLOR);
                                PingWarnings[j]++;
                                if (PingWarnings[j] > sv_warnings_ping.Value()) {
                                    KickPlayer(j, true, KICK_PING, SIXTY_MINUTES / 4, "Ping Kick");
                                }
                            }
                        }
                    }
                }

                // Player Packet Flooding
                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    if (Sprite[j].Active) {
                        if ((net_lan.Value() == LAN && 
                            MessagesASecNum[j] > net_floodingpacketslan.Value()) ||
                            (net_lan.Value() == INTERNET && 
                            MessagesASecNum[j] > net_floodingpacketsinternet.Value())) {
                            MainConsole.Console(std::string(Sprite[j].Player.Name.begin(), Sprite[j].Player.Name.end()) + 
                                              " is flooding the server", WARNING_MESSAGE_COLOR);
                            FloodWarnings[j]++;
                            if (FloodWarnings[j] > sv_warnings_flood.Value()) {
                                KickPlayer(j, true, KICK_FLOODING, SIXTY_MINUTES / 4, "Flood Kicked");
                            }
                        }
                    }
                }

                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    MessagesASecNum[j] = 0;
                }
            }

            if (MainTickCounter % (SECOND * 10) == 0) {
                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    if (Sprite[j].Active) {
                        UDP->UpdateNetworkStats(j);
                    }
                }
            }

            // Packet rate send adjusting
            int heavySendersNum = PlayersNum - SpectatorsNum;

            float adjust = 1.0f;
            if (heavySendersNum < 5) adjust = 0.66f;
            else if (heavySendersNum < 9) adjust = 0.75f;
            else adjust = 1.0f;

            // Send Bundled packets
            if (net_lan.Value() == LAN) {
                if (MainTickCounter % static_cast<int>(std::round(30 * adjust)) == 0) {
                    ServerSpriteSnapshot(NETW);
                }

                if ((MainTickCounter % static_cast<int>(std::round(15 * adjust)) == 0) &&
                    (MainTickCounter % static_cast<int>(std::round(30 * adjust)) != 0)) {
                    ServerSpriteSnapshotMajor(NETW);
                }

                if (MainTickCounter % static_cast<int>(std::round(20 * adjust)) == 0) {
                    ServerSkeletonSnapshot(NETW);
                }

                if (MainTickCounter % static_cast<int>(std::round(59 * adjust)) == 0) {
                    ServerHeartBeat();
                }

                if ((MainTickCounter % static_cast<int>(std::round(4 * adjust)) == 0) &&
                    (MainTickCounter % static_cast<int>(std::round(30 * adjust)) != 0) &&
                    (MainTickCounter % static_cast<int>(std::round(60 * adjust)) != 0)) {
                    for (int j = 1; j <= MAX_SPRITES; j++) {
                        if (Sprite[j].Active && (Sprite[j].Player.ControlMethod == BOT)) {
                            ServerSpriteDeltas(j);
                        }
                    }
                }
            } else if (net_lan.Value() == INTERNET) {
                if (MainTickCounter % static_cast<int>(std::round(net_t1_snapshot.Value() * adjust)) == 0) {
                    ServerSpriteSnapshot(NETW);
                }

                if ((MainTickCounter % static_cast<int>(std::round(net_t1_majorsnapshot.Value() * adjust)) == 0) &&
                    (MainTickCounter % static_cast<int>(std::round(net_t1_snapshot.Value() * adjust)) != 0)) {
                    ServerSpriteSnapshotMajor(NETW);
                }

                if (MainTickCounter % static_cast<int>(std::round(net_t1_deadsnapshot.Value() * adjust)) == 0) {
                    ServerSkeletonSnapshot(NETW);
                }

                if (MainTickCounter % static_cast<int>(std::round(net_t1_heartbeat.Value() * adjust)) == 0) {
                    ServerHeartBeat();
                }

                if ((MainTickCounter % static_cast<int>(std::round(net_t1_delta.Value() * adjust)) == 0) &&
                    (MainTickCounter % static_cast<int>(std::round(net_t1_snapshot.Value() * adjust)) != 0) &&
                    (MainTickCounter % static_cast<int>(std::round(net_t1_majorsnapshot.Value() * adjust)) != 0)) {
                    for (int j = 1; j <= MAX_SPRITES; j++) {
                        if (Sprite[j].Active && (Sprite[j].Player.ControlMethod == BOT)) {
                            ServerSpriteDeltas(j);
                        }
                    }
                }
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j].Active && (Sprite[j].Player.ControlMethod == HUMAN) && 
                    (Sprite[j].Player.Port > 0)) {
                    // Connection problems
                    if (MapChangeCounter < 0) {
                        NoClientupdateTime[j]++;
                    }
                    if (NoClientupdateTime[j] > DISCONNECTION_TIME) {
                        ServerPlayerDisconnect(j, KICK_NORESPONSE);
                        MainConsole.Console(std::string(Sprite[j].Player.Name.begin(), Sprite[j].Player.Name.end()) + 
                                          " could not respond", WARNING_MESSAGE_COLOR);
#ifdef SCRIPT_CODE
                        // ScrptDispatcher.OnLeaveGame(j, false);  // Assuming this function exists
#endif
                        Sprite[j].Kill();
                        DoBalanceBots(1, Sprite[j].Player.Team);
                        continue;
                    }
                    if (NoClientupdateTime[j] < 0) {
                        NoClientupdateTime[j] = 0;
                    }

#ifdef ENABLE_FAE_CODE
                    if (ac_enable.Value()) {
                        // Monotonically increment the anti-cheat ticks counter. A valid response resets it.
                        Sprite[j].Player.FaeTicks++;
                        if (Sprite[j].Player.FaeTicks > (SECOND * 20)) {
                            // Timeout reached; no valid response for 20 seconds. Boot the player.
                            MainConsole.Console(std::string(Sprite[j].Player.Name.begin(), Sprite[j].Player.Name.end()) + 
                                              " no anti-cheat response", WARNING_MESSAGE_COLOR);
                            KickPlayer(j, false, KICK_AC, 0, "No Anti-Cheat Response");
                            continue;
                        } else if ((MainTickCounter % (SECOND * 3) == 0) && 
                                  (!Sprite[j].Player.FaeResponsePending)) {
                            // Send periodic anti-cheat challenge.
                            // ServerSendFaeChallenge(Sprite[j].Player.Peer, false);  // Assuming this function exists
                        }
                    }
#endif

                    if (MainTickCounter % MINUTE == 0) {
                        ServerSyncMsg();
                    }

                    if (net_lan.Value() == LAN) {
                        if (MainTickCounter % static_cast<int>(std::round(21 * adjust)) == 0) {
                            ServerPing(j);
                        }

                        if (MainTickCounter % static_cast<int>(std::round(12 * adjust)) == 0) {
                            ServerThingSnapshot(j);
                        }
                    } else if (net_lan.Value() == INTERNET) {
                        if (MainTickCounter % static_cast<int>(std::round(net_t1_ping.Value() * adjust)) == 0) {
                            ServerPing(j);
                        }

                        if (MainTickCounter % static_cast<int>(std::round(net_t1_thingsnapshot.Value() * adjust)) == 0) {
                            ServerThingSnapshot(j);
                        }
                    }
                }
            }

            // UDP.FlushMsg;  // Assuming this method exists
        }
    }

    inline void UpdateFrame() {
        // Trace('UpdateFrame');  // Assuming this function exists elsewhere
        
        TVector2 M = Default(TVector2);

        if (MapChangeCounter < 0) {
            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j].Active && !Sprite[j].DeadMeat) {
                    if (Sprite[j].IsNotSpectator()) {
                        // Shift old sprite positions
                        for (int i = MAX_OLDPOS; i >= 1; i--) {
                            OldSpritePos[j][i] = OldSpritePos[j][i - 1];
                        }
                        OldSpritePos[j][0] = SpriteParts.Pos[j];
                    }
                }
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j].Active) {
                    if (Sprite[j].IsNotSpectator()) {
                        SpriteParts.DoEulerTimeStepFor(j);  // Integrate sprite particles
                    }
                }
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (Sprite[j].Active) {
                    Sprite[j].Update();  // Update sprite
                }
            }

            // Bullets update
            for (int j = 1; j <= MAX_BULLETS; j++) {
                if (Bullet[j].Active) {
                    Bullet[j].Update();
                }
            }

            BulletParts.DoEulerTimeStep();

            // Update Things
            for (int j = 1; j <= MAX_THINGS; j++) {
                if (Thing[j].Active) {
                    Thing[j].Update();
                }
            }

            // Bonuses spawn
            if (!sv_survivalmode.Value() && !sv_realisticmode.Value()) {
                if (sv_bonus_frequency.Value() > 0) {
                    int bonusFreq = 0;
                    switch (sv_bonus_frequency.Value()) {
                        case 1: bonusFreq = 7400; break;
                        case 2: bonusFreq = 4300; break;
                        case 3: bonusFreq = 2500; break;
                        case 4: bonusFreq = 1600; break;
                        case 5: bonusFreq = 800; break;
                    }

                    if (sv_bonus_berserker.Value()) {
                        if (MainTickCounter % bonusFreq == 0) {
                            if (Random(BERSERKERBONUS_RANDOM) == 0) {
                                SpawnThings(OBJECT_BERSERK_KIT, 1);
                            }
                        }
                    }

                    int j_val = FLAMERBONUS_RANDOM;
                    if (sv_bonus_flamer.Value()) {
                        if (MainTickCounter % 444 == 0) {
                            if (Random(j_val) == 0) {
                                SpawnThings(OBJECT_FLAMER_KIT, 1);
                            }
                        }
                    }

                    if (sv_bonus_predator.Value()) {
                        if (MainTickCounter % bonusFreq == 0) {
                            if (Random(PREDATORBONUS_RANDOM) == 0) {
                                SpawnThings(OBJECT_PREDATOR_KIT, 1);
                            }
                        }
                    }

                    if (sv_bonus_vest.Value()) {
                        if (MainTickCounter % (bonusFreq / 2) == 0) {
                            if (Random(VESTBONUS_RANDOM) == 0) {
                                SpawnThings(OBJECT_VEST_KIT, 1);
                            }
                        }
                    }

                    j_val = CLUSTERBONUS_RANDOM;
                    if (sv_gamemode.Value() == GAMESTYLE_CTF) {
                        j_val = static_cast<int>(std::round(CLUSTERBONUS_RANDOM * 0.75));
                    }
                    if (sv_gamemode.Value() == GAMESTYLE_INF) {
                        j_val = static_cast<int>(std::round(CLUSTERBONUS_RANDOM * 0.75));
                    }
                    if (sv_gamemode.Value() == GAMESTYLE_HTF) {
                        j_val = static_cast<int>(std::round(CLUSTERBONUS_RANDOM * 0.75));
                    }
                    if (sv_bonus_cluster.Value()) {
                        if (MainTickCounter % (BonusFreq / 2) == 0) {
                            if (Random(j_val) == 0) {
                                SpawnThings(OBJECT_CLUSTER_KIT, 1);
                            }
                        }
                    }
                }
            }
        }

        // Bullet timer
        if (BulletTimeTimer > -1) {
            BulletTimeTimer--;
        }

        if (BulletTimeTimer == 0) {
            ToggleBulletTime(false);
            BulletTimeTimer = -1;
        } else if (BulletTimeTimer < 1) {
            // MapChange counter update
            if ((MapChangeCounter > -60) && (MapChangeCounter < 99999999)) {
                MapChangeCounter--;
            }
            if ((MapChangeCounter < 0) && (MapChangeCounter > -59)) {
                ChangeMap();
            }

            // Game Stats save
            if (MainTickCounter % log_filesupdate.Value() == 0) {
                if (log_enable.Value()) {
                    UpdateGameStats();

                    WriteLogFile(KillLog, KillLogFileName);
                    WriteLogFile(GameLog, ConsoleLogFileName);

                    if ((CheckFileSize(KillLogFileName) > MAX_LOGFILESIZE) ||
                        (CheckFileSize(ConsoleLogFileName) > MAX_LOGFILESIZE)) {
                        NewLogFiles();
                    }
                }
            }

            // Anti-Hack for Mass-Flag Cheat
            if (MainTickCounter % SECOND == 0) {
                if (sv_antimassflag.Value()) {
                    for (int j = 1; j <= MAX_SPRITES; j++) {
                        if (Sprite[j].Active && 
                            (Sprite[j].Player.GrabsPerSecond > 0) &&
                            (Sprite[j].Player.ScoresPerSecond > 0) &&
                            (Sprite[j].Player.GrabbedInBase)) {
                            CheatTag[j] = 1;
#ifdef SCRIPT_CODE
                            //if (!ScrptDispatcher.OnVoteKickStart(255, j, "Server: Possible cheating")) {
                            //    StartVote(255, VOTE_KICK, std::to_string(j), "Server: Possible cheating");
                            //    ServerSendVoteOn(VoteType, 255, std::to_string(j), "Server: Possible cheating");
                            //}
#else
                            StartVote(255, VOTE_KICK, std::to_string(j), "Server: Possible cheating");
                            ServerSendVoteOn(VoteType, 255, std::to_string(j), "Server: Possible cheating");
#endif
                            MainConsole.Console("** Detected possible Mass-Flag cheating from " +
                                              std::string(Sprite[j].Player.Name.begin(), Sprite[j].Player.Name.end()), 
                                              WARNING_MESSAGE_COLOR);
                        }
                        Sprite[j].Player.GrabsPerSecond = 0;
                        Sprite[j].Player.ScoresPerSecond = 0;
                        Sprite[j].Player.GrabbedInBase = false;
                    }
                }
            }

            if (sv_healthcooldown.Value() > 0) {
                if (MainTickCounter % (sv_healthcooldown.Value() * SECOND) == 0) {
                    for (int i = 1; i <= MAX_SPRITES; i++) {
                        if (Sprite[i].Active && Sprite[i].HasPack) {
                            Sprite[i].HasPack = false;
                        }
                    }
                }
            }

            // Anti-Chat Flood
            if (MainTickCounter % SECOND == 0) {
                for (int j = 1; j <= MAX_SPRITES; j++) {
                    if (Sprite[j].Active) {
                        if (Sprite[j].Player.ChatWarnings > 5) {
                            // 20 Minutes is too harsh
                            KickPlayer(j, true, KICK_FLOODING, FIVE_MINUTES, "Chat Flood");
                        }
                        if (Sprite[j].Player.ChatWarnings > 0) {
                            Sprite[j].Player.ChatWarnings--;
                        }
                    }
                }
            }

            if (MainTickCounter % SECOND == 0) {
                if ((LastReqIP[0] != "") && (LastReqIP[0] == LastReqIP[1]) &&
                    (LastReqIP[1] == LastReqIP[2]) && (LastReqIP[2] == LastReqIP[3])) {
                    DropIP = LastReqIP[0];
                    MainConsole.Console("Firewalled IP " + DropIP, 0);
                }
            }

            if (MainTickCounter % (SECOND * 3) == 0) {
                for (int j = 0; j < 4; j++) {
                    LastReqIP[j] = "";  // Reset last 4 IP requests in 3 seconds
                }
            }

            if (MainTickCounter % (SECOND * 30) == 0) {
                DropIP = "";  // Clear temporary firewall IP
            }

            if (MainTickCounter % MINUTE == 0) {
                if (sv_lobby.Value()) {
                    // LobbyThread = std::make_unique<TLobbyThread>(); // Assuming TLobbyThread is defined elsewhere
                }
            }

            // *BAN*
            // Ban Timers v2
            if (MainTickCounter % MINUTE == 0) {
                UpdateIPBanList();
                UpdateHWBanList();
            }

            if (MainTickCounter % MINUTE == 0) {
                for (int j = 1; j <= MAX_SPRITES; j++) {
                    if (Sprite[j].Active) {
                        Sprite[j].Player.PlayTime++;
                    }
                }
            }

            // Leftover from old Ban Timers code
            if (MainTickCounter % (SECOND * 10) == 0) {
                if (PlayersNum == 0) {
                    if (MapChangeCounter > 99999999) {
                        MapChangeCounter = -60;
                    }
                }
            }

            SinusCounter += ILLUMINATESPEED;

            // Wave respawn count
            WaveRespawnCounter--;
            if (WaveRespawnCounter < 1) {
                WaveRespawnCounter = WaveRespawnTime;
            }

            for (int j = 1; j <= MAX_SPRITES; j++) {
                if (VoteCooldown[j] > -1) {
                    VoteCooldown[j]--;
                }
            }

            // Time Limit decrease
            if (MapChangeCounter < 99999999) {
                // If (MapChangeCounter<0) then
                if (TimeLimitCounter > 0) {
                    TimeLimitCounter--;
                }
            }
            if (TimeLimitCounter == 1) {
                NextMap();
            }

            TimeLeftMin = TimeLimitCounter / MINUTE;
            TimeLeftSec = (TimeLimitCounter - TimeLeftMin * MINUTE) / 60;

            if (TimeLimitCounter > 0) {
                if (TimeLimitCounter < FIVE_MINUTES + 1) {
                    if (TimeLimitCounter % MINUTE == 0) {
                        MainConsole.Console("Time Left: " +
                                          std::to_string(TimeLimitCounter / MINUTE) + " minutes",
                                          GAME_MESSAGE_COLOR);
                    }
                } else {
                    if (TimeLimitCounter % FIVE_MINUTES == 0) {
                        MainConsole.Console("Time Left: " +
                                          std::to_string(TimeLimitCounter / MINUTE) + " minutes",
                                          GAME_MESSAGE_COLOR);
                    }
                }
            }

            // Trace('UpdateFrame 2');  // Assuming this function exists elsewhere

            // Voting timer
            TimerVote();

            // Consoles Update
            MainConsole.ScrollTick++;
            if (MainConsole.ScrollTick == MainConsole.ScrollTickMax) {
                MainConsole.ScrollConsole();
            }

            if (MainConsole.AlphaCount > 0) {
                MainConsole.AlphaCount--;
            }

            if (!sv_advancemode.Value()) {
                for (int j = 1; j <= MAX_SPRITES; j++) {
                    for (int i = 1; i <= 10; i++) {
                        // WeaponSel[j][i] = 1;
                    }
                }
            }
        }  // bullettime off

        // Trace('UpdateFrame 3');  // Assuming this function exists elsewhere

        // Infiltration mode blue team score point
        int j = sv_inf_bluelimit.Value() * SECOND;
        if (PlayersTeamNum[1] < PlayersTeamNum[2]) {
            j = sv_inf_bluelimit.Value() * SECOND + 2 * SECOND * (PlayersTeamNum[2] - PlayersTeamNum[1]);
        }

        if (sv_gamemode.Value() == GAMESTYLE_INF) {
            if (MapChangeCounter < 0) {
                if (Thing[TeamFlag[2]].InBase) {
                    if ((PlayersTeamNum[1] > 0) && (PlayersTeamNum[2] > 0)) {
                        if (MainTickCounter % j == 0) {
                            TeamScore[2]++;
                            SortPlayers();
                        }
                    }
                }
            }
        }

        // HTF mode team score point
        if (PlayersTeamNum[2] == PlayersTeamNum[1]) {
            HTFTime = sv_htf_pointstime.Value() * 60;
        }

        if (sv_gamemode.Value() == GAMESTYLE_HTF) {
            if (MapChangeCounter < 0) {
                if ((PlayersTeamNum[1] > 0) && (PlayersTeamNum[2] > 0)) {
                    if (MainTickCounter % HTFTime == 0) {
                        for (int i = 1; i <= MAX_SPRITES; i++) {
                            if (Sprite[i].Active && (Sprite[i].HoldedThing > 0)) {
                                if (Thing[Sprite[i].HoldedThing].Style == OBJECT_POINTMATCH_FLAG) {
                                    TeamScore[Sprite[i].Player.Team]++;
                                    
                                    if (Sprite[i].Player.Team == TEAM_ALPHA) {
                                        HTFTime = HTF_SEC_POINT + 2 * SECOND * (PlayersTeamNum[1] - PlayersTeamNum[2]);
                                    }
                                    
                                    if (Sprite[i].Player.Team == TEAM_BRAVO) {
                                        HTFTime = HTF_SEC_POINT + 2 * SECOND * (PlayersTeamNum[2] - PlayersTeamNum[1]);
                                    }
                                    
                                    if (HTFTime < HTF_SEC_POINT) {
                                        HTFTime = HTF_SEC_POINT;
                                    }
                                    
                                    SortPlayers();
                                }
                            }
                        }
                    }
                }
            }
        }

        // Spawn Rambo bow if nobody has it and not on map
        if (sv_gamemode.Value() == GAMESTYLE_RAMBO) {
            if (MainTickCounter % SECOND == 0) {
                long x = 0;

                for (int j = 1; j <= MAX_THINGS; j++) {
                    if (Thing[j].Active) {
                        if (Thing[j].Style == OBJECT_RAMBO_BOW) {
                            x = 1;
                        }
                    }
                }

                for (int j = 1; j <= MAX_PLAYERS; j++) {
                    if (Sprite[j].Active) {
                        if ((Sprite[j].Weapon.Num == Guns[BOW].Num) ||
                            (Sprite[j].Weapon.Num == Guns[BOW2].Num)) {
                            x = 1;
                        }
                    }
                }

                if (x == 0) {
                    M = Default(TVector2);
                    RandomizeStart(M, 15);

                    j = CreateThing(M, 255, OBJECT_RAMBO_BOW, 255);
                }
            }
        }

        // Destroy flags if > 1
        if ((sv_gamemode.Value() == GAMESTYLE_CTF) || (sv_gamemode.Value() == GAMESTYLE_INF)) {
            if (MainTickCounter % (SECOND * 2) == 0) {
                x = 0;

                for (int j = 1; j <= MAX_THINGS; j++) {
                    if (Thing[j].Active) {
                        if (Thing[j].Style == OBJECT_ALPHA_FLAG) {
                            x++;
                        }
                    }
                }

                if (x > 1) {
                    for (int j = MAX_THINGS; j >= 1; j--) {
                        if (Thing[j].Active) {
                            if (Thing[j].Style == OBJECT_ALPHA_FLAG) {
                                Thing[j].Kill();
                                break;
                            }
                        }
                    }
                }

                if (x == 0) {
                    if (RandomizeStart(M, 5)) {
                        TeamFlag[1] = CreateThing(M, 255, OBJECT_ALPHA_FLAG, 255);
                    }
                }

                x = 0;

                for (int j = 1; j <= MAX_THINGS; j++) {
                    if (Thing[j].Active) {
                        if (Thing[j].Style == OBJECT_BRAVO_FLAG) {
                            x++;
                        }
                    }
                }

                if (x > 1) {
                    for (int j = MAX_THINGS; j >= 1; j--) {
                        if (Thing[j].Active) {
                            if (Thing[j].Style == OBJECT_BRAVO_FLAG) {
                                Thing[j].Kill();
                                break;
                            }
                        }
                    }
                }

                if (x == 0) {
                    if (RandomizeStart(M, 6)) {
                        TeamFlag[2] = CreateThing(M, 255, OBJECT_BRAVO_FLAG, 255);
                    }
                }
            }
        }

        if ((sv_gamemode.Value() == GAMESTYLE_POINTMATCH) || (sv_gamemode.Value() == GAMESTYLE_HTF)) {
            if (MainTickCounter % (SECOND * 2) == 0) {
                x = 0;

                for (int j = 1; j <= MAX_THINGS; j++) {
                    if (Thing[j].Active) {
                        if (Thing[j].Style == OBJECT_POINTMATCH_FLAG) {
                            x++;
                        }
                    }
                }

                if (x > 1) {
                    for (int j = MAX_THINGS; j >= 1; j--) {
                        if (Thing[j].Active) {
                            if (Thing[j].Style == OBJECT_POINTMATCH_FLAG) {
                                Thing[j].Kill();
                                break;
                            }
                        }
                    }
                }

                if (x == 0) {
                    if (RandomizeStart(M, 14)) {
                        TeamFlag[1] = CreateThing(M, 255, OBJECT_POINTMATCH_FLAG, 255);
                    }
                }
            }
        }

        if ((demo_autorecord.Value() && !DemoRecorder.Active() && !Map.Name.empty())) {
            std::time_t now = std::time(nullptr);
            std::tm* tm_info = std::localtime(&now);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S_", tm_info);
            std::string timestamp(buffer);
            
            DemoRecorder.StartRecord(UserDirectory + "demos/" + timestamp + Map.Name + ".sdm");
        }
    }
}

// Using declarations to bring into global namespace
using ServerLoopImpl::UpdateFrame;
using ServerLoopImpl::AppOnIdle;

#endif // SERVER_LOOP_H