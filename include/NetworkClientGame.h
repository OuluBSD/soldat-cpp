#ifndef NETWORK_CLIENT_GAME_H
#define NETWORK_CLIENT_GAME_H

//*******************************************************************************
//
//       Network Client Game Unit for SOLDAT
//
//       Placeholder header to fix build errors
//
//*******************************************************************************

#include "Net.h"
#include "Vector.h"

// Client game networking functions would go here
void ClientSendSnapshot();
void ClientReceiveSnapshot();
void ClientSendBulletSnapshot();
void ClientProcessServerMessages();

#endif // NETWORK_CLIENT_GAME_H