#ifndef FILESERVER_H
#define FILESERVER_H

//*******************************************************************************
//                                                                              
//       FileServer Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2012 Daniel Forssten              
//                                                                              
//*******************************************************************************

#include <string>
#include <thread>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cctype>
#include <algorithm>
#include "Server.h"
#include "Net.h"

// Forward declarations for HTTP server functionality
class THTTPServer;
class THTTPRequest;
class THTTPResponse;

// Type definitions
using TServerRequestHandler = void(*)(void* Sender, THTTPRequest& Request, THTTPResponse& Response);

class THTTPServer {
private:
    std::string FAddress;
    uint16_t FPort;
    bool FActive;
    TServerRequestHandler FOnRequest;
    
public:
    THTTPServer();
    ~THTTPServer();
    
    void SetOnRequest(TServerRequestHandler handler) { FOnRequest = handler; }
    void SetAddress(const std::string& address) { FAddress = address; }
    void SetPort(uint16_t port) { FPort = port; }
    void SetActive(bool active) { FActive = active; }
    bool GetActive() const { return FActive; }
    
    std::string GetAddress() const { return FAddress; }
    uint16_t GetPort() const { return FPort; }
    int GetConnectionCount() const { return 0; } // Placeholder
    
    void Start();
    void Stop();
};

class THTTPServerThread {
private:
    std::thread ServerThread;
    std::unique_ptr<THTTPServer> FServer;
    std::string FAddress;
    uint16_t FPort;
    TServerRequestHandler FOnRequest;
    
public:
    THTTPServerThread(const std::string& Address, uint16_t Port, TServerRequestHandler OnRequest);
    ~THTTPServerThread();
    
    THTTPServer* GetServer() const { return FServer.get(); }
    
    void Execute();
    void Terminate();
    void DoTerminate();
};

class TFileServer {
public:
    void DoHandleRequest(void* Sender, THTTPRequest& Request, THTTPResponse& Response);
};

// Function declarations
void StartFileServer();
void StopFileServer();

// Global variables
extern std::unique_ptr<THTTPServerThread> FServerThread;
extern std::unique_ptr<TFileServer> FFileServer;

namespace FileServerImpl {
    inline THTTPServer::THTTPServer() : FAddress("127.0.0.1"), FPort(0), FActive(false), FOnRequest(nullptr) {
    }

    inline THTTPServer::~THTTPServer() {
        if (FActive) {
            Stop();
        }
    }

    inline void THTTPServer::Start() {
        // In a real implementation, this would start the actual HTTP server
        std::cout << "[FileServer] Starting HTTP server on " << FAddress << ":" << FPort << std::endl;
        FActive = true;
    }

    inline void THTTPServer::Stop() {
        // In a real implementation, this would stop the actual HTTP server
        FActive = false;
    }

    inline THTTPServerThread::THTTPServerThread(const std::string& Address, uint16_t Port, 
                                                TServerRequestHandler OnRequest) 
        : FAddress(Address), FPort(Port), FOnRequest(OnRequest) {
        FServer = std::make_unique<THTTPServer>();
        FServer->SetAddress(FAddress);
        FServer->SetPort(FPort);
        FServer->SetOnRequest(FOnRequest);
        
        // Start the thread
        ServerThread = std::thread(&THTTPServerThread::Execute, this);
    }

    inline THTTPServerThread::~THTTPServerThread() {
        Terminate();
        if (ServerThread.joinable()) {
            ServerThread.join();
        }
    }

    inline void THTTPServerThread::Execute() {
        try {
            FServer->SetActive(true);
            while (FServer->GetActive()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate server running
            }
        } catch (const std::exception& e) {
            std::cout << "[FileServer] Error: " << e.what() << std::endl;
        }
    }

    inline void THTTPServerThread::DoTerminate() {
        if (FServer) {
            FServer->SetActive(false);
        }
    }

    inline void THTTPServerThread::Terminate() {
        if (FServer) {
            FServer->SetActive(false);
        }
    }

    inline void TFileServer::DoHandleRequest(void* Sender, THTTPRequest& Request, THTTPResponse& Response) {
        std::string uri = Request.URI;
        std::string userAgent = Request.UserAgent;
        std::string method = Request.Method;
        std::string remoteAddress = Request.RemoteAddress;

        // Check for proper user agent and method
        if ((userAgent != "soldatclient/1.8.0") || (method != "GET")) {
            Response.Free();
            return;
        }

        std::string servePath = UserDirectory + uri;

#ifdef STEAM_CODE
        bool workshopDownload = false;
        if (uri.find("/maps/workshop/") == 0) {  // Starts With '/maps/workshop/'
            std::vector<std::string> split;
            std::istringstream iss(uri);
            std::string segment;
            while (std::getline(iss, segment, '/')) {
                split.push_back(segment);
            }
            
            if (split.size() >= 4) {
                std::string itemIdStr = split[3];
                // Remove .smap extension if present
                if (itemIdStr.length() >= 5 && itemIdStr.substr(itemIdStr.length() - 5) == ".smap") {
                    itemIdStr = itemIdStr.substr(0, itemIdStr.length() - 5);
                }
                
                uint64_t itemId = 0;
                try {
                    itemId = std::stoull(itemIdStr);
                } catch (...) {
                    // Invalid item ID
                }
                
                if (itemId > 0) {
                    // Assuming SteamAPI functionality to check workshop items
                    // char path[4097];
                    // if (SteamAPI.UGC.GetItemInstallInfo(itemId, nullptr, path, 4096, nullptr)) {
                    //     // Find .smap file in workshop directory
                    //     // servePath = path + "/filename.smap";
                    //     workshopDownload = true;
                    // }
                }
            }
        }
#endif

        // Check if requesting a mod or map file
        bool isModOrMapFile = false;
        if ((uri.length() >= 6 && uri.substr(0, 5) == "/mods" && uri.substr(uri.length()-5) == ".smod") ||
            (uri.length() >= 6 && uri.substr(0, 5) == "/maps" && uri.substr(uri.length()-5) == ".smap")
#ifdef STEAM_CODE
            || workshopDownload
#endif
           ) {
            isModOrMapFile = true;
        }

        if (isModOrMapFile) {
            std::cout << "[FileServer] File request: " << uri << " by " << remoteAddress << std::endl;
            
            std::ifstream file(servePath, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::streamsize size = file.tellg();
                file.seekg(0);
                
                // Create a buffer to hold the file content
                std::vector<char> buffer(size);
                if (file.read(buffer.data(), size)) {
                    // Construct proper response
                    std::string filename = servePath.substr(servePath.find_last_of("/\\") + 1);
                    Response.SetHeader("content-disposition", "attachment; filename=\"" + filename + "\"");
                    Response.ContentType = "application/octet-stream";
                    Response.ContentLength = size;
                    Response.Content = std::string(buffer.begin(), buffer.end());
                    Response.SendContent();
                } else {
                    std::cout << "[FileServer] Error reading file: " << servePath << std::endl;
                }
                file.close();
            } else {
                std::cout << "[FileServer] File not found at: " << servePath << std::endl;
            }
        } else {
            Response.Free();
        }
    }

    inline void StartFileServer() {
        if (!FServerThread) {
            uint16_t port;
            if (fileserver_port.Value() == 0) {
                port = net_port.Value() + 10;
            } else {
                port = fileserver_port.Value();
            }

            FServerThread = std::make_unique<THTTPServerThread>(fileserver_ip.Value(), port, 
                                                                [](void* sender, THTTPRequest& req, THTTPResponse& resp) {
                                                                    FFileServer->DoHandleRequest(sender, req, resp);
                                                                });
            std::cout << "[FileServer] Starting fileserver on " + fileserver_ip.Value() + ":" + std::to_string(port) << std::endl;
        }
    }

    inline void StopFileServer() {
        if (FServerThread) {
            std::cout << "[FileServer] Stopping fileserver" << std::endl;
            try {
                // FServerThread->Terminate();
                // FServerThread->DoTerminate();
                FServerThread.reset(); // Reset the unique_ptr to trigger destruction
            } catch (const std::exception& e) {
                std::cout << "[FileServer] Error while stopping: " << e.what() << std::endl;
            }
        }
    }
}

// Using declarations to bring into global namespace
using FileServerImpl::THTTPServer;
using FileServerImpl::THTTPServerThread;
using FileServerImpl::TFileServer;
using FileServerImpl::StartFileServer;
using FileServerImpl::StopFileServer;

// Global variables
extern std::unique_ptr<THTTPServerThread> FServerThread = nullptr;
extern std::unique_ptr<TFileServer> FFileServer = nullptr;

#endif // FILESERVER_H