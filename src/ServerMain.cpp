#include "ServerMain.h"
#include "Server.h"
#include "Constants.h"
#include "ServerHelper.h"
#include "ServerLoop.h"
#include "SysUtils.h"
#include "Util.h"
#include "Vector.h"
#include "Calc.h"
#include "Console.h"
#include "Cvar.h"
#include "Command.h"
#include "LogFile.h"
#include "Version.h"
#include "SharedConfig.h"
#include "Demo.h"
#include "AI.h"
#include "Anims.h"
#include "AutoUpdater.h"
#include "Game.h"
#include "MapFile.h"
#include "Parts.h"
#include "PolyMap.h"
#include "TraceLog.h"
#include "Waypoints.h"
#include "Weapons.h"

// Global variables for server
bool ServerRunning = true;
int ServerTickCounter = 0;

// Function prototypes
bool InitializeServer();
void ShutdownServer();
void UpdateServerState();
void ServerGameLoop();

bool InitializeServer() {
    std::cout << "Initializing Soldat Server v" << SOLDAT_VERSION << std::endl;

    // Initialize server-specific systems
    CommandInit();
    CvarInit();

    // Initialize logging
    NewLogFiles();

    // Set up default server CVars
    sv_gamemode.SetValue(GAMESTYLE_DEATHMATCH);
    sv_timelimit.SetValue(3600); // 1 hour default
    sv_killlimit.SetValue(100);
    sv_maxplayers.SetValue(16);

    std::cout << "Server initialized successfully!" << std::endl;
    return true;
}

void ShutdownServer() {
    std::cout << "Shutting down Soldat Server..." << std::endl;

    // Clean up server resources
    if (GameLog) {
        AddLineToLogFile(GameLog, "Server shutting down", ConsoleLogFileName);
    }

    // Disconnect all players gracefully
    // TODO: Send disconnect messages to all connected clients
}

void UpdateServerState() {
    // Update server-specific game state
    ServerTickCounter++;

    // Update timing
    Number27Timing();

    // Update game state
    UpdateGameStats();

    // Process network events
    // Handle player input, game logic, physics, etc.

    // Update AI if needed
    // Update bots' decisions

    // Check for map changes
    if (MapChangeCounter > 0) {
        MapChangeCounter--;
        if (MapChangeCounter == 0) {
            ChangeMap();
        }
    }

    // Check voting timer
    TimerVote();

    // Update game timers
    if (TimeLimitCounter > 0) {
        TimeLimitCounter--;
        TimeLeftSec = TimeLimitCounter % 60;
        TimeLeftMin = TimeLimitCounter / 60;
    }

    // Sort players by scores
    SortPlayers();
}

void ServerGameLoop() {
    const int SERVER_TICKS_PER_SECOND = 60; // Standard 60 TPS
    const int TICK_INTERVAL_MS = 1000 / SERVER_TICKS_PER_SECOND;

    while (ServerRunning) {
        auto tick_start = std::chrono::high_resolution_clock::now();

        UpdateServerState();

        auto tick_end = std::chrono::high_resolution_clock::now();
        auto tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - tick_start);

        // Sleep for remainder of tick period if needed
        if (tick_duration.count() < TICK_INTERVAL_MS) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_INTERVAL_MS - tick_duration.count()));
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Starting Soldat Server v" << SOLDAT_VERSION << std::endl;

    if (!InitializeServer()) {
        std::cerr << "Failed to initialize server!" << std::endl;
        return -1;
    }

    // Set up initial game state
    // Load default map
    TMapInfo defaultMap;
    defaultMap.Name = "default";
    // Assuming Map is a global variable in the game namespace
    // Map.LoadMap(defaultMap);

    // Main server game loop
    ServerGameLoop();

    ShutdownServer();

    std::cout << "Soldat server terminated." << std::endl;
    return 0;
}