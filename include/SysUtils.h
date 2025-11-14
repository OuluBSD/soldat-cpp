#ifndef SYSUTILS_H
#define SYSUTILS_H

//*******************************************************************************
//
//       SysUtils Unit for SOLDAT (Pascal compatibility)
//
//*******************************************************************************

#include <string>
#include <cstdint>
#include <ctime>
#include <chrono>

// DateTime to Unix timestamp conversion
inline int64_t DateTimeToUnix(double DateTime) {
    // Simplified implementation - in real Pascal this would have a more complex conversion
    // For now we'll use a simple approach based on the standard Unix epoch
    time_t currentTime = time(nullptr);
    return static_cast<int64_t>(currentTime);
}

// Unix timestamp to DateTime conversion
inline double UnixToDateTime(int64_t UnixTime) {
    // Simplified implementation
    return static_cast<double>(UnixTime);
}

// Format string function (simplified)
inline std::string Format(const std::string& FormatStr, const std::string& Arg) {
    // This is a simplified version - a full implementation would handle
    // various format specifiers like '%s', '%d', etc.
    std::string result = FormatStr;
    // Replace first occurrence of %s with Arg
    size_t pos = result.find("%s");
    if (pos != std::string::npos) {
        result.replace(pos, 2, Arg);
    }
    return result;
}

// Format string function with multiple arguments (simplified)
inline std::string Format(const std::string& FormatStr, const std::vector<std::string>& Args) {
    std::string result = FormatStr;
    for (const std::string& arg : Args) {
        size_t pos = result.find("%s");
        if (pos != std::string::npos) {
            result.replace(pos, 2, arg);
        }
    }
    return result;
}

// Now function - returns current time as Unix timestamp
inline int64_t Now() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

// Date/time functions
inline int YearOf(const int64_t DateTime) {
    time_t time = DateTime;
    struct tm *tm_ptr = localtime(&time);
    return tm_ptr ? tm_ptr->tm_year + 1900 : 0;
}

inline int MonthOf(const int64_t DateTime) {
    time_t time = DateTime;
    struct tm *tm_ptr = localtime(&time);
    return tm_ptr ? tm_ptr->tm_mon + 1 : 0;
}

inline int DayOf(const int64_t DateTime) {
    time_t time = DateTime;
    struct tm *tm_ptr = localtime(&time);
    return tm_ptr ? tm_ptr->tm_mday : 0;
}

// String utility functions
inline std::string IntToStr(int Value) {
    return std::to_string(Value);
}

inline std::string FloatToStr(float Value) {
    return std::to_string(Value);
}

inline std::string FloatToStrF(float Value, int Precision) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.*f", Precision, Value);
    return std::string(buffer);
}

inline int StrToInt(const std::string& Str) {
    return std::stoi(Str);
}

inline float StrToFloat(const std::string& Str) {
    return std::stof(Str);
}

inline std::string UpperCase(const std::string& Str) {
    std::string result = Str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}

inline std::string LowerCase(const std::string& Str) {
    std::string result = Str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

inline bool SameText(const std::string& Str1, const std::string& Str2) {
    return LowerCase(Str1) == LowerCase(Str2);
}

// File handling utilities (simplified)
inline bool FileExists(const std::string& FileName) {
    // Using C++17's filesystem library would be better here, but for now:
    FILE* file = fopen(FileName.c_str(), "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Sleep function
inline void Sleep(int Milliseconds) {
#ifdef _WIN32
    // Windows sleep
    Sleep(Milliseconds);
#else
    // Unix/Linux sleep
    struct timespec ts;
    ts.tv_sec = Milliseconds / 1000;
    ts.tv_nsec = (Milliseconds % 1000) * 1000000;
    nanosleep(&ts, nullptr);
#endif
}

// Exception handling (simplified)
class Exception {
public:
    std::string Message;
    
    Exception(const std::string& Msg) : Message(Msg) {}
};

class EAbort : public Exception {
public:
    EAbort() : Exception("EAbort exception") {}
    EAbort(const std::string& Msg) : Exception(Msg) {}
};

#endif // SYSUTILS_H