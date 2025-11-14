#ifndef SERVER_H
#define SERVER_H

//*******************************************************************************
//                                                                              
//       Main Unit for SOLDAT Server                                             
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <thread>
#include <chrono>

#include "Constants.h"
#include "Vector.h"
#include "Util.h"
#include "Cvar.h"
#include "Command.h"
#include "Net.h"
#include "Console.h"
#include "Sprites.h"
#include "Weapons.h"
#include "Game.h"
#include "Things.h"
#include "LogFile.h"

// Forward declarations
struct TServerNetwork;
struct TLobbyThread;

// Type definitions for server-specific structures
struct TFileBuffer {
    std::vector<uint8_t> Data;
    size_t Pos;
    
    TFileBuffer() : Pos(0) {}
};

// Global variables
extern bool ProgReady;
extern std::string BaseDirectory;
extern std::string UserDirectory;
extern uint32_t MainThreadID;

// Server CVars
extern TBooleanCvar log_enable;
extern TIntCvar log_level;
extern TIntCvar log_filesupdate;
extern TBooleanCvar log_timestamp;

extern TBooleanCvar fs_localmount;
extern TStringCvar fs_mod;
extern TBooleanCvar fs_portable;
extern TStringCvar fs_basepath;
extern TStringCvar fs_userpath;

extern TBooleanCvar demo_autorecord;

extern TIntCvar sv_respawntime;
extern TIntCvar sv_respawntime_minwave;
extern TIntCvar sv_respawntime_maxwave;

extern TIntCvar sv_dm_limit;
extern TIntCvar sv_pm_limit;
extern TIntCvar sv_tm_limit;
extern TIntCvar sv_rm_limit;

extern TIntCvar sv_inf_limit;
extern TIntCvar sv_inf_bluelimit;
extern TIntCvar sv_inf_redaward;

extern TIntCvar sv_htf_limit;
extern TIntCvar sv_htf_pointstime;

extern TIntCvar sv_ctf_limit;

extern TIntCvar sv_bonus_frequency;
extern TBooleanCvar sv_bonus_flamer;
extern TBooleanCvar sv_bonus_predator;
extern TBooleanCvar sv_bonus_berserker;
extern TBooleanCvar sv_bonus_vest;
extern TBooleanCvar sv_bonus_cluster;

extern TBooleanCvar sv_stationaryguns;

extern TStringCvar sv_password;
extern TStringCvar sv_adminpassword;
extern TIntCvar sv_maxplayers;
extern TIntCvar sv_maxspectators;
extern TBooleanCvar sv_spectatorchat;
extern TStringCvar sv_greeting;
extern TStringCvar sv_greeting2;
extern TStringCvar sv_greeting3;
extern TStringCvar sv_info;
extern TIntCvar sv_minping;
extern TIntCvar sv_maxping;
extern TIntCvar sv_votepercent;
extern TBooleanCvar sv_lockedmode;
extern TStringCvar sv_pidfilename;
extern TStringCvar sv_maplist;
extern TBooleanCvar sv_lobby;
extern TStringCvar sv_lobbyurl;

extern TBooleanCvar sv_steamonly;

#ifdef STEAM_CODE
extern TBooleanCvar sv_voicechat;
extern TBooleanCvar sv_voicechat_alltalk;
extern TStringCvar sv_setsteamaccount;
#endif

extern TIntCvar sv_warnings_flood;
extern TIntCvar sv_warnings_ping;
extern TIntCvar sv_warnings_votecheat;
extern TIntCvar sv_warnings_knifecheat;
extern TIntCvar sv_warnings_tk;

extern TBooleanCvar sv_anticheatkick;
extern TBooleanCvar sv_punishtk;
extern TBooleanCvar sv_botbalance;
extern TBooleanCvar sv_echokills;
extern TBooleanCvar sv_antimassflag;
extern TIntCvar sv_healthcooldown;
extern TBooleanCvar sv_teamcolors;

extern TIntCvar net_port;
extern TStringCvar net_ip;
extern TStringCvar net_adminip;
extern TIntCvar net_lan;
extern TBooleanCvar net_compression;
extern TBooleanCvar net_allowdownload;
extern TIntCvar net_maxconnections;
extern TIntCvar net_maxadminconnections;

extern TIntCvar net_floodingpacketslan;
extern TIntCvar net_floodingpacketsinternet;

extern TIntCvar net_t1_snapshot;
extern TIntCvar net_t1_majorsnapshot;
extern TIntCvar net_t1_deadsnapshot;
extern TIntCvar net_t1_heartbeat;
extern TIntCvar net_t1_delta;
extern TIntCvar net_t1_ping;
extern TIntCvar net_t1_thingsnapshot;

extern TIntCvar bots_random_noteam;
extern TIntCvar bots_random_alpha;
extern TIntCvar bots_random_bravo;
extern TIntCvar bots_random_charlie;
extern TIntCvar bots_random_delta;
extern TIntCvar bots_difficulty;
extern TBooleanCvar bots_chat;

extern TBooleanCvar sc_enable;
extern TStringCvar sc_onscriptcrash;
extern TIntCvar sc_maxscripts;
extern TBooleanCvar sc_safemode;

extern TBooleanCvar fileserver_enable;
extern TIntCvar fileserver_port;
extern TStringCvar fileserver_ip;

// Syncable CVars
extern TIntCvar sv_gamemode;
extern TBooleanCvar sv_friendlyfire;
extern TIntCvar sv_timelimit;
extern TIntCvar sv_maxgrenades;
extern TBooleanCvar sv_bullettime;
extern TBooleanCvar sv_sniperline;
extern TBooleanCvar sv_balanceteams;
extern TBooleanCvar sv_survivalmode;
extern TBooleanCvar sv_survivalmode_antispy;
extern TBooleanCvar sv_survivalmode_clearweapons;
extern TBooleanCvar sv_realisticmode;
extern TBooleanCvar sv_advancemode;
extern TIntCvar sv_advancemode_amount;
extern TBooleanCvar sv_guns_collide;
extern TBooleanCvar sv_kits_collide;
extern TBooleanCvar sv_minimap;
extern TBooleanCvar sv_advancedspectator;
extern TBooleanCvar sv_radio;
extern TFloatCvar sv_gravity;
extern TStringCvar sv_hostname;
extern TIntCvar sv_killlimit;
extern TStringCvar sv_downloadurl;
extern TBooleanCvar sv_pure;
extern TStringCvar sv_website;

#ifdef ENABLE_FAE_CODE
extern TBooleanCvar ac_enable;
#endif

// Global variables
extern std::string ServerIP;
extern int ServerPort;
extern int BonusFreq;
extern uint8_t WeaponActive[16];  // Pascal arrays from -1 to 15

extern std::vector<std::string> MapsList;

extern uint8_t LastPlayer;

extern TBulletSnapshotMsg OldBulletSnapshotMsg[MAX_SPRITES + 1];

// Mute array
extern std::string MuteList[MAX_PLAYERS + 1];
extern std::string MuteName[MAX_PLAYERS + 1];

// TK array
extern std::string TKList[MAX_PLAYERS + 1];  // IP
extern uint8_t TKListKills[MAX_PLAYERS + 1];  // TK Warnings

extern int64_t TCPBytesSent;
extern int64_t TCPBytesReceived;

// Console
extern TConsole MainConsole;

extern std::vector<std::string> RemoteIPs;
extern std::vector<std::string> AdminIPs;

extern std::string FloodIP[1001];  // Pascal arrays start from 1
extern int FloodNum[1001];         // Pascal arrays start from 1

extern std::string LastReqIP[4];   // Pascal arrays start from 0
extern uint8_t LastReqID;
extern std::string DropIP;

extern std::string AdminFloodIP[MAX_ADMIN_FLOOD_IPS + 1];
extern std::string LastAdminIPs[MAX_LAST_ADMIN_IPS + 1];
extern int AdminIPCounter;

extern int WaveRespawnTime, WaveRespawnCounter;

extern int WeaponsInGame;

extern uint8_t BulletWarningCount[MAX_SPRITES + 1];

extern uint8_t CheatTag[MAX_SPRITES + 1];

#ifdef RCON_CODE
extern TAdminServer* AdminServer;
#endif

// Bullet shot stats
extern float ShotDistance;
extern float ShotLife;
extern int ShotRicochet;

extern int HTFTime;

extern std::string CurrentConf;

extern std::string WMName, WMVersion;
extern std::string LastWepMod;

extern float Grav;

extern std::string ModDir;

extern TServerNetwork* UDP;

extern std::unique_ptr<TLobbyThread> LobbyThread;

#ifdef STEAM_CODE
extern TSteamGS* SteamAPI;
#endif

// Function declarations
void ActivateServer();
uint8_t AddBotPlayer(const std::string& Name, int Team);
void StartServer();
void LoadWeapons(const std::string& Filename);
void Shutdown();
void NextMap();
void SpawnThings(uint8_t Style, uint8_t Amount);
bool KickPlayer(uint8_t Num, bool Ban, int Why, int Time, const std::string& Reason = "");
bool PrepareMapChange(const std::string& Name);

#ifdef STEAM_CODE
void RunManualCallbacks();
#endif

#endif // SERVER_H