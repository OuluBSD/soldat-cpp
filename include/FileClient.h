#ifndef FILECLIENT_H
#define FILECLIENT_H

//*******************************************************************************
//                                                                              
//       FileClient Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2016 Paweł Drzazga               
//                                                                              
//*******************************************************************************

#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "Sha1.h"
#include "Constants.h"
#include "Util.h"

// Constants
const int MAX_DL_SIZE = 150000000; // max download size in bytes

// Forward declarations
struct TSHA1Digest;

// Class definition for download thread
class TDownloadThread {
private:
    std::string FURL;
    std::string FFilename;
    TSHA1Digest FChecksum;
    std::string FErrorMsg;
    uint8_t FStatus;
    uint8_t FProgress;
    int64_t FDownloadPos;
    int64_t FDownloadSize;
    std::thread DownloadWorker;
    std::atomic<bool> IsTerminated;

public:
    TDownloadThread(const std::string& DownloadURL, const std::string& Name, const TSHA1Digest& Checksum);
    ~TDownloadThread();
    
    void Execute();
    void DoProgress(void* Sender, const int64_t ContentLength, const int64_t CurrentPos);
    void DoOnTerminate(void* Sender);
    void CancelDownload();
    uint8_t GetStatus() const { return FStatus; }
    uint8_t GetProgress() const { return FProgress; }
    int64_t GetDownloadPos() const { return FDownloadPos; }
    int64_t GetDownloadSize() const { return FDownloadSize; }
    bool GetTerminated() const { return IsTerminated.load(); }
};

// Global variables
extern uint8_t DownloadRetry;

#endif // FILECLIENT_H