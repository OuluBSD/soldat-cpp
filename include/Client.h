#ifndef CLIENT_H
#define CLIENT_H

//*******************************************************************************
//                                                                              
//       Client Unit for SOLDAT                                                   
//                                                                              
//       Copyright (c) 2003 Michal Marcinkowski            
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "Constants.h"
#include "Vector.h"
#include "Util.h"
#include "LogFile.h"
#include "Console.h"
#include "Weapons.h"
#include "Game.h"
#include "Net.h"
#include "Sprites.h"
#include "Demo.h"

// Forward declarations
struct TClientNetwork;
struct TSteam;

// Type definitions
struct TWeaponStat {
    std::string Name;
    uint32_t TextureID;
    uint32_t Shots, Hits, Kills, Headshots;
    uint8_t Accuracy;
    
    TWeaponStat() : TextureID(0), Shots(0), Hits(0), Kills(0), Headshots(0), Accuracy(0) {}
};

// Global variables
extern bool GameLoopRun;
extern bool ProgReady;

extern std::string JoinPassword;  // server password
extern std::string JoinPort;      // join port to server
extern std::string JoinIP;        // join ip to server

extern bool WindowReady;
extern uint8_t Initing;
extern bool GraphicsInitialized;

extern std::string BaseDirectory;
extern std::string UserDirectory;

extern std::string ModDir;
extern bool UsesServerMod;

// Function declarations
void JoinServer();
void StartGame();
void Shutdown();
void ExitToMenu();
void RestartGraph();
void ShowMessage(const std::string& MessageText); // overload
void ShowMessage(const std::wstring& MessageText); // overload

namespace ClientImpl {
    inline void RestartGraph() {
        // WindowReady = false;
        // DoTextureLoading(true);
        // 
        // // Load Map
        // Map.LoadMap(MapChange, r_forcebg.Value, r_forcebg_color1.Value, r_forcebg_color2.Value);
        // 
        // WindowReady = true;
        // 
        // if (!EscMenu.Active()) {
        //     mx = GameWidthHalf;
        //     my = GameHeightHalf;
        //     MousePrev.x = mx;
        //     MousePrev.y = my;
        // }
        // 
        // MainConsole.Console(_("Graphics restart"), DEBUG_MESSAGE_COLOR);
    }
}

// Using declarations to bring functions into global namespace
using ClientImpl::TWeaponStat;
using ClientImpl::GameLoopRun;
using ClientImpl::ProgReady;
using ClientImpl::JoinPassword;
using ClientImpl::JoinPort;
using ClientImpl::JoinIP;
using ClientImpl::WindowReady;
using ClientImpl::Initing;
using ClientImpl::GraphicsInitialized;
using ClientImpl::BaseDirectory;
using ClientImpl::UserDirectory;
using ClientImpl::ModDir;
using ClientImpl::UsesServerMod;
using ClientImpl::JoinServer;
using ClientImpl::StartGame;
using ClientImpl::Shutdown;
using ClientImpl::ExitToMenu;
using ClientImpl::RestartGraph;
using ClientImpl::ShowMessage;

// Global variable definitions
extern bool GameLoopRun = false;
extern bool ProgReady = false;

extern std::string JoinPassword = "";
extern std::string JoinPort = "23073";
extern std::string JoinIP = "127.0.0.1";

extern bool WindowReady = false;
extern uint8_t Initing = 0;
extern bool GraphicsInitialized = false;

extern std::string BaseDirectory = "";
extern std::string UserDirectory = "";

extern std::string ModDir = "";
extern bool UsesServerMod = false;

// CVar declarations (would be defined elsewhere)
extern TBooleanCvar log_enable;
extern TIntCvar log_level;
extern TIntCvar log_filesupdate;

extern TBooleanCvar fs_localmount;
extern TStringCvar fs_mod;
extern TBooleanCvar fs_portable;
extern TStringCvar fs_basepath;
extern TStringCvar fs_userpath;

extern TIntCvar r_fullscreen;
extern TBooleanCvar r_weathereffects;
extern TBooleanCvar r_dithering;
extern TIntCvar r_swapeffect;
extern TBooleanCvar r_compatibility;
extern TIntCvar r_texturefilter;
extern TBooleanCvar r_optimizetextures;
extern TBooleanCvar r_mipmapping;
extern TFloatCvar r_mipmapbias;
extern TBooleanCvar r_glfinish;
extern TBooleanCvar r_smoothedges;
extern TBooleanCvar r_scaleinterface;
extern TIntCvar r_maxsparks;
extern TBooleanCvar r_animations;
extern TBooleanCvar r_renderbackground;
extern TIntCvar r_maxfps;
extern TBooleanCvar r_fpslimit;
extern TIntCvar r_resizefilter;
extern TIntCvar r_sleeptime;
extern TIntCvar r_screenwidth;
extern TIntCvar r_screenheight;
extern TIntCvar r_renderwidth;
extern TIntCvar r_renderheight;
extern TBooleanCvar r_forcebg;
extern TColorCvar r_forcebg_color1;
extern TColorCvar r_forcebg_color2;
extern TBooleanCvar r_renderui;
extern TFloatCvar r_zoom;
extern TIntCvar r_msaa;

extern TBooleanCvar ui_playerindicator;
extern TIntCvar ui_minimap_transparency;
extern TIntCvar ui_minimap_posx;
extern TIntCvar ui_minimap_posy;
extern TBooleanCvar ui_bonuscolors;
extern TStringCvar ui_style;
extern TIntCvar ui_status_transparency;
extern TBooleanCvar ui_console;
extern TIntCvar ui_console_length;
extern TBooleanCvar ui_killconsole;
extern TIntCvar ui_killconsole_length;
extern TBooleanCvar ui_hidespectators;

extern TFloatCvar cl_sensitivity;
extern TBooleanCvar cl_endscreenshot;
extern TBooleanCvar cl_actionsnap;
extern TBooleanCvar cl_screenshake;
extern TBooleanCvar cl_servermods;

#ifdef STEAM_CODE
extern TBooleanCvar cl_steam_screenshots;
extern TBooleanCvar cl_voicechat;
extern TIntCvar fs_workshop_mod;
extern TIntCvar fs_workshop_interface;
#endif

extern TStringCvar cl_player_name;
extern TIntCvar cl_player_team;
extern TColorCvar cl_player_shirt;
extern TColorCvar cl_player_pants;
extern TColorCvar cl_player_hair;
extern TColorCvar cl_player_jet;
extern TColorCvar cl_player_skin;

extern TIntCvar cl_player_hairstyle;
extern TIntCvar cl_player_headstyle;
extern TIntCvar cl_player_chainstyle;
extern TStringCvar cl_player_secwep;
extern TStringCvar cl_player_wep;

extern TIntCvar cl_runs;
extern TStringCvar cl_lang;

extern TFloatCvar demo_speed;
extern TIntCvar demo_rate;
extern TBooleanCvar demo_showcrosshair;
extern TBooleanCvar demo_autorecord;

extern TIntCvar snd_volume;
extern TBooleanCvar snd_effects_battle;
extern TBooleanCvar snd_effects_explosions;

extern TStringCvar font_1_name;
extern TStringCvar font_1_filename;
extern TIntCvar font_1_scale;
extern TStringCvar font_2_name;
extern TStringCvar font_2_filename;
extern TIntCvar font_2_scale;

extern TIntCvar font_menusize;
extern TIntCvar font_consolesize;
extern TIntCvar font_consolesmallsize;
extern TFloatCvar font_consolelineheight;
extern TIntCvar font_bigsize;
extern TIntCvar font_weaponmenusize;
extern TIntCvar font_killconsolenamespace;

// Matchmaking cvars
extern TBooleanCvar mm_ranked;

extern TIntCvar sv_respawntime; // TODO: Remove
extern TIntCvar sv_inf_redaward; // TODO: Remove
extern TIntCvar net_contype; // TODO: Remove
extern TBooleanCvar net_compression; // TODO: Remove
extern TBooleanCvar net_allowdownload;

// syncable cvars
extern TIntCvar sv_gamemode;
extern TBooleanCvar sv_friendlyfire;
extern TIntCvar sv_timelimit;
extern TIntCvar sv_maxgrenades;
extern TBooleanCvar sv_bullettime;
extern TBooleanCvar sv_sniperline;
extern TBooleanCvar sv_balanceteams;
extern TBooleanCvar sv_guns_collide;
extern TBooleanCvar sv_kits_collide;
extern TBooleanCvar sv_survivalmode;
extern TBooleanCvar sv_survivalmode_antispy;
extern TBooleanCvar sv_survivalmode_clearweapons;
extern TBooleanCvar sv_realisticmode;
extern TBooleanCvar sv_advancemode;
extern TIntCvar sv_advancemode_amount;
extern TBooleanCvar sv_minimap;
extern TBooleanCvar sv_advancedspectator;
extern TBooleanCvar sv_radio;
extern TStringCvar sv_info;
extern TFloatCvar sv_gravity;
extern TStringCvar sv_hostname;
extern TIntCvar sv_killlimit;
extern TStringCvar sv_downloadurl;
extern TBooleanCvar sv_pure;
extern TStringCvar sv_website;

extern std::string ServerIP;
extern int ServerPort;

extern float Grav;

extern uint8_t Connection;

extern uint8_t WeaponActive[MAIN_WEAPONS + 1]; // sync
extern int WeaponsInGame; // sync

extern uint8_t SniperLine;

extern uint8_t Trails;
extern uint8_t Spectator;  // TODO: Remove

extern uint8_t PacketAdjusting;

extern bool LimboLock;
extern uint8_t SelTeam;

extern uint8_t MySprite;

// Network
extern TClientNetwork* UDP;

// Consoles
extern TConsole MainConsole;
extern TConsole BigConsole;
extern TConsole KillConsole;

// Weapon Stats
extern TWeaponStat WepStats[21];
extern uint8_t WepStatsNum;

extern std::string GunDisplayName[17];

extern uint8_t GameThingTarget;
extern int GrenadeEffectTimer;

extern uint8_t BadMapIDCount;

extern std::string ExeName;
extern bool AbnormalTerminate;

extern std::string HWID;

extern uint16_t HitSprayCounter;
extern bool ScreenTaken;

// bullet shot stats
extern int ShotDistanceShow;
extern float ShotDistance;
extern float ShotLife;
extern int ShotRicochet;

extern bool TargetMode;

extern bool MuteAll;

extern bool RedirectToServer;
extern std::string RedirectIP;
extern int RedirectPort;
extern std::string RedirectMsg;

// Radio Menu
extern std::vector<std::string> RadioMenu;
extern char RMenuState[3]; // 0..1
extern bool ShowRadioMenu;
extern uint8_t RadioCooldown;

// screen
extern TVector2 CameraPrev;
extern float CameraX, CameraY;  // camera x and y within world
extern uint8_t CameraFollowSprite;  // Tag number of object to follow

// extern TDownloadThread* DownloadThread;  // Defined elsewhere
#ifdef STEAM_CODE
extern TSteam* SteamAPI;
extern bool VoiceSpeakingNow;
extern bool ForceReconnect;
#endif

#endif // CLIENT_H