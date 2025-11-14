#ifndef LOGFILE_H
#define LOGFILE_H

#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

// Forward declarations
class TStringList {
public:
    std::vector<std::string> strings;
    
    void Add(const std::string& s) { strings.push_back(s); }
    void Clear() { strings.clear(); }
    size_t Count() const { return strings.size(); }
    std::string Strings(size_t index) const { 
        if (index < strings.size()) return strings[index]; 
        return ""; 
    }
};

// Global variables
extern TStringList* GameLog;
extern std::string ConsoleLogFileName;
#ifdef SERVER_CODE
extern TStringList* KillLog;
extern std::string KillLogFileName;
#endif
extern std::mutex LogLock;

// Function declarations
void NewLogFile(TStringList*& F, const std::string& Name);
void WriteLogFile(TStringList*& F, const std::string& Name);
void AddLineToLogFile(TStringList*& F, const std::string& S, const std::string& Name, bool WithDate = true);
void NewLogFiles();

namespace LogFileImpl {
    inline void NewLogFile(TStringList*& F, const std::string& Name) {
        // Assuming log_enable and log_level are defined elsewhere
        // if (!log_enable.Value()) return;
        
        {
            std::lock_guard<std::mutex> lock(LogLock);
            if (F) {
                F->Clear();
            } else {
                F = new TStringList();
            }
        }
        
        // Try to create the file
        std::ofstream file(Name, std::ios::out);
        if (!file.is_open()) {
            // Log error appropriately
            std::cerr << "File logging error (N): Could not create file " << Name << std::endl;
        }
        file.close();
    }

    inline void AddLineToLogFile(TStringList*& F, const std::string& S, const std::string& Name, bool WithDate = true) {
        // Trace(S); - assuming this is a debugging function
        // if (!log_enable.Value()) return;
        // if (log_level.Value() == 0) return;
        
        if (!F) return;
        if (S.empty()) return;
        
        std::string S2 = "";
        if (WithDate) {
            // Get current date/time for logging
            time_t now = time(0);
            char buffer[20];
            strftime(buffer, 20, "%y/%m/%d %H:%M:%S", localtime(&now));
            S2 = std::string(buffer) + " ";
        }
        
        {
            std::lock_guard<std::mutex> lock(LogLock);
            F->Add(S2 + S);
        }
        
        // if (log_level.Value() > 1)
        WriteLogFile(F, Name);
    }

    inline void WriteLogFile(TStringList*& F, const std::string& Name) {
        // if (!log_enable.Value()) return;
        if (!F) return;
        
        std::ofstream file(Name, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "File logging error (W1): Could not append to file " << Name << std::endl;
            return;
        }
        
        if (F->Count() > 1000000) {
            std::cerr << "Too many log entries" << std::endl;
            return;
        }
        
        {
            std::lock_guard<std::mutex> lock(LogLock);
            for (size_t i = 0; i < F->Count(); ++i) {
                file << F->Strings(i) << std::endl;
            }
            F->Clear();
        }
        
        file.close();
    }

    inline void NewLogFiles() {
        // Create logs directory if needed
        // Assuming CreateDirIfMissing from Util.h is available
        // std::string logsDir = UserDirectory + "logs/";
        // CreateDirIfMissing(logsDir);
        
        // Get current date for log files
        time_t now = time(0);
        char buffer[11];
        strftime(buffer, 11, "%y-%m-%d", localtime(&now));
        std::string dateStr(buffer);
        
        // Create console log file name
        int j = 1;
        std::ostringstream oss;
        oss << "logs/consolelog-" << dateStr << "-" << std::setfill('0') << std::setw(2) << j << ".txt";
        ConsoleLogFileName = oss.str();
        
        // Check if file already exists and increment number if needed
        while (/*FileExists(ConsoleLogFileName)*/ false) { // Placeholder for file exists check
            j++;
            std::ostringstream oss2;
            oss2 << "logs/consolelog-" << dateStr << "-" << std::setfill('0') << std::setw(2) << j << ".txt";
            ConsoleLogFileName = oss2.str();
        }
        
        NewLogFile(GameLog, ConsoleLogFileName);
        AddLineToLogFile(GameLog, "   Console Log Started", ConsoleLogFileName);
        
#ifdef SERVER_CODE
        // Create kill log file name
        j = 1;
        std::ostringstream oss3;
        oss3 << "logs/kills/killlog-" << dateStr << "-" << std::setfill('0') << std::setw(2) << j << ".txt";
        KillLogFileName = oss3.str();
        
        // Check if file already exists and increment number if needed
        while (/*FileExists(KillLogFileName)*/ false) { // Placeholder for file exists check
            j++;
            std::ostringstream oss4;
            oss4 << "logs/kills/killlog-" << dateStr << "-" << std::setfill('0') << std::setw(2) << j << ".txt";
            KillLogFileName = oss4.str();
        }
        
        NewLogFile(KillLog, KillLogFileName);
        AddLineToLogFile(KillLog, "   Kill Log Started", KillLogFileName);
#endif
    }
}

// Using declarations to bring into global namespace
using LogFileImpl::TStringList;
using LogFileImpl::NewLogFile;
using LogFileImpl::WriteLogFile;
using LogFileImpl::AddLineToLogFile;
using LogFileImpl::NewLogFiles;

// Global variables
extern TStringList* GameLog = nullptr;
extern std::string ConsoleLogFileName;
#ifdef SERVER_CODE
extern TStringList* KillLog = nullptr;
extern std::string KillLogFileName;
#endif
extern std::mutex LogLock;

#endif // LOGFILE_H