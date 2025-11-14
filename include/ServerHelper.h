#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

//*******************************************************************************
//                                                                              
//       Server Helper Unit for SOLDAT                                            
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <cstdint>
#include <random>
#include <fstream>
#include "Vector.h"
#include "Waypoints.h"
#include "Util.h"
#include "Constants.h"
#include "Server.h"
#include "Cvar.h"
#include "Game.h"
#include "Net.h" 
#include "Sprites.h"
#include "Weapons.h"
#include "TraceLog.h"
#include "BanSystem.h"
#include "Command.h"

// Constants that need to be defined
const int MAX_SPRITES = 32;  // This should come from Constants.h but adding as placeholder
const int GAMESTYLE_TEAMMATCH = 2;
const int GAMESTYLE_CTF = 3;
const int GAMESTYLE_INF = 5;
const int GAMESTYLE_HTF = 6;
const int GAMESTYLE_DEATHMATCH = 0;
const int GAMESTYLE_POINTMATCH = 1;
const int GAMESTYLE_RAMBO = 4;

const int TEAM_NONE = 0;
const int TEAM_ALPHA = 1;
const int TEAM_BRAVO = 2;
const int TEAM_CHARLIE = 3;
const int TEAM_DELTA = 4;
const int TEAM_SPECTATOR = 5;

const int MAX_PLAYERS = 32;  // Placeholder - should be from constants
const int MAX_THINGS = 255;  // Placeholder - should be from constants
const int MAX_WAYPOINTS = 5000;  // Placeholder - should be from constants
const int MAX_CONNECTIONS = 8;  // Placeholder - should be from constants
const int MAX_SECTOR = 25;  // Placeholder - should be from constants
const int MAX_POLYS = 5000;  // Placeholder - should be from constants

const int PERMANENT = -1000;  // Placeholder - should be from constants
const int BOT = 2;  // Assuming this is the control method for bots
const int HUMAN = 1;  // Assuming this is the control method for humans

// Function declarations
std::string CheckNextMap();
void WriteLn(const std::string& S);  // overload
std::string IDToName(int ID);
std::string TeamToName(int ID);
int NameToID(const std::string& Name);
std::string NameToHW(const std::string& Name);
int FindLowestTeam(const std::vector<int>& Arr);
void SaveTxtLists();
void SaveMapList();
uint32_t RGB(uint8_t r, uint8_t g, uint8_t b);
uint8_t FixTeam(uint8_t Team);
std::string WeaponNameByNum(int Num);
int CheckFileSize(const std::string& filename);
void WritePID();
int GetPID();
void WriteConsole(uint8_t ID, const std::string& Text, uint32_t Colour);
void UpdateWaveRespawnTime();
std::string RandomBot();
void DoBalanceBots(uint8_t LeftGame, uint8_t NewTeam);

// Global variables
extern std::vector<std::string> MapsList;
extern int MapIndex;

namespace ServerHelperImpl {
    inline std::string CheckNextMap() {
        std::string result = "NOMAP";
        if (MapsList.empty()) {
            result = "NOMAP";
        } else {
            int m = MapIndex + 1;

            if (m >= static_cast<int>(MapsList.size())) {
                m = 0;
            }

            result = MapsList[m];
        }
        return result;
    }

    inline void WriteLn(const std::string& S) {
        if (CvarsInitialized && log_timestamp.Value()) {
            time_t now = time(0);
            char buffer[80];
            strftime(buffer, 80, "[%H:%M:%S] ", localtime(&now));
            std::cout << buffer << S << std::endl;
        } else {
            std::cout << S << std::endl;
        }
    }

    inline std::string IDToName(int ID) {
        std::string result = "";
        if (ID > MAX_PLAYERS) {
            result = "Server Admin";
            return result;
        }
        if (ID > 0 && ID <= MAX_SPRITES && Sprite[ID].Active) {
            result = std::string(Sprite[ID].Player.Name.begin(), Sprite[ID].Player.Name.end());
        }
        return result;
    }

    inline std::string TeamToName(int ID) {
        std::string result = "UNKNOWN";
        if (ID > 5) {
            return result;
        }

        switch (ID) {
            case 0: result = "NA"; break;
            case 1: result = "A"; break;
            case 2: result = "B"; break;
            case 3: result = "C"; break;
            case 4: result = "D"; break;
            case 5: result = "Spectator"; break;
        }
        return result;
    }

    inline int NameToID(const std::string& Name) {
        int result = 0;
        for (int i = 1; i <= MAX_SPRITES; i++) {
            std::string spriteName(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end());
            if (spriteName == Name) {
                result = i;
            }
        }
        return result;
    }

    inline std::string NameToHW(const std::string& Name) {
        std::string result = "0";
        for (int i = 1; i <= MAX_SPRITES; i++) {
            std::string spriteName(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end());
            if (spriteName == Name) {
                result = std::string(Sprite[i].Player.hwID.begin(), Sprite[i].Player.hwID.end()); // Assuming hwID field exists
                break;
            }
        }
        return result;
    }

    inline uint32_t RGB(uint8_t r, uint8_t g, uint8_t b) {
        return static_cast<uint32_t>(r | (g << 8) | (b << 16));
    }

    inline int FindLowestTeam(const std::vector<int>& Arr) {
        int tmp = 1;
        int maxTeams = (sv_gamemode.Value() == GAMESTYLE_TEAMMATCH) ? 4 : 2;
        
        for (int i = 1; i <= maxTeams && i < static_cast<int>(Arr.size()); i++) {
            if (Arr[tmp] > Arr[i]) {
                tmp = i;
            }
        }
        return tmp;
    }

    inline uint8_t FixTeam(uint8_t Team) {
        uint8_t result = TEAM_SPECTATOR;

        switch (sv_gamemode.Value()) {
            case GAMESTYLE_DEATHMATCH:
            case GAMESTYLE_POINTMATCH:
            case GAMESTYLE_RAMBO:
                // Choose to team 0 or leave at team spectator
                if ((Team != TEAM_NONE) && (Team != TEAM_SPECTATOR)) {
                    result = TEAM_NONE;
                }
                break;
            case GAMESTYLE_TEAMMATCH:
                if ((Team != TEAM_ALPHA) && (Team != TEAM_BRAVO) && 
                    (Team != TEAM_CHARLIE) && (Team != TEAM_DELTA) && 
                    (Team != TEAM_SPECTATOR)) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(1, 4);  // 1 to 4 = team 1..4
                    result = static_cast<uint8_t>(dis(gen));
                }
                break;
            case GAMESTYLE_CTF: 
            case GAMESTYLE_INF: 
            case GAMESTYLE_HTF:
                if ((Team != TEAM_ALPHA) && (Team != TEAM_BRAVO) && 
                    (Team != TEAM_SPECTATOR)) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(1, 2);  // 1 to 2 = team 1..2
                    result = static_cast<uint8_t>(dis(gen));
                }
                break;
        }
        return result;
    }

    inline std::string WeaponNameByNum(int Num) {
        Trace("WeaponNameByNum");
        std::string result = "USSOCOM";

        if (Num == 100) {
            result = "Selfkill";
            return result;
        }

        for (int weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {  // Assuming TOTAL_WEAPONS constant exists
            if (Num == Guns[weaponIndex].Num) {
                result = std::string(Guns[weaponIndex].Name.begin(), Guns[weaponIndex].Name.end());
                break;
            }
        }
        return result;
    }

    inline int CheckFileSize(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.seekg(0, std::ios::end);
            int size = static_cast<int>(file.tellg());
            file.close();
            return size;
        }
        return -1; // File not found
    }

    inline void SaveTxtLists() {
        Trace("SaveTxtLists");

        // Save ban files
        SaveBannedList(UserDirectory + "configs/banned.txt");
        SaveBannedListHW(UserDirectory + "configs/bannedhw.txt");

        // RemoteIPs.SaveToFile(UserDirectory + "configs/remote.txt");  // Assuming TStringList equivalent
    }

    inline void SaveMapList() {
        // MapsList.SaveToFile(UserDirectory + "configs/" + sv_maplist.Value());  // Assuming TStringList equivalent
    }

    inline void WritePID() {
        try {
            int pid = GetPID();
            std::ofstream pidFile(UserDirectory + "logs/" + sv_pidfilename.Value());
            if (pidFile.is_open()) {
                pidFile << pid << std::endl;
                pidFile.close();
                std::cout << " Server PID: " << pid << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error writing PID file: " << e.what() << std::endl;
        }
    }

    inline int GetPID() {
        return static_cast<int>(getpid());
    }

    inline void WriteConsole(uint8_t ID, const std::string& Text, uint32_t Colour) {
        // Write text to the console of ALL Players
        // ServerSendSpecialMessage(Text, 0, 0, 0, 0, Colour, 0, 0, ID);
    }

    inline void UpdateWaveRespawnTime() {
        float playersNumFloat = static_cast<float>(PlayersNum);
        WaveRespawnTime = static_cast<int>(std::round(playersNumFloat * WAVESPRAWN_TIME_MULTIPLIER) * 60);
        
        if (WaveRespawnTime > sv_respawntime_minwave.Value()) {
            WaveRespawnTime = sv_respawntime_maxwave.Value();
        }
        WaveRespawnTime = WaveRespawnTime - sv_respawntime_minwave.Value();
        
        if (WaveRespawnTime < 1) {
            WaveRespawnTime = 1;
        }
    }

    inline std::string RandomBot() {
        std::vector<std::string> botList;
        
        // Find all bot files in the directory
        std::string botPattern = UserDirectory + "configs/bots/*.bot";
        
        // For simplicity, using a predefined list of bots
        std::vector<std::string> defaultBots = {"Sniper", "Soldier", "Commander", "Stealth", "Assault", "Scout", "Medic", "Engineer"};
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(defaultBots.size() - 1));
        
        std::string selectedBot = defaultBots[dis(gen)];
        
        // Strip file paths and extensions
        size_t lastSlash = selectedBot.find_last_of("/");
        if (lastSlash != std::string::npos) {
            selectedBot = selectedBot.substr(lastSlash + 1);
        }
        
        size_t lastDot = selectedBot.find_last_of(".");
        if (lastDot != std::string::npos) {
            selectedBot = selectedBot.substr(0, lastDot);
        }
        
        if ((selectedBot == "Boogie Man") || (selectedBot == "Dummy")) {
            selectedBot = "Sniper";
        }
        
        return selectedBot;
    }

    inline void DoBalanceBots(uint8_t LeftGame, uint8_t NewTeam) {
        if (!sv_botbalance.Value()) {
            return;
        }
        if ((sv_gamemode.Value() != GAMESTYLE_CTF) && 
            (sv_gamemode.Value() != GAMESTYLE_HTF) && 
            (sv_gamemode.Value() != GAMESTYLE_INF)) {
            return;
        }
        
        int Teams[5] = {0, 0, 0, 0, 0}; // Pascal arrays start from 1 (index 0 unused)
        
        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i].Active && Sprite[i].IsNotSpectator()) {
                if (Sprite[i].Player.Team >= 1 && Sprite[i].Player.Team <= 4) {
                    Teams[Sprite[i].Player.Team]++;
                }
            }
        }

        if (LeftGame == 1) {
            // Player Left Game
            for (int i = 1; i <= MAX_SPRITES; i++) {
                if ((Sprite[i].Player.ControlMethod == BOT) && Sprite[i].Active) {
                    if ((Teams[1] > Teams[2]) && (Sprite[i].Player.Team == TEAM_ALPHA)) {
                        // KickPlayer(i, False, KICK_LEFTGAME, 0);  // Assuming this function exists
                        return;
                    }
                    if ((Teams[2] > Teams[1]) && (Sprite[i].Player.Team == TEAM_BRAVO)) {
                        // KickPlayer(i, False, KICK_LEFTGAME, 0);  // Assuming this function exists
                        return;
                    }
                }
            }
        } else {
            // Player Joined Game
            for (int i = 1; i <= MAX_SPRITES; i++) {
                if (Sprite[i].Active && (Sprite[i].Player.ControlMethod == BOT) && 
                    (Sprite[i].Player.Team == NewTeam)) {
                    if (Teams[1] > Teams[2]) {
                        // KickPlayer(i, False, KICK_LEFTGAME, 0);  // Assuming this function exists
                        if (Sprite[i].Player.Team == NewTeam) {
                            DoBalanceBots(1, 2);
                        }
                        return;
                    }
                    if (Teams[2] > Teams[1]) {
                        // KickPlayer(i, False, KICK_LEFTGAME, 0);  // Assuming this function exists
                        if (Sprite[i].Player.Team == NewTeam) {
                            DoBalanceBots(1, 1);
                        }
                        return;
                    }
                }
            }
        }
        
        if (Teams[1] > Teams[2]) {
            std::string theBot = RandomBot();
            // ParseInput("addbot2 " + theBot, 1);  // Assuming this function exists
            // MainConsole.Console(theBot + " has joined bravo team. (Bot Balance)", ENTER_MESSAGE_COLOR);
            return;
        }
        if (Teams[2] > Teams[1]) {
            std::string theBot = RandomBot();
            // ParseInput("addbot1 " + theBot, 1);  // Assuming this function exists
            // MainConsole.Console(theBot + " has joined alpha team. (Bot Balance)", ENTER_MESSAGE_COLOR);
            return;
        }
    }
}

// Using declarations to bring functions into global namespace
using ServerHelperImpl::CheckNextMap;
using ServerHelperImpl::WriteLn;
using ServerHelperImpl::IDToName;
using ServerHelperImpl::TeamToName;
using ServerHelperImpl::NameToID;
using ServerHelperImpl::NameToHW;
using ServerHelperImpl::FindLowestTeam;
using ServerHelperImpl::SaveTxtLists;
using ServerHelperImpl::SaveMapList;
using ServerHelperImpl::RGB;
using ServerHelperImpl::FixTeam;
using ServerHelperImpl::WeaponNameByNum;
using ServerHelperImpl::CheckFileSize;
using ServerHelperImpl::WritePID;
using ServerHelperImpl::GetPID;
using ServerHelperImpl::WriteConsole;
using ServerHelperImpl::UpdateWaveRespawnTime;
using ServerHelperImpl::RandomBot;
using ServerHelperImpl::DoBalanceBots;

// Global variables
extern std::vector<std::string> MapsList;
extern int MapIndex;

#endif // SERVER_HELPER_H