#include "Server.h"
#include <cstring>
#include <algorithm>
#include <cmath>
#include <random>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>  // Using nlohmann JSON or similar
#include "PhysFS.h"
#include "Net.h"
#include "Game.h"
#include "Sprites.h"
#include "Anims.h"
#include "PolyMap.h"
#include "SharedConfig.h"
#include "TraceLog.h"
#include "Weapons.h"
#include "BanSystem.h"
#include "LogFile.h"
#include "Version.h"
#include "Constants.h"
#include "ServerHelper.h"
#include "ServerCommands.h"
#include "FileServer.h"
#include "LobbyClient.h"
#include "Vector.h"
#include "Util.h"
#include "Cvar.h"
#include "Command.h"

// Include system-specific headers
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/param.h>
#endif

namespace ServerImpl {
    // Global variables
    bool ProgReady = false;
    std::string BaseDirectory = "";
    std::string UserDirectory = "";
    uint32_t MainThreadID = 0;

    // Cvars - these would be defined elsewhere with CVar system
    TBooleanCvar log_enable;
    TIntCvar log_level;
    TIntCvar log_filesupdate;
    TBooleanCvar log_timestamp;

    TBooleanCvar fs_localmount;
    TStringCvar fs_mod;
    TBooleanCvar fs_portable;
    TStringCvar fs_basepath;
    TStringCvar fs_userpath;

    TBooleanCvar demo_autorecord;

    TIntCvar sv_respawntime;
    TIntCvar sv_respawntime_minwave;
    TIntCvar sv_respawntime_maxwave;

    TIntCvar sv_dm_limit;
    TIntCvar sv_pm_limit;
    TIntCvar sv_tm_limit;
    TIntCvar sv_rm_limit;

    TIntCvar sv_inf_limit;
    TIntCvar sv_inf_bluelimit;
    TIntCvar sv_inf_redaward;

    TIntCvar sv_htf_limit;
    TIntCvar sv_htf_pointstime;

    TIntCvar sv_ctf_limit;

    TIntCvar sv_bonus_frequency;
    TBooleanCvar sv_bonus_flamer;
    TBooleanCvar sv_bonus_predator;
    TBooleanCvar sv_bonus_berserker;
    TBooleanCvar sv_bonus_vest;
    TBooleanCvar sv_bonus_cluster;

    TBooleanCvar sv_stationaryguns;

    TStringCvar sv_password;
    TStringCvar sv_adminpassword;
    TIntCvar sv_maxplayers;
    TIntCvar sv_maxspectators;
    TBooleanCvar sv_spectatorchat;
    TStringCvar sv_greeting;
    TStringCvar sv_greeting2;
    TStringCvar sv_greeting3;
    TStringCvar sv_info;
    TIntCvar sv_minping;
    TIntCvar sv_maxping;
    TIntCvar sv_votepercent;
    TBooleanCvar sv_lockedmode;
    TStringCvar sv_pidfilename;
    TStringCvar sv_maplist;
    TBooleanCvar sv_lobby;
    TStringCvar sv_lobbyurl;

    TBooleanCvar sv_steamonly;

#ifdef STEAM_CODE
    TBooleanCvar sv_voicechat;
    TBooleanCvar sv_voicechat_alltalk;
    TStringCvar sv_setsteamaccount;
#endif

    TIntCvar sv_warnings_flood;
    TIntCvar sv_warnings_ping;
    TIntCvar sv_warnings_votecheat;
    TIntCvar sv_warnings_knifecheat;
    TIntCvar sv_warnings_tk;

    TBooleanCvar sv_anticheatkick;
    TBooleanCvar sv_punishtk;
    TBooleanCvar sv_botbalance;
    TBooleanCvar sv_echokills;
    TBooleanCvar sv_antimassflag;
    TIntCvar sv_healthcooldown;
    TBooleanCvar sv_teamcolors;

    TIntCvar net_port;
    TStringCvar net_ip;
    TStringCvar net_adminip;
    TIntCvar net_lan;
    TBooleanCvar net_compression;
    TBooleanCvar net_allowdownload;
    TIntCvar net_maxconnections;
    TIntCvar net_maxadminconnections;

    TIntCvar net_floodingpacketslan;
    TIntCvar net_floodingpacketsinternet;

    TIntCvar net_t1_snapshot;
    TIntCvar net_t1_majorsnapshot;
    TIntCvar net_t1_deadsnapshot;
    TIntCvar net_t1_heartbeat;
    TIntCvar net_t1_delta;
    TIntCvar net_t1_ping;
    TIntCvar net_t1_thingsnapshot;

    TIntCvar bots_random_noteam;
    TIntCvar bots_random_alpha;
    TIntCvar bots_random_bravo;
    TIntCvar bots_random_charlie;
    TIntCvar bots_random_delta;
    TIntCvar bots_difficulty;
    TBooleanCvar bots_chat;

    TBooleanCvar sc_enable;
    TStringCvar sc_onscriptcrash;
    TIntCvar sc_maxscripts;
    TBooleanCvar sc_safemode;

    TBooleanCvar fileserver_enable;
    TIntCvar fileserver_port;
    TStringCvar fileserver_ip;

    // syncable cvars
    TIntCvar sv_gamemode;
    TBooleanCvar sv_friendlyfire;
    TIntCvar sv_timelimit;
    TIntCvar sv_maxgrenades;
    TBooleanCvar sv_bullettime;
    TBooleanCvar sv_sniperline;
    TBooleanCvar sv_balanceteams;
    TBooleanCvar sv_guns_collide;
    TBooleanCvar sv_kits_collide;
    TBooleanCvar sv_survivalmode;
    TBooleanCvar sv_survivalmode_antispy;
    TBooleanCvar sv_survivalmode_clearweapons;
    TBooleanCvar sv_realisticmode;
    TBooleanCvar sv_advancemode;
    TIntCvar sv_advancemode_amount;
    TBooleanCvar sv_minimap;
    TBooleanCvar sv_advancedspectator;
    TBooleanCvar sv_radio;
    TStringCvar sv_info;
    TFloatCvar sv_gravity;
    TStringCvar sv_hostname;
    TIntCvar sv_killlimit;
    TStringCvar sv_downloadurl;
    TBooleanCvar sv_pure;
    TStringCvar sv_website;

#ifdef ENABLE_FAE_CODE
    TBooleanCvar ac_enable;
#endif

    // Global variables
    std::string ServerIP = "127.0.0.1";
    int ServerPort = 23073;
    int BonusFrequency = 3600;
    uint8_t WeaponActive[16] = {0};  // Pascal arrays from -1 to 15
    int WeaponsInGame = 0;

    int BulletWarningCount[MAX_SPRITES + 1] = {0};
    uint8_t CheatTag[MAX_SPRITES + 1] = {0};

    std::vector<std::string> MapsList;
    uint8_t LastPlayer = 0;

    // Bullet snapshot messages
    TBulletSnapshotMsg OldBulletSnapshotMsg[MAX_SPRITES + 1] = {};

    // Mute array
    std::string MuteList[MAX_PLAYERS + 1] = {};  // Pascal arrays start from 1
    std::string MuteName[MAX_PLAYERS + 1] = {};  // Pascal arrays start from 1

    // TK array
    std::string TKList[MAX_PLAYERS + 1] = {};       // Pascal arrays start from 1
    uint8_t TKListKills[MAX_PLAYERS + 1] = {};      // Pascal arrays start from 1

    int64_t TCPBytesSent = 0;
    int64_t TCPBytesReceived = 0;

    // Consoles
    TConsole MainConsole;

    std::unique_ptr<TStringList> RemoteIPs;
    std::unique_ptr<TStringList> AdminIPs;

    std::string FloodIP[1001] = {};  // Pascal arrays start from 1
    int FloodNum[1001] = {};         // Pascal arrays start from 1

    std::string LastReqIP[5] = {};   // Pascal arrays start from 0 (0-4)
    uint8_t LastReqID = 0;
    std::string DropIP = "";

    std::string AdminFloodIP[MAX_ADMIN_FLOOD_IPS + 1] = {};
    std::string LastAdminIPs[MAX_LAST_ADMIN_IPS + 1] = {};
    int AdminIPCounter = 0;

    int WaveRespawnTime, WaveRespawnCounter;

    float ShotDistance = 0.0f;
    float ShotLife = 0.0f;
    int ShotRicochet = 0;

    int HTFTime = HTF_SEC_POINT;

    std::string CurrentConf = "soldat.ini";

    std::string WMName, WMVersion;
    std::string LastWepMod;

    float Grav = 0.06f;

    std::string ModDir = "";

    TServerNetwork* UDP = nullptr;

    std::unique_ptr<TLobbyThread> LobbyThread;

#ifdef STEAM_CODE
    TSteamGS* SteamAPI = nullptr;
#endif

    inline int GetOS() {
#ifdef _WIN32
        return 0;  // Windows
#elif __linux__
        return 1;  // Linux
#elif __APPLE__
        return 2;  // Darwin/MacOS
#else
        return -1; // Unknown OS
#endif
    }

    void ActivateServer() {
        MainThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

        std::cout << std::endl;
        std::cout << "             -= Soldat Dedicated Server " << SOLDAT_VERSION << " - " <<
                     DEDVERSION << " (build " << SOLDAT_VERSION_LONG << ") =-" << std::endl;
        std::cout << std::endl;
        std::cout << "----------------------------------------------------------------" << std::endl;
        std::cout << "         Soldat Dedicated Server initializing..." << std::endl;
        std::cout << "----------------------------------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "   Need help running your server?" << std::endl;
        std::cout << "   Discord: https://discord.gg/soldat" << std::endl;
        std::cout << std::endl;
        std::cout << "   ---> https://forums.soldat.pl/" << std::endl;
        std::cout << std::endl;
        std::cout << "   Additional parameters:" << std::endl;
        std::cout << "   ./soldatserver -net_port PORT -sv_maxplayers MAXPLAYERS -sv_password PASSWORD" << std::endl;
        std::cout << "   Example: ./soldatserver -net_port 23073 -sv_maxplayers 16 -sv_password \"my pass\"" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        // Initialize default format settings
        // DecimalSeparator := '.';
        // DateSeparator := '-';

        ServerTickCounter = 0;
        MainTickCounter = 0;

        // Initialize player dummy objects (cf. DummyPlayer definition for documentation)
        // Assuming DummyPlayer is defined elsewhere
        // DummyPlayer = std::make_unique<TPlayer>();
        // for (int i = 1; i <= MAX_SPRITES; i++) {
        //     Sprite[i].Player = DummyPlayer.get();
        // }

        // Create Consoles
        MainConsole.CountMax = 7;
        MainConsole.ScrollTickMax = 150;
        MainConsole.NewMessageWait = 150;
        MainConsole.AlphaCount = 255;

        // if (GetEnvironmentVariable('COLORTERM') != '') then
        //     MainConsole.TerminalColors = True;

        CvarInit();
        InitServerCommands();
        ParseCommandLine();

        if (fs_basepath.Value().empty()) {
            BaseDirectory = GetExecutablePath(); // Assuming this function exists
        }
        if (fs_userpath.Value().empty()) {
            UserDirectory = GetExecutablePath(); // Assuming this function exists
        }

        Debug("[FS] UserDirectory: " + UserDirectory);
        Debug("[FS] BaseDirectory: " + BaseDirectory);

        SetCurrentDir(UserDirectory); // Assuming this function exists

        if (!PHYSFS_init(nullptr)) {
            std::cout << "Could not initialize PhysFS." << std::endl;
            ProgReady = false;
            sc_enable.SetValue(false);
            return;
        }
        
        std::string basePath = BaseDirectory + "/soldat.smod";
        if (!PHYSFS_mount(basePath.c_str(), "/", 0)) {
            std::cout << "Could not load base game archive (soldat.smod)." << std::endl;
            ProgReady = false;
            sc_enable.SetValue(false);
            return;
        }

        // GameModChecksum = Sha1File(BaseDirectory + "/soldat.smod", 4096);

        ModDir = "";

        if (!fs_mod.Value().empty()) {
            std::string modPath = UserDirectory + "mods/" + ToLower(fs_mod.Value()) + ".smod";
            std::string mountPoint = "mods/" + ToLower(fs_mod.Value()) + "/";
            
            if (!PHYSFS_mount(modPath.c_str(), mountPoint.c_str(), 0)) {
                std::cout << "Could not load mod archive (" << fs_mod.Value() << ")." << std::endl;
                ProgReady = false;
                sc_enable.SetValue(false);
                return;
            }
            ModDir = mountPoint;
            // CustomModChecksum = Sha1File(UserDirectory + "mods/" + ToLower(fs_mod.Value()) + ".smod", 4096);
        }

        // Create the basic folder structure
        CreateDirIfMissing(UserDirectory + "/configs");
        CreateDirIfMissing(UserDirectory + "/demos");
        CreateDirIfMissing(UserDirectory + "/logs");
        CreateDirIfMissing(UserDirectory + "/logs/kills");
        CreateDirIfMissing(UserDirectory + "/maps");
        CreateDirIfMissing(UserDirectory + "/mods");

        // Copy default configs if they are missing
        // PHYSFS_CopyFileFromArchive("configs/server.cfg", (UserDirectory + "/configs/server.cfg").c_str());
        // PHYSFS_CopyFileFromArchive("configs/weapons.ini", (UserDirectory + "/configs/weapons.ini").c_str());
        // PHYSFS_CopyFileFromArchive("configs/weapons_realistic.ini", (UserDirectory + "/configs/weapons_realistic.ini").c_str());

        LoadConfig("server.cfg");

        CvarsInitialized = true;

        NewLogFiles();

        Debug("ActivateServer");

        if (net_ip.Value().empty()) {
            net_ip.SetValue("0.0.0.0");
        }

#ifdef STEAM_CODE
        // SteamAPI initialization would go here
#endif

        ProgReady = true;

#ifndef SCRIPT_CODE
        sc_enable.SetValue("0");
#endif

        // Initialize weapon active array
        for (int i = 1; i <= MAIN_WEAPONS; i++) {
            WeaponActive[i] = 1;
        }

        LoadAnimObjects("");
        if (!ModDir.empty()) {
            LoadAnimObjects(ModDir);
        }

        // Greet!
        std::cout << " Hit CTRL+C to Exit" << std::endl;
        std::cout << " Please command the server using the Soldat Admin program" << std::endl;

        MapChangeCounter = -60;

        SinusCounter = 0;

        AddLineToLogFile(GameLog, "Loading Maps List", ConsoleLogFileName);
        MapsList.clear();

        std::string mapsListFile = UserDirectory + "configs/" + sv_maplist.Value();
        if (FileExists(mapsListFile)) {
            std::ifstream file(mapsListFile);
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    MapsList.push_back(line);
                }
            }
            file.close();
        }

        if (MapsList.empty()) {
            std::cout << std::endl;
            std::cout << "  No maps list found (adding default). " <<
                         "Please add maps in configs/mapslist.txt" << std::endl;
            std::cout << std::endl;
            if (!IsTeamGame()) {
                MapsList.push_back("Arena");
            } else {
                MapsList.push_back("ctf_Ash");
            }
        }

        for (int i = 1; i <= MAX_SPRITES; i++) {
            for (int j = 1; j <= MAX_SPRITES; j++) {
                // OldHelmetMsg[i][j].WearHelmet = 1;
            }
        }

        // Banned IPs text file
        CreateFileIfMissing(UserDirectory + "configs/banned.txt");
        CreateFileIfMissing(UserDirectory + "configs/bannedhw.txt");

        LoadBannedList(UserDirectory + "configs/banned.txt");
        LoadBannedListHW(UserDirectory + "configs/bannedhw.txt");

        RemoteIPs = std::make_unique<TStringList>();
        std::string remoteFile = UserDirectory + "configs/remote.txt";
        if (FileExists(remoteFile)) {
            RemoteIPs->LoadFromFile(remoteFile);
        }

        AdminIPs = std::make_unique<TStringList>();
        AdminIPs->Assign(*RemoteIPs);
        AdminIPs->Add("127.0.0.1");

        // Flood IP stuff
        for (int i = 1; i <= MAX_FLOODIPS; i++) {
            FloodIP[i] = " ";
            FloodNum[i] = 0;
        }

        WeaponsInGame = 0;
        for (int j = 1; j <= MAIN_WEAPONS; j++) {
            if (WeaponActive[j] == 1) {
                WeaponsInGame++;
            }
        }

        std::cout << " Server name: " << sv_hostname.Value() << std::endl;
        UpdateGameStats();
        WriteLogFile(KillLog, KillLogFileName);
        WriteLogFile(GameLog, ConsoleLogFileName);

        RunDeferredCommands();
    }

    void ShutDown() {
        Debug("ShutDown");
        ProgReady = false;

        MainConsole.Console(L"Shutting down server...", GAME_MESSAGE_COLOR);
        // SysUtils.DeleteFile(UserDirectory + "logs/" + sv_pidfilename.Value());

        if (UDP != nullptr) {
            ServerDisconnect();

            MainConsole.Console(L"Shutting down game networking.", GAME_MESSAGE_COLOR);

            delete UDP;  // Assuming UDP is dynamically allocated
            UDP = nullptr;
        }

        // StopFileServer();

#ifdef SCRIPT_CODE
        // ScrptDispatcher.Free();
#endif

#ifdef STEAM_CODE
        Debug("[Steam] Shutdown");
        // SteamAPI->GameServer.Shutdown();
#endif

#ifndef STEAM_CODE
        // GameNetworkingSockets_Kill();
#endif

        try {
            AddLineToLogFile(GameLog, "   End of Log.", ConsoleLogFileName);
            Debug("Updating gamestats");
            UpdateGameStats();
            Debug("Saving killlog");
            WriteLogFile(KillLog, KillLogFileName);
            Debug("Saving gamelog");
            WriteLogFile(GameLog, ConsoleLogFileName);
            Debug("Freeing gamelog");
            // FreeAndNil(GameLog);
            Debug("Freeing killlog");
            // FreeAndNil(KillLog);
        } catch (const std::exception& e) {
            std::cout << "Error on SHUTDOWN during log writing: " << e.what() << std::endl;
        }
    }

    inline bool PrepareMapChange(const std::string& Name) {
        TMapInfo Status;
        
        if (GetMapInfo(Name, UserDirectory, Status)) {
            MapChange = Status;
            MapChangeCounter = MapChangeTime;
            // send to client that map changes
            ServerSendMapChange(ALL_PLAYERS);  // Assuming this function exists
            MainConsole.Console(L"Next map: " + std::wstring(Status.Name.begin(), Status.Name.end()), 
                               GAME_MESSAGE_COLOR);
#ifdef SCRIPT_CODE
            // ScrptDispatcher.OnBeforeMapChange(Status.Name);  // Assuming this function exists
#endif
            return true;
        }
        return false;
    }

    inline void NextMap() {
        Debug("NextMap");

        if (MapsList.empty()) {
            MainConsole.Console(L"Can't load maps from mapslist", GAME_MESSAGE_COLOR);
        } else {
            MapIndex++;
            
            if (MapIndex >= static_cast<int>(MapsList.size())) {
                MapIndex = 0;
            }
            PrepareMapChange(MapsList[MapIndex]);
        }
    }

    inline void SpawnThings(uint8_t Style, uint8_t Amount) {
        Debug("SpawnThings");

        TVector2 a = {0.0f, 0.0f};
        int k = 0;
        switch (Style) {
            case OBJECT_MEDICAL_KIT: k = 8; break;
            case OBJECT_GRENADE_KIT: k = 7; break;
            case OBJECT_FLAMER_KIT: k = 11; break;
            case OBJECT_PREDATOR_KIT: k = 13; break;
            case OBJECT_VEST_KIT: k = 10; break;
            case OBJECT_BERSERK_KIT: k = 12; break;
            case OBJECT_CLUSTER_KIT: k = 9; break;
        }

        for (int i = 1; i <= Amount; i++) {
            int team = 0;
            if (sv_gamemode.Value() == GAMESTYLE_CTF) {
                if ((Style == OBJECT_MEDICAL_KIT) || (Style == OBJECT_GRENADE_KIT)) {
                    if (i % 2 == 0) {
                        team = 1;
                    } else {
                        team = 2;
                    }
                }
            }

            // Thing[MAX_THINGS - 1].Team = team;

            bool spawnSuccess = false;
            if (team == 0) {
                spawnSuccess = RandomizeStart(a, k);
            } else {
                // if (!SpawnBoxes(a, k, MAX_THINGS - 1)) {
                //     spawnSuccess = RandomizeStart(a, k);
                // } else {
                //     spawnSuccess = true;
                // }
            }

            if (!spawnSuccess) continue;

            // Add random offset to position
            a.x += -SPAWNRANDOMVELOCITY + (rand() % static_cast<int>(2 * SPAWNRANDOMVELOCITY * 100)) / 100.0f;
            a.y += -SPAWNRANDOMVELOCITY + (rand() % static_cast<int>(2 * SPAWNRANDOMVELOCITY * 100)) / 100.0f;
            
            // int l = CreateThing(a, 255, Style, 255);

            // if ((l > 0) && (l < MAX_THINGS + 1)) {
            //     Thing[l].Team = team;
            // }
        }
    }

    inline bool KickPlayer(uint8_t Num, bool Ban, int Why, int Time, 
                          const std::string& Reason = "") {
        bool result = false;
        Debug("KickPlayer");

        // Bound check
        if ((Num > MAX_PLAYERS) || (Num < 1)) {
            return result;
        }

        int i = Num;

        if (!Sprite[i].Active) {
            return result;
        }

        if ((Why == KICK_CHEAT) && sv_anticheatkick.Value()) {
            return result;
        }

        // Check if admin should be kicked
        // if ((Why == KICK_PING || Why == KICK_FLOODING || Why == KICK_VOTED) &&
        //      Sprite[i].Player.IP.length() > 5) {
        //     if (IsRemoteAdminIP(Sprite[i].Player.IP) ||
        //         IsAdminIP(Sprite[i].Player.IP)) {
        //         MainConsole.Console(Sprite[i].Player.Name +
        //         " is admin and cannot be kicked.", CLIENT_MESSAGE_COLOR);
        //         return result;
        //     }
        // }

        // Display appropriate message based on reason
        switch (Why) {
            case KICK_LEFTGAME:
                switch (Sprite[i].Player.Team) {
                    case 0: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left the game.", ENTER_MESSAGE_COLOR);
                        break;
                    case 1: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left alpha team.", ALPHAJ_MESSAGE_COLOR);
                        break;
                    case 2: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left bravo team.", BRAVOJ_MESSAGE_COLOR);
                        break;
                    case 3: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left charlie team.", CHARLIEJ_MESSAGE_COLOR);
                        break;
                    case 4: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left delta team.", DELTAJ_MESSAGE_COLOR);
                        break;
                    case 5: 
                        MainConsole.Console(std::wstring(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end()) + 
                                           L" has left spectators", DELTAJ_MESSAGE_COLOR);
                        break;
                }
                break;
            default:
                if (!Ban && Why != KICK_LEFTGAME && Why != KICK_SILENT) {
                    std::wstring name(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end());
                    std::wstring ip(Sprite[i].Player.IP.begin(), Sprite[i].Player.IP.end());
                    
                    MainConsole.Console(name + L" has been kicked." +
                        (Sprite[i].Player.ControlMethod == HUMAN ? L"" : L"(" + ip + L")"), 
                        CLIENT_MESSAGE_COLOR);
                }
                break;
        }

        if (Ban) {
            AddBannedIP(Sprite[i].Player.IP, Reason, Time);
#ifdef STEAM_CODE
            // AddBannedHW(std::to_string(Sprite[i].Player.SteamID.GetAccountID()), Reason, Time);
#else
            // AddBannedHW(Sprite[i].Player.HWid, Reason, Time);
#endif
        }

        if (Ban) {
            std::string timeStr;
            if (Time > 0) {
                int timeValue = (Time + 1) / 3600;
                if (timeValue > 1439) {
                    timeStr = std::to_string((Time + 1) / 5184000) + " days";
                } else {
                    timeStr = std::to_string(timeValue) + " minutes";
                }
                std::wstring name(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end());
                std::wstring r(Reason.begin(), Reason.end());
                
                MainConsole.Console(name + L" has been kicked and banned for " + 
                    std::wstring(timeStr.begin(), timeStr.end()) + L" (" + r + L")",
                    CLIENT_MESSAGE_COLOR);
            } else {
                std::wstring name(Sprite[i].Player.Name.begin(), Sprite[i].Player.Name.end());
                std::wstring r(Reason.begin(), Reason.end());
                
                MainConsole.Console(name + L" has been kicked and permanently banned (" + r + L")",
                    CLIENT_MESSAGE_COLOR);
            }
        }

        // SaveTxtLists();  // Assuming function exists elsewhere

        if (!Sprite[i].Active) {
            return result;
        }

#ifdef SCRIPT_CODE
        if ((why == KICK_AC) || (why == KICK_CHEAT) || (why == KICK_CONSOLE) ||
            (why == KICK_PING) || (why == KICK_NORESPONSE) ||
            (why == KICK_NOCHEATRESPONSE) || (why == KICK_FLOODING) ||
            (why == KICK_VOTED) || (why == KICK_SILENT)) {
            // ScrptDispatcher.OnLeaveGame(i, true);  // Assuming function exists elsewhere
        }
#endif

        ServerPlayerDisconnect(i, why);  // Assuming function exists elsewhere

        if ((Why != KICK_AC) && (Why != KICK_CHEAT) && (Why != KICK_CONSOLE) &&
            (Why != KICK_VOTED)) {
            // Sprite[i].DropWeapon();  // Assuming method exists elsewhere
        }

        Sprite[i].Kill();  // Assuming method exists

        result = true;
        return result;
    }

    inline void CreateNormalWeapons() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->HitMultiply = 1.81f;
        gun->FireInterval = 24;
        gun->Ammo = 7;
        gun->ReloadTime = 87;
        gun->Speed = 19.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.009f;
        gun->BulletSpread = 0.15f;
        gun->Recoil = 0;
        gun->Push = 0.0176f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // MP5
        gun = &Guns[MP5];
        gun->HitMultiply = 1.01f;
        gun->FireInterval = 6;
        gun->Ammo = 30;
        gun->ReloadTime = 105;
        gun->Speed = 18.9f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.14f;
        gun->Recoil = 0;
        gun->Push = 0.0112f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Additional weapons would follow the same pattern...
    }

    inline void CreateRealisticWeapons() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->HitMultiply = 1.66f;
        gun->FireInterval = 27;
        gun->Ammo = 7;
        gun->ReloadTime = 106;
        gun->Speed = 19.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.02f;
        gun->BulletSpread = 0.1f;
        gun->Recoil = 55;
        gun->Push = 0.0164f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Additional weapons would follow the same pattern...
    }

    inline void CreateWeaponsBase() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->Name = "Desert Eagles";
        gun->IniName = gun->Name;
        gun->Num = EAGLE_NUM;
        gun->TextureNum = GFX_WEAPONS_DEAGLES;
        gun->ClipTextureNum = GFX_WEAPONS_DEAGLES_CLIP;
        gun->ClipReload = true;
        gun->BulletImageStyle = GFX_WEAPONS_DEAGLES_BULLET;
        gun->FireStyle = GFX_WEAPONS_DEAGLES_FIRE;
        gun->FireMode = 2;

        // Additional weapons would follow...
    }

    inline void CreateDefaultWeapons(bool RealisticMode) {
        if (RealisticMode) {
            CreateRealisticWeapons();
        } else {
            CreateNormalWeapons();
        }

        // Set defaults for weapon menu selection comparisons
        for (int weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            TGun* gun = &Guns[weaponIndex];
            TGun* defaultGun = &DefaultGuns[weaponIndex];

            defaultGun->HitMultiply = gun->HitMultiply;
            defaultGun->FireInterval = gun->FireInterval;
            defaultGun->Ammo = gun->Ammo;
            defaultGun->ReloadTime = gun->ReloadTime;
            defaultGun->Speed = gun->Speed;
            defaultGun->BulletStyle = gun->BulletStyle;
            defaultGun->StartUpTime = gun->StartUpTime;
            defaultGun->Bink = gun->Bink;
            defaultGun->MovementAcc = gun->MovementAcc;
            defaultGun->BulletSpread = gun->BulletSpread;
            defaultGun->Recoil = gun->Recoil;
            defaultGun->Push = gun->Push;
            defaultGun->InheritedVelocity = gun->InheritedVelocity;
            defaultGun->ModifierLegs = gun->ModifierLegs;
            defaultGun->ModifierChest = gun->ModifierChest;
            defaultGun->ModifierHead = gun->ModifierHead;
        }

        BuildWeapons();  // Assuming this function exists elsewhere
    }

    inline void CreateWeapons(bool RealisticMode) {
        CreateWeaponsBase();
        CreateDefaultWeapons(RealisticMode);
    }

    inline void BuildWeapons() {
        // Implementation would go here to finalize weapon settings
    }

    inline uint32_t CreateWMChecksum() {
        // Implementation would compute checksum of weapon mod properties
        uint32_t hash = 5381;
        // This would hash all the weapon parameters
        return hash;
    }

    inline int16_t WeaponNumToIndex(uint8_t Num) {
        for (int i = 1; i <= TOTAL_WEAPONS; i++) {
            if (Num == Guns[i].Num) {
                return static_cast<int16_t>(i);
            }
        }
        return -1;
    }

    inline int WeaponNameToNum(const std::string& Name) {
        for (int i = 1; i <= TOTAL_WEAPONS; i++) {
            if (Name == std::string(Guns[i].Name.begin(), Guns[i].Name.end())) {
                return Guns[i].Num;
            }
        }
        return -1;
    }

    inline std::string WeaponNumToName(int Num) {
        switch (Num) {
            case EAGLE_NUM: return std::string(Guns[EAGLE].Name.begin(), Guns[EAGLE].Name.end());
            case MP5_NUM: return std::string(Guns[MP5].Name.begin(), Guns[MP5].Name.end());
            case AK74_NUM: return std::string(Guns[AK74].Name.begin(), Guns[AK74].Name.end());
            // Additional weapons would follow...
            default: return "";
        }
    }

    inline std::string WeaponNameByNum(int Num) {
        for (int weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            if (Num == Guns[weaponIndex].Num) {
                return std::string(Guns[weaponIndex].Name.begin(), Guns[weaponIndex].Name.end());
            }
        }
        return "";
    }

    inline bool IsMainWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= 1) && (WeaponIndex <= MAIN_WEAPONS);
    }

    inline bool IsSecondaryWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= PRIMARY_WEAPONS + 1) && (WeaponIndex <= MAIN_WEAPONS);
    }

    inline bool IsExtendedWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= 1) && (WeaponIndex <= EXTENDED_WEAPONS);
    }

    inline uint16_t CalculateBink(uint16_t Accumulated, uint16_t Bink) {
        // Adding bink has diminishing returns as more gets accumulated
        return static_cast<uint16_t>(
            Accumulated + Bink - static_cast<int>(
                Accumulated * (Accumulated / ((10 * Bink) + Accumulated))
            )
        );
    }

    inline uint8_t WeaponNumInternalToExternal(uint8_t Num) {
        switch (Num) {
            case KNIFE_NUM: return 14;
            case CHAINSAW_NUM: return 15;
            case LAW_NUM: return 16;
            case FLAMER_NUM: return 11;
            case BOW_NUM: return 12;
            case BOW2_NUM: return 13;
            default: return Num;
        }
    }

    inline uint8_t WeaponNumExternalToInternal(uint8_t Num) {
        switch (Num) {
            case 11: return FLAMER_NUM;
            case 12: return BOW_NUM;
            case 13: return BOW2_NUM;
            case 14: return KNIFE_NUM;
            case 15: return CHAINSAW_NUM;
            case 16: return LAW_NUM;
            default: return Num;
        }
    }
}

// Using declarations to bring functions into global namespace
using ServerImpl::TLobbyThread;
using ServerImpl::ActivateServer;
using ServerImpl::Shutdown;
using ServerImpl::NextMap;
using ServerImpl::SpawnThings;
using ServerImpl::KickPlayer;
using ServerImpl::PrepareMapChange;
using ServerImpl::CreateWeapons;
using ServerImpl::CreateDefaultWeapons;
using ServerImpl::CreateWeaponsBase;
using ServerImpl::CreateNormalWeapons;
using ServerImpl::CreateRealisticWeapons;
using ServerImpl::BuildWeapons;
using ServerImpl::CreateWMChecksum;
using ServerImpl::WeaponNumToIndex;
using ServerImpl::WeaponNameToNum;
using ServerImpl::WeaponNumToName;
using ServerImpl::WeaponNameByNum;
using ServerImpl::IsMainWeaponIndex;
using ServerImpl::IsSecondaryWeaponIndex;
using ServerImpl::IsExtendedWeaponIndex;
using ServerImpl::CalculateBink;
using ServerImpl::WeaponNumInternalToExternal;
using ServerImpl::WeaponNumExternalToInternal;

// Global variables
extern bool ProgReady = false;
extern std::string BaseDirectory = "";
extern std::string UserDirectory = "";
extern uint32_t MainThreadID = 0;

// Define weapons array globally
extern std::vector<TGun> Guns(TOTAL_WEAPONS + 1);
extern std::vector<TGun> DefaultGuns(TOTAL_WEAPONS + 1);
extern uint32_t DefaultWMChecksum = 0;
extern uint32_t LoadedWMChecksum = 0;