#ifndef MAPFILE_H
#define MAPFILE_H

//*******************************************************************************
//                                                                              
//       MapFile Unit for SOLDAT                                                   
//                                                                              
//       Copyright (c) 2002-2003 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <vector>
#include <string>
#include <cstdint>
#include "Vector.h"
#include "Waypoints.h"
#include "Util.h"

// Forward declarations
struct TMapInfo;

// Type definitions
using TMapColor = uint8_t[4];  // [r,g,b,a] - Pascal array indexed from 0

struct TMapVertex {
    float x, y, z, rhw;
    TMapColor Color;
    float u, v;
    
    TMapVertex() : x(0.0f), y(0.0f), z(0.0f), rhw(0.0f), u(0.0f), v(0.0f) {
        Color[0] = 0; Color[1] = 0; Color[2] = 0; Color[3] = 0;
    }
};

struct TMapPolygon {
    TMapVertex Vertices[4];  // Pascal arrays starting from 1 with 3 vertices
    TVector3 Normals[4];     // Pascal arrays starting from 1 with 3 normals
    uint8_t PolyType;
    uint8_t TextureIndex;
    
    TMapPolygon() : PolyType(0), TextureIndex(0) {
        // Vertices and Normals will be initialized by their constructors
    }
};

struct TMapSector {
    std::vector<uint16_t> Polys;
    
    TMapSector() = default;
};

struct TMapProp {
    bool Active;
    uint16_t Style;
    int Width, Height;
    float x, y;
    float Rotation;
    float ScaleX, ScaleY;
    uint8_t Alpha;
    TMapColor Color;
    uint8_t Level;
    
    TMapProp() : Active(false), Style(0), Width(0), Height(0), x(0.0f), y(0.0f),
                 Rotation(0.0f), ScaleX(1.0f), ScaleY(1.0f), Alpha(255), Level(0) {
        Color[0] = 255; Color[1] = 255; Color[2] = 255; Color[3] = 255;  // Default white
    }
};

struct TMapScenery {
    std::string Filename;
    int Date;
    
    TMapScenery() : Date(0) {}
};

struct TMapCollider {
    bool Active;
    float x, y;
    float Radius;
    
    TMapCollider() : Active(false), x(0.0f), y(0.0f), Radius(0.0f) {}
};

struct TMapSpawnpoint {
    bool Active;
    int x, y, Team;
    
    TMapSpawnpoint() : Active(false), x(0), y(0), Team(0) {}
};

struct TMapFile {
    std::string Filename;
    TMapInfo MapInfo;
    uint32_t Hash;
    int Version;
    std::string MapName;
    std::vector<std::string> Textures;
    TMapColor BgColorTop;
    TMapColor BgColorBtm;
    int StartJet;
    uint8_t GrenadePacks;
    uint8_t Medikits;
    uint8_t Weather;
    uint8_t Steps;
    int RandomID;
    std::vector<TMapPolygon> Polygons;
    int SectorsDivision;
    int SectorsNum;
    std::vector<TMapSector> Sectors;
    std::vector<TMapProp> Props;
    std::vector<TMapScenery> Scenery;
    std::vector<TMapCollider> Colliders;
    std::vector<TMapSpawnpoint> Spawnpoints;
    std::vector<TWaypoint> Waypoints;
    
    TMapFile() : Hash(0), Version(0), StartJet(0), GrenadePacks(0), Medikits(0),
                 Weather(0), Steps(0), RandomID(0), SectorsDivision(0), SectorsNum(0) {
        // Initialize color arrays to default values
        for (int i = 0; i < 4; i++) {
            BgColorTop[i] = 0;
            BgColorBtm[i] = 0;
        }
    }
};

// Function declarations
bool LoadMapFile(const TMapInfo& MapInfo, TMapFile& Map);
TMapColor MapColor(int32_t Color);
bool IsPropActive(TMapFile& Map, int Index);

// Constants
const int MAX_POLYS = 5000;
const int MAX_SECTOR = 25;
const int MAX_PROPS = 500;
const int MAX_COLLIDERS = 128;
const int MAX_SPAWNPOINTS = 255;
const int MAX_WAYPOINTS = 5000;
const int MAX_CONNECTIONS = 8;

#endif // MAPFILE_H