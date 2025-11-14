#ifndef POLYMAP_H
#define POLYMAP_H

//*******************************************************************************
//                                                                              
//       PolyMap Unit for SOLDAT                                                 
//                                                                              
//       Copyright (c) 2001-02 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include "MapFile.h"
#include "Waypoints.h"
#include "Vector.h"
#include "Util.h"
#include <string>
#include <vector>
#include <set>

// Constants
const int MAX_POLYS = 5000;
const int MIN_SECTOR = -25;
const int MAX_SECTOR = 25;
const int MIN_SECTORZ = -35;
const int MAX_SECTORZ = 35;
const int SECTOR_TILE = 3;
const int MIN_TILE = MIN_SECTOR * SECTOR_TILE;
const int MAX_TILE = MAX_SECTOR * SECTOR_TILE;
const int MAX_PROPS = 500;
const int MAX_SPAWNPOINTS = 255;
const int MAX_COLLIDERS = 128;

// Polygon constants
const int POLY_TYPE_NORMAL                = 0;
const int POLY_TYPE_ONLY_BULLETS          = 1;
const int POLY_TYPE_ONLY_PLAYER           = 2;
const int POLY_TYPE_DOESNT               = 3;
const int POLY_TYPE_ICE                  = 4;
const int POLY_TYPE_DEADLY               = 5;
const int POLY_TYPE_BLOODY_DEADLY        = 6;
const int POLY_TYPE_HURTS                = 7;
const int POLY_TYPE_REGENERATES          = 8;
const int POLY_TYPE_LAVA                 = 9;
const int POLY_TYPE_RED_BULLETS          = 10;
const int POLY_TYPE_RED_PLAYER           = 11;
const int POLY_TYPE_BLUE_BULLETS         = 12;
const int POLY_TYPE_BLUE_PLAYER          = 13;
const int POLY_TYPE_YELLOW_BULLETS       = 14;
const int POLY_TYPE_YELLOW_PLAYER        = 15;
const int POLY_TYPE_GREEN_BULLETS        = 16;
const int POLY_TYPE_GREEN_PLAYER         = 17;
const int POLY_TYPE_BOUNCY               = 18;
const int POLY_TYPE_EXPLODES             = 19;
const int POLY_TYPE_HURTS_FLAGGERS       = 20;
const int POLY_TYPE_ONLY_FLAGGERS        = 21;
const int POLY_TYPE_NOT_FLAGGERS         = 22;
const int POLY_TYPE_NON_FLAGGER_COLLIDES = 23;
const int POLY_TYPE_BACKGROUND           = 24;
const int POLY_TYPE_BACKGROUND_TRANSITION = 25;

// Background constants
const int BACKGROUND_NORMAL       = 0;
const int BACKGROUND_TRANSITION   = 1;
const int BACKGROUND_POLY_UNKNOWN = -2;
const int BACKGROUND_POLY_NONE    = -1;

// Forward declarations
struct TMapFile;
struct TWaypoints;
struct TVector2;

// Types
using TLoadMapGraphics = void(*)(TMapFile&, bool, int, int);

struct TPolyMap {
    uint32_t MapID;
    TMapInfo MapInfo;
    std::string Name;
    std::string Filename;
    int SectorsDivision;
    int SectorsNum;
    int StartJet;
    uint8_t Grenades;
    uint8_t Medikits;
    uint8_t Weather;
    uint8_t Steps;
    int PolyCount;
    int BackPolyCount;
    int ColliderCount;
    TMapPolygon Polys[MAX_POLYS + 1];  // Pascal arrays start from 1
    TMapPolygon* BackPolys[MAX_POLYS + 1];
    uint8_t PolyType[MAX_POLYS + 1];
    TVector2 Perp[MAX_POLYS + 1][4];  // Pascal arrays start from 1, and 1-3 for 3 edges
    float Bounciness[MAX_POLYS + 1];
    TMapSector Sectors[MAX_SECTORZ + 1 - MIN_SECTORZ][MAX_SECTORZ + 1 - MIN_SECTORZ];  // Indexed from MIN_SECTORZ to MAX_SECTORZ
    TMapSpawnpoint Spawnpoints[MAX_SPAWNPOINTS + 1];
    TMapCollider Collider[MAX_COLLIDERS + 1];
    int FlagSpawn[3];  // Pascal arrays start from 1
    TLoadMapGraphics LoadGraphics;

    TPolyMap() : MapID(0), SectorsDivision(0), SectorsNum(0), StartJet(0), 
                 Grenades(0), Medikits(0), Weather(0), Steps(0), PolyCount(0), 
                 BackPolyCount(0), ColliderCount(0) {
        // Initialize arrays to zero
        std::memset(PolyType, 0, sizeof(PolyType));
        std::memset(Bounciness, 0, sizeof(Bounciness));
        std::memset(Perp, 0, sizeof(Perp));
        std::memset(BackPolys, 0, sizeof(BackPolys));
        std::memset(FlagSpawn, 0, sizeof(FlagSpawn));
    }

    void Initialize();
    void LoadData(TMapFile& MapFile);
    bool LoadMap(const TMapInfo& Map); // overload
#ifndef SERVER_CODE
    bool LoadMap(const TMapInfo& Map, bool BgForce, int BgColorTop, int BgColorBtm); // overload
#endif
    bool LineInPoly(const TVector2& a, const TVector2& b, int Poly, TVector2& v);
    bool PointInPolyEdges(float x, float y, int i);
    bool PointInPoly(const TVector2& p, const TMapPolygon& Poly);
    TVector2 ClosestPerpendicular(int j, TVector2 Pos, float& d, int& n);
    bool CollisionTest(TVector2 Pos, TVector2& PerpVec, bool IsFlag = false);
    bool CollisionTestExcept(TVector2 Pos, TVector2& PerpVec, int c);
    bool RayCast(const TVector2& a, const TVector2& b, float& Distance, float MaxDist, 
                 bool Player = false, bool Flag = false, bool Bullet = true, 
                 bool CheckCollider = false, uint8_t Team = 0);
};

void CheckOutOfBounds(float& x, float& y); // overload
void CheckOutOfBounds(int16_t& x, int16_t& y); // overload

#endif // POLYMAP_H