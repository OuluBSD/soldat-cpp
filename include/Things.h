#ifndef THINGS_H
#define THINGS_H

//*******************************************************************************
//
//       Things Unit for SOLDAT
//
//       Copyright (c) 2002-03 Michal Marcinkowski
//
//*******************************************************************************

#include <vector>
#include <memory>
#include <cstdint>

#include "Vector.h"
#include "Constants.h"

// Forward declaration
struct TThing;

// TThing structure definition
struct TThing {
    uint8_t Num;
    bool Active;
    uint8_t Style;
    uint8_t Type;
    TVector2 Pos;
    TVector2 OldPos;
    TVector2 Velocity;
    uint8_t Owner;
    uint8_t HoldingSprite;
    uint8_t Constraints;
    uint8_t Ammo;
    uint8_t WeaponNum;
    uint8_t SecondaryNum;
    uint8_t TimeOut;
    bool OnGround;
    bool Obstructed;
    uint8_t Anim;
    uint8_t AnimType;
    int8_t AnimDir;
    uint8_t AnimFrame;
    uint8_t AnimCounter;
    uint8_t AnimSpeed;
    uint8_t AnimStart;
    uint8_t AnimStop;
    float Angle;
    float Rot;
    float OldRot;
    bool Visible;
    uint8_t Team;
    
    // Constructor
    TThing();
    
    // Methods
    void Reset();
    void SetPosition(TVector2 NewPos);
    void SetVelocity(TVector2 NewVelocity);
    void Update();
    void UpdatePhysics();
    void UpdateAnimations();
    void SetOwner(uint8_t NewOwner);
    void SetHoldingSprite(uint8_t SpriteNum);
    void Drop();
    void Take(uint8_t SpriteNum);
    void SendSnapshot();
};

// Thing type constants
const uint8_t THING_WEAPON = 1;
const uint8_t THING_SECONDARY = 2;
const uint8_t THING_GRENADE = 3;
const uint8_t THING_FLAG = 4;
const uint8_t THING_BONUS = 5;
const uint8_t THING_KIT = 6;

// Thing style constants
const uint8_t WEAPON_THING = 1;
const uint8_t SECONDARY_THING = 2;
const uint8_t GRENADE_THING = 3;
const uint8_t FLAG_RED = 4;
const uint8_t FLAG_BLUE = 5;
const uint8_t FLAG_GREEN = 6;
const uint8_t FLAG_YELLOW = 7;
const uint8_t BONUS_FLAMER = 8;
const uint8_t BONUS_PREDATOR = 9;
const uint8_t BONUS_BERSERKER = 10;
const uint8_t KIT_VEST = 11;
const uint8_t KIT_LASER = 12;
const uint8_t KIT_LAUNCHER = 13;
const uint8_t KIT_HMG = 14;

// Global things array
extern std::vector<std::unique_ptr<TThing>> Thing;

// Global variables
extern uint8_t TotalThings;
extern uint8_t ActiveThings;

// Function declarations
void InitializeThings();
void UpdateThings();
void DrawThings();
void ResetAllThings();
void CreateThing(uint8_t Num, uint8_t Style, TVector2 Pos, uint8_t Owner = 0);
void DeleteThing(uint8_t Num);
void SetThingPosition(uint8_t Num, TVector2 Pos);
void SetThingVelocity(uint8_t Num, TVector2 Vel);
void SetThingOwner(uint8_t Num, uint8_t Owner);
void SetThingHoldingSprite(uint8_t Num, uint8_t SpriteNum);
bool IsThingActive(uint8_t Num);
uint8_t GetNextFreeThingNum();

#endif // THINGS_H