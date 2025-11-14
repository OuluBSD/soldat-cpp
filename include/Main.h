#ifndef MAIN_H
#define MAIN_H

//*******************************************************************************
//                                                                              
//       Main Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2012 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Server.h"
#include "Constants.h"
#include "ServerHelper.h"
#include "ServerLoop.h"
#include "SysUtils.h"
#include <csignal>
#include <cstdlib>
#include <iostream>

// Function declaration
void RunServer();

namespace MainImpl {

    // Internal variables
    extern bool CtrlCHit;

    // Cross-platform signal handling
    #ifdef _WIN32
    #include <windows.h>

    // The windows server needs a hook to make soldatserver exit normally
    inline BOOL ConsoleHandlerRoutine(DWORD CtrlType) {
        if (CtrlType == CTRL_C_EVENT) {
            if (!CtrlCHit) {
                ProgReady = false;
                CtrlCHit = true;
                std::cout << "Control-C hit, shutting down" << std::endl;
                return TRUE;
            } else {
                std::cout << "OK, OK, exiting immediately" << std::endl;
                exit(1);
            }
        }
        return FALSE;
    }

    inline void SetSigHooks() {
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandlerRoutine, TRUE);
    }

    inline void ClearSigHooks() {
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandlerRoutine, FALSE);
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    }
    #else
    // Linux/Unix signal handling
    inline void HandleSig(int Signal) {
        if (Signal == SIGINT) {
            std::cout << std::endl;
            if (!CtrlCHit) {
                std::cout << "Control-C hit, shutting down" << std::endl;
                CtrlCHit = true;
                ProgReady = false;
            } else {
                std::cout << "OK, OK, exiting immediately" << std::endl;
                exit(1);
            }
        }

        if ((Signal == SIGTERM) || (Signal == SIGQUIT)) {
            MainConsole.Console("", GAME_MESSAGE_COLOR);
            MainConsole.Console("Signal received, shutting down", GAME_MESSAGE_COLOR);
            ProgReady = false;
        }
    }

    inline void SetSigHooks() {
        signal(SIGTERM, HandleSig);
        signal(SIGINT, HandleSig);
        signal(SIGQUIT, HandleSig);
        signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE
    }

    inline void ClearSigHooks() {
        signal(SIGTERM, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGPIPE, SIG_DFL);
    }
    #endif

    inline bool IsRoot() {
    #ifdef _WIN32
        return false;  // Ignore for Windows users
    #else
        return (geteuid() == 0);
    #endif
    }

    inline void RunServer() {
        if (IsRoot()) {
            std::cout << "You are running soldatserver as root! Don't do that! " <<
                         "There are not many valid" << std::endl <<
                         "reasons for this and it can, in theory, cause great damage!" << std::endl;
            return;
        }

        SetSigHooks();

        try {
            ActivateServer();
            WritePID();

            #ifdef SCRIPT
            if (sc_enable.Value) {
                ScrptDispatcher.Prepare();
            }
            #endif

            std::cout << 
                "----------------------------------------------------------------" << std::endl;

            if (ProgReady) {
                StartServer();
            }
            
            while (ProgReady) {
                AppOnIdle();
                #ifdef _WIN32
                Sleep(1);
                #else
                usleep(1000); // 1ms in microseconds
                #endif
            }
        }
        catch (const std::exception& e) {
            ProgReady = false;
            MainConsole.Console("Server Encountered an error:", GAME_MESSAGE_COLOR);
            MainConsole.Console(std::string(e.what()), GAME_MESSAGE_COLOR);
        }
        catch (...) {
            ProgReady = false;
            MainConsole.Console("Server Encountered an unknown error:", GAME_MESSAGE_COLOR);
        }

        // Any needed cleanup code here
        ShutDown();
        ClearSigHooks();
    }

} // namespace MainImpl

using MainImpl::RunServer;

#endif // MAIN_H