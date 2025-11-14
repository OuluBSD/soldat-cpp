#ifndef CONSTANTS_H
#define CONSTANTS_H

//*******************************************************
//                                                      
//       Constants Unit                                 
//                                                      
//       Copyright (c) 2011 Gregor A. Cieslak           
//                                                      
//*******************************************************

// Define some conditional compilation macros if needed
#ifdef SERVER
#define SERVER_CODE
#else
#define CLIENT_CODE
#endif

// Font constants
const char* DEFAULT_FONT = "play-regular.ttf";

// Head styles
const int HEADSTYLE_NONE   = 0;
const int HEADSTYLE_HELMET = 1;
const int HEADSTYLE_HAT    = 2;

// Display constants
const int DEFAULT_WIDTH  = 640;
const int DEFAULT_HEIGHT = 480;

const int DEFAULT_GOALTICKS = 60;

const int SCALE = 3;

const double MAX_FOV = 1.78;
#ifdef SERVER_CODE
const int MAX_GAME_WIDTH = static_cast<int>(480 * MAX_FOV);
#else
const double MIN_FOV = 1.25;
const int MAX_BIG_MESSAGES = 255;
#endif

// Speeds
const double RUNSPEED       = 0.118;
const double RUNSPEEDUP     = RUNSPEED / 6;
const double FLYSPEED       = 0.03;
const double JUMPSPEED      = 0.66;
const double CROUCHRUNSPEED = RUNSPEED / 0.6;
const double PRONESPEED     = RUNSPEED * 4.0;
const double ROLLSPEED      = RUNSPEED / 1.2;
const double JUMPDIRSPEED   = 0.30;
const double JETSPEED       = 0.10;
const double CAMSPEED       = 0.14;

const int CLUSTER_GRENADES = 3;

// Aim distances
const double DEFAULTAIMDIST = 7;
const double SNIPERAIMDIST  = 3.5;
const double CROUCHAIMDIST  = 4.5;
const double SPECTATORAIMDIST = 30;
const double AIMDISTINCR = 0.05;

const int BULLETCHECKARRAYSIZE = 20;
const int MAX_LOGFILESIZE = 512000;
const int SOUND_MAXDIST = 750;
const int SOUND_PANWIDTH = 1000;
const int SOUND_METERLENGTH = 2000;

// Trails
const int BULLETTRAIL = 13;
const int M79TRAIL    = 6;

#ifndef SERVER_CODE
const int BULLETLENGTH = 21;
#endif

// Healths
const int DEFAULT_HEALTH      = 150;
const int REALISTIC_HEALTH    = 65;
const int BRUTALDEATHHEALTH   = -400;
const int HEADCHOPDEATHHEALTH = -90;
const int HELMETFALLHEALTH    = 70;
const int HURT_HEALTH         = 25;

// Time
const int PERMANENT      = -1000;
const int SECOND         = 60;
const int HALF_MINUTE    = SECOND * 30;
const int MINUTE         = SECOND * 60;
const int FIVE_MINUTES   = MINUTE * 5;
const int TWENTY_MINUTES = MINUTE * 20;
const int HALF_HOUR      = MINUTE * 30;
const int SIXTY_MINUTES  = MINUTE * 60;
const int HOUR           = SIXTY_MINUTES;
const int DAY            = HOUR * 24;

#ifndef SERVER_CODE
const int MORECHATTEXT = 60;
const int MAXCHATTEXT  = 85;
// Display time for chars
const int SPACECHARDELAY = 68;
const int CHARDELAY      = 25;
#endif

const int MAX_CHATDELAY = SECOND * 7 + 40;

const int KILLCONSOLE_SEPARATE_HEIGHT = 8;

// Sound
const int DEFAULT_VOLUME_SETTING = 50;

// Animations
const int EXPLOSION_ANIMS = 16;
const int SMOKE_ANIMS     = 10;

const double EXPLOSION_IMPACT_MULTIPLY     = 3.75;
const double EXPLOSION_DEADIMPACT_MULTIPLY = 4.5;

const int BULLET_TIMEOUT   = SECOND * 7;
const int GRENADE_TIMEOUT  = SECOND * 3;
const int M2BULLET_TIMEOUT = SECOND;
const int FLAMER_TIMEOUT   = 32;
const int MELEE_TIMEOUT    = 1;

const int M2HITMULTIPLY  = 2;
const int M2GUN_OVERAIM  = 4;
const int M2GUN_OVERHEAT = 18;
const int GUNRESISTTIME  = SECOND * 20;

const int GUN_RADIUS  = 10;
const int BOW_RADIUS  = 20;
const int KIT_RADIUS  = 12;
const int STAT_RADIUS = 15;

const int ARROW_RESIST = 280;
const double ILUMINATESPEED = 0.085;
const double MINMOVEDELTA  = 0.63;

const double POSDELTA = 60.0;
const double VELDELTA = 0.27;

const int MOUSEAIMDELTA = 30;
const int SPAWNRANDOMVELOCITY = 25;

const int FLAG_TIMEOUT     = SECOND * 25;
const int WAYPOINTTIMEOUT  = SECOND * 5 + 20;  // = 320
const int WAYPOINT_TIMEOUT = 480; // TODO: why the duplication?

const int WAYPOINTSEEKRADIUS = 21;

const int DEFAULT_INTEREST_TIME = SECOND * 5 + 50;
const int FLAG_INTEREST_TIME    = SECOND * 25;
const int BOW_INTEREST_TIME     = SECOND * 41 + 40;

const int DEFAULT_MAPCHANGE_TIME = SECOND * 5 + 20;

const int MEDIKITTHINGSDIV    = 23;
const int GRENADEKITTHINGSDIV = 23;

const int CONNECTIONPROBLEM_TIME  = SECOND * 4;
const int CONNECTIONPROBLEM_TIME2 = SECOND * 5;

const int DISCONNECTION_TIME        = SECOND * 15;

const int KILLMESSAGEWAIT       = SECOND * 4;
const int CAPTUREMESSAGEWAIT    = SECOND * 6;
const int GAMESTARTMESSAGEWAIT  = SECOND * 5 + 20;
const int CAPTURECTFMESSAGEWAIT = SECOND * 7;

const int BLOOD_RANDOM_NORMAL = 10;
const int BLOOD_RANDOM_LOW    = 22;
const int BLOOD_RANDOM_HIGH   = 6;

const int TORCH_RANDOM_NORMAL = 6;
const int TORCH_RANDOM_LOW    = 12;

const int FIRE_RANDOM_HIGH   = 30;
const int FIRE_RANDOM_NORMAL = 50;
const int FIRE_RANDOM_LOW    = 70;

const int CLIENTMAXPOSITIONDELTA = 169;
const int DEFAULT_CEASEFIRE_TIME = 90;
const int PREDATORALPHA = 5;
const int DEFAULTVEST = 100;

const int FLAMERBONUSTIME    = 600;
const int PREDATORBONUSTIME  = 1500;
const int BERSERKERBONUSTIME = 900;

const int FLAMERBONUS_RANDOM    = 5;
const int PREDATORBONUS_RANDOM  = 5;
const int VESTBONUS_RANDOM      = 4;
const int BERSERKERBONUS_RANDOM = 4;
const int CLUSTERBONUS_RANDOM   = 4;

// Bonus types
const int BONUS_NONE      = 0;
const int BONUS_GRENADES  = 17;
const int BONUS_FLAMEGOD  = 18;
const int BONUS_PREDATOR  = 19;
const int BONUS_VEST      = 20;
const int BONUS_BERSERKER = 21;
const int BONUS_CLUSTERS  = 22;

const int CURSORSPRITE_DISTANCE = 15;
const int CLIENTSTOPMOVE_RETRYS = 90;
const int MULTIKILLINTERVAL = 180;

const int DEFAULT_IDLETIME = SECOND * 8;
const int LONGER_IDLETIME  = HALF_MINUTE;

const int FRAGSMENU_PLAYER_HEIGHT = 15;
const int GRENADEEFFECT_DIST = 38;
const int HTF_SEC_POINT = 300;

const int BACKGROUND_WIDTH = 64;

const int MAX_ADMIN_FLOOD_IPS = 200;
const int MAX_LAST_ADMIN_IPS  = 5;

const int WAVERESPAWN_TIME_MULITPLIER = 1;

const double PARA_SPEED    = -0.5 * 0.06; // GRAV
const int PARA_DISTANCE = 500;

const int MAX_OLDPOS = 125;
#ifdef SERVER_CODE
const int MAX_PUSHTICK = 0;
#else
const int MAX_PUSHTICK = 125;
#endif
const double MAX_INACCURACY = 0.5;

const int THING_PUSH_MULTIPLIER    = 9;
const int THING_COLLISION_COOLDOWN = 60;

const int FIREINTERVAL_NET = 5;
const int MELEE_DIST = 12;

// Multikill messages
const char* MULTIKILLMESSAGE[] = {
    "", "", "DOUBLE KILL", "TRIPLE KILL",
    "MULTI KILL", "MULTI KILL X2", "SERIAL KILL", "INSANE KILLS", "GIMME MORE!",
    "MASTA KILLA!", "MASTA KILLA!", "MASTA KILLA!", "STOP IT!!!!",
    "MERCY!!!!!!!!!!", "CHEATER!!!!!!!!",
    "Phased-plasma rifle in the forty watt range",
    "Hey, just what you see, pal", "just what you see, pal..."
};

const unsigned int DEFAULT_JETCOLOR = 0xFFFFBD24;

const int IDLE_KICK      = MINUTE * 3;
const int MENU_TIME      = SECOND;
const int LESSBLEED_TIME = SECOND * 2;
const int NOBLEED_TIME   = SECOND * 5;
const int ONFIRE_TIME    = SECOND * 4;

const int SURVIVAL_RESPAWNTIME = SECOND * 5;
const int DEFAULT_VOTE_TIME    = MINUTE * 2;
const int DEFAULT_VOTING_TIME  = SECOND * 20;

#ifndef SERVER_CODE
const int WEP_RESTRICT_WIDTH  = 64;
const int WEP_RESTRICT_HEIGHT = 64;
const int GOS_RESTRICT_WIDTH  = 16;
const int GOS_RESTRICT_HEIGHT = 16;
#endif

// Text styles
const int TEXTSTYLE  = 0;
const int HORIZONTAL = 1;
const int VERTICAL   = 2;

// Colors
const unsigned int DEFAULT_MESSAGE_COLOR = 0xEECCFFAA;
const unsigned int DEBUG_MESSAGE_COLOR   = 0xEEFF8989;
const unsigned int GAME_MESSAGE_COLOR    = 0xEE71F981;
const unsigned int WARNING_MESSAGE_COLOR = 0xEEE36952;

const unsigned int SERVER_MESSAGE_COLOR = 0xF9FBDA22;
const unsigned int CLIENT_MESSAGE_COLOR = 0xF9FCD822;

const unsigned int ENTER_MESSAGE_COLOR = 0xF1C3C3C3;

const unsigned int ABOVECHAT_MESSAGE_COLOR = 0xFDFDF9;
const unsigned int CHAT_MESSAGE_COLOR      = 0xEEEFFEEA;
const unsigned int TEAMCHAT_MESSAGE_COLOR  = 0xEEFEDA7C;

const unsigned int KILL_MESSAGE_COLOR    = 0xFFEA3530;
const unsigned int SUICIDE_MESSAGE_COLOR = 0xD6B3A717;
const unsigned int DIE_MESSAGE_COLOR     = 0xFFC53025;

const unsigned int DEATH_MESSAGE_COLOR  = 0xEE801304;
const unsigned int KILLER_MESSAGE_COLOR = 0xEE52D119;

const unsigned int GAMESTART_MESSAGE_COLOR = 0xFFD3CA34;

const unsigned int CAPTURE_MESSAGE_COLOR = 0xFF77D334;
const unsigned int RETURN_MESSAGE_COLOR  = 0xFF71A331;

const unsigned int ALPHA_MESSAGE_COLOR   = 0xFFDF3131;
const unsigned int BRAVO_MESSAGE_COLOR   = 0xFF3131DF;
const unsigned int CHARLIE_MESSAGE_COLOR = 0xFFDFDF31;
const unsigned int DELTA_MESSAGE_COLOR   = 0xFF31DF31;

const unsigned int ALPHAJ_MESSAGE_COLOR   = 0xFFE15353;
const unsigned int BRAVOJ_MESSAGE_COLOR   = 0xFF5353E1;
const unsigned int CHARLIEJ_MESSAGE_COLOR = 0xFFDFDF53;
const unsigned int DELTAJ_MESSAGE_COLOR   = 0xFF53DF53;

const unsigned int BONUS_MESSAGE_COLOR   = 0xFFEF3121;
const unsigned int VOTE_MESSAGE_COLOR    = 0xEEDDEE99;
const unsigned int MUSIC_MESSAGE_COLOR   = 0xEEADFE99;
const unsigned int INFO_MESSAGE_COLOR    = 0xEEDDDEA2;
const unsigned int REGINFO_MESSAGE_COLOR = 0xEEA2DEDD;
const unsigned int MODE_MESSAGE_COLOR    = 0xEE81DA41;

const unsigned int OUTOFSCREEN_MESSAGE_COLOR     = 0x99DF99;
const unsigned int OUTOFSCREENDEAD_MESSAGE_COLOR = 0x983333;
const unsigned int OUTOFSCREENFLAG_MESSAGE_COLOR = 0xDCDC33;

const unsigned int AC_MESSAGE_COLOR = 0xEEE739B1;

const unsigned int ALPHA_K_MESSAGE_COLOR   = 0xEBFFE3E3;
const unsigned int BRAVO_K_MESSAGE_COLOR   = 0xEBD3E3FF;
const unsigned int CHARLIE_K_MESSAGE_COLOR = 0xEBFFFFE3;
const unsigned int DELTA_K_MESSAGE_COLOR   = 0xEBD3FFE3;

const unsigned int ALPHA_D_MESSAGE_COLOR     = 0xEBDAB0B0;
const unsigned int BRAVO_D_MESSAGE_COLOR     = 0xEBA0B0DA;
const unsigned int CHARLIE_D_MESSAGE_COLOR   = 0xEBD0D0B0;
const unsigned int DELTA_D_MESSAGE_COLOR     = 0xEBA0D0BA;
const unsigned int SPECTATOR_D_MESSAGE_COLOR = 0xEBD3B727;

const unsigned int ALPHA_C_MESSAGE_COLOR     = 0xF5FEE8E8;
const unsigned int BRAVO_C_MESSAGE_COLOR     = 0xF5E3E8FE;
const unsigned int CHARLIE_C_MESSAGE_COLOR   = 0xF5FEFEE8;
const unsigned int DELTA_C_MESSAGE_COLOR     = 0xF5E8FEE8;
const unsigned int SPECTATOR_C_MESSAGE_COLOR = 0xF5DF7AB0;

// 0 represents in some cases all players
const int ALL_PLAYERS = 0;

// Player teams
const int TEAM_NONE      = 0;
const int TEAM_ALPHA     = 1;
const int TEAM_BRAVO     = 2;
const int TEAM_CHARLIE   = 3;
const int TEAM_DELTA     = 4;
const int TEAM_SPECTATOR = 5;

// Game styles
const int GAMESTYLE_DEATHMATCH = 0;
const int GAMESTYLE_POINTMATCH = 1;
const int GAMESTYLE_TEAMMATCH  = 2;
const int GAMESTYLE_CTF        = 3;
const int GAMESTYLE_RAMBO      = 4;
const int GAMESTYLE_INF        = 5;
const int GAMESTYLE_HTF        = 6;

// Vote types
const int VOTE_MAP  = 0;
const int VOTE_KICK = 1;

const unsigned int COLOR_TRANSPARENCY_UNREGISTERED = 0xFF000000;
const unsigned int COLOR_TRANSPARENCY_REGISTERED   = 0xFE000000;
const unsigned int COLOR_TRANSPARENCY_SPECIAL      = 0xFD000000;
const unsigned int COLOR_TRANSPARENCY_BOT          = 0xFB000000;

// Polygon types
const int PT_ONLYBULLETS        = 1;
const int PT_ONLYPLAYERS        = 2;
const int PT_DOESNTCOLLIDE      = 3;
const int PT_ICE                = 4;
const int PT_DEADLY             = 5;
const int PT_BLOODYDEADLY       = 6;
const int PT_HURTS              = 7;
const int PT_REGENERATES        = 8;
const int PT_LAVA               = 9;
const int PT_ALPHABULLETS       = 10;
const int PT_ALPHAPLAYERS       = 11;
const int PT_BRAVOBULLETS       = 12;
const int PT_BRAVOPLAYERS       = 13;
const int PT_CHARLIEBULLETS     = 14;
const int PT_CHARLIEPLAYERS     = 15;
const int PT_DELTABULLETS       = 16;
const int PT_DELTAPLAYERS       = 17;
const int PT_BOUNCY             = 18;
const int PT_EXPLOSIVE          = 19;
const int PT_HURTFLAGGERS       = 20;
const int PT_FLAGGERCOLLIDES    = 21;
const int PT_NONFLAGGERCOLLIDES = 22;
const int PT_FLAGCOLLIDES       = 23;

// Game objects
const int OBJECT_NUM_NONWEAPON   = 12;
const int OBJECT_NUM_FLAGS       = 3;

const int OBJECT_ALPHA_FLAG      = 1;
const int OBJECT_BRAVO_FLAG      = 2;
const int OBJECT_POINTMATCH_FLAG = 3;
const int OBJECT_USSOCOM         = 4;
const int OBJECT_DESERT_EAGLE    = 5;
const int OBJECT_HK_MP5          = 6;
const int OBJECT_AK74            = 7;
const int OBJECT_STEYR_AUG       = 8;
const int OBJECT_SPAS12          = 9;
const int OBJECT_RUGER77         = 10;
const int OBJECT_M79             = 11;
const int OBJECT_BARRET_M82A1    = 12;
const int OBJECT_MINIMI          = 13;
const int OBJECT_MINIGUN         = 14;
const int OBJECT_RAMBO_BOW       = 15;
const int OBJECT_MEDICAL_KIT     = 16;
const int OBJECT_GRENADE_KIT     = 17;
const int OBJECT_FLAMER_KIT      = 18;
const int OBJECT_PREDATOR_KIT    = 19;
const int OBJECT_VEST_KIT        = 20;
const int OBJECT_BERSERK_KIT     = 21;
const int OBJECT_CLUSTER_KIT     = 22;
const int OBJECT_PARACHUTE       = 23;
const int OBJECT_COMBAT_KNIFE    = 24;
const int OBJECT_CHAINSAW        = 25;
const int OBJECT_LAW             = 26;
const int OBJECT_STATIONARY_GUN  = 27;

// Sound effects
const int SFX_AK74_FIRE          = 1;
const int SFX_ROCKETZ            = 2;
const int SFX_AK74_RELOAD        = 3;
const int SFX_M249_FIRE          = 5;
const int SFX_RUGER77_FIRE       = 6;
const int SFX_RUGER77_RELOAD     = 7;
const int SFX_M249_RELOAD        = 8;
const int SFX_MP5_FIRE           = 9;
const int SFX_MP5_RELOAD         = 10;
const int SFX_SPAS12_FIRE        = 11;
const int SFX_SPAS12_RELOAD      = 12;
const int SFX_STANDUP            = 13;
const int SFX_FALL               = 14;
const int SFX_SPAWN              = 15;
const int SFX_M79_FIRE           = 16;
const int SFX_M79_EXPLOSION      = 17;
const int SFX_M79_RELOAD         = 18;
const int SFX_GRENADE_THROW      = 19;
const int SFX_GRENADE_EXPLOSION  = 20;
const int SFX_GRENADE_BOUNCE     = 21;
const int SFX_BRYZG              = 22;
const int SFX_INFILTMUS          = 23;
const int SFX_HEADCHOP           = 24;
const int SFX_EXPLOSION_ERG      = 25;
const int SFX_WATER_STEP         = 26;
const int SFX_BULLETBY           = 27;
const int SFX_BODYFALL           = 28;
const int SFX_DESERTEAGLE_FIRE   = 29;
const int SFX_DESERTEAGLE_RELOAD = 30;
const int SFX_STEYRAUG_FIRE      = 31;
const int SFX_STEYRAUG_RELOAD    = 32;
const int SFX_BARRETM82_FIRE     = 33;
const int SFX_BARRETM82_RELOAD   = 34;
const int SFX_MINIGUN_FIRE       = 35;
const int SFX_MINIGUN_RELOAD     = 36;
const int SFX_MINIGUN_START      = 37;
const int SFX_MINIGUN_END        = 38;
const int SFX_PICKUPGUN          = 39;
const int SFX_CAPTURE            = 40;
const int SFX_COLT1911_FIRE      = 41;
const int SFX_COLT1911_RELOAD    = 42;
const int SFX_CHANGEWEAPON       = 43;
const int SFX_SHELL              = 44;
const int SFX_SHELL2             = 45;
const int SFX_DEAD_HIT           = 46;
const int SFX_THROWGUN           = 47;
const int SFX_BOW_FIRE           = 48;
const int SFX_TAKEBOW            = 49;
const int SFX_TAKEMEDIKIT        = 50;
const int SFX_WERMUSIC           = 51;
const int SFX_TS                 = 52;
const int SFX_CTF                = 53;
const int SFX_BERSERKER          = 54;
const int SFX_GODFLAME           = 55;
const int SFX_FLAMER             = 56;
const int SFX_PREDATOR           = 57;
const int SFX_KILLBERSERK        = 58;
const int SFX_VESTHIT            = 59;
const int SFX_BURN               = 60;
const int SFX_VESTTAKE           = 61;
const int SFX_CLUSTERGRENADE     = 62;
const int SFX_CLUSTER_EXPLOSION  = 63;
const int SFX_GRENADE_PULLOUT    = 64;
const int SFX_SPIT               = 65;
const int SFX_STUFF              = 66;
const int SFX_SMOKE              = 67;
const int SFX_MATCH              = 68;
const int SFX_ROAR               = 69;
const int SFX_STEP               = 70;
const int SFX_STEP2              = 71;
const int SFX_STEP3              = 72;
const int SFX_STEP4              = 73;
const int SFX_HUM                = 74;
const int SFX_RIC                = 75;
const int SFX_RIC2               = 76;
const int SFX_RIC3               = 77;
const int SFX_RIC4               = 78;
const int SFX_DIST_M79           = 79;
const int SFX_DIST_GRENADE       = 80;
const int SFX_DIST_GUN1          = 81;
const int SFX_DIST_GUN2          = 82;
const int SFX_DIST_GUN3          = 83;
const int SFX_DIST_GUN4          = 84;
const int SFX_DEATH              = 85;
const int SFX_DEATH2             = 86;
const int SFX_DEATH3             = 87;
const int SFX_CROUCH_MOVE        = 88;
const int SFX_HIT_ARG            = 89;
const int SFX_HIT_ARG2           = 90;
const int SFX_HIT_ARG3           = 91;
const int SFX_GOPRONE            = 92;
const int SFX_ROLL               = 93;
const int SFX_FALL_HARD          = 94;
const int SFX_ONFIRE             = 95;
const int SFX_FIRECRACK          = 96;
const int SFX_SCOPE              = 97;
const int SFX_SCOPEBACK          = 98;
const int SFX_PLAYERDEATH        = 99;
const int SFX_CHANGESPIN         = 100;
const int SFX_ARG                = 101;
const int SFX_LAVA               = 102;
const int SFX_REGENERATE         = 103;
const int SFX_PRONE_MOVE         = 104;
const int SFX_JUMP               = 105;
const int SFX_CROUCH             = 106;
const int SFX_CROUCH_MOVEL       = 107;
const int SFX_STEP5              = 108;
const int SFX_STEP6              = 109;
const int SFX_STEP7              = 110;
const int SFX_STEP8              = 111;
const int SFX_STOP               = 112;
const int SFX_BULLETBY2          = 113;
const int SFX_BULLETBY3          = 114;
const int SFX_BULLETBY4          = 115;
const int SFX_BULLETBY5          = 116;
const int SFX_WEAPONHIT          = 117;
const int SFX_CLIPFALL           = 118;
const int SFX_BONECRACK          = 119;
const int SFX_GAUGESHELL         = 120;
const int SFX_COLLIDERHIT        = 121;
const int SFX_KIT_FALL           = 122;
const int SFX_KIT_FALL2          = 123;
const int SFX_FLAG               = 124;
const int SFX_FLAG2              = 125;
const int SFX_TAKEGUN            = 126;
const int SFX_INFILT_POINT       = 127;
const int SFX_MENUCLICK          = 128;
const int SFX_KNIFE              = 129;
const int SFX_SLASH              = 130;
const int SFX_CHAINSAW_D         = 131;
const int SFX_CHAINSAW_M         = 132;
const int SFX_CHAINSAW_R         = 133;
const int SFX_PISS               = 134;
const int SFX_LAW                = 135;
const int SFX_CHAINSAW_O         = 136;
const int SFX_M2FIRE             = 137;
const int SFX_M2EXPLODE          = 138;
const int SFX_M2OVERHEAT         = 139;
const int SFX_SIGNAL             = 140;
const int SFX_M2USE              = 141;
const int SFX_SCOPERUN           = 142;
const int SFX_MERCY              = 143;
const int SFX_RIC5               = 144;
const int SFX_RIC6               = 145;
const int SFX_RIC7               = 146;
const int SFX_LAW_START          = 147;
const int SFX_LAW_END            = 148;
const int SFX_BOOMHEADSHOT       = 149;
const int SFX_SNAPSHOT           = 150;
const int SFX_RADIO_EFCUP        = 151;
const int SFX_RADIO_EFCMID       = 152;
const int SFX_RADIO_EFCDOWN      = 153;
const int SFX_RADIO_FFCUP        = 154;
const int SFX_RADIO_FFCMID       = 155;
const int SFX_RADIO_FFCDOWN      = 156;
const int SFX_RADIO_ESUP         = 157;
const int SFX_RADIO_ESMID        = 158;
const int SFX_RADIO_ESDOWN       = 159;
const int SFX_BOUNCE             = 160;
const int SFX_RAIN               = 161;
const int SFX_SNOW               = 162;
const int SFX_WIND               = 163;

#endif // CONSTANTS_H