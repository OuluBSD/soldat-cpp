#include "Client.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <SDL2/SDL.h>

// Include other headers
#include "Constants.h"
#include "Util.h"
#include "Vector.h"
#include "Game.h"
#include "Sprites.h"
#include "Net.h"
#include "TraceLog.h"
#include "ClientGame.h"
#include "ControlGame.h"
#include "InterfaceGraphics.h"
#include "Input.h"

namespace ClientImpl {
    // Initialize global variables
    bool GameLoopRun = false;
    bool ProgReady = false;

    std::string JoinPassword = "";
    std::string JoinPort = "23073";
    std::string JoinIP = "127.0.0.1";

    bool WindowReady = false;
    uint8_t Initing = 0;
    bool GraphicsInitialized = false;

    std::string BaseDirectory = "";
    std::string UserDirectory = "";

    std::string ModDir = "";
    bool UsesServerMod = false;

    // Initialize other global variables
    std::string ServerIP = "127.0.0.1";
    int ServerPort = 23073;

    float Grav = 0.06f;

    uint8_t Connection = CONNECTION_INTERNET;

    uint8_t WeaponActive[MAIN_WEAPONS + 1] = {0}; // sync
    int WeaponsInGame = 0; // sync

    uint8_t SniperLine = 0;

    uint8_t Trails = 1;
    uint8_t Spectator = 0;  // TODO: Remove

    uint8_t PacketAdjusting = 1;

    bool LimboLock = false;
    uint8_t SelTeam = 0;

    uint8_t MySprite = 0;

    // Network
    TClientNetwork* UDP = nullptr;

    // Consoles
    TConsole MainConsole;
    TConsole BigConsole;
    TConsole KillConsole;

    // Weapon Stats
    TWeaponStat WepStats[21] = {}; // Initialize to default
    uint8_t WepStatsNum = 0;

    std::string GunDisplayName[17] = {};

    uint8_t GameThingTarget = 0;
    int GrenadeEffectTimer = 0;

    uint8_t BadMapIDCount = 0;

    std::string ExeName = "";
    bool AbnormalTerminate = false;

    std::string HWID = "";

    uint16_t HitSprayCounter = 0;
    bool ScreenTaken = false;

    // bullet shot stats
    int ShotDistanceShow = 0;
    float ShotDistance = 0.0f;
    float ShotLife = 0.0f;
    int ShotRicochet = 0;

    bool TargetMode = false;

    bool MuteAll = false;

    bool RedirectToServer = false;
    std::string RedirectIP = "";
    int RedirectPort = 0;
    std::string RedirectMsg = "";

    // Radio Menu
    std::vector<std::string> RadioMenu = {};
    char RMenuState[3] = {' ', ' '}; // 0..1
    bool ShowRadioMenu = false;
    uint8_t RadioCooldown = 3;

    // screen
    TVector2 CameraPrev = {0.0f, 0.0f};
    float CameraX = 0.0f, CameraY = 0.0f;  // camera x and y within world
    uint8_t CameraFollowSprite = 0;  // Tag number of object to follow

#ifdef STEAM_CODE
    TSteam* SteamAPI = nullptr;
    bool VoiceSpeakingNow = false;
    bool ForceReconnect = false;
#endif

    inline void RestartGraph() {
        WindowReady = false;

        // DoTextureLoading(true);

        // Load Map
        // Map.LoadMap(MapChange, r_forcebg.Value(), r_forcebg_color1.Value(), r_forcebg_color2.Value());

        WindowReady = true;

        if (!EscMenu.Active()) {
            mx = GameWidthHalf;
            my = GameHeightHalf;
            MousePrev.x = mx;
            MousePrev.y = my;
        }

        // MainConsole.Console(_("Graphics restart"), DEBUG_MESSAGE_COLOR);
    }

    inline void LoadWeaponNames() {
        // MainConsole.Console("Loading Weapon Names from " + ModDir + "txt/weaponnames.txt", DEBUG_MESSAGE_COLOR);
        // if (FileExists(ModDir + "txt/weaponnames.txt")) {
        //     PHYSFS_File* tf;
        //     tf = PHYSFS_openRead((ModDir + "txt/weaponnames.txt").c_str());
        //     for (int i = 0; i < EXTENDED_WEAPONS - 1; i++) {
        //         PhysFS_ReadLN(tf, GunDisplayName[WeaponNumExternalToInternal(i)]);
        //     }
        //     PHYSFS_close(tf);
        // }
    }

    inline void RedirectDialog() {
        // RenderGameInfo("Server Redirect");
        // 
        // TSDL_MessageBoxButtonData buttons[2];
        // buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        // buttons[0].buttonid = 0;
        // buttons[0].text = "Yes";
        // buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        // buttons[1].buttonid = 1;
        // buttons[1].text = "No";
        // 
        // TSDL_MessageBoxData data;
        // data.flags = 0;
        // data.window = GameWindow;  // Assuming GameWindow is defined elsewhere
        // data.title = "Server Redirect";
        // data.message = (RedirectMsg + "\n\nRedirect to server " + 
        //                 RedirectIP + ":" + std::to_string(RedirectPort) + "?").c_str();
        // data.numbuttons = 2;
        // data.buttons = buttons;
        // data.colorscheme = nullptr;
        // 
        // int response;
        // if (SDL_ShowMessageBox(&data, &response) < 0) {
        //     return;
        // }
        // 
        // RedirectToServer = false;
        // 
        // if (response == 0) {  // Yes
        //     JoinIP = RedirectIP;
        //     JoinPort = std::to_string(RedirectPort);
        //     JoinServer();
        // } else {  // No
        //     RedirectIP = "";
        //     RedirectPort = 0;
        //     RedirectMsg = "";
        //     ExitToMenu();
        // }
    }

    inline void ExitToMenu() {
        GOALTICKS = DEFAULT_GOALTICKS;

        // Reset network state and show the status string (if any)
        //ShouldRenderFrames := False;
        //NetEncActive := False;
        ResetSyncCvars();

        if (DemoRecorder.Active()) {
            DemoRecorder.StopRecord();
        }

        if (DemoPlayer.Active()) {
            DemoPlayer.StopDemo();
        }

        if (MySprite > 0) {
            ClientDisconnect();
        } else {
            if (UDP) {
                UDP->Disconnect(true);
            }
        }

        // StopSound(CHANNEL_WEATHER);  // Assuming sound function

        Map.Name = "";

        if (EscMenu) {
            GameMenuShow(EscMenu, false);
        }

        Map.Filename = "";  // force reloading next time
        MapChangeCounter = -60;
        //WindowReady := False;

        for (int i = 1; i <= MAX_SPRITES; i++) {
            if (Sprite[i].Active) {
                Sprite[i].Kill();
            }
        }
        for (int i = 1; i <= MAX_BULLETS; i++) {
            Bullet[i].Kill();
        }
        for (int i = 1; i <= MAX_SPARKS; i++) {
            Spark[i].Kill();
        }
        for (int i = 1; i <= MAX_THINGS; i++) {
            Thing[i].Kill();
        }

        // Reset World and Big Texts
        for (int I = 0; I < MAX_BIG_MESSAGES; I++) {
            // Big Text
            BigText[I] = L"";
            BigDelay[I] = 0;
            BigScale[I] = 0.0f;
            BigColor[I] = 0;
            BigPosX[I] = 0.0f;
            BigPosY[I] = 0.0f;
            BigX[I] = 0.0f;
            // World Text
            WorldText[I] = L"";
            WorldDelay[I] = 0;
            WorldScale[I] = 0.0f;
            WorldColor[I] = 0;
            WorldPosX[I] = 0.0f;
            WorldPosY[I] = 0.0f;
            WorldX[I] = 0.0f;
        }

        // Reset ABOVE CHAT MESSAGE
        for (int I = 1; I <= MAX_SPRITES; I++) {
            ChatDelay[I] = 0;
            ChatMessage[I] = L"";
            ChatTeam[I] = false;
        }

        MySprite = 0;
        CameraFollowSprite = 0;
        GameThingTarget = 0;

        if (RedirectToServer) {
            RedirectDialog();
        }
    }

    inline void StartGame() {
        ExeName = "soldat"; // Simulating ParamStr(0)
        
        // CvarInit();
        // InitClientCommands();
        // ParseCommandLine(); // Assuming this function exists

        if (fs_portable.Value()) {
            UserDirectory = BaseDirectory;
            // Debug("[FS] Portable mode enabled.");
        } else {
            if (fs_userpath.Value().empty()) {
                UserDirectory = SDL_GetPrefPath("Soldat", "Soldat");
            }
            if (fs_basepath.Value().empty()) {
                const char* basePath = SDL_GetBasePath();
                if (basePath) {
                    BaseDirectory = std::string(basePath);
                }
            }
        }

        // Debug("[FS] UserDirectory: " + UserDirectory);
        // Debug("[FS] BaseDirectory: " + BaseDirectory);

        // Create the basic folder structure
        CreateDirIfMissing(UserDirectory + "/configs");
        CreateDirIfMissing(UserDirectory + "/screens");
        CreateDirIfMissing(UserDirectory + "/demos");
        CreateDirIfMissing(UserDirectory + "/logs");
        CreateDirIfMissing(UserDirectory + "/logs/kills");
        CreateDirIfMissing(UserDirectory + "/maps");
        CreateDirIfMissing(UserDirectory + "/mods");

        NewLogFiles();

        // Initialize console parameters
        // MainConsole.CountMax = static_cast<int>(round(15 * _rscala.y));
        // MainConsole.ScrollTickMax = 150;
        // MainConsole.NewMessageWait = 150;
        // MainConsole.AlphaCount = 255;
        // MainConsole.Count = 0;
        // MainConsole.CountMax = static_cast<int>(round(ui_console_length.Value() * _rscala.y));

        if (MainConsole.CountMax > 254) {
            MainConsole.CountMax = 254;
        }

        // HWID := '00000000000';  // TODO remove HWIDs, replace by Fae auth tickets

        /*
        if (r_sleeptime.Value() > 0) {
#ifdef _WIN32
            timeBeginPeriod(r_sleeptime.Value());
#endif
        }*/

        Initing = 0;
        // DefaultFormatSettings.DecimalSeparator := '.';
        // DefaultFormatSettings.DateSeparator := '-';

        // TODO: Steam initialization would go here if STEAM_CODE is defined

        // Initialize PhysFS
        if (!PHYSFS_Init(nullptr)) {
            ShowMessage("Could not initialize PhysFS. Try to reinstall the game.");
            return;
        }

        // Debug("[PhysFS] Mounting game archive");

        if (fs_localmount.Value()) {
            if (!PHYSFS_mount(UserDirectory.c_str(), "/", 0)) {
                ShowMessage("Could not load base game archive (game directory).");
                return;
            }
        } else {
            std::string gameArchive = BaseDirectory + "/soldat.smod";
            if (!PHYSFS_mount(gameArchive.c_str(), "/", 0)) {
                ShowMessage("Could not load base game archive (soldat.smod). Try to reinstall the game.");
                return;
            }
            // GameModChecksum := Sha1File(BaseDirectory + '/soldat.smod', 4096);
        }

        ModDir = "";

        // Load mod if specified
        if (fs_mod.Value() != "") {
            // Debug("[PhysFS] Mounting mods/" + LowerCase(fs_mod.Value()) + ".smod");
            std::string modPath = UserDirectory + "mods/" + toLower(fs_mod.Value()) + ".smod";
            if (!PHYSFS_mount(modPath.c_str(), ("/mods/" + toLower(fs_mod.Value()) + "/").c_str(), 0)) {
                ShowMessage("Could not load mod archive (" + fs_mod.Value() + ").");
                return;
            }
            ModDir = "mods/" + toLower(fs_mod.Value()) + "/";
            // CustomModChecksum := Sha1File(UserDirectory + "mods/" + toLower(fs_mod.Value()) + ".smod", 4096);
        }

        // TODO: Steam workshop mod loading would go here if defined

        // Copy default config file if it doesn't exist
        // PHYSFS_copyFileFromArchive("configs/client.cfg", (UserDirectory + "/configs/client.cfg").c_str());

        // LoadConfig("client.cfg");

        CvarsInitialized = true;

        // these might change so keep a backup to avoid changing the settings file
        int ScreenWidth = r_screenwidth.Value();
        int ScreenHeight = r_screenheight.Value();
        int RenderHeight = r_renderheight.Value();
        int RenderWidth = r_renderwidth.Value();

        // SDL_Init(SDL_INIT_VIDEO);
        // 
        // SDL_DisplayMode currentDisplay;
        // SDL_GetCurrentDisplayMode(0, &currentDisplay);

        if ((ScreenWidth == 0) || (ScreenHeight == 0)) {
            ScreenWidth = 1024;  // Use a default if not specified
            ScreenHeight = 768;
        }

        if ((RenderWidth == 0) || (RenderHeight == 0)) {
            RenderWidth = ScreenWidth;
            RenderHeight = ScreenHeight;
        }

        // Calculate FOV to check for too high/low vision
        float fov = static_cast<float>(RenderWidth) / RenderHeight;
        if (fov > MAX_FOV) {
            RenderWidth = static_cast<int>(ceil(RenderHeight * MAX_FOV));
            fov = MAX_FOV;
        } else if (fov < MIN_FOV) {
            RenderHeight = static_cast<int>(ceil(RenderWidth / MIN_FOV));
            fov = MIN_FOV;
        }

        // Calculate internal game width based on the fov and internal height
        GameWidth = static_cast<int>(round(fov * GameHeight));
        GameWidthHalf = static_cast<float>(GameWidth) / 2.0f;
        GameHeightHalf = static_cast<float>(GameHeight) / 2.0f;

        if (r_fullscreen.Value() == 0) {
            // avoid black bars in windowed mode
            if ((static_cast<float>(ScreenWidth) / ScreenHeight) >= (static_cast<float>(RenderWidth) / RenderHeight)) {
                ScreenWidth = static_cast<int>(round(ScreenHeight * (static_cast<float>(RenderWidth) / RenderHeight)));
            } else {
                ScreenHeight = static_cast<int>(round(ScreenWidth * (static_cast<float>(RenderHeight) / RenderWidth)));
            }
        }

        // window size equals "screen" size except in windowed fullscreen
        WindowWidth = ScreenWidth;
        WindowHeight = ScreenHeight;

        if (r_fullscreen.Value() == 2) {
            // Fullscreen windowed mode
            // WindowWidth = currentDisplay.w;
            // WindowHeight = currentDisplay.h;
        }

        // GfxLog("Window size: " + std::to_string(WindowWidth) + "x" + std::to_string(WindowHeight));
        // GfxLog("Target resolution: " + std::to_string(ScreenWidth) + "x" + std::to_string(ScreenHeight));
        // GfxLog("Internal resolution: " + std::to_string(RenderWidth) + "x" + std::to_string(RenderHeight));

        // IsFullscreen := (WindowWidth = currentDisplay.w) and (WindowHeight = currentDisplay.h);

        // interface is hard-coded to work on 4:3 aspect ratio,
        // but luckily for us the interface rendering code
        // translates the points using _RScala scale factor
        // above, so all we really need to do to make interface
        // work for widescreens is translate those points to a wider
        // area, which we can do by using the 640/480 as scale factors
        // even in widescreen resolutions. The interface code does NOT
        // use the _RScala to scale the interface, so this won't make
        // it look distorted.
        if (r_scaleinterface.Value()) {
            _RScala.x = 1.0f;
            _RScala.y = 1.0f;

            _iscala.x = static_cast<float>(GameWidth) / DEFAULT_WIDTH;
            _iscala.y = 1.0f;

            // fragx = floor(GameWidthHalf - 300) - 25;
        } else {
            _RScala.x = static_cast<float>(RenderWidth) / GameWidth;
            _RScala.y = static_cast<float>(RenderHeight) / GameHeight;

            _iscala.x = static_cast<float>(RenderWidth) / 640.0f;
            _iscala.y = static_cast<float>(RenderHeight) / 480.0f;

            // fragx = floor(RenderWidth / 2 - 300) - 25;

            if (RenderHeight > GameHeight) {
                // fragy = Round(10 * _rscala.y);
            }
        }

        // Reset frame timing
        // ResetFrameTiming();

        // GfxLog("Loading game graphics");
        // 
        // if (!InitGameGraphics()) {
        //     ShowMessage("The required OpenGL functionality isn't supported. " +
        //                 "Please, update your video drivers and try again.");
        //     // ExitButtonClick would be called here
        //     return;
        // }

        WindowReady = true;

        // Language initialization
        std::string SystemLang = cl_lang.Value();
        std::string SystemFallbackLang = "en_US";

        if (SystemLang.empty()) {
            // GetLanguageIDs(SystemLang, SystemFallbackLang); // Assuming this function exists
        }

        // if (InitTranslation(ModDir + "/txt/" + SystemLang + ".mo")) {
        //     Debug("Game captions loaded from " + ModDir + "/txt/" + SystemLang);
        // } else {
        //     Debug("Game captions not found");
        // }

        // Sound initialization
        // AddLineToLogFile(GameLog, "Initializing Sound Library.", ConsoleLogFileName);
        // if (!InitSound()) {
        //     AddLineToLogFile(GameLog, "Failed to initialize Sound Library.", ConsoleLogFileName);
        //     // Let the player know that he has no sound (no popup window)
        // }
        // 
        // LoadSounds("");
        // if (!ModDir.empty()) {
        //     LoadSounds(ModDir);
        // }

        // Create Consoles
        MainConsole.CountMax = static_cast<int>(round(ui_console_length.Value() * _rscala.y));
        MainConsole.ScrollTickMax = 150;
        MainConsole.NewMessageWait = 150;
        MainConsole.AlphaCount = 255;
        MainConsole.Count = 0;
        if (MainConsole.CountMax > 254) {
            MainConsole.CountMax = 254;
        }

        BigConsole.CountMax = static_cast<int>(floor((0.85f * RenderHeight) /
            (font_consolelineheight.Value() * FontStyleSize(FONT_SMALL))));
        BigConsole.ScrollTickMax = 1500000;
        BigConsole.NewMessageWait = 0;
        BigConsole.AlphaCount = 255;
        BigConsole.Count = 0;
        if (BigConsole.CountMax > 254) {
            BigConsole.CountMax = 254;
        }

        KillConsole.CountMax = static_cast<int>(round(ui_killconsole_length.Value() * _rscala.y));
        KillConsole.ScrollTickMax = 240;
        KillConsole.NewMessageWait = 70;

        // Create static player objects
        // for (int i = 1; i <= MAX_SPRITES; i++) {
        //     Sprite[i].Player = new TPlayer(); // Assuming TPlayer constructor exists
        // }

        // Load animation objects
        // LoadAnimObjects("");
        // if (!ModDir.empty()) {
        //     LoadAnimObjects(ModDir);
        // }

        // Greet the player
        // MainConsole.Console(_("Welcome to Soldat ") + SOLDAT_VERSION, DEFAULT_MESSAGE_COLOR);

        // Check if FAE (Anti-Cheat) is enabled
        // #ifdef ENABLE_FAE
        // if (FaeIsEnabled()) {
        //     MainConsole.Console("Multi-player sessions are protected by Fae Anti-Cheat", AC_MESSAGE_COLOR);
        // }
        // #endif

        // Load weapon display names
        LoadWeaponNames();
        // CreateWeaponsBase(); // This would be called from Weapons.h

        MapChangeCounter = -60;

        PlayerNamesShow = true;

        // CursorText = "";  // Assuming this is defined elsewhere

        InitGameMenus();

        // Load radio menu
        // RadioMenu = new TStringList();  // Assuming TStringList equivalent exists
        // std::istringstream stream("RadioMenuDefaultContent");  // Would need actual file content
        // // Read radio menu options from file

        // Initial settings
        FreeCam = 1;
        NoTexts = 0;
        ShotDistanceShow = -1;

        if (r_compatibility.Value()) {
            cl_actionsnap.SetValue(false);
        }

        // WriteLogFile(GameLog, ConsoleLogFileName);
        // RunDeferredCommands();
    }

    inline void Shutdown() {
        ExitToMenu();

        /*
        #ifdef _WIN32
        if (r_sleeptime.Value() > 0) {
            timeEndPeriod(r_sleeptime.Value());
        }
        #endif
        */

        if (AbnormalTerminate) {
            return;
        }

        // AddLineToLogFile(GameLog, "Freeing sprites.", ConsoleLogFileName);

        // Free GFX
        // DestroyGameGraphics();

        SDL_Quit();

        // AddLineToLogFile(GameLog, "UDP closing.", ConsoleLogFileName);

        if (UDP) {
            delete UDP;
            UDP = nullptr;
        }

        // AddLineToLogFile(GameLog, "Sound closing.", ConsoleLogFileName);

        // CloseSound();  // Assuming this function exists

        // AddLineToLogFile(GameLog, "PhysFS closing.", ConsoleLogFileName);

        PHYSFS_deinit();

#ifdef STEAM_CODE
        // AddLineToLogFile(GameLog, "Steam API closing.", ConsoleLogFileName);
        // SteamAPI_Shutdown();  // Assuming this function exists
#endif

        // AddLineToLogFile(GameLog, "   End of Log.", ConsoleLogFileName);

        // WriteLogFile(GameLog, ConsoleLogFileName);

        exit(0);
    }

    inline void StartGameLoop() {
        while (GameLoopRun) {
            // UDP.ProcessLoop();
            // GameInput();
            // GameLoop();
        }
    }

    inline void JoinServer() {
        // ResetFrameTiming();

        Initing++;
        if (Initing > 10) Initing = 10;

        ServerIP = JoinIP;  // Assuming Trim and TryStrToInt functions work similarly
        try {
            ServerPort = std::stoi(JoinPort);
        } catch (...) {
            return; // Invalid port number
        }

        // InitGameGraphics();
        // DoTextureLoading(true);

        // FAE preflight check if enabled
        // if (FaeIsEnabled) {
        //     RenderGameInfo("Initializing");
        //     // FaePreflight();  // Assuming Fae preflight function exists
        // }

        UDP = new TClientNetwork();  // Assuming TClientNetwork exists
        
        // DEMO connection
        if (JoinPort == "0") {
            // DemoPlayer.OpenDemo(UserDirectory + "demos/" + JoinIP + ".sdm");
            // DemoPlayer.ProcessDemo();
            ProgReady = true;
            GameLoopRun = true;
            // RenderGameInfo("Loading");
            StartGameLoop();
        } else {
            // std::string loadingMsg = "Connecting to " + ServerIP + ":" + std::to_string(ServerPort);
            // RenderGameInfo(WideString(loadingMsg));

            if (UDP->Connect(ServerIP, ServerPort)) {
                ProgReady = true;
                GameLoopRun = true;
                // RenderGameInfo("Loading");
                // ClientRequestGame();  // Assuming this function exists
                StartGameLoop();
            } else {
                // RenderGameInfo("Connection timed out.");
            }
        }
    }

    inline void ShowMessage(const std::string& MessageText) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", 
                                 MessageText.c_str(), nullptr);
    }

    inline void ShowMessage(const std::wstring& MessageText) {
        std::string utf8Str(MessageText.begin(), MessageText.end());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", 
                                 utf8Str.c_str(), nullptr);
    }
}

// Global variable definitions
bool GameLoopRun = false;
bool ProgReady = false;

std::string JoinPassword = "";
std::string JoinPort = "23073";
std::string JoinIP = "127.0.0.1";

bool WindowReady = false;
uint8_t Initing = 0;
bool GraphicsInitialized = false;

std::string BaseDirectory = "";
std::string UserDirectory = "";

std::string ModDir = "";
bool UsesServerMod = false;

// Initialize CVar objects (these would be defined in Cvar system)
TBooleanCvar log_enable;
TIntCvar log_level;
TIntCvar log_filesupdate;

TBooleanCvar fs_localmount;
TStringCvar fs_mod;
TBooleanCvar fs_portable;
TStringCvar fs_basepath;
TStringCvar fs_userpath;

TIntCvar r_fullscreen;
TBooleanCvar r_weathereffects;
TBooleanCvar r_dithering;
TIntCvar r_swapeffect;
TBooleanCvar r_compatibility;
TIntCvar r_texturefilter;
TBooleanCvar r_optimizetextures;
TBooleanCvar r_mipmapping;
TFloatCvar r_mipmapbias;
TBooleanCvar r_glfinish;
TBooleanCvar r_smoothedges;
TBooleanCvar r_scaleinterface;
TIntCvar r_maxsparks;
TBooleanCvar r_animations;
TBooleanCvar r_renderbackground;
TIntCvar r_maxfps;
TBooleanCvar r_fpslimit;
TIntCvar r_resizefilter;
TIntCvar r_sleeptime;
TIntCvar r_screenwidth;
TIntCvar r_screenheight;
TIntCvar r_renderwidth;
TIntCvar r_renderheight;
TBooleanCvar r_forcebg;
TColorCvar r_forcebg_color1;
TColorCvar r_forcebg_color2;
TBooleanCvar r_renderui;
TFloatCvar r_zoom;
TIntCvar r_msaa;

TBooleanCvar ui_playerindicator;
TIntCvar ui_minimap_transparency;
TIntCvar ui_minimap_posx;
TIntCvar ui_minimap_posy;
TBooleanCvar ui_bonuscolors;
TStringCvar ui_style;
TIntCvar ui_status_transparency;
TBooleanCvar ui_console;
TIntCvar ui_console_length;
TBooleanCvar ui_killconsole;
TIntCvar ui_killconsole_length;
TBooleanCvar ui_hidespectators;

TFloatCvar cl_sensitivity;
TBooleanCvar cl_endscreenshot;
TBooleanCvar cl_actionsnap;
TBooleanCvar cl_screenshake;
TBooleanCvar cl_servermods;

#ifdef STEAM_CODE
TBooleanCvar cl_steam_screenshots;
TBooleanCvar cl_voicechat;
TIntCvar fs_workshop_mod;
TIntCvar fs_workshop_interface;
#endif

TStringCvar cl_player_name;
TIntCvar cl_player_team;
TColorCvar cl_player_shirt;
TColorCvar cl_player_pants;
TColorCvar cl_player_hair;
TColorCvar cl_player_jet;
TColorCvar cl_player_skin;

TIntCvar cl_player_hairstyle;
TIntCvar cl_player_headstyle;
TIntCvar cl_player_chainstyle;
TStringCvar cl_player_secwep;
TStringCvar cl_player_wep;

TIntCvar cl_runs;
TStringCvar cl_lang;

TFloatCvar demo_speed;
TIntCvar demo_rate;
TBooleanCvar demo_showcrosshair;
TBooleanCvar demo_autorecord;

TIntCvar snd_volume;
TBooleanCvar snd_effects_battle;
TBooleanCvar snd_effects_explosions;

TStringCvar font_1_name;
TStringCvar font_1_filename;
TIntCvar font_1_scale;
TStringCvar font_2_name;
TStringCvar font_2_filename;
TIntCvar font_2_scale;

TIntCvar font_menusize;
TIntCvar font_consolesize;
TIntCvar font_consolesmallsize;
TFloatCvar font_consolelineheight;
TIntCvar font_bigsize;
TIntCvar font_weaponmenusize;
TIntCvar font_killconsolenamespace;

// Matchmaking cvars
TBooleanCvar mm_ranked;

TIntCvar sv_respawntime; // TODO: Remove
TIntCvar sv_inf_redaward; // TODO: Remove
TIntCvar net_contype; // TODO: Remove
TBooleanCvar net_compression; // TODO: Remove
TBooleanCvar net_allowdownload;

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

std::string ServerIP = "127.0.0.1";
int ServerPort = 23073;

float Grav = 0.06f;

uint8_t Connection = CONNECTION_INTERNET;

uint8_t WeaponActive[MAIN_WEAPONS + 1] = {0}; // sync
int WeaponsInGame = 0; // sync

uint8_t SniperLine = 0;

uint8_t Trails = 1;
uint8_t Spectator = 0;  // TODO: Remove

uint8_t PacketAdjusting = 1;

bool LimboLock = false;
uint8_t SelTeam = 0;

uint8_t MySprite = 0;

// Network
TClientNetwork* UDP = nullptr;

// Consoles
TConsole MainConsole;
TConsole BigConsole;
TConsole KillConsole;

// Weapon Stats
TWeaponStat WepStats[21];
uint8_t WepStatsNum = 0;

std::string GunDisplayName[17];

uint8_t GameThingTarget = 0;
int GrenadeEffectTimer = 0;

uint8_t BadMapIDCount = 0;

std::string ExeName = "";
bool AbnormalTerminate = false;

std::string HWID = "";

uint16_t HitSprayCounter = 0;
bool ScreenTaken = false;

// bullet shot stats
int ShotDistanceShow = 0;
float ShotDistance = 0.0f;
float ShotLife = 0.0f;
int ShotRicochet = 0;

bool TargetMode = false;

bool MuteAll = false;

bool RedirectToServer = false;
std::string RedirectIP = "";
int RedirectPort = 0;
std::string RedirectMsg = "";

// Radio Menu
std::vector<std::string> RadioMenu;
char RMenuState[3] = {' ', ' '}; // 0..1
bool ShowRadioMenu = false;
uint8_t RadioCooldown = 3;

// screen
TVector2 CameraPrev = {0.0f, 0.0f};
float CameraX = 0.0f, CameraY = 0.0f;  // camera x and y within world
uint8_t CameraFollowSprite = 0;  // Tag number of object to follow

#ifdef STEAM_CODE
TSteam* SteamAPI = nullptr;
bool VoiceSpeakingNow = false;
bool ForceReconnect = false;
#endif