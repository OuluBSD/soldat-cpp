#ifndef NETWORK_CLIENT_MESSAGES_H
#define NETWORK_CLIENT_MESSAGES_H

//*******************************************************************************
//
//       Network Client Messages Unit for SOLDAT
//
//       Placeholder header to fix build errors
//
//*******************************************************************************

#include "Net.h"
#include <string>

// Client message processing functions would go here
void ProcessServerHeartbeat(void* msg);
void ProcessServerSpriteSnapshot(void* msg);
void ProcessBulletSnapshot(void* msg);
void ProcessChatMessage(void* msg);
void ProcessServerThingSnapshot(void* msg);
void ProcessServerThingTaken(void* msg);
void ProcessSpriteDeath(void* msg);
void ProcessPlayerInfo(void* msg);
void ProcessPlayersList(void* msg);
void ProcessNewPlayer(void* msg);
void ProcessServerDisconnect(void* msg);
void ProcessPlayerDisconnect(void* msg);
void ProcessDeltaMovement(void* msg);
void ProcessDeltaWeapons(void* msg);
void ProcessDeltaHelmet(void* msg);
void ProcessDeltaMouseAim(void* msg);
void ProcessPing(void* msg);
void ProcessPong(void* msg);
void ProcessFlagInfo(void* msg);
void ProcessServerThingMustSnapshot(void* msg);
void ProcessIdleAnimation(void* msg);
void ProcessServerSpriteSnapshotMajor(void* msg);
void ProcessClientSpriteSnapshotMov(void* msg);
void ProcessClientSpriteSnapshotDead(void* msg);
void ProcessUnAccepted(void* msg);
void ProcessVoteOn(void* msg);
void ProcessVoteMap(void* msg);
void ProcessVoteMapReply(void* msg);
void ProcessVoteKick(void* msg);
void ProcessRequestThing(void* msg);
void ProcessServerVars(void* msg);
void ProcessServerSyncMsg(void* msg);
void ProcessClientFreeCam(void* msg);
void ProcessVoteOff(void* msg);
void ProcessSpecialMessage(void* msg);
void ProcessWeaponActiveMessage(void* msg);
void ProcessForcePosition(void* msg);
void ProcessForceVelocity(void* msg);
void ProcessForceWeapon(void* msg);
void ProcessChangeTeam(void* msg);
void ProcessRequestGame(void* msg);
void ProcessJoinServer(void* msg);
void ProcessPlaySound(void* msg);
void ProcessSyncCvars(void* msg);
void ProcessVoiceData(void* msg);

#endif // NETWORK_CLIENT_MESSAGES_H