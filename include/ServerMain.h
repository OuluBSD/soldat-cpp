#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

//*******************************************************************************
//                                                                              
//       Main Unit for SOLDAT Server                                             
//                                                                              
//       Copyright (c) 2012 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <iostream>
#include <string>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <exception>

#include "Server.h"
#include "Constants.h"
#include "ServerHelper.h"
#include "ServerLoop.h"
#include "SysUtils.h"

#ifdef __linux__
#include <sys/prctl.h>
#include <csignal>
#endif

// Function declarations
void RunServer();

namespace ServerMainImpl {
    bool CtrlCHit = false;
    
#ifdef _WIN32
    // Windows signal handling
    static BOOL WINAPI ConsoleHandlerRoutine(DWORD CtrlType);
    
    inline void SetSigHooks() {
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandlerRoutine, TRUE);
    }

    inline void ClearSigHooks() {
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandlerRoutine, FALSE);
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    }

    BOOL WINAPI ConsoleHandlerRoutine(DWORD CtrlType) {
        if (CtrlType == CTRL_C_EVENT) {
            if (!CtrlCHit) {
                ProgReady = false;
                CtrlCHit = true;
                std::cout << "Control-C hit, shutting down" << std::endl;
            } else {
                std::cout << "OK, OK, exiting immediately" << std::endl;
                exit(1);
            }
            return TRUE;
        }
        return FALSE;
    }
#else
    // Linux signal handling
    struct sigaction oldSigTerm, oldSigQuit, oldSigInt, oldSigPipe;

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

        if (Signal == SIGTERM || Signal == SIGQUIT) {
            MainConsole.Console(L"", GAME_MESSAGE_COLOR);
            MainConsole.Console(L"Signal received, shutting down", GAME_MESSAGE_COLOR);
            ProgReady = false;
        }
    }

    inline void SetSigHooks() {
        struct sigaction newAction;
        std::memset(&newAction, 0, sizeof(newAction));
        newAction.sa_handler = HandleSig;
        sigemptyset(&newAction.sa_mask);
        newAction.sa_flags = 0;

        sigaction(SIGTERM, &newAction, &oldSigTerm);
        sigaction(SIGINT, &newAction, &oldSigInt);
        sigaction(SIGQUIT, &newAction, &oldSigQuit);
        sigaction(SIGPIPE, &newAction, &oldSigPipe);
    }

    inline void ClearSigHooks() {
        sigaction(SIGTERM, &oldSigTerm, nullptr);
        sigaction(SIGINT, &oldSigInt, nullptr);
        sigaction(SIGQUIT, &oldSigQuit, nullptr);
        sigaction(SIGPIPE, &oldSigPipe, nullptr);
    }
#endif

    inline bool IsRoot() {
#ifdef _WIN32
        return false;  // Ignore for Windows users
#else
        return (geteuid() == 0);
#endif
    }

    inline void WritePID() {
        pid_t pid = getpid();
        std::ofstream pidFile("soldatserver.pid");
        if (pidFile.is_open()) {
            pidFile << pid << std::endl;
            pidFile.close();
        }
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
#ifndef DEBUG_BUILD
            try {
#endif
                ActivateServer();
                WritePID();

#ifdef SCRIPT_CODE
                if (sc_enable.Value()) {
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
                    Sleep(1);  // Windows sleep (1 ms)
#else
                    usleep(1000);  // Linux sleep (1000 microseconds = 1 ms)
#endif
                }
#ifndef DEBUG_BUILD
            } catch (const std::exception& e) {
                ProgReady = false;
                MainConsole.Console(L"Server Encountered an error:", GAME_MESSAGE_COLOR);
                MainConsole.Console(std::wstring(e.what(), e.what() + strlen(e.what())), GAME_MESSAGE_COLOR);
            }
#endif
        } catch (const std::exception& e) {
            ProgReady = false;
            MainConsole.Console(L"Server Encountered an error:", GAME_MESSAGE_COLOR);
            MainConsole.Console(std::wstring(e.what(), e.what() + strlen(e.what())), GAME_MESSAGE_COLOR);
        }

        // Cleanup in finally block equivalent
        Shutdown();
        ClearSigHooks();
    }
}

// Using declarations to bring functions into global namespace
using ServerMainImpl::RunServer;
using ServerMainImpl::CtrlCHit;
using ServerMainImpl::IsRoot;
using ServerMainImpl::WritePID;
using ServerMainImpl::SetSigHooks;
using ServerMainImpl::ClearSigHooks;
using ServerMainImpl::HandleSig;

#endif // SERVER_MAIN_H