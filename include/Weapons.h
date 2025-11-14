#ifndef WEAPONS_H
#define WEAPONS_H

//*******************************************************************************
//                                                                              
//       Weapons Unit for SOLDAT                                                 
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <cstdint>

// Constants
const int EAGLE          = 1;
const int MP5            = 2;
const int AK74           = 3;
const int STEYRAUG       = 4;
const int SPAS12         = 5;
const int RUGER77        = 6;
const int M79            = 7;
const int BARRETT        = 8;
const int M249           = 9;
const int MINIGUN        = 10;
const int COLT           = 11;
const int KNIFE          = 12;
const int CHAINSAW       = 13;
const int LAW            = 14;
const int BOW2           = 15;
const int BOW            = 16;
const int FLAMER         = 17;
const int M2             = 18;
const int NOWEAPON       = 19;
const int FRAGGRENADE    = 20;
const int CLUSTERGRENADE = 21;
const int CLUSTER        = 22;
const int THROWNKNIFE    = 23;

const int PRIMARY_WEAPONS   = 10;
const int SECONDARY_WEAPONS = 4;
const int BONUS_WEAPONS     = 3;
const int MAIN_WEAPONS      = PRIMARY_WEAPONS + SECONDARY_WEAPONS;
const int EXTENDED_WEAPONS  = MAIN_WEAPONS + BONUS_WEAPONS;

const int ORIGINAL_WEAPONS  = 20;
const int TOTAL_WEAPONS     = 23;

// Weapon number constants
const int EAGLE_NUM          = 1;
const int MP5_NUM            = 2;
const int AK74_NUM           = 3;
const int STEYRAUG_NUM       = 4;
const int SPAS12_NUM         = 5;
const int RUGER77_NUM        = 6;
const int M79_NUM            = 7;
const int BARRETT_NUM        = 8;
const int M249_NUM           = 9;
const int MINIGUN_NUM        = 10;
const int COLT_NUM           = 0;
const int KNIFE_NUM          = 11;
const int CHAINSAW_NUM       = 12;
const int LAW_NUM            = 13;
const int BOW2_NUM           = 16;
const int BOW_NUM            = 15;
const int FLAMER_NUM         = 14;
const int M2_NUM             = 30;
const int NOWEAPON_NUM       = 255;
const int FRAGGRENADE_NUM    = 50;
const int CLUSTERGRENADE_NUM = 51;
const int CLUSTER_NUM        = 52;
const int THROWNKNIFE_NUM    = 53;

// Bullet style types
const int BULLET_STYLE_PLAIN       = 1;
const int BULLET_STYLE_FRAGNADE    = 2;
const int BULLET_STYLE_SHOTGUN     = 3;
const int BULLET_STYLE_M79         = 4;
const int BULLET_STYLE_FLAME       = 5;
const int BULLET_STYLE_PUNCH       = 6;
const int BULLET_STYLE_ARROW       = 7;
const int BULLET_STYLE_FLAMEARROW  = 8;
const int BULLET_STYLE_CLUSTERNADE = 9;
const int BULLET_STYLE_CLUSTER     = 10;
const int BULLET_STYLE_KNIFE       = 11;
const int BULLET_STYLE_LAW         = 12;
const int BULLET_STYLE_THROWNKNIFE = 13;
const int BULLET_STYLE_M2          = 14;

// Used for NoCollision attribute
const uint32_t WEAPON_NOCOLLISION_ENEMY = (1 << 0);
const uint32_t WEAPON_NOCOLLISION_TEAM = (1 << 1);
const uint32_t WEAPON_NOCOLLISION_SELF = (1 << 2);
const uint32_t WEAPON_NOCOLLISION_EXP_ENEMY = (1 << 3);
const uint32_t WEAPON_NOCOLLISION_EXP_TEAM = (1 << 4);
const uint32_t WEAPON_NOCOLLISION_EXP_SELF = (1 << 5);

struct TGun {
    uint8_t Ammo;
    uint8_t AmmoCount;
    uint8_t Num;
    float MovementAcc;
    int16_t Bink;
    uint16_t Recoil;
    uint16_t FireInterval;
    uint16_t FireIntervalPrev;
    uint16_t FireIntervalCount;
    float FireIntervalReal;
    uint16_t StartUpTime;
    uint16_t StartUpTimeCount;
    uint16_t ReloadTime;
    uint16_t ReloadTimePrev;
    uint16_t ReloadTimeCount;
    float ReloadTimeReal;
    uint8_t TextureNum;
    uint8_t ClipTextureNum;
    bool ClipReload;
    uint16_t ClipInTime;
    uint16_t ClipOutTime;
    std::string Name;
    std::string IniName;
    float Speed;
    float HitMultiply;
    float BulletSpread;
    float Push;
    float InheritedVelocity;
    float ModifierLegs;
    float ModifierChest;
    float ModifierHead;
    uint8_t NoCollision;
    uint8_t FireMode;
    uint16_t Timeout;
    uint8_t BulletStyle;
    uint8_t FireStyle;
    uint8_t BulletImageStyle;
    
    // Additional fields that would be initialized during build
    uint16_t ClipOutTime;
    uint16_t ClipInTime;
    uint16_t FireIntervalCountPrev;
    uint16_t ReloadTimeCountPrev;
    uint16_t StartUpTimeCountPrev;
    
    TGun() : Ammo(0), AmmoCount(0), Num(0), MovementAcc(0.0f), Bink(0), Recoil(0),
             FireInterval(0), FireIntervalPrev(0), FireIntervalCount(0), FireIntervalReal(0.0f),
             StartUpTime(0), StartUpTimeCount(0), ReloadTime(0), ReloadTimePrev(0),
             ReloadTimeCount(0), ReloadTimeReal(0.0f), TextureNum(0), ClipTextureNum(0),
             ClipReload(false), ClipInTime(0), ClipOutTime(0), Name(""), IniName(""),
             Speed(0.0f), HitMultiply(0.0f), BulletSpread(0.0f), Push(0.0f),
             InheritedVelocity(0.0f), ModifierLegs(0.0f), ModifierChest(0.0f),
             ModifierHead(0.0f), NoCollision(0), FireMode(0), Timeout(0),
             BulletStyle(0), FireStyle(0), BulletImageStyle(0), ClipOutTime(0),
             ClipInTime(0), FireIntervalCountPrev(0), ReloadTimeCountPrev(0), StartUpTimeCountPrev(0) {}
};

// Global variables
extern TGun Guns[TOTAL_WEAPONS + 1];  // Pascal arrays start from 1
extern TGun DefaultGuns[TOTAL_WEAPONS + 1];  // Pascal arrays start from 1
extern uint32_t DefaultWMChecksum;
extern uint32_t LoadedWMChecksum;

// Function declarations
void CreateWeapons(bool RealisticMode);
void CreateDefaultWeapons(bool RealisticMode);
void CreateWeaponsBase();
void CreateNormalWeapons();
void CreateRealisticWeapons();
void BuildWeapons();
uint32_t CreateWMChecksum();
int16_t WeaponNumToIndex(uint8_t Num);
int WeaponNameToNum(const std::string& Name);
std::string WeaponNumToName(int Num);
std::string WeaponNameByNum(int Num);
bool IsMainWeaponIndex(int16_t WeaponIndex);
bool IsSecondaryWeaponIndex(int16_t WeaponIndex);
bool IsExtendedWeaponIndex(int16_t WeaponIndex);
uint16_t CalculateBink(uint16_t Accumulated, uint16_t Bink);
uint8_t WeaponNumInternalToExternal(uint8_t Num);
uint8_t WeaponNumExternalToInternal(uint8_t Num);

namespace WeaponsImpl {
    inline void CreateWeapons(bool RealisticMode) {
        CreateWeaponsBase();
        CreateDefaultWeapons(RealisticMode);
    }

    inline void CreateDefaultWeapons(bool RealisticMode) {
        if (RealisticMode) {
            CreateRealisticWeapons();
        } else {
            CreateNormalWeapons();
        }

        // Set defaults for weapon menu selection comparisons
        for (int weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            TGun* gun = &Guns[weaponIndex];
            TGun* defaultGun = &DefaultGuns[weaponIndex];

            defaultGun->HitMultiply = gun->HitMultiply;
            defaultGun->FireInterval = gun->FireInterval;
            defaultGun->Ammo = gun->Ammo;
            defaultGun->ReloadTime = gun->ReloadTime;
            defaultGun->Speed = gun->Speed;
            defaultGun->BulletStyle = gun->BulletStyle;
            defaultGun->StartUpTime = gun->StartUpTime;
            defaultGun->Bink = gun->Bink;
            defaultGun->MovementAcc = gun->MovementAcc;
            defaultGun->BulletSpread = gun->BulletSpread;
            defaultGun->Recoil = gun->Recoil;
            defaultGun->Push = gun->Push;
            defaultGun->InheritedVelocity = gun->InheritedVelocity;
            defaultGun->ModifierLegs = gun->ModifierLegs;
            defaultGun->ModifierChest = gun->ModifierChest;
            defaultGun->ModifierHead = gun->ModifierHead;
        }

        BuildWeapons();
    }

    inline void CreateWeaponsBase() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->Name = "Desert Eagles";
        gun->IniName = gun->Name;
        gun->Num = EAGLE_NUM;
        // gun->TextureNum = GFX_WEAPONS_DEAGLES;  // Assuming constant from graphics system
        // gun->ClipTextureNum = GFX_WEAPONS_DEAGLES_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_DEAGLES_BULLET;
        // gun->FireStyle = GFX_WEAPONS_DEAGLES_FIRE;
        gun->FireMode = 2;

        // MP5
        gun = &Guns[MP5];
        gun->Name = "HK MP5";
        gun->IniName = gun->Name;
        gun->Num = MP5_NUM;
        // gun->TextureNum = GFX_WEAPONS_MP5;
        // gun->ClipTextureNum = GFX_WEAPONS_MP5_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_MP5_BULLET;
        // gun->FireStyle = GFX_WEAPONS_MP5_FIRE;
        gun->FireMode = 0;

        // AK-74
        gun = &Guns[AK74];
        gun->Name = "Ak-74";
        gun->IniName = gun->Name;
        gun->Num = AK74_NUM;
        // gun->TextureNum = GFX_WEAPONS_AK74;
        // gun->ClipTextureNum = GFX_WEAPONS_AK74_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_AK74_BULLET;
        // gun->FireStyle = GFX_WEAPONS_AK74_FIRE;
        gun->FireMode = 0;

        // Steyr AUG
        gun = &Guns[STEYRAUG];
        gun->Name = "Steyr AUG";
        gun->IniName = gun->Name;
        gun->Num = STEYRAUG_NUM;
        // gun->TextureNum = GFX_WEAPONS_STEYR;
        // gun->ClipTextureNum = GFX_WEAPONS_STEYR_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_STEYR_BULLET;
        // gun->FireStyle = GFX_WEAPONS_STEYR_FIRE;
        gun->FireMode = 0;

        // SPAS-12
        gun = &Guns[SPAS12];
        gun->Name = "Spas-12";
        gun->IniName = gun->Name;
        gun->Num = SPAS12_NUM;
        // gun->TextureNum = GFX_WEAPONS_SPAS;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_SPAS_FIRE;
        gun->FireMode = 2;

        // Ruger 77
        gun = &Guns[RUGER77];
        gun->Name = "Ruger 77";
        gun->IniName = gun->Name;
        gun->Num = RUGER77_NUM;
        // gun->TextureNum = GFX_WEAPONS_RUGER;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        // gun->BulletImageStyle = GFX_WEAPONS_RUGER_BULLET;
        // gun->FireStyle = GFX_WEAPONS_RUGER_FIRE;
        gun->FireMode = 2;

        // M79 grenade launcher
        gun = &Guns[M79];
        gun->Name = "M79";
        gun->IniName = gun->Name;
        gun->Num = M79_NUM;
        // gun->TextureNum = GFX_WEAPONS_M79;
        // gun->ClipTextureNum = GFX_WEAPONS_M79_CLIP;
        gun->ClipReload = true;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_M79_FIRE;
        gun->FireMode = 0;

        // Barrett M82A1
        gun = &Guns[BARRETT];
        gun->Name = "Barrett M82A1";
        gun->IniName = "Barret M82A1";
        gun->Num = BARRETT_NUM;
        // gun->TextureNum = GFX_WEAPONS_BARRETT;
        // gun->ClipTextureNum = GFX_WEAPONS_BARRETT_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_BARRETT_BULLET;
        // gun->FireStyle = GFX_WEAPONS_BARRETT_FIRE;
        gun->FireMode = 2;

        // M249
        gun = &Guns[M249];
        gun->Name = "FN Minimi";
        gun->IniName = gun->Name;
        gun->Num = M249_NUM;
        // gun->TextureNum = GFX_WEAPONS_MINIMI;
        // gun->ClipTextureNum = GFX_WEAPONS_MINIMI_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_MINIMI_BULLET;
        // gun->FireStyle = GFX_WEAPONS_MINIMI_FIRE;
        gun->FireMode = 0;

        // Minigun
        gun = &Guns[MINIGUN];
        gun->Name = "XM214 Minigun";
        gun->IniName = gun->Name;
        gun->Num = MINIGUN_NUM;
        // gun->TextureNum = GFX_WEAPONS_MINIGUN;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        // gun->BulletImageStyle = GFX_WEAPONS_MINIGUN_BULLET;
        // gun->FireStyle = GFX_WEAPONS_MINIGUN_FIRE;
        gun->FireMode = 0;

        // Colt 1911
        gun = &Guns[COLT];
        gun->Name = "USSOCOM";
        gun->IniName = gun->Name;
        gun->Num = COLT_NUM;
        // gun->TextureNum = GFX_WEAPONS_SOCOM;
        // gun->ClipTextureNum = GFX_WEAPONS_SOCOM_CLIP;
        gun->ClipReload = true;
        // gun->BulletImageStyle = GFX_WEAPONS_COLT_BULLET;
        // gun->FireStyle = GFX_WEAPONS_SOCOM_FIRE;
        gun->FireMode = 2;

        // Knife
        gun = &Guns[KNIFE];
        gun->Name = "Combat Knife";
        gun->IniName = gun->Name;
        gun->Num = KNIFE_NUM;
        // gun->TextureNum = GFX_WEAPONS_KNIFE;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        gun->FireStyle = 0;
        gun->FireMode = 0;

        // Chainsaw
        gun = &Guns[CHAINSAW];
        gun->Name = "Chainsaw";
        gun->IniName = gun->Name;
        gun->Num = CHAINSAW_NUM;
        // gun->TextureNum = GFX_WEAPONS_CHAINSAW;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_CHAINSAW_FIRE;
        gun->FireMode = 0;

        // M72 LAW
        gun = &Guns[LAW];
        gun->Name = "LAW";
        gun->IniName = "M72 LAW";
        gun->Num = LAW_NUM;
        // gun->TextureNum = GFX_WEAPONS_LAW;
        gun->ClipTextureNum = 0;
        gun->ClipReload = true;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_LAW_FIRE;
        gun->FireMode = 0;

        // Rambo Bow with flame
        gun = &Guns[BOW2];
        gun->Name = "Flame Bow";
        gun->IniName = "Flamed Arrows";
        gun->Num = BOW2_NUM;
        // gun->TextureNum = GFX_WEAPONS_BOW;
        // gun->ClipTextureNum = GFX_WEAPONS_BOW_S;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_BOW_FIRE;
        gun->FireMode = 0;

        // Rambo Bow
        gun = &Guns[BOW];
        gun->Name = "Bow";
        gun->IniName = "Rambo Bow";
        gun->Num = BOW_NUM;
        // gun->TextureNum = GFX_WEAPONS_BOW;
        // gun->ClipTextureNum = GFX_WEAPONS_BOW_S;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_BOW_FIRE;
        gun->FireMode = 0;

        // Flamethrower
        gun = &Guns[FLAMER];
        gun->Name = "Flamer";
        gun->IniName = gun->Name;
        gun->Num = FLAMER_NUM;
        // gun->TextureNum = GFX_WEAPONS_FLAMER;
        // gun->ClipTextureNum = GFX_WEAPONS_FLAMER;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_FLAMER_FIRE;
        gun->FireMode = 0;

        // M2
        gun = &Guns[M2];
        gun->Name = "M2 MG";
        gun->IniName = "Stationary Gun";
        gun->Num = M2_NUM;
        // gun->TextureNum = GFX_WEAPONS_MINIGUN;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        gun->FireStyle = 0;
        gun->FireMode = 0;

        // No weapon
        gun = &Guns[NOWEAPON];
        gun->Name = "Hands";
        gun->IniName = "Punch";
        gun->Num = NOWEAPON_NUM;
        gun->TextureNum = 0;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        gun->FireStyle = 0;
        gun->FireMode = 0;

        // Frag grenade
        gun = &Guns[FRAGGRENADE];
        gun->Name = "Frag Grenade";
        gun->IniName = "Grenade";
        gun->Num = FRAGGRENADE_NUM;
        // gun->TextureNum = GFX_WEAPONS_FRAG_GRENADE;
        // gun->ClipTextureNum = GFX_WEAPONS_FRAG_GRENADE;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_AK74_FIRE;
        gun->FireMode = 0;

        // Cluster grenade
        gun = &Guns[CLUSTERGRENADE];
        gun->Name = "Frag grenade";
        gun->IniName = "";
        gun->Num = CLUSTERGRENADE_NUM;
        // gun->TextureNum = GFX_WEAPONS_FRAG_GRENADE;
        // gun->ClipTextureNum = GFX_WEAPONS_FRAG_GRENADE;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_AK74_FIRE;
        gun->FireMode = 0;

        // Cluster
        gun = &Guns[CLUSTER];
        gun->Name = "Frag grenade";
        gun->IniName = "";
        gun->Num = CLUSTER_NUM;
        // gun->TextureNum = GFX_WEAPONS_FRAG_GRENADE;
        // gun->ClipTextureNum = GFX_WEAPONS_FRAG_GRENADE;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        // gun->FireStyle = GFX_WEAPONS_AK74_FIRE;
        gun->FireMode = 0;

        // Thrown knife
        gun = &Guns[THROWNKNIFE];
        gun->Name = "Combat Knife";
        gun->IniName = "";
        gun->Num = THROWNKNIFE_NUM;
        // gun->TextureNum = GFX_WEAPONS_KNIFE;
        gun->ClipTextureNum = 0;
        gun->ClipReload = false;
        gun->BulletImageStyle = 0;
        gun->FireStyle = 0;
        gun->FireMode = 0;
    }

    inline void CreateNormalWeapons() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->HitMultiply = 1.81f;
        gun->FireInterval = 24;
        gun->Ammo = 7;
        gun->ReloadTime = 87;
        gun->Speed = 19.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.009f;
        gun->BulletSpread = 0.15f;
        gun->Recoil = 0;
        gun->Push = 0.0176f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // MP5
        gun = &Guns[MP5];
        gun->HitMultiply = 1.01f;
        gun->FireInterval = 6;
        gun->Ammo = 30;
        gun->ReloadTime = 105;
        gun->Speed = 18.9f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.14f;
        gun->Recoil = 0;
        gun->Push = 0.0112f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // AK-74
        gun = &Guns[AK74];
        gun->HitMultiply = 1.004f;
        gun->FireInterval = 10;
        gun->Ammo = 35;
        gun->ReloadTime = 165;
        gun->Speed = 24.6f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = -12;
        gun->MovementAcc = 0.011f;
        gun->BulletSpread = 0.025f;
        gun->Recoil = 0;
        gun->Push = 0.01376f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Steyr AUG
        gun = &Guns[STEYRAUG];
        gun->HitMultiply = 0.71f;
        gun->FireInterval = 7;
        gun->Ammo = 25;
        gun->ReloadTime = 125;
        gun->Speed = 26.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.075f;
        gun->Recoil = 0;
        gun->Push = 0.0084f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // SPAS-12
        gun = &Guns[SPAS12];
        gun->HitMultiply = 1.22f;
        gun->FireInterval = 32;
        gun->Ammo = 7;
        gun->ReloadTime = 175;
        gun->Speed = 14.0f;
        gun->BulletStyle = BULLET_STYLE_SHOTGUN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.8f;
        gun->Recoil = 0;
        gun->Push = 0.0188f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Ruger 77
        gun = &Guns[RUGER77];
        gun->HitMultiply = 2.49f;
        gun->FireInterval = 45;
        gun->Ammo = 4;
        gun->ReloadTime = 78;
        gun->Speed = 33.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.03f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.012f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.2f;
        gun->ModifierChest = 1.05f;
        gun->ModifierLegs = 1.0f;

        // M79 grenade launcher
        gun = &Guns[M79];
        gun->HitMultiply = 1550.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 178;
        gun->Speed = 10.7f;
        gun->BulletStyle = BULLET_STYLE_M79;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.036f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Barrett M82A1
        gun = &Guns[BARRETT];
        gun->HitMultiply = 4.45f;
        gun->FireInterval = 225;
        gun->Ammo = 10;
        gun->ReloadTime = 70;
        gun->Speed = 55.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 19;
        gun->Bink = 65;
        gun->MovementAcc = 0.05f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.018f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.0f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 1.0f;

        // M249
        gun = &Guns[M249];
        gun->HitMultiply = 0.85f;
        gun->FireInterval = 9;
        gun->Ammo = 50;
        gun->ReloadTime = 250;
        gun->Speed = 27.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.013f;
        gun->BulletSpread = 0.064f;
        gun->Recoil = 0;
        gun->Push = 0.0128f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Minigun
        gun = &Guns[MINIGUN];
        gun->HitMultiply = 0.468f;
        gun->FireInterval = 3;
        gun->Ammo = 100;
        gun->ReloadTime = 480;
        gun->Speed = 29.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 25;
        gun->Bink = 0;
        gun->MovementAcc = 0.0625f;
        gun->BulletSpread = 0.3f;
        gun->Recoil = 0;
        gun->Push = 0.0104f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Colt 1911
        gun = &Guns[COLT];
        gun->HitMultiply = 1.49f;
        gun->FireInterval = 10;
        gun->Ammo = 14;
        gun->ReloadTime = 60;
        gun->Speed = 18.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.02f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // Knife
        gun = &Guns[KNIFE];
        gun->HitMultiply = 2150.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 3;
        gun->Speed = 6.0f;
        gun->BulletStyle = BULLET_STYLE_KNIFE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.12f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Chainsaw
        gun = &Guns[CHAINSAW];
        gun->HitMultiply = 50.0f;
        gun->FireInterval = 2;
        gun->Ammo = 200;
        gun->ReloadTime = 110;
        gun->Speed = 8.0f;
        gun->BulletStyle = BULLET_STYLE_KNIFE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0028f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // M72 LAW
        gun = &Guns[LAW];
        gun->HitMultiply = 1550.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 300;
        gun->Speed = 23.0f;
        gun->BulletStyle = BULLET_STYLE_LAW;
        gun->StartUpTime = 13;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.028f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Rambo Bow with flame
        gun = &Guns[BOW2];
        gun->HitMultiply = 8.0f;
        gun->FireInterval = 10;
        gun->Ammo = 1;
        gun->ReloadTime = 39;
        gun->Speed = 18.0f;
        gun->BulletStyle = BULLET_STYLE_FLAMEARROW;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Rambo Bow
        gun = &Guns[BOW];
        gun->HitMultiply = 12.0f;
        gun->FireInterval = 10;
        gun->Ammo = 1;
        gun->ReloadTime = 25;
        gun->Speed = 21.0f;
        gun->BulletStyle = BULLET_STYLE_ARROW;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0148f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Flamethrower
        gun = &Guns[FLAMER];
        gun->HitMultiply = 19.0f;
        gun->FireInterval = 6;
        gun->Ammo = 200;
        gun->ReloadTime = 5;
        gun->Speed = 10.5f;
        gun->BulletStyle = BULLET_STYLE_FLAME;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.016f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // M2
        gun = &Guns[M2];
        gun->HitMultiply = 1.8f;
        gun->FireInterval = 10;
        gun->Ammo = 100;
        gun->ReloadTime = 366;
        gun->Speed = 36.0f;
        gun->BulletStyle = BULLET_STYLE_M2;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0088f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 0.95f;
        gun->ModifierLegs = 0.85f;

        // No weapon
        gun = &Guns[NOWEAPON];
        gun->HitMultiply = 330.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 3;
        gun->Speed = 5.0f;
        gun->BulletStyle = BULLET_STYLE_PUNCH;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.15f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.9f;

        // Frag grenade
        gun = &Guns[FRAGGRENADE];
        gun->HitMultiply = 1500.0f;
        gun->FireInterval = 80;
        gun->Ammo = 1;
        gun->ReloadTime = 20;
        gun->Speed = 5.0f;
        gun->BulletStyle = BULLET_STYLE_FRAGNADE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.0f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 1.0f;
        gun->ModifierHead = 1.0f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 1.0f;
    }

    inline void CreateRealisticWeapons() {
        TGun* gun;

        // Desert Eagle
        gun = &Guns[EAGLE];
        gun->HitMultiply = 1.66f;
        gun->FireInterval = 27;
        gun->Ammo = 7;
        gun->ReloadTime = 106;
        gun->Speed = 19.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.02f;
        gun->BulletSpread = 0.1f;
        gun->Recoil = 55;
        gun->Push = 0.0164f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // MP5
        gun = &Guns[MP5];
        gun->HitMultiply = 0.94f;
        gun->FireInterval = 6;
        gun->Ammo = 30;
        gun->ReloadTime = 110;
        gun->Speed = 18.9f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = -10;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.03f;
        gun->Recoil = 9;
        gun->Push = 0.0164f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // AK-74
        gun = &Guns[AK74];
        gun->HitMultiply = 1.08f;
        gun->FireInterval = 11;
        gun->Ammo = 35;
        gun->ReloadTime = 158;
        gun->Speed = 24.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = -10;
        gun->MovementAcc = 0.02f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 13;
        gun->Push = 0.0132f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Steyr AUG
        gun = &Guns[STEYRAUG];
        gun->HitMultiply = 0.68f;
        gun->FireInterval = 7;
        gun->Ammo = 30;
        gun->ReloadTime = 126;
        gun->Speed = 26.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = -9;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 11;
        gun->Push = 0.012f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // SPAS-12
        gun = &Guns[SPAS12];
        gun->HitMultiply = 1.2f;
        gun->FireInterval = 35;
        gun->Ammo = 7;
        gun->ReloadTime = 175;
        gun->Speed = 13.2f;
        gun->BulletStyle = BULLET_STYLE_SHOTGUN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.8f;
        gun->Recoil = 65;
        gun->Push = 0.0224f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Ruger 77
        gun = &Guns[RUGER77];
        gun->HitMultiply = 2.22f;
        gun->FireInterval = 52;
        gun->Ammo = 4;
        gun->ReloadTime = 104;
        gun->Speed = 33.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 14;
        gun->MovementAcc = 0.03f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 54;
        gun->Push = 0.0096f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // M79 grenade launcher
        gun = &Guns[M79];
        gun->HitMultiply = 1600.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 173;
        gun->Speed = 11.4f;
        gun->BulletStyle = BULLET_STYLE_M79;
        gun->StartUpTime = 0;
        gun->Bink = 45;
        gun->MovementAcc = 0.03f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 420;
        gun->Push = 0.024f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Barrett M82A1
        gun = &Guns[BARRETT];
        gun->HitMultiply = 4.95f;
        gun->FireInterval = 200;
        gun->Ammo = 10;
        gun->ReloadTime = 170;
        gun->Speed = 55.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 16;
        gun->Bink = 80;
        gun->MovementAcc = 0.07f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 0;
        gun->Push = 0.0056f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // M249
        gun = &Guns[M249];
        gun->HitMultiply = 0.81f;
        gun->FireInterval = 10;
        gun->Ammo = 50;
        gun->ReloadTime = 261;
        gun->Speed = 27.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = -8;
        gun->MovementAcc = 0.02f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 8;
        gun->Push = 0.0116f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Minigun
        gun = &Guns[MINIGUN];
        gun->HitMultiply = 0.43f;
        gun->FireInterval = 4;
        gun->Ammo = 100;
        gun->ReloadTime = 320;
        gun->Speed = 29.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 33;
        gun->Bink = -2;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.1f;
        gun->Recoil = 4;
        gun->Push = 0.0108f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Colt 1911
        gun = &Guns[COLT];
        gun->HitMultiply = 1.30f;
        gun->FireInterval = 12;
        gun->Ammo = 12;
        gun->ReloadTime = 72;
        gun->Speed = 18.0f;
        gun->BulletStyle = BULLET_STYLE_PLAIN;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.02f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 28;
        gun->Push = 0.0172f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Knife
        gun = &Guns[KNIFE];
        gun->HitMultiply = 2250.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 3;
        gun->Speed = 6.0f;
        gun->BulletStyle = BULLET_STYLE_KNIFE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.028f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Chainsaw
        gun = &Guns[CHAINSAW];
        gun->HitMultiply = 21.0f;
        gun->FireInterval = 2;
        gun->Ammo = 200;
        gun->ReloadTime = 110;
        gun->Speed = 7.6f;
        gun->BulletStyle = BULLET_STYLE_KNIFE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 1;
        gun->Push = 0.0028f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // M72 LAW
        gun = &Guns[LAW];
        gun->HitMultiply = 1500.0f;
        gun->FireInterval = 30;
        gun->Ammo = 1;
        gun->ReloadTime = 495;
        gun->Speed = 23.0f;
        gun->BulletStyle = BULLET_STYLE_LAW;
        gun->StartUpTime = 12;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 9;
        gun->Push = 0.012f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Rambo Bow with flame
        gun = &Guns[BOW2];
        gun->HitMultiply = 8.0f;
        gun->FireInterval = 10;
        gun->Ammo = 1;
        gun->ReloadTime = 39;
        gun->Speed = 18.0f;
        gun->BulletStyle = BULLET_STYLE_FLAMEARROW;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Rambo Bow
        gun = &Guns[BOW];
        gun->HitMultiply = 12.0f;
        gun->FireInterval = 10;
        gun->Ammo = 1;
        gun->ReloadTime = 25;
        gun->Speed = 21.0f;
        gun->BulletStyle = BULLET_STYLE_ARROW;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.0148f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Flamethrower
        gun = &Guns[FLAMER];
        gun->HitMultiply = 12.0f;
        gun->FireInterval = 6;
        gun->Ammo = 200;
        gun->ReloadTime = 5;
        gun->Speed = 12.5f;
        gun->BulletStyle = BULLET_STYLE_FLAME;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.016f;
        gun->InheritedVelocity = 0.5f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // M2
        gun = &Guns[M2];
        gun->HitMultiply = 1.55f;
        gun->FireInterval = 14;
        gun->Ammo = 100;
        gun->ReloadTime = 366;
        gun->Speed = 36.0f;
        gun->BulletStyle = BULLET_STYLE_M2;
        gun->StartUpTime = 21;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.0088f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // No weapon
        gun = &Guns[NOWEAPON];
        gun->HitMultiply = 330.0f;
        gun->FireInterval = 6;
        gun->Ammo = 1;
        gun->ReloadTime = 3;
        gun->Speed = 5.0f;
        gun->BulletStyle = BULLET_STYLE_PUNCH;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 0.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;

        // Frag grenade
        gun = &Guns[FRAGGRENADE];
        gun->HitMultiply = 1500.0f;
        gun->FireInterval = 80;
        gun->Ammo = 1;
        gun->ReloadTime = 20;
        gun->Speed = 5.0f;
        gun->BulletStyle = BULLET_STYLE_FRAGNADE;
        gun->StartUpTime = 0;
        gun->Bink = 0;
        gun->MovementAcc = 0.01f;
        gun->BulletSpread = 0.0f;
        gun->Recoil = 10;
        gun->Push = 0.0f;
        gun->InheritedVelocity = 1.0f;
        gun->ModifierHead = 1.1f;
        gun->ModifierChest = 1.0f;
        gun->ModifierLegs = 0.6f;
    }

    inline void BuildWeapons() {
        TGun* gun;
        int weaponIndex;

        // Cluster grenade
        gun = &Guns[CLUSTERGRENADE];
        gun->HitMultiply = Guns[FRAGGRENADE].HitMultiply;
        gun->FireInterval = Guns[FRAGGRENADE].FireInterval;
        gun->Ammo = Guns[FRAGGRENADE].Ammo;
        gun->ReloadTime = Guns[FRAGGRENADE].ReloadTime;
        gun->Speed = Guns[FRAGGRENADE].Speed;
        gun->BulletStyle = BULLET_STYLE_CLUSTERNADE;
        gun->StartUpTime = Guns[FRAGGRENADE].StartUpTime;
        gun->Bink = Guns[FRAGGRENADE].Bink;
        gun->MovementAcc = Guns[FRAGGRENADE].MovementAcc;
        gun->BulletSpread = Guns[FRAGGRENADE].BulletSpread;
        gun->Recoil = Guns[FRAGGRENADE].Recoil;
        gun->Push = Guns[FRAGGRENADE].Push;
        gun->InheritedVelocity = Guns[FRAGGRENADE].InheritedVelocity;

        // Cluster
        gun = &Guns[CLUSTER];
        gun->HitMultiply = Guns[CLUSTERGRENADE].HitMultiply;
        gun->FireInterval = Guns[CLUSTERGRENADE].FireInterval;
        gun->Ammo = Guns[CLUSTERGRENADE].Ammo;
        gun->ReloadTime = Guns[CLUSTERGRENADE].ReloadTime;
        gun->Speed = Guns[CLUSTERGRENADE].Speed;
        gun->BulletStyle = BULLET_STYLE_CLUSTER;
        gun->StartUpTime = Guns[CLUSTERGRENADE].StartUpTime;
        gun->Bink = Guns[CLUSTERGRENADE].Bink;
        gun->MovementAcc = Guns[CLUSTERGRENADE].MovementAcc;
        gun->BulletSpread = Guns[CLUSTERGRENADE].BulletSpread;
        gun->Recoil = Guns[CLUSTERGRENADE].Recoil;
        gun->Push = Guns[CLUSTERGRENADE].Push;
        gun->InheritedVelocity = Guns[CLUSTERGRENADE].InheritedVelocity;

        // Thrown knife
        gun = &Guns[THROWNKNIFE];
        gun->HitMultiply = Guns[KNIFE].HitMultiply;
        gun->FireInterval = Guns[KNIFE].FireInterval;
        gun->Ammo = Guns[KNIFE].Ammo;
        gun->ReloadTime = Guns[KNIFE].ReloadTime;
        gun->Speed = Guns[KNIFE].Speed;
        gun->BulletStyle = BULLET_STYLE_THROWNKNIFE;
        gun->StartUpTime = Guns[KNIFE].StartUpTime;
        gun->Bink = Guns[KNIFE].Bink;
        gun->MovementAcc = Guns[KNIFE].MovementAcc;
        gun->BulletSpread = Guns[KNIFE].BulletSpread;
        gun->Recoil = Guns[KNIFE].Recoil;
        gun->Push = Guns[KNIFE].Push;
        gun->InheritedVelocity = Guns[KNIFE].InheritedVelocity;

        for (weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            gun = &Guns[weaponIndex];

            gun->FireIntervalPrev = gun->FireInterval;
            gun->FireIntervalCount = gun->FireInterval;
            gun->AmmoCount = gun->Ammo;
            gun->ReloadTimePrev = gun->ReloadTime;
            gun->ReloadTimeCount = gun->ReloadTime;
            gun->StartUpTimeCount = gun->StartUpTime;

            // Set timings for when to let out and in a magazine, if at all
            if (gun->ClipReload) {
                gun->ClipOutTime = static_cast<uint16_t>(gun->ReloadTime * 0.8);
                gun->ClipInTime = static_cast<uint16_t>(gun->ReloadTime * 0.3);
            } else {
                gun->ClipOutTime = 0;
                gun->ClipInTime = 0;
            }

            // Set bullet lifetime
            switch (gun->BulletStyle) {
                case BULLET_STYLE_FRAGNADE:
                case BULLET_STYLE_CLUSTERNADE:
                    gun->Timeout = 300; // Assuming GRENADE_TIMEOUT value
                    break;
                case BULLET_STYLE_FLAME:
                    gun->Timeout = 60; // Assuming FLAMER_TIMEOUT value
                    break;
                case BULLET_STYLE_PUNCH:
                case BULLET_STYLE_KNIFE:
                    gun->Timeout = 10; // Assuming MELEE_TIMEOUT value
                    break;
                case BULLET_STYLE_M2:
                    gun->Timeout = 180; // Assuming M2BULLET_TIMEOUT value
                    break;
                default:
                    gun->Timeout = 60; // Assuming BULLET_TIMEOUT value
                    break;
            }
        }

        // Force M79 reload on spawn
        Guns[M79].AmmoCount = 0;
    }

    inline uint32_t CreateWMChecksum() {
        uint32_t hash = 5381;
        int weaponIndex;

        // djb2 hashing algorithm
        for (weaponIndex = 1; weaponIndex <= ORIGINAL_WEAPONS; weaponIndex++) {
            TGun& gun = Guns[weaponIndex];

            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.HitMultiply * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.FireInterval * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.Ammo * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.ReloadTime * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.Speed * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.BulletStyle * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.StartUpTime * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.Bink * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.MovementAcc * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.BulletSpread * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.Recoil * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.Push * 1000));
            hash = hash + ((hash << 5) + static_cast<uint32_t>(gun.InheritedVelocity * 1000));
        }

        return hash;
    }

    inline int16_t WeaponNumToIndex(uint8_t Num) {
        int weaponIndex;
        for (weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            if (Num == Guns[weaponIndex].Num) {
                return static_cast<int16_t>(weaponIndex);
            }
        }
        return -1;
    }

    inline int WeaponNameToNum(const std::string& Name) {
        int i;
        for (i = 1; i <= TOTAL_WEAPONS; i++) {
            if (Name == Guns[i].Name) {
                return Guns[i].Num;
            }
        }
        return -1;
    }

    inline std::string WeaponNumToName(int Num) {
        switch (Num) {
            case EAGLE_NUM: return Guns[EAGLE].Name;
            case MP5_NUM: return Guns[MP5].Name;
            case AK74_NUM: return Guns[AK74].Name;
            case STEYRAUG_NUM: return Guns[STEYRAUG].Name;
            case SPAS12_NUM: return Guns[SPAS12].Name;
            case RUGER77_NUM: return Guns[RUGER77].Name;
            case M79_NUM: return Guns[M79].Name;
            case BARRETT_NUM: return Guns[BARRETT].Name;
            case M249_NUM: return Guns[M249].Name;
            case MINIGUN_NUM: return Guns[MINIGUN].Name;
            case COLT_NUM: return Guns[COLT].Name;
            case KNIFE_NUM: return Guns[KNIFE].Name;
            case CHAINSAW_NUM: return Guns[CHAINSAW].Name;
            case LAW_NUM: return Guns[LAW].Name;
            case BOW2_NUM: return Guns[BOW2].Name;
            case BOW_NUM: return Guns[BOW].Name;
            case FLAMER_NUM: return Guns[FLAMER].Name;
            case M2_NUM: return Guns[M2].Name;
            case NOWEAPON_NUM: return Guns[NOWEAPON].Name;
            case FRAGGRENADE_NUM: return Guns[FRAGGRENADE].Name;
            case CLUSTERGRENADE_NUM: return Guns[CLUSTERGRENADE].Name;
            case CLUSTER_NUM: return Guns[CLUSTER].Name;
            case THROWNKNIFE_NUM: return Guns[THROWNKNIFE].Name;
            default: return "";
        }
    }

    inline bool IsMainWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= 1) && (WeaponIndex <= MAIN_WEAPONS);
    }

    inline bool IsSecondaryWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= PRIMARY_WEAPONS + 1) && (WeaponIndex <= MAIN_WEAPONS);
    }

    inline bool IsExtendedWeaponIndex(int16_t WeaponIndex) {
        return (WeaponIndex >= 1) && (WeaponIndex <= EXTENDED_WEAPONS);
    }

    inline uint16_t CalculateBink(uint16_t Accumulated, uint16_t Bink) {
        // Adding bink has diminishing returns as more gets accumulated
        return static_cast<uint16_t>(
            Accumulated + Bink - static_cast<int>(
                Accumulated * (Accumulated / ((10 * Bink) + Accumulated))
            )
        );
    }

    inline uint8_t WeaponNumInternalToExternal(uint8_t Num) {
        switch (Num) {
            case KNIFE_NUM:    return 14;
            case CHAINSAW_NUM: return 15;
            case LAW_NUM:      return 16;
            case FLAMER_NUM:   return 11;
            case BOW_NUM:      return 12;
            case BOW2_NUM:     return 13;
            default:           return Num;
        }
    }

    inline uint8_t WeaponNumExternalToInternal(uint8_t Num) {
        switch (Num) {
            case 11: return FLAMER_NUM;
            case 12: return BOW_NUM;
            case 13: return BOW2_NUM;
            case 14: return KNIFE_NUM;
            case 15: return CHAINSAW_NUM;
            case 16: return LAW_NUM;
            default: return Num;
        }
    }

    inline std::string WeaponNameByNum(int Num) {
        for (int weaponIndex = 1; weaponIndex <= TOTAL_WEAPONS; weaponIndex++) {
            if (Num == Guns[weaponIndex].Num) {
                return Guns[weaponIndex].Name;
            }
        }
        return "";
    }
}

// Using declarations to bring into global namespace
using WeaponsImpl::TGun;
using WeaponsImpl::Guns;
using WeaponsImpl::DefaultGuns;
using WeaponsImpl::DefaultWMChecksum;
using WeaponsImpl::LoadedWMChecksum;
using WeaponsImpl::CreateWeapons;
using WeaponsImpl::CreateDefaultWeapons;
using WeaponsImpl::CreateWeaponsBase;
using WeaponsImpl::CreateNormalWeapons;
using WeaponsImpl::CreateRealisticWeapons;
using WeaponsImpl::BuildWeapons;
using WeaponsImpl::CreateWMChecksum;
using WeaponsImpl::WeaponNumToIndex;
using WeaponsImpl::WeaponNameToNum;
using WeaponsImpl::WeaponNumToName;
using WeaponsImpl::IsMainWeaponIndex;
using WeaponsImpl::IsSecondaryWeaponIndex;
using WeaponsImpl::IsExtendedWeaponIndex;
using WeaponsImpl::CalculateBink;
using WeaponsImpl::WeaponNumInternalToExternal;
using WeaponsImpl::WeaponNumExternalToInternal;
using WeaponsImpl::WeaponNameByNum;

// Global variables
extern TGun Guns[TOTAL_WEAPONS + 1];  // Pascal arrays start from 1
extern TGun DefaultGuns[TOTAL_WEAPONS + 1];  // Pascal arrays start from 1
extern uint32_t DefaultWMChecksum;
extern uint32_t LoadedWMChecksum;

#endif // WEAPONS_H