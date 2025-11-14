#include "BanSystem.h"
#include "Server.h"  // For MainConsole and other server functions
#include "Util.h"    // For utility functions

// Global variables
std::vector<TBanIP> BannedIPList;
std::vector<TBanHW> BannedHWList;
std::string LastBan;
std::string LastBanHW;

namespace BanSystemImpl {
    inline void AddBannedIP(const std::string& IP, const std::string& Reason, int Duration) {
        int findex = 0;

        if (CheckBannedIP(IP)) {
            return;
        }

        // Look for empty slot in the existing list
        for (size_t i = 1; i < BannedIPList.size(); i++) {  // Pascal convention starts from 1
            if (BannedIPList[i].IP.empty()) {
                findex = static_cast<int>(i);
                break;
            }
        }

        if (findex == 0) {
            // No empty slots, add a new entry
            BannedIPList.emplace_back(IP, Duration, Reason);
        } else {
            // Use existing empty slot
            BannedIPList[findex].IP = IP;
            BannedIPList[findex].Time = Duration;
            BannedIPList[findex].Reason = Reason;
        }

        LastBan = IP;
    }

    inline bool DelBannedIP(const std::string& IP) {
        bool result = false;

        for (size_t i = 1; i < BannedIPList.size(); i++) {  // Pascal convention starts from 1
            if ((BannedIPList[i].IP == IP) && (!IP.empty())) {
                BannedIPList[i].IP = "";
                BannedIPList[i].Time = PERMANENT;  // Using PERMANENT as placeholder for "unlimited" ban
                BannedIPList[i].Reason = "";
                result = true;
            }
        }

        return result;
    }

    inline bool CheckBannedIP(const std::string& IP) {
        bool result = false;

        for (size_t i = 1; i < BannedIPList.size(); i++) {  // Pascal convention starts from 1
            // MatchesMask equivalent would check if IP matches the ban entry (could include wildcards)
            // For now, using simple equality check
            if (BannedIPList[i].IP == IP) {
                result = true;
                break;
            }
        }

        return result;
    }

    inline int FindBan(const std::string& IP) {
        int result = -1;

        for (size_t i = 1; i < BannedIPList.size(); i++) {  // Pascal convention starts from 1
            // MatchesMask equivalent would check if IP matches the ban entry (could include wildcards)
            // For now, using simple equality check
            if (BannedIPList[i].IP == IP) {
                result = static_cast<int>(i);
                break;
            }
        }

        return result;
    }

    inline void LoadBannedList(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        BannedIPList.clear();
        BannedIPList.emplace_back();  // Add dummy element at index 0 to match Pascal indexing (starting from 1)

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Split line by colon (format: IP:Time:Reason)
            size_t firstColon = line.find(':');
            size_t secondColon = line.find(':', firstColon + 1);

            if (firstColon == std::string::npos || secondColon == std::string::npos) {
                continue;
            }

            std::string banIP = line.substr(0, firstColon);
            std::string timeStr = line.substr(firstColon + 1, secondColon - firstColon - 1);
            std::string reason = line.substr(secondColon + 1);

            int timeValue;
            try {
                timeValue = std::stoi(timeStr);
            } catch (...) {
                continue;  // Skip invalid time values
            }

            // Add entry to the list
            BannedIPList.emplace_back(banIP, timeValue, reason);
        }

        file.close();
    }

    inline void SaveBannedList(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        std::string buff = "";
        for (size_t i = 1; i < BannedIPList.size(); i++) {  // Start from 1 (Pascal indexing)
            if (!BannedIPList[i].IP.empty()) {
                buff += BannedIPList[i].IP + ":" + 
                       std::to_string(BannedIPList[i].Time) + ":" + 
                       BannedIPList[i].Reason + "\n";
            }
        }

        file << buff;
        file.close();
    }

    inline void UpdateIPBanList() {
        for (size_t j = 1; j < BannedIPList.size(); j++) {  // Start from 1 (Pascal indexing)
            if (!BannedIPList[j].IP.empty()) {
                if (BannedIPList[j].Time != PERMANENT && BannedIPList[j].Time > 0) {
                    BannedIPList[j].Time -= 3600; // Decrease by 1 hour (3600 seconds)
                    if (BannedIPList[j].Time < 0) {
                        BannedIPList[j].Time = 0;
                    }
                }

                if (BannedIPList[j].Time == 0 && BannedIPList[j].Time != PERMANENT) {
                    // MainConsole.Console("IP number " + BannedIPList[j].IP +
                    //                   " (" + BannedIPList[j].Reason + ") unbanned",
                    //                   CLIENT_MESSAGE_COLOR);
                    DelBannedIP(BannedIPList[j].IP);
                    // SaveTxtLists(); // Assuming this function exists elsewhere
                }
            }
        }
    }

    inline void AddBannedHW(const std::string& HW, const std::string& Reason, int Duration) {
        int findex = 0;

        if (CheckBannedHW(HW)) {
            return;
        }

        // Look for empty slot in the existing list
        for (size_t i = 1; i < BannedHWList.size(); i++) {  // Pascal convention starts from 1
            if (BannedHWList[i].HW.empty()) {
                findex = static_cast<int>(i);
                break;
            }
        }

        if (findex == 0) {
            // No empty slots, add a new entry
            BannedHWList.emplace_back(HW, Duration, Reason);
        } else {
            // Use existing empty slot
            BannedHWList[findex].HW = HW;
            BannedHWList[findex].Time = Duration;
            BannedHWList[findex].Reason = Reason;
        }

        LastBanHW = HW;
    }

    inline bool DelBannedHW(const std::string& HW) {
        bool result = false;

        for (size_t i = 1; i < BannedHWList.size(); i++) {  // Pascal convention starts from 1
            if ((BannedHWList[i].HW == HW) && (!HW.empty())) {
                BannedHWList[i].HW = "";
                BannedHWList[i].Time = PERMANENT;  // Using PERMANENT as placeholder for "unlimited" ban
                BannedHWList[i].Reason = "";
                result = true;
            }
        }

        return result;
    }

    inline bool CheckBannedHW(const std::string& HW) {
        bool result = false;

        for (size_t i = 1; i < BannedHWList.size(); i++) {  // Pascal convention starts from 1
            // MatchesMask equivalent would check if HW matches the ban entry (could include wildcards)
            // For now, using simple equality check
            if (BannedHWList[i].HW == HW) {
                result = true;
                break;
            }
        }

        return result;
    }

    inline int FindBanHW(const std::string& HW) {
        int result = -1;

        for (size_t i = 1; i < BannedHWList.size(); i++) {  // Pascal convention starts from 1
            // MatchesMask equivalent would check if HW matches the ban entry (could include wildcards)
            // For now, using simple equality check
            if (BannedHWList[i].HW == HW) {
                result = static_cast<int>(i);
                break;
            }
        }

        return result;
    }

    inline void LoadBannedListHW(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        BannedHWList.clear();
        BannedHWList.emplace_back();  // Add dummy element at index 0 to match Pascal indexing (starting from 1)

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Split line by colon (format: HW:Time:Reason)
            size_t firstColon = line.find(':');
            size_t secondColon = line.find(':', firstColon + 1);

            if (firstColon == std::string::npos || secondColon == std::string::npos) {
                continue;
            }

            std::string banHW = line.substr(0, firstColon);
            std::string timeStr = line.substr(firstColon + 1, secondColon - firstColon - 1);
            std::string reason = line.substr(secondColon + 1);

            int timeValue;
            try {
                timeValue = std::stoi(timeStr);
            } catch (...) {
                continue;  // Skip invalid time values
            }

            // Add entry to the list
            BannedHWList.emplace_back(banHW, timeValue, reason);
        }

        file.close();
    }

    inline void SaveBannedListHW(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        std::string buff = "";
        for (size_t i = 1; i < BannedHWList.size(); i++) {  // Start from 1 (Pascal indexing)
            if (!BannedHWList[i].HW.empty()) {
                buff += BannedHWList[i].HW + ":" + 
                       std::to_string(BannedHWList[i].Time) + ":" + 
                       BannedHWList[i].Reason + "\n";
            }
        }

        file << buff;
        file.close();
    }

    inline void UpdateHWBanList() {
        for (size_t j = 1; j < BannedHWList.size(); j++) {  // Start from 1 (Pascal indexing)
            if (!BannedHWList[j].HW.empty()) {
                if (BannedHWList[j].Time != PERMANENT && BannedHWList[j].Time > 0) {
                    BannedHWList[j].Time -= 3600;  // Decrease by 1 hour (3600 seconds)
                    if (BannedHWList[j].Time < 0) {
                        BannedHWList[j].Time = 0;
                    }
                }

                if (BannedHWList[j].Time == 0 && BannedHWList[j].Time != PERMANENT) {
                    // MainConsole.Console("Hardware ID " + BannedHWList[j].HW +
                    //                   " (" + BannedHWList[j].Reason + ") unbanned",
                    //                   CLIENT_MESSAGE_COLOR);
                    DelBannedHW(BannedHWList[j].HW);
                    // SaveTxtLists(); // Assuming this function exists elsewhere
                }
            }
        }
    }
}

// Using declarations to bring into global namespace
using BanSystemImpl::TBanIP;
using BanSystemImpl::TBanHW;
using BanSystemImpl::AddBannedIP;
using BanSystemImpl::DelBannedIP;
using BanSystemImpl::CheckBannedIP;
using BanSystemImpl::FindBan;
using BanSystemImpl::LoadBannedList;
using BanSystemImpl::SaveBannedList;
using BanSystemImpl::AddBannedHW;
using BanSystemImpl::DelBannedHW;
using BanSystemImpl::CheckBannedHW;
using BanSystemImpl::FindBanHW;
using BanSystemImpl::LoadBannedListHW;
using BanSystemImpl::SaveBannedListHW;
using BanSystemImpl::UpdateIPBanList;
using BanSystemImpl::UpdateHWBanList;