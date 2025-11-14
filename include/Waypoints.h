#ifndef WAYPOINTS_H
#define WAYPOINTS_H

//*******************************************************************************
//                                                                              
//       Waypoints Unit for SOLDAT                                               
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include <fstream>
#include <algorithm>

const int MAX_WAYPOINTS = 5000;
const int MAX_CONNECTIONS = 20;

struct TWaypoint {
    bool Active;
    int id;
    int X, Y;
    bool Left, Right, Up, Down, M2;
    uint8_t PathNum;
    uint8_t C1, C2, C3;
    int ConnectionsNum;
    int Connections[MAX_CONNECTIONS + 1];  // Pascal arrays start from 1

    TWaypoint() : Active(false), id(0), X(0), Y(0), Left(false), Right(false), 
                  Up(false), Down(false), M2(false), PathNum(0), C1(0), C2(0), C3(0),
                  ConnectionsNum(0) {
        std::fill(Connections, Connections + MAX_CONNECTIONS + 1, 0);
    }
};

// For file use
struct TPathRec {
    TWaypoint Waypoint[501];  // Pascal array starting from 1

    TPathRec() {
        for (int i = 0; i < 501; i++) {
            Waypoint[i] = TWaypoint();
        }
    }
};

struct TWaypoints {
    TWaypoint Waypoint[MAX_WAYPOINTS + 1];  // Pascal arrays start from 1

    TWaypoints() {
        for (int i = 0; i <= MAX_WAYPOINTS; i++) {
            Waypoint[i] = TWaypoint();
        }
    }

    void LoadFromFile(const std::string& filename);
    void SaveToFile(const std::string& filename);
    int FindClosest(float X, float Y, float Radius, int CurrWaypoint);
    int CreateWaypoint(int sX, int sY, int sPath);
};

namespace WaypointsImpl {
    inline void TWaypoints::LoadFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return;
        }

        // Read the file in binary format as it was originally stored
        TPathRec addrRec;
        file.read(reinterpret_cast<char*>(&addrRec), sizeof(addrRec));
        file.close();

        // Copy data to the waypoints array
        for (int i = 1; i <= MAX_WAYPOINTS; i++) {
            Waypoint[i] = addrRec.Waypoint[i];
        }

        // Initialize inactive waypoints
        for (int i = 1; i <= MAX_WAYPOINTS; i++) {
            if (!Waypoint[i].Active) {
                Waypoint[i].ConnectionsNum = 0;
                for (int j = 1; j <= MAX_CONNECTIONS; j++) {
                    Waypoint[i].Connections[j] = 0;
                }
            }
        }
    }

    inline void TWaypoints::SaveToFile(const std::string& filename) {
        TPathRec addrRec;

        for (int i = 1; i <= MAX_WAYPOINTS; i++) {
            addrRec.Waypoint[i] = Waypoint[i];
        }

        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<char*>(&addrRec), sizeof(addrRec));
            file.close();
        }
    }

    inline float Distance(float x1, float y1, float x2, float y2) {
        return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }

    inline int TWaypoints::FindClosest(float X, float Y, float Radius, int CurrWaypoint) {
        float d;
        for (int i = 1; i <= MAX_WAYPOINTS; i++) {
            if (Waypoint[i].Active && CurrWaypoint != i) {
                d = Distance(X, Y, Waypoint[i].X, Waypoint[i].Y);
                if (d < Radius) {
                    return i;
                }
            }
        }
        return 0;
    }

    inline int TWaypoints::CreateWaypoint(int sX, int sY, int sPath) {
        for (int i = 1; i <= MAX_WAYPOINTS + 1; i++) {
            if (i == MAX_WAYPOINTS + 1) {
                return -1; // indicates failure
            }
            if (!Waypoint[i].Active) {
                break; // found the first inactive waypoint
            }
        }

        // i is now the index of the waypoint to create
        Waypoint[i].Active = true;
        Waypoint[i].X = sX;
        Waypoint[i].Y = sY;
        Waypoint[i].id = i;
        Waypoint[i].ConnectionsNum = 0;
        Waypoint[i].PathNum = sPath;

        // Initialize connections to 0
        for (int j = 1; j <= MAX_CONNECTIONS; j++) {
            Waypoint[i].Connections[j] = 0;
        }

        // Activate waypoint
        return i;
    }
}

// Using declarations to bring into global namespace
using WaypointsImpl::TWaypoint;
using WaypointsImpl::TPathRec;
using WaypointsImpl::TWaypoints;

#endif // WAYPOINTS_H