#ifndef SHA1_H
#define SHA1_H

//*******************************************************************************
//
//       SHA1 Unit for SOLDAT
//
//*******************************************************************************

#include <string>
#include <cstdint>
#include <array>

// Define SHA1 digest type
struct TSHA1Digest {
    uint8_t data[20];  // 20 bytes for SHA1
    
    TSHA1Digest() {
        std::fill(data, data + 20, 0);
    }
    
    bool operator==(const TSHA1Digest& other) const {
        return std::equal(data, data + 20, other.data);
    }
    
    bool operator!=(const TSHA1Digest& other) const {
        return !(*this == other);
    }
};

// Function declarations for SHA1 operations
TSHA1Digest SHA1String(const std::string& Str);
TSHA1Digest SHA1File(const std::string& FileName);
std::string SHA1DigestToString(const TSHA1Digest& Digest);
bool CompareSHA1Digests(const TSHA1Digest& Digest1, const TSHA1Digest& Digest2);
TSHA1Digest EmptySHA1Digest();

// Utility function for hashing strings
inline TSHA1Digest SHA1String(const std::string& Str) {
    // Placeholder: In a real implementation, this would calculate the proper SHA1
    // For now, returning a zero-filled array to allow compilation
    TSHA1Digest result = {};
    return result;
}

// Utility function for hashing files
inline TSHA1Digest SHA1File(const std::string& FileName) {
    // Placeholder: In a real implementation, this would calculate SHA1 of a file
    TSHA1Digest result = {};
    return result;
}

// Convert SHA1 digest to string representation
inline std::string SHA1DigestToString(const TSHA1Digest& Digest) {
    // Convert to hex string representation
    std::string result;
    result.reserve(40); // 20 bytes * 2 chars per byte
    
    for (uint8_t byte : Digest) {
        char buf[3];
        sprintf(buf, "%02x", byte);
        result += buf;
    }
    
    return result;
}

// Compare two SHA1 digests
inline bool CompareSHA1Digests(const TSHA1Digest& Digest1, const TSHA1Digest& Digest2) {
    return Digest1 == Digest2;
}

// Return an empty SHA1 digest
inline TSHA1Digest EmptySHA1Digest() {
    TSHA1Digest result = {};
    return result;
}

#endif // SHA1_H