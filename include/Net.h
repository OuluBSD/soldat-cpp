#ifndef NET_H
#define NET_H

//*******************************************************************************
//
//       Net Unit for SOLDAT
//
//       Copyright (c) 2002-03 Michal Marcinkowski
//
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>

#include "Vector.h"
#include "Constants.h"
#include "Weapons.h"
#include "Version.h"  // for SOLDAT_VERSION_CHARS
#include "Util.h"     // for SHA1

// Constants from Pascal net unit
const int B1 = 1;
const int B2 = 2;
const int B3 = 4;
const int B4 = 8;
const int B5 = 16;
const int B6 = 32;
const int B7 = 64;
const int B8 = 128;
const int B9 = 256;
const int B10 = 512;
const int B11 = 1024;
const int B12 = 2048;
const int B13 = 4096;
const int B14 = 8192;
const int B15 = 16384;
const int B16 = 32768;

// Message IDs
const int MsgID_Custom = 0;
const int MsgID_HeartBeat = MsgID_Custom + 2;
const int MsgID_ServerSpriteSnapshot = MsgID_Custom + 3;
const int MsgID_ClientSpriteSnapshot = MsgID_Custom + 4;
const int MsgID_BulletSnapshot = MsgID_Custom + 5;
const int MsgID_ChatMessage = MsgID_Custom + 6;
const int MsgID_ServerSkeletonSnapshot = MsgID_Custom + 7;
const int MsgID_MapChange = MsgID_Custom + 8;
const int MsgID_ServerThingSnapshot = MsgID_Custom + 9;
const int MsgID_ThingTaken = MsgID_Custom + 12;
const int MsgID_SpriteDeath = MsgID_Custom + 13;
const int MsgID_PlayerInfo = MsgID_Custom + 15;
const int MsgID_PlayersList = MsgID_Custom + 16;
const int MsgID_NewPlayer = MsgID_Custom + 17;
const int MsgID_ServerDisconnect = MsgID_Custom + 18;
const int MsgID_PlayerDisconnect = MsgID_Custom + 19;
const int MsgID_Delta_Movement = MsgID_Custom + 21;
const int MsgID_Delta_Weapons = MsgID_Custom + 25;
const int MsgID_Delta_Helmet = MsgID_Custom + 26;
const int MsgID_Delta_MouseAim = MsgID_Custom + 29;
const int MsgID_Ping = MsgID_Custom + 30;
const int MsgID_Pong = MsgID_Custom + 31;
const int MsgID_FlagInfo = MsgID_Custom + 32;
const int MsgID_ServerThingMustSnapshot = MsgID_Custom + 33;
const int MsgID_IdleAnimation = MsgID_Custom + 37;
const int MsgID_ServerSpriteSnapshot_Major = MsgID_Custom + 41;
const int MsgID_ClientSpriteSnapshot_Mov = MsgID_Custom + 42;
const int MsgID_ClientSpriteSnapshot_Dead = MsgID_Custom + 43;
const int MsgID_UnAccepted = MsgID_Custom + 44;
const int MsgID_VoteOn = MsgID_Custom + 45;
const int MsgID_VoteMap = MsgID_Custom + 46;
const int MsgID_VoteMapReply = MsgID_Custom + 47;
const int MsgID_VoteKick = MsgID_Custom + 48;
const int MsgID_RequestThing = MsgID_Custom + 51;
const int MsgID_ServerVars = MsgID_Custom + 52;
const int MsgID_ServerSyncMsg = MsgID_Custom + 54;
const int MsgID_ClientFreeCam = MsgID_Custom + 55;
const int MsgID_VoteOff = MsgID_Custom + 56;
const int MsgID_FaeData = MsgID_Custom + 57;
const int MsgID_RequestGame = MsgID_Custom + 58;
const int MsgID_ForcePosition = MsgID_Custom + 60;
const int MsgID_ForceVelocity = MsgID_Custom + 61;
const int MsgID_ForceWeapon = MsgID_Custom + 62;
const int MsgID_ChangeTeam = MsgID_Custom + 63;
const int MsgID_SpecialMessage = MsgID_Custom + 64;
const int MsgID_WeaponActiveMessage = MsgID_Custom + 65;
const int MsgID_JoinServer = MsgID_Custom + 68;
const int MsgID_PlaySound = MsgID_Custom + 70;
const int MsgID_SyncCvars = MsgID_Custom + 71;
const int MsgID_VoiceData = MsgID_Custom + 72;

const int MAX_PLAYERS = 32;

// ControlMethod
const int HUMAN = 1;
const int BOT = 2;

// Request Reply States
const int OK = 1;
const int WRONG_VERSION = 2;
const int WRONG_PASSWORD = 3;
const int BANNED_IP = 4;
const int SERVER_FULL = 5;
const int INVALID_HANDSHAKE = 8;
const int WRONG_CHECKSUM = 9;
const int ANTICHEAT_REQUIRED = 10;
const int ANTICHEAT_REJECTED = 11;
const int STEAM_ONLY = 12;

const int LAN = 1;
const int INTERNET = 0;

// FLAG INFO
const int RETURNRED = 1;
const int RETURNBLUE = 2;
const int CAPTURERED = 3;
const int CAPTUREBLUE = 4;

// Kick/Ban Why's
const int KICK_UNKNOWN = 0;
const int KICK_NORESPONSE = 1;
const int KICK_NOCHEATRESPONSE = 2;
const int KICK_CHANGETEAM = 3;
const int KICK_PING = 4;
const int KICK_FLOODING = 5;
const int KICK_CONSOLE = 6;
const int KICK_CONNECTCHEAT = 7;
const int KICK_CHEAT = 8;
const int KICK_LEFTGAME = 9;
const int KICK_VOTED = 10;
const int KICK_AC = 11;
const int KICK_SILENT = 12;
const int KICK_STEAMTICKET = 13;
const int _KICK_END = 14;

// Join types
const int JOIN_NORMAL = 0;
const int JOIN_SILENT = 1;

// RECORD
const int NETW = 0;
const int REC = 1;

const int CLIENTPLAYERRECIEVED_TIME = 3 * 60;

const int FLOODIP_MAX = 18;
const int MAX_FLOODIPS = 1000;
const int MAX_BANIPS = 1000;

const int PLAYERNAME_CHARS = 24;
const int PLAYERHWID_CHARS = 11;
const int MAPNAME_CHARS = 64;
const int REASON_CHARS = 26;

const int ACTYPE_NONE = 0;
const int ACTYPE_FAE = 1;

const int MSGTYPE_CMD = 0;
const int MSGTYPE_PUB = 1;
const int MSGTYPE_TEAM = 2;
const int MSGTYPE_RADIO = 3;

// Forward declarations
struct TPlayer;

// TNetwork class declaration
class TNetwork {
protected:
    bool FInit;
    bool FActive;
    // For now, using a simple placeholder for networking structures
    // In a real implementation, this would use actual network API structures
    void* FAddress; 
    void* FPeer;
    void* FHost;
    #ifdef SERVER_CODE
    void* FPollGroup;
    #endif
    void* NetworkingSockets;
    void* NetworkingUtils;

public:
    bool GetActive() const { return FActive; }
    void SetActive(bool value) { FActive = value; }
    
    TNetwork();
    virtual ~TNetwork();

    bool Disconnect(bool Now);
    void FlushMsg();
    virtual void ProcessEvents(void* pInfo) = 0; // Pure virtual function

    std::string GetDetailedConnectionStatus(void* hConn);
    void* GetQuickConnectionStatus(void* hConn);
    void SetConnectionName(void* hConn, const std::string& Name);
    std::string GetStringAddress(void* pAddress, bool Port);

    bool SetGlobalConfigValueInt32(int eValue, int32_t val);
    bool SetGlobalConfigValueFloat(int eValue, float val);
    bool SetGlobalConfigValueString(int eValue, const char* val);
    bool SetConnectionConfigValueInt32(void* hConn, int eValue, int32_t val);
    bool SetConnectionConfigValueFloat(void* hConn, int eValue, int32_t val);
    bool SetConnectionConfigValueString(void* hConn, int eValue, int32_t val);

    void SetDebugLevel(int Level);

    void* GetHost() const { return FHost; }
    void* GetPeer() const { return FPeer; }
    void* GetNetworkingSocket() const { return NetworkingSockets; }
    void* GetNetworkingUtil() const { return NetworkingUtils; }

    uint16_t GetPort() const;
    void SetPort(uint16_t value);
    void* GetAddress() const { return FAddress; }
};

// Server vs Client specific network classes
#ifdef SERVER_CODE
class TServerNetwork : public TNetwork {
public:
    void ProcessEvents(void* pInfo) override;
    TServerNetwork(const std::string& Host, uint16_t Port);
    virtual ~TServerNetwork();
    void ProcessLoop();
    void HandleMessages(void* IncomingMsg);
    bool SendData(const void* Data, int Size, void* peer, int Flags);
    void UpdateNetworkStats(uint8_t Player);
};
#else
class TClientNetwork : public TNetwork {
public:
    void ProcessEvents(void* pInfo) override;
    TClientNetwork();
    virtual ~TClientNetwork();
    bool Connect(const std::string& Host, uint16_t Port);
    void ProcessLoop();
    void HandleMessages(void* IncomingMsg);
    bool SendData(const void* Data, int Size, int Flags);
};
#endif

// Network Player Class
class TPlayer {
public:
    // Client/server shared stuff:
    std::string Name;
    uint32_t ShirtColor, PantsColor, SkinColor, HairColor, JetColor;
    int Kills, Deaths;
    uint8_t Flags;
    int PingTicks, PingTicksB, PingTime, Ping;
    uint16_t RealPing;
    uint8_t ConnectionQuality;
    uint8_t Team;
    uint8_t ControlMethod;
    uint8_t Chain, HeadCap, HairStyle;
    uint8_t SecWep;
    uint8_t Camera;
    uint8_t Muted;
    uint8_t SpriteNum; // 0 if no sprite exists yet
    bool DemoPlayer;
    
    #ifdef STEAM_CODE
    uint64_t SteamID;
    bool SteamStats;
    int LastReceiveVoiceTime;
    bool SteamFriend;
    #endif

    // Server only below this line:
    #ifdef SERVER_CODE
    std::string IP;
    int Port;

    // anti-cheat client handles and state
    #ifdef ENABLE_FAE
    bool FaeResponsePending;
    bool FaeKicked;
    int FaeTicks;
    // TFaeSecret FaeSecret; // Assuming this is defined elsewhere
    #endif

    void* Peer;  // Placeholder for actual network connection handle
    std::string HWID;
    int PlayTime;
    bool GameRequested;

    // counters for warnings:
    uint8_t ChatWarnings;
    uint8_t TKWarnings;

    // anti mass flag counters:
    int ScoresPerSecond;
    int GrabsPerSecond;
    bool GrabbedInBase;  // To prevent false accusations
    uint8_t StandingPolyType;  // Testing
    uint8_t KnifeWarnings;

    TPlayer();  // Constructor
    virtual ~TPlayer(); // Destructor
    #endif  // SERVER_CODE

    void ApplyShirtColorFromTeam(); // TODO remove, see comment before Name
    std::unique_ptr<TPlayer> Clone();
};

// TPlayers type using vector
using TPlayers = std::vector<std::unique_ptr<TPlayer>>;

// Message structure definitions
struct TMsgHeader {
    uint8_t ID;
};

struct TMsg_StringMessage {
    TMsgHeader Header;
    uint8_t Num;
    uint8_t MsgType;
    // Variable length array of WideChar would follow
};

struct TMsg_Ping {
    TMsgHeader Header;
    uint8_t PingTicks;
    uint8_t PingNum;
};

struct TMsg_Pong {
    TMsgHeader Header;
    uint8_t PingNum;
};

// HEARTBEAT TYPE
// - every while information about frags, server status etc.
struct TMsg_HeartBeat {
    TMsgHeader Header;
    uint32_t MapID;
    uint16_t TeamScore[4 + 1]; // 1-indexed in Pascal
    bool Active[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint16_t Kills[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Caps[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Team[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint16_t Deaths[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Ping[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint16_t RealPing[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t ConnectionQuality[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Flags[MAX_PLAYERS + 1]; // 1-indexed in Pascal
};

// SERVERSPRITESNAPSHOT TYPE
// - servers object status - uses for all objects in game
struct TMsg_ServerSpriteSnapshot {
    TMsgHeader Header;
    uint8_t Num;
    TVector2 Pos, Velocity;
    int16_t MouseAimX, MouseAimY;
    uint8_t Position;
    uint16_t Keys16;
    uint8_t Look;
    float Vest;
    float Health;
    uint8_t AmmoCount, GrenadeCount;
    uint8_t WeaponNum, SecondaryWeaponNum;
    int32_t ServerTicks;
};

struct TMsg_ServerSpriteSnapshot_Major {
    TMsgHeader Header;
    uint8_t Num;
    TVector2 Pos, Velocity;
    float Health;
    int16_t MouseAimX, MouseAimY;
    uint8_t Position;
    uint16_t Keys16;
    int32_t ServerTicks;
};

struct TMsg_ServerVars {
    TMsgHeader Header;
    float Damage[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint8_t Ammo[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint16_t ReloadTime[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float Speed[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint8_t BulletStyle[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint16_t StartUpTime[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    int16_t Bink[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint16_t FireInterval[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float MovementAcc[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float BulletSpread[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint16_t Recoil[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float Push[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float InheritedVelocity[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float ModifierHead[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float ModifierChest[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    float ModifierLegs[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint8_t NoCollision[ORIGINAL_WEAPONS + 1]; // 1-indexed in Pascal
    uint8_t WeaponActive[MAIN_WEAPONS + 1]; // 1-indexed in Pascal
};

struct TMsg_ServerSyncCvars {
    TMsgHeader Header;
    uint8_t ItemCount;
    uint8_t Data[1]; // Variable size
};

// CLIENTSPRITESNAPSHOT TYPE
// - current players status
struct TMsg_ClientSpriteSnapshot {
    TMsgHeader Header;
    uint8_t AmmoCount, SecondaryAmmoCount;
    uint8_t WeaponNum, SecondaryWeaponNum;
    uint8_t Position;
};

struct TMsg_ClientSpriteSnapshot_Mov {
    TMsgHeader Header;
    TVector2 Pos, Velocity;
    uint16_t Keys16;
    int16_t MouseAimX, MouseAimY;
};

struct TMsg_ClientSpriteSnapshot_Dead {
    TMsgHeader Header;
    uint8_t CameraFocus;
};

// BULLETSNAPSHOT TYPE
// - for server's bullet information
struct TMsg_BulletSnapshot {
    TMsgHeader Header;
    uint8_t Owner, WeaponNum;
    TVector2 Pos, Velocity;
    uint16_t Seed;
    bool Forced; // CreateBullet() forced bullet?
};

// BULLETSNAPSHOT TYPE
// - for clients' bullet information
struct TMsg_ClientBulletSnapshot {
    TMsgHeader Header;
    uint8_t WeaponNum;
    TVector2 Pos, Velocity;
    uint16_t Seed;
    int32_t ClientTicks;
};

// SERVERSKELETONSNAPSHOT TYPE
// - info on the sprites skeleton - used when sprite is DeadMeat
struct TMsg_ServerSkeletonSnapshot {
    TMsgHeader Header;
    uint8_t Num;
    // Constraints: byte;
    int16_t RespawnCounter;
};

// MAPCHANGE TYPE
struct TMsg_MapChange {
    TMsgHeader Header;
    int16_t Counter;
    char MapName[MAPNAME_CHARS + 1]; // Pascal strings have size + 1
    uint8_t MapChecksum[20]; // SHA1 is 20 bytes
};

// SERVERTHINGSNAPSHOT TYPE
// - info on the things in world that move
struct TMsg_ServerThingSnapshot {
    TMsgHeader Header;
    uint8_t Num, Owner, Style, HoldingSprite;
    TVector2 Pos[4 + 1]; // 1-indexed in Pascal
    TVector2 OldPos[4 + 1]; // 1-indexed in Pascal
};

// SERVERTHINGMUSTSNAPSHOT TYPE
// - info on the things in world
struct TMsg_ServerThingMustSnapshot {
    TMsgHeader Header;
    uint8_t Num, Owner, Style, HoldingSprite;
    TVector2 Pos[4 + 1]; // 1-indexed in Pascal
    TVector2 OldPos[4 + 1]; // 1-indexed in Pascal
    int32_t Timeout;
};

// SERVERTHINGTAKENINFO TYPE
// - sent when thing is taken
struct TMsg_ServerThingTaken {
    TMsgHeader Header;
    uint8_t Num, Who;
    uint8_t Style, AmmoCount;
};

// SPRITEDEATH TYPE
// - if sprite dies this is sent
struct TMsg_SpriteDeath {
    TMsgHeader Header;
    uint8_t Num, Killer, KillBullet, Where;
    uint8_t Constraints;
    TVector2 Pos[16 + 1]; // 1-indexed in Pascal
    TVector2 OldPos[16 + 1]; // 1-indexed in Pascal
    float Health;
    uint8_t OnFire;
    int16_t RespawnCounter;
    float ShotDistance, ShotLife;
    uint8_t ShotRicochet;
};

// REQUEST GAME TYPE
struct TMsg_RequestGame {
    TMsgHeader Header;
    char Version[SOLDAT_VERSION_CHARS]; // Pascal strings have size + 1
    uint8_t Forwarded;
    uint8_t HaveAntiCheat;
    char HardwareID[PLAYERHWID_CHARS + 1]; // Pascal strings have size + 1
    char Password[25]; // 0..24 in Pascal
};

// PLAYER INFO TYPE
struct TMsg_PlayerInfo {
    TMsgHeader Header;
    char Name[PLAYERNAME_CHARS]; // 0..PLAYERNAME_CHARS-1 in Pascal
    uint8_t Look;
    uint8_t Team;
    uint32_t ShirtColor, PantsColor, SkinColor, HairColor, JetColor;
    uint8_t GameModChecksum[20]; // SHA1 is 20 bytes
    uint8_t CustomModChecksum[20]; // SHA1 is 20 bytes
};

// PLAYERS LIST TYPE
struct TMsg_PlayersList {
    TMsgHeader Header;
    char ModName[MAPNAME_CHARS]; // 0..MAPNAME_CHARS-1 in Pascal
    uint8_t ModChecksum[20]; // SHA1 is 20 bytes
    char MapName[MAPNAME_CHARS]; // 0..MAPNAME_CHARS-1 in Pascal
    uint8_t MapChecksum[20]; // SHA1 is 20 bytes
    uint8_t Players;
    char Name[MAX_PLAYERS + 1][PLAYERNAME_CHARS]; // 1-indexed in Pascal
    uint32_t ShirtColor[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint32_t PantsColor[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint32_t SkinColor[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint32_t HairColor[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint32_t JetColor[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Team[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t PredDuration[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint8_t Look[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    TVector2 Pos[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    TVector2 Vel[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    uint64_t SteamID[MAX_PLAYERS + 1]; // 1-indexed in Pascal
    int CurrentTime;
    int32_t ServerTicks;
    bool AntiCheatRequired;
};

// REJECTED CONNECTION TYPE
struct TMsg_UnAccepted {
    TMsgHeader Header;
    uint8_t State;
    char Version[SOLDAT_VERSION_CHARS]; // Pascal strings have size + 1
    char Text[1]; // Variable size
};

// NEW PLAYER TYPE
struct TMsg_NewPlayer {
    TMsgHeader Header;
    uint8_t Num;
    uint8_t AdoptSpriteID;
    uint8_t JoinType;
    char Name[PLAYERNAME_CHARS]; // 0..PLAYERNAME_CHARS-1 in Pascal
    uint32_t ShirtColor, PantsColor, SkinColor, HairColor, JetColor;
    uint8_t Team;
    uint8_t Look;
    TVector2 Pos;
    uint64_t SteamID;
};

// SERVER DISCONNECT TYPE
struct TMsg_ServerDisconnect {
    TMsgHeader Header;
};

// PLAYER DISCONNECT TYPE
struct TMsg_PlayerDisconnect {
    TMsgHeader Header;
    uint8_t Num;
    uint8_t Why;
};

// IDLE ANIMATION TYPE
struct TMsg_IdleAnimation {
    TMsgHeader Header;
    uint8_t Num;
    int16_t IdleRandom;
};

struct TMsg_ClientFreeCam {
    TMsgHeader Header;
    uint8_t FreeCamOn;
    TVector2 TargetPos;
};

// DELTAS
struct TMsg_ServerSpriteDelta_Movement {
    TMsgHeader Header;
    uint8_t Num;
    TVector2 Pos, Velocity;
    uint16_t Keys16;
    int16_t MouseAimX, MouseAimY;
    int32_t ServerTick;
};

struct TMsg_ServerSpriteDelta_MouseAim {
    TMsgHeader Header;
    uint8_t Num;
    int16_t MouseAimX, MouseAimY;
};

struct TMsg_ServerSpriteDelta_Weapons {
    TMsgHeader Header;
    uint8_t Num;
    uint8_t WeaponNum, SecondaryWeaponNum;
    uint8_t AmmoCount;
};

struct TMsg_ServerSpriteDelta_Helmet {
    TMsgHeader Header;
    uint8_t Num;
    uint8_t WearHelmet;
};

struct TMsg_ServerFlagInfo {
    TMsgHeader Header;
    uint8_t Style, Who;
};

struct TMsg_ServerSyncMsg {
    TMsgHeader Header;
    int Time;
    uint8_t Pause;
};

struct TMsg_ForcePosition {
    TMsgHeader Header;
    TVector2 Pos;
    uint8_t PlayerID;
};

struct TMsg_ForceVelocity {
    TMsgHeader Header;
    TVector2 Vel;
    uint8_t PlayerID;
};

struct TMsg_ForceWeapon {
    TMsgHeader Header;
    uint8_t WeaponNum, SecondaryWeaponNum;
    uint8_t AmmoCount, SecAmmoCount;
};

struct TMsg_ChangeTeam {
    TMsgHeader Header;
    uint8_t Team;
};

struct TMsg_RequestThing {
    TMsgHeader Header;
    uint8_t ThingID;
};

// Voting Messages

// VOTING ON TYPE
struct TMsg_VoteOn {
    TMsgHeader Header;
    uint8_t VoteType;
    uint16_t Timer;
    uint8_t Who;
    char TargetName[MAPNAME_CHARS]; // 0..MAPNAME_CHARS-1 in Pascal
    char Reason[REASON_CHARS]; // 0..REASON_CHARS-1 in Pascal
};

// VOTING OFF TYPE
struct TMsg_VoteOff {
    TMsgHeader Header;
};

// VOTING MAP LIST QUERY
struct TMsg_VoteMap {
    TMsgHeader Header;
    uint16_t MapID;
};

// VOTING MAP LIST RESPONSE
struct TMsg_VoteMapReply {
    TMsgHeader Header;
    uint16_t Count;
    char MapName[MAPNAME_CHARS + 1]; // Pascal strings have size + 1
};

// VOTING KICK TYPE
struct TMsg_VoteKick {
    TMsgHeader Header;
    uint8_t Ban;
    uint8_t Num;
    char Reason[REASON_CHARS + 1]; // Pascal strings have size + 1
};

// MESSAGE PACKET
struct TMsg_ServerSpecialMessage {
    TMsgHeader Header;
    uint8_t MsgType;  // 0 - console, 1 - big text, 2 - world text
    uint8_t LayerId;  // only used for big text and world text
    int Delay;
    float Scale;
    uint32_t Color;
    float X, Y;
    char Text[1]; // Variable size
};

// HIDE/SHOW WEAPON IN MENU FOR SPECIFIC PLAYER
struct TMsg_WeaponActiveMessage {
    TMsgHeader Header;
    uint8_t Active, Weapon;
};

struct TMsg_JoinServer {
    TMsgHeader Header;
    uint32_t IP;
    uint16_t Port;
    char ShowMsg[51]; // 0..50 in Pascal
};

struct TMsg_PlaySound {
    TMsgHeader Header;
    char Name[27]; // 0..26 in Pascal
    TVector2 Emitter;
};

struct TMsg_VoiceData {
    TMsgHeader Header;
    uint8_t Speaker;
    char Data[1]; // Variable size
};

// Global variables declaration
extern int MainTickCounter;
// Stores all network-generated TPlayer objects
extern TPlayers Players;

#ifndef SERVER_CODE
extern int ClientTickCount, LastHeartBeatCounter;
extern bool ClientPlayerReceived, ClientPlayerSent;
extern int ClientPlayerReceivedCounter;
extern bool ClientVarsRecieved;
extern bool RequestingGame;
extern int NoHeartbeatTime;
extern std::string VoteMapName;
extern uint16_t VoteMapCount;
#else
extern int ServerTickCounter;
extern int NoClientUpdateTime[MAX_PLAYERS + 1];
extern int MessagesASecNum[MAX_PLAYERS + 1];
extern uint8_t FloodWarnings[MAX_PLAYERS + 1];
extern uint8_t PingWarnings[MAX_PLAYERS + 1];
extern int BulletTime[MAX_PLAYERS + 1];
extern int GrenadeTime[MAX_PLAYERS + 1];
extern bool KnifeCan[MAX_PLAYERS + 1];
extern int LastPlayerNum[MAX_PLAYERS + 1];
extern bool PlayerReady[MAX_PLAYERS + 1];
extern int PlayerWarnings[MAX_PLAYERS + 1];
extern int PlayerWarningsType[MAX_PLAYERS + 1];
extern int PlayerWarningTime[MAX_PLAYERS + 1];
extern int PlayerLastChat[MAX_PLAYERS + 1];
extern bool PlayerMuted[MAX_PLAYERS + 1];
extern int PlayerTK[MAX_PLAYERS + 1];
extern int TKTime[MAX_PLAYERS + 1];
extern std::string FloodIP[MAX_FLOODIPS + 1];
extern int FloodNum[MAX_FLOODIPS + 1];
extern std::string LastReqIP[3 + 1];
extern uint8_t LastReqID;
extern std::string DropIP;
extern std::string AdminFloodIP[MAX_ADMIN_FLOOD_IPS + 1];
extern std::string LastAdminIPs[MAX_LAST_ADMIN_IPS + 1];
extern int AdminIPCounter;
extern std::unique_ptr<TPlayer> DummyPlayer;
extern std::string LastWepMod2;
extern std::string LastReqIP2[3 + 1];
extern uint8_t LastReqID2;
extern int LastReqTime[3 + 1];
extern int LastReqTime2[3 + 1];
extern std::string LastReqIP3[4 + 1];
extern uint8_t LastReqID3;
extern int LastReqTime3[4 + 1];
extern int LastReqTime4[4 + 1];
extern int LastReqTime5[4 + 1];
extern int LastReqTime6[4 + 1];
extern uint8_t LastReqID4;
extern std::string LastReqIP4[4 + 1];
extern std::string LastReqIP5[4 + 1];
extern std::string LastReqIP6[4 + 1];
extern int LastReqTime7[4 + 1];
extern std::string LastReqIP7[4 + 1];
extern uint8_t LastReqID5;
extern uint8_t LastReqID6;
extern uint8_t LastReqID7;
extern bool PlayerLastReqGood[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID[MAX_PLAYERS + 1];
extern int PlayerLastReqTime[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood2[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID2[MAX_PLAYERS + 1];
extern int PlayerLastReqTime2[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood3[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID3[MAX_PLAYERS + 1];
extern int PlayerLastReqTime3[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood4[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID4[MAX_PLAYERS + 1];
extern int PlayerLastReqTime4[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood5[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID5[MAX_PLAYERS + 1];
extern int PlayerLastReqTime5[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood6[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID6[MAX_PLAYERS + 1];
extern int PlayerLastReqTime6[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood7[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID7[MAX_PLAYERS + 1];
extern int PlayerLastReqTime7[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood8[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID8[MAX_PLAYERS + 1];
extern int PlayerLastReqTime8[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood9[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID9[MAX_PLAYERS + 1];
extern int PlayerLastReqTime9[MAX_PLAYERS + 1];
extern bool PlayerLastReqGood10[MAX_PLAYERS + 1];
extern uint8_t PlayerLastReqID10[MAX_PLAYERS + 1];
extern int PlayerLastReqTime10[MAX_PLAYERS + 1];
extern int PlayerTKCounter[MAX_PLAYERS + 1];
extern int PlayerTKTime[MAX_PLAYERS + 1];
extern int PlayerTKCounter2[MAX_PLAYERS + 1];
extern int PlayerTKTime2[MAX_PLAYERS + 1];
extern int PlayerTKCounter3[MAX_PLAYERS + 1];
extern int PlayerTKTime3[MAX_PLAYERS + 1];
extern int PlayerTKCounter4[MAX_PLAYERS + 1];
extern int PlayerTKTime4[MAX_PLAYERS + 1];
extern int PlayerTKCounter5[MAX_PLAYERS + 1];
extern int PlayerTKTime5[MAX_PLAYERS + 1];
extern int PlayerTKCounter6[MAX_PLAYERS + 1];
extern int PlayerTKTime6[MAX_PLAYERS + 1];
extern int PlayerTKCounter7[MAX_PLAYERS + 1];
extern int PlayerTKTime7[MAX_PLAYERS + 1];
extern int PlayerTKCounter8[MAX_PLAYERS + 1];
extern int PlayerTKTime8[MAX_PLAYERS + 1];
extern int PlayerTKCounter9[MAX_PLAYERS + 1];
extern int PlayerTKTime9[MAX_PLAYERS + 1];
extern int PlayerTKCounter10[MAX_PLAYERS + 1];
extern int PlayerTKTime10[MAX_PLAYERS + 1];
extern int MassFlagCount[MAX_PLAYERS + 1];
extern int MassFlagTime[MAX_PLAYERS + 1];
#endif

#endif // NET_H