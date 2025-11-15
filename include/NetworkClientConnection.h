#ifndef NETWORK_CLIENT_CONNECTION_H
#define NETWORK_CLIENT_CONNECTION_H

//*******************************************************************************
//
//       Network Client Connection Unit for SOLDAT
//
//       Placeholder header to fix build errors
//
//*******************************************************************************

#include "Net.h"
#include <string>
#include <memory>

// Forward declaration of client network class
#ifdef SERVER_CODE
// Server doesn't need client network connection
#else

// Client network connection class
class TNetworkClientConnection {
public:
    TNetworkClientConnection();
    ~TNetworkClientConnection();
    
    bool Connect(const std::string& host, uint16_t port);
    void Disconnect();
    bool IsConnected() const;
    bool SendData(const void* data, size_t size);
    void ProcessEvents();
    
    // Additional client connection methods would go here
private:
    std::unique_ptr<TClientNetwork> FNetwork;
    bool FConnected;
};

// Global client connection instance
extern TNetworkClientConnection* ClientConnection;

#endif // SERVER_CODE

#endif // NETWORK_CLIENT_CONNECTION_H