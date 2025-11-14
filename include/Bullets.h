#ifndef BULLETS_H
#define BULLETS_H

//*******************************************************************************
//
//       Bullets Unit for SOLDAT
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
struct TBullet;

// Bullet structure definition
struct TBullet {
    uint8_t Num;
    bool Active;
    uint8_t Owner;
    uint8_t WeaponNum;
    TVector2 Pos;
    TVector2 OldPos;
    TVector2 Velocity;
    float Angle;
    float Rot;
    float OldRot;
    uint16_t Seed;
    uint8_t Constraints;
    uint8_t Style;
    uint8_t Bounces;
    uint8_t Ricochets;
    float Damage;
    uint8_t TimeOut;
    bool Exploded;
    bool Obstructed;
    uint8_t Team;
    bool FromSprite;
    uint8_t KillBullet;
    uint8_t Killer;
    uint8_t Where;
    bool Forced;
    
    // Constructor
    TBullet();
    
    // Methods
    void Reset();
    void SetPosition(TVector2 NewPos);
    void SetVelocity(TVector2 NewVelocity);
    void Update();
    void UpdatePhysics();
    void Explode();
    void Ricochet();
    void ApplyDamage(float Damage);
    void SetOwner(uint8_t NewOwner);
    void SetWeaponNum(uint8_t NewWeapon);
    void SendSnapshot();
};

// Global bullets array
extern std::vector<std::unique_ptr<TBullet>> Bullet;

// Global variables
extern uint8_t TotalBullets;
extern uint8_t ActiveBullets;

// Function declarations
void InitializeBullets();
void UpdateBullets();
void DrawBullets();
void ResetAllBullets();
void CreateBullet(uint8_t Num, uint8_t WeaponNum, TVector2 Pos, TVector2 Vel, uint8_t Owner, bool Forced = false);
void DeleteBullet(uint8_t Num);
void SetBulletPosition(uint8_t Num, TVector2 Pos);
void SetBulletVelocity(uint8_t Num, TVector2 Vel);
void SetBulletOwner(uint8_t Num, uint8_t Owner);
void SetBulletWeapon(uint8_t Num, uint8_t WeaponNum);
bool IsBulletActive(uint8_t Num);
uint8_t GetNextFreeBulletNum();

#endif // BULLETS_H