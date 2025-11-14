#ifndef RCON_H
#define RCON_H

//*******************************************************************************
//                                                                              
//       Remote Console (Rcon) Unit for SOLDAT                                   
//                                                                              
//       Copyright (c) 2012 Daniel Forssten              
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

#include "Constants.h"
#include "Net.h"
#include "Vector.h"
#include "Sprites.h"
#include "Server.h"
#include "Util.h"
#include "TraceLog.h"
#include "Command.h"
#include "Game.h"
#include "ServerHelper.h"
#include "Version.h"

// Constants
const int REFRESHX_HEADER_CHARS = 10;
const std::string REFRESHX_HEADER_STR = "REFRESHX\r\n";

// Type definitions
struct TAdminMessage {
    std::string Msg;
    std::string IP;
    uint16_t Port;
    
    TAdminMessage() : Port(0) {}
    TAdminMessage(const std::string& msg, const std::string& ip, uint16_t port) 
        : Msg(msg), IP(ip), Port(port) {}
};

// Structure for refresh message with extended server info
struct TMsg_RefreshX {
    char Header[REFRESHX_HEADER_CHARS + 1];  // Pascal arrays start from 1
    std::string Name[MAX_PLAYERS + 1];      // Pascal arrays start from 1
    std::string HWID[MAX_PLAYERS + 1];      // Pascal arrays start from 1
    uint8_t Team[MAX_PLAYERS + 1];          // Pascal arrays start from 1
    uint16_t Kills[MAX_PLAYERS + 1];        // Pascal arrays start from 1
    uint8_t Caps[MAX_PLAYERS + 1];          // Pascal arrays start from 1
    uint16_t Deaths[MAX_PLAYERS + 1];       // Pascal arrays start from 1
    int32_t Ping[MAX_PLAYERS + 1];          // Pascal arrays start from 1
    uint8_t Number[MAX_PLAYERS + 1];        // Pascal arrays start from 1
    uint8_t IP[MAX_PLAYERS + 1][4];         // Pascal arrays start from 1
    float X[MAX_PLAYERS + 1];               // Pascal arrays start from 1
    float Y[MAX_PLAYERS + 1];               // Pascal arrays start from 1
    
    float RedFlagX;
    float RedFlagY;
    float BlueFlagX;
    float BlueFlagY;
    
    uint16_t TeamScore[5];                  // Pascal arrays start from 1
    std::string MapName;
    int32_t TimeLimit;
    int32_t CurrentTime;
    uint16_t KillLimit;
    uint8_t GameStyle;
    uint8_t MaxPlayers;
    uint8_t MaxSpectators;
    uint8_t Passworded;
    std::string NextMap;
    
    TMsg_RefreshX() : RedFlagX(0.0f), RedFlagY(0.0f), BlueFlagX(0.0f), BlueFlagY(0.0f),
                      TimeLimit(0), CurrentTime(0), KillLimit(0), GameStyle(0),
                      MaxPlayers(0), MaxSpectators(0), Passworded(0) {}
};

// Class declaration for the admin server
class TAdminServer {
private:
    std::string FPassword;
    std::vector<std::string> FAdminIPs;  // List of admin IPs
    std::string FConnectMessage;
    int64_t FBytesSent;
    int64_t FBytesReceived;
    std::queue<std::unique_ptr<TAdminMessage>> FMessageQueue;
    std::mutex FQueueLock;
    
    // Internal methods (equivalent to the Pascal private methods)
    void HandleConnect(const std::string& IP, uint16_t Port);
    void HandleDisconnect(const std::string& IP, uint16_t Port);
    void HandleExecute(const std::string& IP, uint16_t Port, const std::string& Message);

public:
    TAdminServer(const std::string& Pass, const std::string& ConnectMessage = "");
    ~TAdminServer();
    
    std::vector<std::string> GetAdmins() const { return FAdminIPs; }
    const std::string& GetPassword() const { return FPassword; }
    void SetPassword(const std::string& Pass) { FPassword = Pass; }
    const std::string& GetConnectMessage() const { return FConnectMessage; }
    void SetConnectMessage(const std::string& Msg) { FConnectMessage = Msg; }
    int64_t GetBytesSent() const { return FBytesSent; }
    int64_t GetBytesReceived() const { return FBytesReceived; }
    
    void SendToAll(const std::string& Message);
    void SendToIP(const std::string& IP, const std::string& Message);
    void ProcessCommands();
    bool IsActive() const { return true; } // Placeholder - would track actual server state
    void SetActive(bool Active) {} // Placeholder - would handle actual server activation
};

// Function declarations
void BroadcastMsg(const std::string& Text);
void SendMessageToAdmin(const std::string& ToIP, const std::string& Text);

// Global variables
extern std::unique_ptr<TAdminServer> AdminServer;

namespace RconImpl {
    inline TAdminServer::TAdminServer(const std::string& Pass, const std::string& ConnectMessage) 
        : FPassword(Pass), FConnectMessage(ConnectMessage), FBytesSent(0), FBytesReceived(0) {
        // Initialize the server with connection parameters
        // In actual implementation, this would set up socket listeners
    }

    inline TAdminServer::~TAdminServer() {
        SetActive(false);  // Disconnect and cleanup
        
        // Clean up admin list
        FAdminIPs.clear();
    }

    inline void TAdminServer::SendToAll(const std::string& Message) {
        // This would send the message to all connected admin clients in a real implementation
        // For now, we'll just print to console
        std::cout << "[RCON] Sending to all admins: " << Message << std::endl;
        
        // Update bytes sent counter
        FBytesSent += Message.length();
    }

    inline void TAdminServer::SendToIP(const std::string& IP, const std::string& Message) {
        // This would send the message to a specific admin client by IP
        // For now, we'll just print to console if the IP matches
        std::cout << "[RCON] Sending to IP " << IP << ": " << Message << std::endl;
        
        // Update bytes sent counter
        FBytesSent += Message.length();
    }

    inline void TAdminServer::ProcessCommands() {
        // Process queued admin commands
        if (FMessageQueue.empty()) {
            return;
        }

        std::unique_ptr<TAdminMessage> msg;
        {
            std::lock_guard<std::mutex> lock(FQueueLock);
            if (!FMessageQueue.empty()) {
                msg = std::move(FMessageQueue.front());
                FMessageQueue.pop();
            }
        }

        if (!msg) {
            return;
        }

        if (msg->Msg == "SHUTDOWN") {
            // MainConsole.Console("SHUTDOWN (" + msg->IP + ").", GAME_MESSAGE_COLOR);
            ProgReady = false;
            return;
        }
        else if (msg->Msg == "REFRESHX") {
            // Create and send refresh message with extended server info
            TMsg_RefreshX refreshMsg;
            
            for (int i = 1; i <= MAX_PLAYERS; i++) {
                if (i <= static_cast<int>(SortedPlayers.size()) && SortedPlayers[i].PlayerNum > 0) {
                    refreshMsg.Name[i] = Sprite[SortedPlayers[i].PlayerNum].Player.Name;
                    
#ifdef STEAM_CODE
                    // refreshMsg.HWID[i] = std::to_string(TSteamID(Sprite[SortedPlayers[i].PlayerNum].Player.SteamID).GetAccountID());
#else
                    refreshMsg.HWID[i] = Sprite[SortedPlayers[i].PlayerNum].Player.HWid;
#endif
                    
                    refreshMsg.Kills[i] = Sprite[SortedPlayers[i].PlayerNum].Player.Kills;
                    refreshMsg.Caps[i] = Sprite[SortedPlayers[i].PlayerNum].Player.Flags;
                    refreshMsg.Deaths[i] = Sprite[SortedPlayers[i].PlayerNum].Player.Deaths;
                    refreshMsg.Ping[i] = Sprite[SortedPlayers[i].PlayerNum].Player.RealPing;  // Using RealPing as per original
                    refreshMsg.Team[i] = Sprite[SortedPlayers[i].PlayerNum].Player.Team;
                    refreshMsg.Number[i] = Sprite[SortedPlayers[i].PlayerNum].Num;
                    refreshMsg.X[i] = Sprite[SortedPlayers[i].PlayerNum].DeadMeat ? 0.0f : 
                                    Sprite[SortedPlayers[i].PlayerNum].Skeleton.Pos[1].x;
                    refreshMsg.Y[i] = Sprite[SortedPlayers[i].PlayerNum].DeadMeat ? 0.0f : 
                                    Sprite[SortedPlayers[i].PlayerNum].Skeleton.Pos[1].y;

                    if (Sprite[SortedPlayers[i].PlayerNum].Player.ControlMethod == HUMAN) {
                        // Parse IP address into octets
                        std::string ips = std::string(Sprite[SortedPlayers[i].PlayerNum].Player.IP.begin(), 
                                                     Sprite[SortedPlayers[i].PlayerNum].Player.IP.end());
                        int n = 1;
                        int c = 0;
                        std::string s[5]; // Pascal arrays start from 1
                        
                        for (size_t j = 0; j < ips.length(); j++) {
                            c++;
                            if (ips[j] == '.') {
                                c = 0;
                                n++;
                            } else {
                                s[n] += ips[j];
                            }
                        }

                        for (int j = 1; j <= 4; j++) {
                            try {
                                refreshMsg.IP[i][j-1] = static_cast<uint8_t>(std::stoi(s[j]));
                            } catch (...) {
                                refreshMsg.IP[i][j-1] = 0;
                            }
                        }
                    } else {
                        for (int j = 0; j < 4; j++) {
                            refreshMsg.IP[i][j] = 0;
                        }
                    }
                } else {
                    refreshMsg.Team[i] = 255;  // Not active
                }
            }

            std::copy(REFRESHX_HEADER_STR.begin(), REFRESHX_HEADER_STR.end(), refreshMsg.Header);
            refreshMsg.TeamScore[TEAM_ALPHA] = TeamScore[TEAM_ALPHA];
            refreshMsg.TeamScore[TEAM_BRAVO] = TeamScore[TEAM_BRAVO];
            refreshMsg.TeamScore[TEAM_CHARLIE] = TeamScore[TEAM_CHARLIE];
            refreshMsg.TeamScore[TEAM_DELTA] = TeamScore[TEAM_DELTA];

            if (TeamFlag[TEAM_ALPHA] > 0) {
                refreshMsg.RedFlagX = Thing[TeamFlag[TEAM_ALPHA]].Skeleton.Pos[1].x;
                refreshMsg.RedFlagY = Thing[TeamFlag[TEAM_ALPHA]].Skeleton.Pos[1].y;
            }

            if (TeamFlag[TEAM_BRAVO] > 0) {
                refreshMsg.BlueFlagX = Thing[TeamFlag[TEAM_BRAVO]].Skeleton.Pos[1].x;
                refreshMsg.BlueFlagY = Thing[TeamFlag[TEAM_BRAVO]].Skeleton.Pos[1].y;
            }

            refreshMsg.MapName = Map.Name;
            refreshMsg.TimeLimit = sv_timelimit.Value();
            refreshMsg.CurrentTime = TimeLimitCounter;
            refreshMsg.KillLimit = sv_killlimit.Value();
            refreshMsg.GameStyle = sv_gamemode.Value();
            refreshMsg.MaxPlayers = sv_maxplayers.Value();
            refreshMsg.MaxSpectators = sv_maxspectators.Value();
            refreshMsg.Passworded = (sv_password.Value().empty()) ? 0 : 1;
            refreshMsg.NextMap = CheckNextMap();

            // In a real implementation, we would send the binary data
            // IOHandlerSocket.Write(reinterpret_cast<const char*>(&refreshMsg), sizeof(refreshMsg));
        } else {
            // Process regular command
            std::string ip = msg->IP;
            // MainConsole.Console(msg->Msg + " (" + ip + ")", GAME_MESSAGE_COLOR);
            
#ifdef SCRIPT_CODE
            // ScrptDispatcher.OnAdminMessage(msg->IP, msg->Port, msg->Msg);
#endif
            
            if (msg->Msg[0] == '/') {
                // Process command starting with '/'
                std::string command = msg->Msg.substr(1); // Remove leading '/'
                
#ifdef SCRIPT_CODE
                // if (!ScrptDispatcher.OnConsoleCommand(msg->IP, msg->Port, command)) {
#endif
                    ParseInput(command, 255);  // Assuming 255 means admin
#ifdef SCRIPT_CODE
                // }
#endif
            }
        }
    }

    inline uint32_t FindAdminFloodID(const std::string& SrcIP) {
        const uint32_t FLOOD_ID_NOT_FOUND = 0;
        
        for (int i = 1; i <= MAX_ADMIN_FLOOD_IPS; i++) {
            if (AdminFloodIP[i] == SrcIP) {
                return static_cast<uint32_t>(i);
            }
        }
        return FLOOD_ID_NOT_FOUND;
    }

    inline uint32_t AddAdminFloodIP(const std::string& SrcIP) {
        const uint32_t FLOOD_ID_NOT_FOUND = 0;
        
        for (int i = 1; i <= MAX_ADMIN_FLOOD_IPS; i++) {
            if (AdminFloodIP[i].empty()) {
                AdminFloodIP[i] = SrcIP;
                return static_cast<uint32_t>(i);
            }
        }
        return FLOOD_ID_NOT_FOUND;
    }

    inline uint32_t UpdateAntiAdminFlood(const std::string& IP) {
        // Update last requested admin IP array
        LastAdminIPs[AdminIPCounter] = IP;
        AdminIPCounter = (AdminIPCounter + 1) % MAX_LAST_ADMIN_IPS + 1;

        // Check for flood
        bool isFlood = true;
        for (int i = 0; i < MAX_LAST_ADMIN_IPS; i++) {
            int nextIdx = (i + 1) % MAX_LAST_ADMIN_IPS + 1;
            if (LastAdminIPs[i] != LastAdminIPs[nextIdx]) {
                isFlood = false;
                break;
            }
        }

        // If flood detected, ban
        if (isFlood) {
            uint32_t idx = AddAdminFloodIP(IP);
            if (idx == 0) {
                std::cout << "Failed adding IP to ban list" << std::endl;
            }
            return 1;
        } else {
            return 0;
        }
    }

    inline void HandleConnect(TAdminServer* adminServer, const std::string& IP, uint16_t Port) {
        // Check if IP is banned
        if (FindAdminFloodID(IP) != 0) {
            // Disconnect
            return;
        }

        if (UpdateAntiAdminFlood(IP) > 0) {
            return;
        }

        // In a real implementation, we would send welcome message and authenticate
        // For now, just add the IP to the admin list if authenticated
        adminServer->FAdminIPs.push_back(IP);
        std::cout << "Admin connected from IP: " << IP << std::endl;
    }

    inline void HandleDisconnect(TAdminServer* adminServer, const std::string& IP, uint16_t Port) {
        // Remove IP from admin list
        auto it = std::find(adminServer->FAdminIPs.begin(), adminServer->FAdminIPs.end(), IP);
        if (it != adminServer->FAdminIPs.end()) {
            adminServer->FAdminIPs.erase(it);
            std::cout << "Admin disconnected from IP: " << IP << std::endl;
        }
    }

    inline void HandleExecute(TAdminServer* adminServer, const std::string& IP, uint16_t Port, const std::string& Message) {
        // Check if IP is banned
        if (FindAdminFloodID(IP) != 0) {
            // Disconnect
            return;
        }

        // Add message to queue for processing
        std::unique_ptr<TAdminMessage> msg = std::make_unique<TAdminMessage>(Message, IP, Port);
        
        {
            std::lock_guard<std::mutex> lock(adminServer->FQueueLock);
            adminServer->FMessageQueue.push(std::move(msg));
        }
    }

    inline void BroadcastMsg(const std::string& Text) {
        Trace("BroadcastMsg");
        
        if (sv_adminpassword.Value().empty()) {
            return;
        }
        
        if (!AdminServer) {
            return;
        }

        AdminServer->SendToAll(Text);
    }

    inline void SendMessageToAdmin(const std::string& ToIP, const std::string& Text) {
        if (sv_adminpassword.Value().empty()) {
            return;
        }
        
        if (!AdminServer) {
            return;
        }

        AdminServer->SendToIP(ToIP, Text);
    }
}

// Using declarations to bring into global namespace
using RconImpl::TAdminMessage;
using RconImpl::TMsg_RefreshX;
using RconImpl::TAdminServer;
using RconImpl::BroadcastMsg;
using RconImpl::SendMessageToAdmin;
using RconImpl::FindAdminFloodID;
using RconImpl::AddAdminFloodIP;
using RconImpl::UpdateAntiAdminFlood;

// Global variable
extern std::unique_ptr<TAdminServer> AdminServer;

#endif // RCON_H