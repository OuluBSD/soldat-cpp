#ifndef BANSYSTEM_H
#define BANSYSTEM_H

//*******************************************************************************
//                                                                              
//       BanSystem Unit for SOLDAT                                               
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include "Constants.h"  // Assuming this defines PERMANENT

// Structures for bans
struct TBanIP {
    std::string IP;
    int Time;
    std::string Reason;

    TBanIP() : Time(0) {}
    TBanIP(const std::string& ip, int time, const std::string& reason)
        : IP(ip), Time(time), Reason(reason) {}
};

struct TBanHW {
    std::string HW;
    int Time;
    std::string Reason;

    TBanHW() : Time(0) {}
    TBanHW(const std::string& hw, int time, const std::string& reason)
        : HW(hw), Time(time), Reason(reason) {}
};

// Function declarations
void AddBannedIP(const std::string& IP, const std::string& Reason, int Duration = PERMANENT);
bool DelBannedIP(const std::string& IP);
bool CheckBannedIP(const std::string& IP);
int FindBan(const std::string& IP);
void LoadBannedList(const std::string& filename);
void SaveBannedList(const std::string& filename);

// Hardware ID bans
void AddBannedHW(const std::string& HW, const std::string& Reason, int Duration = PERMANENT);
bool DelBannedHW(const std::string& HW);
bool CheckBannedHW(const std::string& HW);
int FindBanHW(const std::string& HW);
void LoadBannedListHW(const std::string& filename);
void SaveBannedListHW(const std::string& filename);
void UpdateIPBanList();
void UpdateHWBanList();

// Global variables
extern std::vector<TBanIP> BannedIPList;
extern std::vector<TBanHW> BannedHWList;
extern std::string LastBan;
extern std::string LastBanHW;

#endif // BANSYSTEM_H