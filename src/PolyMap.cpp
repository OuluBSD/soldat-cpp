#include "PolyMap.h"
#include "MapFile.h"
#include "Waypoints.h"
#include "Vector.h"
#include "Util.h"
#include "Math.h"
#include "Calc.h"
#include "Game.h"
#include "Constants.h"

#include <cstring>
#include <algorithm>
#include <cmath>

namespace PolyMapImpl {

    void TPolyMap::Initialize() {
        MapID = 0;
        Name = "";
        Filename = "";
        SectorsDivision = 0;
        SectorsNum = 0;
        StartJet = 0;
        Grenades = 0;
        Medikits = 0;
        Weather = 0;
        Steps = 0;
        PolyCount = 0;
        BackPolyCount = 0;
        ColliderCount = 0;

        std::memset(&Polys[1], 0, sizeof(Polys) - sizeof(Polys[0]));  // Skip index 0
        std::memset(&BackPolys[1], 0, sizeof(BackPolys) - sizeof(BackPolys[0]));  // Skip index 0
        std::memset(&PolyType[1], 0, sizeof(PolyType) - sizeof(PolyType[0]));  // Skip index 0
        std::memset(&Perp[1][1], 0, sizeof(Perp) - sizeof(Perp[0]));  // Skip index 0
        std::memset(&Bounciness[1], 0, sizeof(Bounciness) - sizeof(Bounciness[0]));  // Skip index 0
        std::memset(&Spawnpoints[1], 0, sizeof(Spawnpoints) - sizeof(Spawnpoints[0]));  // Skip index 0
        std::memset(&Collider[1], 0, sizeof(Collider) - sizeof(Collider[0]));  // Skip index 0
        std::memset(&FlagSpawn[1], 0, sizeof(FlagSpawn) - sizeof(FlagSpawn[0]));  // Skip index 0

        for (int i = MIN_SECTORZ; i <= MAX_SECTORZ; i++) {
            for (int j = MIN_SECTORZ; j <= MAX_SECTORZ; j++) {
                Sectors[i - MIN_SECTORZ][j - MIN_SECTORZ].Polys.clear();
            }
        }

        // Initialize BotPath - this would need to be handled elsewhere where BotPath is defined
        // std::memset(BotPath.Waypoint[1], 0, sizeof(BotPath.Waypoint));
    }

    void TPolyMap::LoadData(TMapFile& MapFile) {
        MapID = MapFile.Hash;
        SectorsDivision = MapFile.SectorsDivision;
        SectorsNum = MapFile.SectorsNum;
        StartJet = 119 * MapFile.StartJet / 100;  // quickfix
        Grenades = MapFile.GrenadePacks;
        Medikits = MapFile.Medikits;
        Weather = MapFile.Weather;
        Steps = MapFile.Steps;
        PolyCount = static_cast<int>(MapFile.Polygons.size());
        ColliderCount = static_cast<int>(MapFile.Colliders.size());

        if (PolyCount > 0 && PolyCount <= MAX_POLYS) {
            for (int i = 0; i < PolyCount; i++) {
                Polys[i + 1] = MapFile.Polygons[i];  // Pascal arrays start from 1
            }
        }

        if (ColliderCount > 0 && ColliderCount <= MAX_COLLIDERS) {
            for (int i = 0; i < ColliderCount; i++) {
                Collider[i + 1] = MapFile.Colliders[i];  // Pascal arrays start from 1
            }
        }

        int spawnPointCount = std::min(static_cast<int>(MapFile.Spawnpoints.size()), MAX_SPAWNPOINTS);
        for (int i = 0; i < spawnPointCount; i++) {
            Spawnpoints[i + 1] = MapFile.Spawnpoints[i];  // Pascal arrays start from 1
        }

        // Handle waypoints (assuming BotPath is defined elsewhere)
        int waypointCount = std::min(static_cast<int>(MapFile.Waypoints.size()), MAX_WAYPOINTS);
        for (int i = 0; i < waypointCount; i++) {
            // BotPath.Waypoint[i + 1] = MapFile.Waypoints[i];  // Pascal arrays start from 1
        }

        for (int i = 1; i <= PolyCount; i++) {
            PolyType[i] = Polys[i].PolyType;

            Perp[i][1].x = Polys[i].Normals[1].x;
            Perp[i][1].y = Polys[i].Normals[1].y;
            Perp[i][2].x = Polys[i].Normals[2].x;
            Perp[i][2].y = Polys[i].Normals[2].y;
            Perp[i][3].x = Polys[i].Normals[3].x;
            Perp[i][3].y = Polys[i].Normals[3].y;

            Bounciness[i] = Vec2Length(Perp[i][3]);  // gg

            Vec2Normalize(Perp[i][1], Perp[i][1]);
            Vec2Normalize(Perp[i][2], Perp[i][2]);
            Vec2Normalize(Perp[i][3], Perp[i][3]);

            if ((PolyType[i] == POLY_TYPE_BACKGROUND) || 
                (PolyType[i] == POLY_TYPE_BACKGROUND_TRANSITION)) {
                BackPolyCount++;
                BackPolys[BackPolyCount] = &Polys[i];
            }
        }

        int k = 0;
        for (int i = -SectorsNum; i <= SectorsNum; i++) {
            for (int j = -SectorsNum; j <= SectorsNum; j++) {
                if (k < static_cast<int>(MapFile.Sectors.size()) && 
                    !MapFile.Sectors[k].Polys.empty()) {
                    Sectors[i - MIN_SECTORZ][j - MIN_SECTORZ].Polys.resize(MapFile.Sectors[k].Polys.size() + 1);
                    for (size_t p = 0; p < MapFile.Sectors[k].Polys.size(); p++) {
                        Sectors[i - MIN_SECTORZ][j - MIN_SECTORZ].Polys[p + 1] = MapFile.Sectors[k].Polys[p];
                    }
                }
                k++;
            }
        }

        for (int i = 1; i <= spawnPointCount; i++) {
            // Check if spawnpoint is out of bounds
            if ((std::abs(Spawnpoints[i].x) >= 2000000) || 
                (std::abs(Spawnpoints[i].y) >= 2000000)) {
                Spawnpoints[i].Active = false;
            }

            if (Spawnpoints[i].Active) {
                if ((FlagSpawn[1] == 0) && (Spawnpoints[i].Team == 5)) {
                    FlagSpawn[1] = i;
                }
                if ((FlagSpawn[2] == 0) && (Spawnpoints[i].Team == 6)) {
                    FlagSpawn[2] = i;
                }
            }
        }

        for (int i = 1; i <= waypointCount; i++) {
            if ((std::abs(/*BotPath.Waypoint[i].x*/) >= 2000000) || 
                (std::abs(/*BotPath.Waypoint[i].y*/) >= 2000000)) {
                // BotPath.Waypoint[i].Active = false;  // Assuming BotPath is defined elsewhere
            }
        }
    }

    bool TPolyMap::LoadMap(const TMapInfo& Map) {
        Initialize();
        TMapFile mapFile = {};

        if (LoadMapFile(Map, mapFile)) {
            Filename = Map.Name;
            LoadData(mapFile);
            Name = Map.Name;
            MapInfo = Map;
            // MainConsole.Console(mapFile.MapName, GAME_MESSAGE_COLOR);
            return true;
        }
        return false;
    }

#ifndef SERVER_CODE
    bool TPolyMap::LoadMap(const TMapInfo& Map, bool BgForce, int BgColorTop, int BgColorBtm) {
        if (Filename == Map.Name) {
            return true;
        }

        Initialize();
        TMapFile mapFile = {};

        if (LoadMapFile(Map, mapFile)) {
            Filename = Map.Name;
            LoadData(mapFile);
            Name = Map.Name;
            MapInfo = Map;

            if (LoadGraphics) {
                LoadGraphics(mapFile, BgForce, MapColor(BgColorTop), MapColor(BgColorBtm));
            }

            // MainConsole.Console(mapFile.MapName, GAME_MESSAGE_COLOR);
            return true;
        }
        return false;
    }
#endif

    bool TPolyMap::LineInPoly(const TVector2& a, const TVector2& b, int Poly, 
                              TVector2& v) {
        for (int i = 1; i <= 3; i++) {
            int j = (i == 3) ? 1 : i + 1;
            TMapVertex* p = &(Polys[Poly].Vertices[i]);
            TMapVertex* q = &(Polys[Poly].Vertices[j]);

            if ((b.x != a.x) || (q->x != p->x)) {
                if (b.x == a.x) {
                    float bk = (q->y - p->y) / (q->x - p->x);
                    float bm = p->y - bk * p->x;
                    v.x = a.x;
                    v.y = bk * v.x + bm;

                    if ((v.x > std::min(p->x, q->x)) && (v.x < std::max(p->x, q->x)) &&
                        (v.y > std::min(a.y, b.y)) && (v.y < std::max(a.y, b.y))) {
                        return true;
                    }
                } else if (q->x == p->x) {
                    float ak = (b.y - a.y) / (b.x - a.x);
                    float am = a.y - ak * a.x;
                    v.x = p->x;
                    v.y = ak * v.x + am;

                    if ((v.y > std::min(p->y, q->y)) && (v.y < std::max(p->y, q->y)) &&
                        (v.x > std::min(a.x, b.x)) && (v.x < std::max(a.x, b.x))) {
                        return true;
                    }
                } else {
                    float ak = (b.y - a.y) / (b.x - a.x);
                    float bk = (q->y - p->y) / (q->x - p->x);

                    if (ak != bk) {
                        float am = a.y - ak * a.x;
                        float bm = p->y - bk * p->x;
                        v.x = (bm - am) / (ak - bk);
                        v.y = ak * v.x + am;

                        if ((v.x > std::min(p->x, q->x)) && (v.x < std::max(p->x, q->x)) &&
                            (v.x > std::min(a.x, b.x)) && (v.x < std::max(a.x, b.x))) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool TPolyMap::PointInPolyEdges(float x, float y, int i) {
        TVector2 u;
        float d;

        u.x = x - Polys[i].Vertices[1].x;
        u.y = y - Polys[i].Vertices[1].y;
        d = Perp[i][1].x * u.x + Perp[i][1].y * u.y;
        if (d < 0) return false;

        u.x = x - Polys[i].Vertices[2].x;
        u.y = y - Polys[i].Vertices[2].y;
        d = Perp[i][2].x * u.x + Perp[i][2].y * u.y;
        if (d < 0) return false;

        u.x = x - Polys[i].Vertices[3].x;
        u.y = y - Polys[i].Vertices[3].y;
        d = Perp[i][3].x * u.x + Perp[i][3].y * u.y;
        if (d < 0) return false;

        return true;
    }

    bool TPolyMap::PointInPoly(const TVector2& p, const TMapPolygon& Poly) {
        const TMapVertex* a = &(Poly.Vertices[1]);
        const TMapVertex* b = &(Poly.Vertices[2]);
        const TMapVertex* c = &(Poly.Vertices[3]);

        float ap_x = p.x - a->x;
        float ap_y = p.y - a->y;

        bool p_ab = ((b->x - a->x) * ap_y - (b->y - a->y) * ap_x) > 0;
        bool p_ac = ((c->x - a->x) * ap_y - (c->y - a->y) * ap_x) > 0;

        if (p_ac == p_ab) return false;

        // p_bc <> p_ab
        if (((c->x - b->x) * (p.y - b->y) - (c->y - b->y) * (p.x - b->x) > 0) != p_ab) {
            return false;
        }

        return true;
    }

    TVector2 TPolyMap::ClosestPerpendicular(int j, TVector2 Pos, float& d, int& n) {
        TVector2 result = {0.0f, 0.0f};
        
        float px[4] = {0, Polys[j].Vertices[1].x, Polys[j].Vertices[2].x, Polys[j].Vertices[3].x};
        float py[4] = {0, Polys[j].Vertices[1].y, Polys[j].Vertices[2].y, Polys[j].Vertices[3].y};

        // Find closest edge
        TVector2 p1 = {px[1], py[1]};
        TVector2 p2 = {px[2], py[2]};

        float d1 = PointLineDistance(p1, p2, Pos);
        d = d1;
        int edgeV1 = 1;
        int edgeV2 = 2;

        p1.x = px[2]; p1.y = py[2];
        p2.x = px[3]; p2.y = py[3];

        float d2 = PointLineDistance(p1, p2, Pos);

        if (d2 < d1) {
            edgeV1 = 2;
            edgeV2 = 3;
            d = d2;
        }

        p1.x = px[3]; p1.y = py[3];
        p2.x = px[1]; p2.y = py[1];

        float d3 = PointLineDistance(p1, p2, Pos);

        if ((d3 < d2) && (d3 < d1)) {
            edgeV1 = 3;
            edgeV2 = 1;
            d = d3;
        }

        if ((edgeV1 == 1) && (edgeV2 == 2)) {
            result = Perp[j][1];
            n = 1;
        }

        if ((edgeV1 == 2) && (edgeV2 == 3)) {
            result = Perp[j][2];
            n = 2;
        }

        if ((edgeV1 == 3) && (edgeV2 == 1)) {
            result = Perp[j][3];
            n = 3;
        }

        return result;
    }

    bool TPolyMap::CollisionTest(TVector2 Pos, TVector2& PerpVec, bool IsFlag) {
        const std::set<int> EXCLUDED1 = {1, 2, 3, 11, 13, 15, 17, 24, 25};
        const std::set<int> EXCLUDED2 = {21, 22, 23};
        
        int kx = static_cast<int>(std::round(Pos.x / SectorsDivision));
        int ky = static_cast<int>(std::round(Pos.y / SectorsDivision));

        if ((kx > -SectorsNum) && (kx < SectorsNum) && 
            (ky > -SectorsNum) && (ky < SectorsNum)) {
            
            const auto& sector = Sectors[kx - MIN_SECTORZ][ky - MIN_SECTORZ];
            
            for (size_t j = 1; j < sector.Polys.size(); j++) {
                int w = sector.Polys[j];

                if (EXCLUDED1.find(PolyType[w]) == EXCLUDED1.end() &&
                    (IsFlag || EXCLUDED2.find(PolyType[w]) == EXCLUDED2.end())) {
                    
                    if (PointInPoly(Pos, Polys[w])) {
                        int b = 0;
                        float d = 0.0f;
                        PerpVec = ClosestPerpendicular(w, Pos, d, b);
                        PerpVec = Vec2Scale(PerpVec, 1.5f * d);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TPolyMap::CollisionTestExcept(TVector2 Pos, TVector2& PerpVec, int c) {
        const std::set<int> EXCLUDED = {1, 2, 3, 11, 24, 25};
        
        int kx = static_cast<int>(std::round(Pos.x / SectorsDivision));
        int ky = static_cast<int>(std::round(Pos.y / SectorsDivision));

        if ((kx > -SectorsNum) && (kx < SectorsNum) && 
            (ky > -SectorsNum) && (ky < SectorsNum)) {
            
            const auto& sector = Sectors[kx - MIN_SECTORZ][ky - MIN_SECTORZ];
            
            for (size_t j = 1; j < sector.Polys.size(); j++) {
                int w = sector.Polys[j];

                if (w != c && EXCLUDED.find(PolyType[w]) == EXCLUDED.end()) {
                    if (PointInPoly(Pos, Polys[w])) {
                        int b = 0;
                        float d = 0.0f;
                        PerpVec = ClosestPerpendicular(w, Pos, d, b);
                        PerpVec = Vec2Scale(PerpVec, 1.5f * d);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TPolyMap::RayCast(const TVector2& a, const TVector2& b, float& Distance, float MaxDist, 
                           bool Player, bool Flag, bool Bullet, bool CheckCollider, uint8_t Team) {
        TVector2 d_vec = {0.0f, 0.0f};
        Distance = Vec2Length(Vec2Subtract(a, b));
        if (Distance > MaxDist) {
            Distance = 9999999.0f;
            return true;
        }

        int ax = static_cast<int>(std::round(std::min(a.x, b.x) / SectorsDivision));
        int ay = static_cast<int>(std::round(std::min(a.y, b.y) / SectorsDivision));
        int bx = static_cast<int>(std::round(std::max(a.x, b.x) / SectorsDivision));
        int by = static_cast<int>(std::round(std::max(a.y, b.y) / SectorsDivision));

        if ((ax > MAX_SECTORZ) || (bx < MIN_SECTORZ) || 
            (ay > MAX_SECTORZ) || (by < MIN_SECTORZ)) {
            return false;
        }

        ax = std::max(MIN_SECTORZ, ax);
        ay = std::max(MIN_SECTORZ, ay);
        bx = std::min(MAX_SECTORZ, bx);
        by = std::min(MAX_SECTORZ, by);

        bool npCol = !Player;
        bool nbCol = !Bullet;

        for (int i = ax; i <= bx; i++) {
            for (int j = ay; j <= by; j++) {
                const auto& sector = Sectors[i - MIN_SECTORZ][j - MIN_SECTORZ];
                
                for (size_t p = 1; p < sector.Polys.size(); p++) {
                    int w = sector.Polys[p];

                    bool testcol = true;
                    if (((PolyType[w] == POLY_TYPE_RED_BULLETS) && ((Team != TEAM_ALPHA) || nbCol)) || 
                        ((PolyType[w] == POLY_TYPE_RED_PLAYER) && ((Team != TEAM_ALPHA) || npCol))) {
                        testcol = false;
                    }
                    if (((PolyType[w] == POLY_TYPE_BLUE_BULLETS) && ((Team != TEAM_BRAVO) || nbCol)) || 
                        ((PolyType[w] == POLY_TYPE_BLUE_PLAYER) && ((Team != TEAM_BRAVO) || npCol))) {
                        testcol = false;
                    }
                    if (((PolyType[w] == POLY_TYPE_YELLOW_BULLETS) && ((Team != TEAM_CHARLIE) || nbCol)) || 
                        ((PolyType[w] == POLY_TYPE_YELLOW_PLAYER) && ((Team != TEAM_CHARLIE) || npCol))) {
                        testcol = false;
                    }
                    if (((PolyType[w] == POLY_TYPE_GREEN_BULLETS) && ((Team != TEAM_DELTA) || nbCol)) || 
                        ((PolyType[w] == POLY_TYPE_GREEN_PLAYER) && ((Team != TEAM_DELTA) || npCol))) {
                        testcol = false;
                    }
                    if (((!Flag || npCol) && (PolyType[w] == POLY_TYPE_ONLY_FLAGGERS)) || 
                        ((Flag || npCol) && (PolyType[w] == POLY_TYPE_NOT_FLAGGERS))) {
                        testcol = false;
                    }
                    if (((!Flag || npCol || nbCol) && (PolyType[w] == POLY_TYPE_NON_FLAGGER_COLLIDES))) {
                        testcol = false;
                    }
                    if (((PolyType[w] == POLY_TYPE_ONLY_BULLETS) && nbCol) || 
                        ((PolyType[w] == POLY_TYPE_ONLY_PLAYER) && npCol) || 
                        (PolyType[w] == POLY_TYPE_DOESNT) || 
                        (PolyType[w] == POLY_TYPE_BACKGROUND) || 
                        (PolyType[w] == POLY_TYPE_BACKGROUND_TRANSITION)) {
                        testcol = false;
                    }
                    if (testcol) {
                        if (PointInPoly(a, Polys[w])) {
                            Distance = 0;
                            return true;
                        }
                        TVector2 temp_d;
                        if (LineInPoly(a, b, w, temp_d)) {
                            TVector2 c = Vec2Subtract(temp_d, a);
                            Distance = Vec2Length(c);
                            return true;
                        }
                    }
                }
            }
        }

        if (CheckCollider) {
            // check if vector crosses any colliders
            // |A*x + B*y + C| / Sqrt(A^2 + B^2) < r
            float e = a.y - b.y;
            float f = b.x - a.x;
            float g = a.x * b.y - a.y * b.x;
            float h = std::sqrt(e*e + f*f);
            for (int i = 1; i <= ColliderCount; i++) {
                if (Collider[i].Active) {
                    if (std::abs(e * Collider[i].x + f * Collider[i].y + g) / h <= Collider[i].Radius) {
                        float r = SqrDist(a.x, a.y, b.x, b.y) + Collider[i].Radius * Collider[i].Radius;
                        if (SqrDist(a.x, a.y, Collider[i].x, Collider[i].y) <= r) {
                            if (SqrDist(b.x, b.y, Collider[i].x, Collider[i].y) <= r) {
                                return false;
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    void CheckOutOfBounds(float& x, float& y) {
        if (x < (10 * (-Map.SectorsNum * Map.SectorsDivision) + 50)) {
            x = 1;
        } else if (x > (10 * (Map.SectorsNum * Map.SectorsDivision) - 50)) {
            x = 1;
        }

        if (y < (10 * (-Map.SectorsNum * Map.SectorsDivision) + 50)) {
            y = 1;
        } else if (y > (10 * (Map.SectorsNum * Map.SectorsDivision) - 50)) {
            y = 1;
        }
    }

    void CheckOutOfBounds(int16_t& x, int16_t& y) {
        if (x < (10 * (-Map.SectorsNum * Map.SectorsDivision) + 50)) {
            x = 1;
        } else if (x > (10 * (Map.SectorsNum * Map.SectorsDivision) - 50)) {
            x = 1;
        }

        if (y < (10 * (-Map.SectorsNum * Map.SectorsDivision) + 50)) {
            y = 1;
        } else if (y > (10 * (Map.SectorsNum * Map.SectorsDivision) - 50)) {
            y = 1;
        }
    }
}

// Using declarations to bring into global namespace
using PolyMapImpl::TPolyMap;
using PolyMapImpl::CheckOutOfBounds;