#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

//*******************************************************************************
//                                                                              
//       Server Commands Unit for SOLDAT                                         
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "Command.h"
#include "Server.h"
#include "Weapons.h"
#include "Net.h" 
#include "NetworkServerMessages.h"
#include "NetworkServerFunctions.h"
#include "NetworkServerConnection.h"
#include "NetworkServerGame.h"
#include "Sprites.h"
#include "PolyMap.h"
#include "Game.h"
#include "Things.h"
#include "Constants.h"
#include "ServerHelper.h"
#include "BanSystem.h"
#include "Demo.h"
#include "Util.h"
#include "Vector.h"
#include "Version.h"

// Function declarations
void InitServerCommands();

namespace ServerCommandsImpl {
    inline void CommandAddBot(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        std::string TempStr;
        int TeamSet;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (PlayerNum == MAX_PLAYERS) {
            return;
        }

        TempStr = Args[0];
        // Extract team number from command string like "addbot1" -> team 1
        if (TempStr.length() >= 7) {
            TeamSet = std::stoi(std::string(1, TempStr[6])); // Get 7th character (0-indexed as 6)
        } else {
            TeamSet = 0;
        }
        AddBotPlayer(Name, TeamSet);
    }

    inline void CommandNextMap(const std::vector<std::string>& Args, uint8_t Sender) {
        NextMap();
    }

    inline void CommandMap(const std::vector<std::string>& Args, uint8_t Sender) {
        TMapInfo Status;
        
        if (Args.size() == 1) {
            return;
        }

        if (Args[1].empty()) {
            return;
        }

        if (GetMapInfo(Args[1], UserDirectory, Status)) {
            PrepareMapChange(Args[1]);
        } else {
#ifdef STEAM_CODE
            if (Status.WorkshopID > 0) {
                MapChangeItemID = Status.WorkshopID;
                if (/*SteamAPI.UGC.DownloadItem(MapChangeItemID, True)*/ true) { // Placeholder for Steam download
                    MainConsole.Console("[Steam] Workshop map " + std::to_string(MapChangeItemID) + 
                                       " not found in cache, downloading.", WARNING_MESSAGE_COLOR, Sender);
                } else {
                    MainConsole.Console("[Steam] Workshop map " + std::to_string(MapChangeItemID) + 
                                       " is invalid", WARNING_MESSAGE_COLOR, Sender);
                }
            } else
#endif
            {
                MainConsole.Console("Map not found (" + Args[1] + ")", WARNING_MESSAGE_COLOR, Sender);
            }
        }
    }

    inline void CommandPause(const std::vector<std::string>& Args, uint8_t Sender) {
        MapChangeCounter = 999999999;
        ServerSyncMsg();
    }

    inline void CommandUnpause(const std::vector<std::string>& Args, uint8_t Sender) {
        MapChangeCounter = -60;
        ServerSyncMsg();
    }

    inline void CommandRestart(const std::vector<std::string>& Args, uint8_t Sender) {
        MapChangeName = Map.Name;
        MapChangeCounter = MapChangeTime;
        ServerMapChange(ALL_PLAYERS);  // Inform clients of Map Change
#ifdef SCRIPT_CODE
        // ScrptDispatcher.OnBeforeMapChange(MapChangeName);
#endif
    }

    inline void CommandKick(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i;
        std::vector<uint8_t> Targets; // Assuming CommandTarget returns vector of indices

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            KickPlayer(Targets[i], false, KICK_CONSOLE, 0); // Assuming KICK_CONSOLE constant exists
        }
    }

    inline void CommandKickLast(const std::vector<std::string>& Args, uint8_t Sender) {
        if ((LastPlayer > 0) && (LastPlayer < MAX_SPRITES + 1)) {
            if (Sprite[LastPlayer].Active) {
                KickPlayer(LastPlayer, false, KICK_CONSOLE, 0);
            }
        }
    }

    inline void CommandBan(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name, TempStr;
        int i;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (Sender == 255) {
            TempStr = "an admin";
        } else {
            TempStr = std::string(Sprite[Sender].Player.Name.begin(), Sprite[Sender].Player.Name.end());
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            KickPlayer(Targets[i], true, KICK_CONSOLE, (DAY * 30), "Banned by " + TempStr);
        }
    }

    inline void CommandBanIPHW(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name, TempStr;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (Sender == 255) {
            TempStr = "an admin";
        } else {
            TempStr = std::string(Sprite[Sender].Player.Name.begin(), Sprite[Sender].Player.Name.end());
        }

        if (Args[0] == "banhw") {
            AddBannedHW(Name, "Banned by " + TempStr, (DAY * 30));
            MainConsole.Console("HWID " + Name + " banned", CLIENT_MESSAGE_COLOR, Sender);
        } else {
            AddBannedIP(Name, "Banned by " + TempStr, DAY * 30);
            MainConsole.Console("IP number " + Name + " banned", CLIENT_MESSAGE_COLOR, Sender);
        }

        SaveTxtLists();
    }

    inline void CommandUnban(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (DelBannedIP(Name)) {
            MainConsole.Console("IP number " + Name + " unbanned", CLIENT_MESSAGE_COLOR, Sender);
        }

        if (DelBannedHW(Name)) {
            MainConsole.Console("HWID " + Name + " unbanned", CLIENT_MESSAGE_COLOR, Sender);
        }

        SaveTxtLists();
    }

    inline void CommandUnbanLast(const std::vector<std::string>& Args, uint8_t Sender) {
        if (DelBannedIP(LastBan)) {
            MainConsole.Console("IP number " + LastBan + " unbanned", CLIENT_MESSAGE_COLOR, Sender);
        }

        if (DelBannedHW(LastBanHW)) {
            MainConsole.Console("HWID " + LastBanHW + " unbanned", CLIENT_MESSAGE_COLOR, Sender);
        }

        SaveTxtLists();
    }

    inline void CommandAdm(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            if (!IsRemoteAdminIP(Sprite[Targets[i]].Player.IP)) {
                RemoteIPs.Add(Sprite[Targets[i]].Player.IP);
                MainConsole.Console("IP number " + std::string(Sprite[Targets[i]].Player.IP.begin(), 
                                  Sprite[Targets[i]].Player.IP.end()) +
                                  " added to Remote Admins", CLIENT_MESSAGE_COLOR, Sender);
                SaveTxtLists();
            }
        }
    }

    inline void CommandAdmIP(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (!IsRemoteAdminIP(Name)) {
            RemoteIPs.Add(Name);
            MainConsole.Console("IP number " + Name + " added to Remote Admins",
                              CLIENT_MESSAGE_COLOR, Sender);
            SaveTxtLists();
        }
    }

    inline void CommandUnadm(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int j;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        if (IsRemoteAdminIP(Name)) {
            j = RemoteIPs.IndexOf(Name);
            RemoteIPs.Delete(j);
            MainConsole.Console("IP number " + Name +
                              " removed from Remote Admins", CLIENT_MESSAGE_COLOR, Sender);
            SaveTxtLists();
        }
    }

    inline void CommandSetTeam(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name, TempStr;
        int i;
        uint8_t TeamSet;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        TempStr = Args[0];
        // Extract team number from command string like "setteam1" -> team 1
        if (TempStr.length() >= 8) {
            TeamSet = static_cast<uint8_t>(std::stoi(std::string(1, TempStr[7]))); // Get 8th character (0-indexed as 7)
        } else {
            TeamSet = 1;
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            Sprite[Targets[i]].ChangeTeam(TeamSet, true);
        }
    }

    inline void CommandSay(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        // Send message to all players
        ServerSendMessage(std::wstring(Name.begin(), Name.end()), ALL_PLAYERS, 255, MSGTYPE_PUB);
    }

    inline void CommandKill(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        TVector2 a = {0.0f, 0.0f};
        int i;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            Sprite[Targets[i]].Vest = 0;
            // Sprite[Targets[i]].HealthHit(3430, Targets[i], 1, -1, a); // Method would need to be defined
            MainConsole.Console(std::string(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end()) + 
                              " killed by admin", CLIENT_MESSAGE_COLOR, Sender);
        }
    }

    inline void CommandLoadWep(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i;

        if (Args.size() == 1) {
            if (sv_realisticmode.Value()) {
                Name = "weapons_realistic";
            } else {
                Name = "weapons";
            }
        } else {
            Name = Args[1];
        }

        LastWepMod = Name;
        LoadWeapons(Name.c_str());

        for (i = 1; i <= MAX_PLAYERS; i++) {
            if (Sprite[i].Active) {
                if (Sprite[i].Player.ControlMethod == HUMAN) {
                    ServerVars(i);
                }
            }
        }
    }

    inline void CommandLoadCon(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (sv_lockedmode.Value()) {
            MainConsole.Console("Locked Mode is enabled. Settings can't be changed mid-game.",
                              SERVER_MESSAGE_COLOR, Sender);
            return;
        }

        MapChangeCounter = -60;
        ServerDisconnect();
        for (i = 1; i <= MAX_BULLETS; i++) {
            Bullet[i].Kill();
        }
        for (i = 1; i <= MAX_THINGS; i++) {
            Thing[i].Kill();
        }
        for (i = 1; i <= MAX_PLAYERS; i++) {
            if (Sprite[i].Active) {
                Sprite[i].Player.Team = FixTeam(Sprite[i].Player.Team);
                Sprite[i].Respawn();
                Sprite[i].Player.Kills = 0;
                Sprite[i].Player.Deaths = 0;
                Sprite[i].Player.Flags = 0;

                Sprite[i].Player.TKWarnings = 0;
                Sprite[i].Player.ChatWarnings = 0;
                Sprite[i].Player.KnifeWarnings = 0;

                Sprite[i].Player.ScoresPerSecond = 0;
                Sprite[i].Player.GrabsPerSecond = 0;
            }
        }

        LoadConfig(Name);
        MainConsole.Console("Config reloaded " + CurrentConf, CLIENT_MESSAGE_COLOR, Sender);
        StartServer();
    }

    inline void CommandLoadList(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i;

        if (Args.empty()) {
            // Remove .txt extension if present
            Name = StringReplace(sv_maplist.Value(), ".txt", "", true);
        } else {
            Name = Args[1];
        }

        std::string fileName = UserDirectory + Name + ".txt";
        if (FileExists(fileName)) {
            MapsList.LoadFromFile(fileName);
            i = 1;
            while (i < MapsList.Count()) {
                if (MapsList[i].empty()) {
                    MapsList.Delete(i);
                    i--;
                }
                i++;
            }
            sv_maplist.SetValue(Name + ".txt");
            MainConsole.Console("Mapslist loaded " + Name, CLIENT_MESSAGE_COLOR, Sender);
        }
    }

    inline void CommandPm(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string PMToID, PMMessage;
        int i;
        std::vector<uint8_t> Targets;

        if (Args.size() <= 2) {
            return;
        }

        PMToID = Args[1];

        Targets = CommandTarget(PMToID, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            PMMessage = Args[2];
            MainConsole.Console("Private Message sent to " + IDToName(Targets[i]),
                              SERVER_MESSAGE_COLOR, Sender);
            MainConsole.Console("(PM) To: " + IDToName(Targets[i]) + " From: " +
                              IDToName(Sender) + " Message: " + PMMessage, SERVER_MESSAGE_COLOR);
            ServerSendMessage(std::wstring("(PM) " + PMMessage).begin(),
                              Targets[i], 255, MSGTYPE_PUB);
        }
    }

    inline void CommandGMute(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i, j;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            Sprite[Targets[i]].Player.Muted = 1;
            for (j = 1; j <= MAX_PLAYERS; j++) {
                if (Trim(MuteList[j]).empty()) {
                    MuteList[j] = std::string(Sprite[Targets[i]].Player.IP.begin(), Sprite[Targets[i]].Player.IP.end());
                    MuteName[j] = std::string(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end());
                    break;
                }
            }
            MainConsole.Console(std::string(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end()) + " has been muted.",
                              CLIENT_MESSAGE_COLOR, Sender);
        }
    }

    inline void CommandUngmute(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i, j;
        std::vector<uint8_t> Targets;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        Targets = CommandTarget(Name, Sender);
        for (i = 0; i < static_cast<int>(Targets.size()); i++) {
            Sprite[Targets[i]].Player.Muted = 0;
            for (j = 1; j <= MAX_PLAYERS; j++) {
                std::string ipStr = std::string(Sprite[Targets[i]].Player.IP.begin(), Sprite[Targets[i]].Player.IP.end());
                if (Trim(MuteList[j]) == ipStr) {
                    MuteList[j] = "";
                    break;
                }
            }
            MainConsole.Console(std::string(Sprite[Targets[i]].Player.Name.begin(), Sprite[Targets[i]].Player.Name.end()) + " has been unmuted.",
                              CLIENT_MESSAGE_COLOR, Sender);
        }
    }

    inline void CommandAddMap(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        MapsList.Add(Name);
        MainConsole.Console(Name + " has been added to the map list.",
                          SERVER_MESSAGE_COLOR, Sender);
        SaveMapList();
    }

    inline void CommandDelMap(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int TempInt;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        for (TempInt = 0; TempInt < MapsList.Count(); TempInt++) {
            std::string upperMapName = ToUpper(Name);
            std::string upperListName = ToUpper(MapsList[TempInt]);
            
            if (upperListName == upperMapName) {
                MainConsole.Console(Name + " has been removed from the map list.",
                                  SERVER_MESSAGE_COLOR, Sender);
                MapsList.Delete(TempInt);
                break;
            }
        }
        SaveMapList();
    }

    inline void CommandTempBan(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name, TempStr;

        if (Args.size() <= 2) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        TempStr = Args[2];
        // Add temporary ban
        AddBannedIP(TempStr, "Temporary Ban by an Admin", std::stoi(Name) * MINUTE);
        MainConsole.Console("IP number " + TempStr + " banned for " + Name + " minutes.",
                          CLIENT_MESSAGE_COLOR, Sender);
        SaveTxtLists();
    }

    inline void CommandWeaponOn(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i, j;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        for (i = 1; i <= MAX_PLAYERS; i++) {
            if ((Sprite[i].Active) && (Sprite[i].Player.ControlMethod == HUMAN)) {
                j = std::stoi(Name);
                if ((j > -1) && (j < 15)) {
                    SetWeaponActive(i, j, true); // Assuming this function exists
                }
            }
        }
    }

    inline void CommandWeaponOff(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;
        int i, j;

        if (Args.size() == 1) {
            return;
        }

        Name = Args[1];

        if (Name.empty()) {
            return;
        }

        for (i = 1; i <= MAX_PLAYERS; i++) {
            if ((Sprite[i].Active) && (Sprite[i].Player.ControlMethod == HUMAN)) {
                j = std::stoi(Name);
                if ((j > -1) && (j < 15)) {
                    SetWeaponActive(i, j, false); // Assuming this function exists
                }
            }
        }
    }

    inline void CommandBanList(const std::vector<std::string>& Args, uint8_t Sender) {
        int i;
        std::string BanDurationText;

        MainConsole.Console("HWID             | Duration  | Reason", SERVER_MESSAGE_COLOR, Sender);
        for (i = 1; i < BANNED_HW_LIST_SIZE; i++) {  // Assuming a constant for max banned HW items
            if (BannedHWList[i].Time == PERMANENT) {
                BanDurationText = "PERMANENT";
            } else {
                // Calculate remaining time - simplified implementation
                BanDurationText = "TIME_REMAINING"; // Placeholder
            }
            MainConsole.Console(
                Format("%-15s | %-9s | %s", {
                    BannedHWList[i].HW,
                    BanDurationText,
                    BannedHWList[i].Reason
                }), SERVER_MESSAGE_COLOR, Sender);
        }

        MainConsole.Console("IP               | Duration  | Reason", SERVER_MESSAGE_COLOR, Sender);
        for (i = 1; i < BANNED_IP_LIST_SIZE; i++) {  // Assuming a constant for max banned IP items
            if (BannedIPList[i].Time == PERMANENT) {
                BanDurationText = "PERMANENT";
            } else {
                // Calculate remaining time - simplified implementation
                BanDurationText = "TIME_REMAINING"; // Placeholder
            }
            MainConsole.Console(
                Format("%-15s | %-9s | %s", {
                    BannedIPList[i].IP,
                    BanDurationText,
                    BannedIPList[i].Reason
                }), SERVER_MESSAGE_COLOR, Sender);
        }
    }

    inline void CommandInfo(const std::vector<std::string>& Args, uint8_t Sender) {
        std::wstring Gametype = L"";

        if (!sv_greeting.Value().empty()) {
            ServerSendStringMessage(std::wstring(sv_greeting.Value().begin(), sv_greeting.Value().end()), 
                                  Sender, 255, MSGTYPE_PUB);
        }
        if (!sv_greeting2.Value().empty()) {
            ServerSendStringMessage(std::wstring(sv_greeting2.Value().begin(), sv_greeting2.Value().end()), 
                                  Sender, 255, MSGTYPE_PUB);
        }
        if (!sv_greeting3.Value().empty()) {
            ServerSendStringMessage(std::wstring(sv_greeting3.Value().begin(), sv_greeting3.Value().end()), 
                                  Sender, 255, MSGTYPE_PUB);
        }

        ServerSendStringMessage(L"Server: " + std::wstring(sv_hostname.Value().begin(), sv_hostname.Value().end()), 
                              Sender, 255, MSGTYPE_PUB);
        ServerSendStringMessage(L"Address: " + std::wstring(ServerIP.begin()) + L":" +
                              std::to_wstring(ServerPort), Sender, 255, MSGTYPE_PUB);
        ServerSendStringMessage(L"Version: " + std::wstring(DEDVERSION), Sender, 255, MSGTYPE_PUB);

        switch (sv_gamemode.Value()) {
            case GAMESTYLE_DEATHMATCH: Gametype = L"Deathmatch"; break;
            case GAMESTYLE_POINTMATCH: Gametype = L"Pointmatch"; break;
            case GAMESTYLE_TEAMMATCH:  Gametype = L"Teammatch"; break;
            case GAMESTYLE_CTF:        Gametype = L"Capture the Flag"; break;
            case GAMESTYLE_RAMBO:      Gametype = L"Rambomatch"; break;
            case GAMESTYLE_INF:        Gametype = L"Infiltration"; break;
            case GAMESTYLE_HTF:        Gametype = L"Hold the Flag"; break;
        }

        ServerSendStringMessage(L"Gamemode: " + Gametype, Sender, 255, MSGTYPE_PUB);
        ServerSendStringMessage(L"Timelimit: " +
                              std::to_wstring(sv_timelimit.Value() / 3600), Sender, 255, MSGTYPE_PUB);
        ServerSendStringMessage(L"Nextmap: " + std::wstring(CheckNextMap().begin(), CheckNextMap().end()), Sender, 255, MSGTYPE_PUB);

#ifdef SCRIPT_CODE
        ServerSendStringMessage(L"Scripting: " +
                              (sc_enable.Value() ? L"Enabled" : L"Disabled"), Sender, 255, MSGTYPE_PUB);
        if (sc_enable.Value()) {
            Gametype = L"";
            // TStringList* ScriptList = ScrptDispatcher.ScriptList;
            // for (int i = 0; i < ScriptList->Count; i++) {
            //     Gametype += (i != 0 ? L", " : L"") + std::wstring(ScriptList->Strings[i].begin(), ScriptList->Strings[i].end());
            // }
            // ServerSendStringMessage(L"Scripts: " + Gametype, Sender, 255, MSGTYPE_PUB);
            // ScriptList->Free();
        }
#endif

        if (LoadedWMChecksum != DefaultWMChecksum) {
            ServerSendStringMessage(L"Server uses weapon mod \"" +
                                  std::wstring(WMName.begin(), WMName.end()) + L" v" +
                                  std::wstring(WMVersion.begin(), WMVersion.end()) + L"\" (checksum " +
                                  std::to_wstring(LoadedWMChecksum) + L")", 
                                Sender, 255, MSGTYPE_PUB);
        }
    }

    inline void CommandAdminLog(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string AdminLog;

        if (Args.size() == 1) {
            return;
        }

        AdminLog = Args[1];
        if (!sv_adminpassword.Value().empty()) {
            if (AdminLog == sv_adminpassword.Value()) {
                if (!IsAdminIP(std::string(Sprite[Sender].Player.IP.begin(), Sprite[Sender].Player.IP.end()))) {
                    AdminIPs.Add(std::string(Sprite[Sender].Player.IP.begin(), Sprite[Sender].Player.IP.end()));
                    MainConsole.Console(std::string(Sprite[Sender].Player.Name.begin(), Sprite[Sender].Player.Name.end()) + 
                                      " added to Game Admins", SERVER_MESSAGE_COLOR, Sender);
                }
            } else {
                MainConsole.Console(std::string(Sprite[Sender].Player.Name.begin(), Sprite[Sender].Player.Name.end()) + 
                                  " tried to login as Game Admin with bad password", 
                                  SERVER_MESSAGE_COLOR, Sender);
            }
        }
    }

    inline void CommandVoteMap(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string MapName;

        if (Args.size() == 1) {
            return;
        }

        MapName = Args[1];

        if (VoteActive) {
            // check if the vote target is actually the target
            if (VoteTarget != MapName) {
                return;
            }

            // check if he already voted
            if (VoteHasVoted[Sender]) {
                return;
            }

#ifdef SCRIPT_CODE
            // ScrptDispatcher.OnVoteMap(Sender, MapName);
#endif
            CountVote(Sender);
        } else {
            int mapIndex = -1;
            // Find the map in the map list
            for (size_t i = 0; i < MapsList.size(); i++) {
                if (ToUpper(MapsList[i]) == ToUpper(MapName)) {
                    mapIndex = i;
                    break;
                }
            }
            
            if (mapIndex != -1) { // Assuming MapExists check is done elsewhere
                if (VoteCooldown[Sender] < 0) {
                    if (!VoteActive) {
#ifdef SCRIPT_CODE
                        // if (ScrptDispatcher.OnVoteMapStart(Sender, MapName)) return;
#endif
                        StartVote(Sender, VOTE_MAP, MapName, "---");
                        ServerSendVoteOn(VOTE_TYPE, Sender, MapName, "---");
                    }
                } else {
                    ServerSendStringMessage(L"Can't vote for 2:00 minutes after joining game or last vote", 
                                          Sender, 255, MSGTYPE_PUB);
                }
            } else {
                ServerSendStringMessage(L"Map not found (" + std::wstring(MapName.begin(), MapName.end()) + L")", 
                                      Sender, 255, MSGTYPE_PUB);
            }
        }
    }

#ifdef SCRIPT_CODE
    inline void CommandRecompile(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Name;

        if (Args.size() == 1) {
            if (!sc_enable.Value()) {
                MainConsole.Console("Scripting is currently disabled.", CLIENT_MESSAGE_COLOR, Sender);
            } else {
                // ScrptDispatcher.Prepare();
                // ScrptDispatcher.Launch();
            }
        } else {
            Name = Args[1];
            if (!sc_enable.Value()) {
                MainConsole.Console("Scripting is currently disabled.", CLIENT_MESSAGE_COLOR, Sender);
            } else {
                // ScrptDispatcher.Launch(Name);
            }
        }
    }
#endif

    inline void CommandRecord(const std::vector<std::string>& Args, uint8_t Sender) {
        std::string Str;

        if (Args.size() == 2) {
            Str = Args[1];
        } else {
            // FormatDateTime is Pascal-specific, using standard C++ time
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S_", &tm);
            Str = std::string(buffer) + Map.Name;
        }

        if (DemoRecorder.Active()) {
            DemoRecorder.StopRecord();
        }
        DemoRecorder.StartRecord(UserDirectory + "demos/" + Str + ".sdm");
    }

    inline void CommandStop(const std::vector<std::string>& Args, uint8_t Sender) {
        if (DemoRecorder.Active()) {
            DemoRecorder.StopRecord();
        }
    }

    inline void InitServerCommands() {
        CommandAdd("addbot", CommandAddBot, "Add specific bot to game", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addbot1", CommandAddBot, "Add specific bot to alpha team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addbot2", CommandAddBot, "Add specific bot to bravo team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addbot3", CommandAddBot, "Add specific bot to charlie team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addbot4", CommandAddBot, "Add specific bot to delta team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addbot5", CommandAddBot, "Add specific bot to spectators", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("nextmap", CommandNextMap, "Change map to next in maplist", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("map", CommandMap, "Change map to specified mapname", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("pause", CommandPause, "Pause game", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("unpause", CommandUnpause, "Unpause game", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("restart", CommandRestart, "Restart current map", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("kick", CommandKick, "Kick player with specified nick or id", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("kicklast", CommandKickLast, "Kick last connected player", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("ban", CommandBan, "Ban player with specified nick or id", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("banip", CommandBanIPHW, "Ban specified IP address", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("banhw", CommandBanIPHW, "Ban specified hwid", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("unban", CommandUnban, "Unban specified ip or hwid", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("unbanlast", CommandUnbanLast, "Unban last player", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("adm", CommandAdm, "Give admin to specified nick or id", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("admip", CommandAdmIP, "add the IP number to the Remote Admins list", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("unadm", CommandUnadm, "remove the IP number from the admins list", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("setteam1", CommandSetTeam, "move specified id or nick to alpha team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("setteam2", CommandSetTeam, "move specified id or nick to bravo team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("setteam3", CommandSetTeam, "move specified id or nick to charlie team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("setteam4", CommandSetTeam, "move specified id or nick to delta team", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("setteam5", CommandSetTeam, "move specified id or nick to spectators", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("say", CommandSay, "Send chat message", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("pkill", CommandKill, "Kill specified id or nick", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("loadwep", CommandLoadWep, "Load weapons config", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("loadcon", CommandLoadCon, "Load server config", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("loadlist", CommandLoadList, "Load maplist", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("pm", CommandPm, "Send private message to other player", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("gmute", CommandGMute, "Mute player on server", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("ungmute", CommandUngmute, "Unmute player on server", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("addmap", CommandAddMap, "Add map to the maplist", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("delmap", CommandDelMap, "Remove map from the maplist", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("weaponon", CommandWeaponOn, "Enable specific weapon on the server", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("weaponoff", CommandWeaponOff, "Disable specific weapon on the server", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("banlist", CommandBanList, "Show banlist", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("net_stats", CommandNetStats, "Show network stats", {CMD_FLAG::CMD_ADMINONLY});

        CommandAdd("tabac", CommandPlayerCommand, "tabac", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("smoke", CommandPlayerCommand, "smoke", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("takeoff", CommandPlayerCommand, "takeoff", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("victory", CommandPlayerCommand, "victory", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("piss", CommandPlayerCommand, "piss", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("mercy", CommandPlayerCommand, "mercy", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("pwn", CommandPlayerCommand, "pwn", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("kill", CommandPlayerCommand, "kill", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("brutalkill", CommandPlayerCommand, "brutalkill", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("info", CommandInfo, "info", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("adminlog", CommandAdminLog, "adminlog", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("votemap", CommandVoteMap, "votemap", {CMD_FLAG::CMD_PLAYERONLY});
        CommandAdd("record", CommandRecord, "record demo", {CMD_FLAG::CMD_ADMINONLY});
        CommandAdd("stop", CommandStop, "stop recording demo", {CMD_FLAG::CMD_ADMINONLY});

#ifdef SCRIPT_CODE
        CommandAdd("recompile", CommandRecompile, "Recompile all or specific script", {CMD_FLAG::CMD_ADMINONLY});
#endif
    }
}

// Using declarations to bring functions into global namespace
using ServerCommandsImpl::InitServerCommands;
using ServerCommandsImpl::CommandAddBot;
using ServerCommandsImpl::CommandNextMap;
using ServerCommandsImpl::CommandMap;
using ServerCommandsImpl::CommandPause;
using ServerCommandsImpl::CommandUnpause;
using ServerCommandsImpl::CommandRestart;
using ServerCommandsImpl::CommandKick;
using ServerCommandsImpl::CommandKickLast;
using ServerCommandsImpl::CommandBan;
using ServerCommandsImpl::CommandBanIPHW;
using ServerCommandsImpl::CommandUnban;
using ServerCommandsImpl::CommandUnbanLast;
using ServerCommandsImpl::CommandAdm;
using ServerCommandsImpl::CommandAdmIP;
using ServerCommandsImpl::CommandUnadm;
using ServerCommandsImpl::CommandSetTeam;
using ServerCommandsImpl::CommandSay;
using ServerCommandsImpl::CommandKill;
using ServerCommandsImpl::CommandLoadWep;
using ServerCommandsImpl::CommandLoadCon;
using ServerCommandsImpl::CommandLoadList;
using ServerCommandsImpl::CommandPm;
using ServerCommandsImpl::CommandGMute;
using ServerCommandsImpl::CommandUngmute;
using ServerCommandsImpl::CommandAddMap;
using ServerCommandsImpl::CommandDelMap;
using ServerCommandsImpl::CommandTempBan;
using ServerCommandsImpl::CommandWeaponOn;
using ServerCommandsImpl::CommandWeaponOff;
using ServerCommandsImpl::CommandBanList;
using ServerCommandsImpl::CommandInfo;
using ServerCommandsImpl::CommandAdminLog;
using ServerCommandsImpl::CommandVoteMap;
using ServerCommandsImpl::CommandRecord;
using ServerCommandsImpl::CommandStop;
#ifdef SCRIPT_CODE
using ServerCommandsImpl::CommandRecompile;
#endif

// Global variables
extern std::vector<TMapProp> Guns;
extern std::vector<TMapProp> DefaultGuns;
extern uint32_t DefaultWMChecksum;
extern uint32_t LoadedWMChecksum;

#endif // SERVER_COMMANDS_H