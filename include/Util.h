#ifndef UTIL_H
#define UTIL_H

//*******************************************************
//                                                      
//       Util Unit for SOLDAT                            
//                                                      
//       Copyright (c) 2003 Michal Marcinkowski          
//                                                      
//*******************************************************

#include <string>
#include <vector>
#include <cstdint>
#include <variant>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Forward declarations
#include "Sha1.h"  // Include Sha1.h to get TSHA1Digest definition

class TMapInfo;    // defined below

// Type definitions
using TColor = uint32_t; // 0..$FFFFFFFF

struct TMapInfo {
    std::string Name;
    std::string MapName;
    uint64_t WorkshopID;
    std::string Path;
};


// Implementation of the functions
namespace UtilImpl {
    inline std::string GetPiece(const std::string& Source, const std::string& Delimiter,
                                int Piece) {
        // SplitStr was removed in newer Pascal version, so implementing logic directly
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end = Source.find(Delimiter);

        int limit = Piece; // Use Piece as limit
        int index = 0;
        while (end != std::string::npos) {
            if (limit > -1 && index == limit) {
                break;
            }

            parts.push_back(Source.substr(start, end - start));
            start = end + Delimiter.length();
            end = Source.find(Delimiter, start);
            index++;
        }

        parts.push_back(Source.substr(start));

        if (Piece - 1 >= 0 && Piece - 1 < static_cast<int>(parts.size())) {
            return parts[Piece - 1];
        }
        return "";
    }

    inline std::variant<int, float, std::string> Iif(bool Condition, 
                                                     const std::variant<int, float, std::string>& TruePart,
                                                     const std::variant<int, float, std::string>& FalsePart) {
        if (Condition) {
            return TruePart;
        } else {
            return FalsePart;
        }
    }

    inline std::variant<int, float, std::string> Choose(int Index, 
                                                        const std::vector<std::variant<int, float, std::string>>& Choices) {
        if (Index < static_cast<int>(Choices.size())) {
            return Choices[Index];
        }
        return 0; // Default value
    }

    // Helper function to find substring starting from a position
    inline size_t PosEx(const std::string& SubStr, const std::string& S, size_t Offset = 1) {
        if (Offset <= S.length()) {
            size_t pos = S.find(SubStr, Offset - 1);
            if (pos != std::string::npos) {
                return pos + 1; // Return 1-based position
            }
        }
        return 0; // Not found
    }

    inline uint32_t ColorToHex(TColor Color) {
        // Convert color from BGR to ABGR format
        uint32_t temp = Color;
        std::stringstream ss;
        ss << std::hex << std::setw(6) << std::setfill('0') << temp;
        std::string temp2 = ss.str();
        
        // Rearrange from BGR to ABGR (add alpha FF and reorder)
        std::string rearranged = "FF" + temp2.substr(4, 2) + temp2.substr(2, 2) + temp2.substr(0, 2);
        
        uint32_t result;
        std::stringstream(rearranged) >> std::hex >> result;
        return result;
    }

    inline TColor StringToColor(const std::string& S) {
        uint32_t result;
        std::stringstream ss(S);
        ss >> std::hex >> result;
        return static_cast<TColor>(result);
    }

#ifndef SERVER_CODE
    inline std::string NumberFormat(uint32_t Num) {
        std::stringstream ss;
        ss.imbue(std::locale(""));
        ss << std::fixed << Num;
        return ss.str();
    }

    // Note: CheckFileSize requires file system operations that would be implemented based on platform
    // This is a simplified version
    inline int CheckFileSize(const std::string& filename) {
        (void)filename; // Suppress unused parameter warning
        // Implementation would require file system access
        return 0; // Placeholder
    }
#endif

    inline std::string OverrideFileExt(const std::string& Filename, const std::string& Ext) {
        std::string result = Filename;
        // This is a simplified version - would need proper path manipulation
        size_t lastdot = result.find_last_of(".");
        if (lastdot != std::string::npos) {
            std::string currentExt = result.substr(lastdot);
            if (currentExt != Ext) {
                result = result.substr(0, lastdot) + Ext;
            }
        }
        return result;
    }

    // Placeholder for MD5 functionality - implementation would depend on MD5 library
    inline std::string Md5StringHelper(std::string Text) {
        (void)Text; // Suppress unused parameter warning
        return ""; // Placeholder
    }

    // Placeholder for directory creation - would use platform-specific APIs
    inline bool CreateDirIfMissing(const std::string& Dir) {
        (void)Dir; // Suppress unused parameter warning
        return true; // Placeholder
    }

    // Placeholder for file creation - would use platform-specific APIs
    inline bool CreateFileIfMissing(const std::string& Filename) {
        (void)Filename; // Suppress unused parameter warning
        return true; // Placeholder
    }

    inline std::string GetSize(int64_t Bytes) {
        int64_t fileSize = Bytes / 1024;

        if (fileSize < 0.5) {
            return std::to_string(Bytes) + " B";
        } else if (fileSize > 1024) {
            fileSize = fileSize / 1024;

            std::string result = std::to_string(fileSize) + " Mb";
            if (fileSize > 1024) {
                result = std::to_string(fileSize / 1024) + " Gb";
            }
            return result;
        } else {
            return std::to_string(fileSize) + " Kb";
        }
    }

    // Placeholder implementations for map functions
    inline bool VerifyMapChecksum(TMapInfo Map, TSHA1Digest Checksum) {
        (void)Map;        // Suppress unused parameter warning
        (void)Checksum;   // Suppress unused parameter warning
        return false; // Placeholder
    }

    inline TSHA1Digest GetMapChecksum(TMapInfo Map) {
        (void)Map; // Suppress unused parameter warning
        // Placeholder - would return a default SHA1 digest
        return TSHA1Digest(); // Assuming default constructor exists
    }

    inline bool GetMapInfo(std::string MapName, std::string Directory, TMapInfo& MapInfo) {
        (void)MapName;    // Suppress unused parameter warning
        (void)Directory;  // Suppress unused parameter warning
        (void)MapInfo;    // Suppress unused parameter warning
        // Placeholder implementation
        return false;
    }
}

// Using declarations to bring the functions into the global scope
using UtilImpl::GetPiece;
using UtilImpl::Iif;
using UtilImpl::Choose;
using UtilImpl::ColorToHex;
using UtilImpl::StringToColor;
using UtilImpl::PosEx;
using UtilImpl::NumberFormat;
using UtilImpl::CheckFileSize;
using UtilImpl::OverrideFileExt;
using UtilImpl::Md5StringHelper;
using UtilImpl::CreateDirIfMissing;
using UtilImpl::CreateFileIfMissing;
using UtilImpl::GetSize;
using UtilImpl::GetMapChecksum;
using UtilImpl::GetMapInfo;
using UtilImpl::VerifyMapChecksum;

#endif // UTIL_H