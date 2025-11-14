#ifndef TRACELOG_H
#define TRACELOG_H

//*******************************************************************************
//                                                                              
//       TraceLog Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <string>
#include <iostream>
#include <cstdio>

// Conditional compilation for server/client differences
#ifdef SERVER_CODE
    #include "Server.h"
#else
    #include "Client.h"
#endif

#include "Cvar.h"  // For log_level Cvar

// Constants for logging levels
const int LEVEL_OFF = 0;
const int LEVEL_DEBUG = 1;
const int LEVEL_TRACE = 2;

// Function declarations
void Debug(const std::string& Msg);
void Trace(const std::string& Msg);
#ifdef STEAM_CODE
extern "C" void SteamWarning(int Severity, const char* WarnMessage);
#endif

namespace TraceLogImpl {
    inline void Debug(const std::string& Msg) {
        // Check if logging level allows debug messages
        if (log_level.Value() >= LEVEL_DEBUG) {
            std::cout << Msg << std::endl;
        }
    }

    inline void Trace(const std::string& Msg) {
        // Check if logging level allows trace messages
        if (log_level.Value() >= LEVEL_TRACE) {
            std::cout << Msg << std::endl;
        }
    }

#ifdef STEAM_CODE
    extern "C" inline void SteamWarning(int Severity, const char* WarnMessage) {
        std::string message = "[Steam] " + std::string(WarnMessage) + " Severity:" + std::to_string(Severity);
        std::cout << message << std::endl;
    }
#endif
}

// Using declarations to bring functions into global namespace
using TraceLogImpl::Debug;
using TraceLogImpl::Trace;
#ifdef STEAM_CODE
using TraceLogImpl::SteamWarning;
#endif

#endif // TRACELOG_H