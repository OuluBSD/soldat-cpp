//*******************************************************************************
//                                                                              
//       FileClient Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2016 Paweł Drzazga               
//                                                                              
//*******************************************************************************

#include "FileClient.h"
#include "Client.h"
#include "Util.h"
#include <thread>
#include <chrono>

// Global variable
uint8_t DownloadRetry = 0;

void TDownloadThread::SetStatus() {
    // This function would update the game UI with download progress
    std::string fileName = ExtractFileName(FFilename);
    std::string progressStr = std::to_string(FProgress) + "%";
    std::string posStr = std::to_string(FDownloadPos);
    std::string sizeStr = std::to_string(FDownloadSize);
    
    std::string message = "Downloading " + fileName + " - " + progressStr + 
                          " (" + posStr + "/" + sizeStr + ")";
    // RenderGameInfo(WideString(message));  // Assuming function exists elsewhere
}

void TDownloadThread::SetError() {
    std::string errorMsg = "Download error: " + FErrorMsg;
    // ShowMessage(WideString(errorMsg));  // Assuming function exists elsewhere
    // MainConsole.Console(errorMsg, DEBUG_MESSAGE_COLOR);  // Assuming function exists elsewhere
}

void TDownloadThread::Execute() {
    // This function would handle the actual download
    // It would use an HTTP client to download the file and check checksum
    try {
        // Check if file exceeds maximum download size
        // This would be done after getting the content length from the HTTP header
        if (FDownloadSize > MAX_DL_SIZE) {
            FStatus = 3;  // File too large error
            return;
        }

        // Perform download operation
        // The actual download code would go here using an HTTP library like libcurl
        // This is a simplified representation
        
        // After download completes, verify checksum
        // This is a simplified checksum verification
        // if (!Sha1Match(Sha1File(FFilename, 4096), FChecksum)) {
        //     throw std::runtime_error("Checksum mismatch");
        // }

        // If everything succeeds
        FStatus = 1;
    } catch (const std::exception& e) {
        // Delete the file since download failed
        std::remove(FFilename.c_str());
        FErrorMsg = e.what();
        SetError();  // Call error handling
    }
    
    // Call the termination handler
    DoOnTerminate(nullptr);
}

void TDownloadThread::DoOnTerminate(void* Sender) {
    // Terminate the download worker
    IsTerminated = true;
    
    DownloadRetry++;
    if (DownloadRetry == 1) {
        if (FStatus == 1) {  // Successful download
            // JoinServer();  // Assuming this function exists elsewhere
        }
    }
    
    // In a real implementation this would handle cleanup and deletion of this object
    // For now, we'll just set the global DownloadThread to null
}

TDownloadThread::TDownloadThread(const std::string& DownloadURL, 
                              const std::string& Name, 
                              const TSHA1Digest& Checksum) 
    : FURL(DownloadURL), FFilename(Name), FChecksum(Checksum), 
      FStatus(0), FProgress(0), FDownloadPos(0), FDownloadSize(0),
      IsTerminated(false) {
    // Sanitize filename by removing dangerous sequences like ".."
    size_t pos = 0;
    while ((pos = FFilename.find("..", pos)) != std::string::npos) {
        FFilename.replace(pos, 2, "");
    }

    // Start the download worker thread
    DownloadWorker = std::thread(&TDownloadThread::Execute, this);
}

TDownloadThread::~TDownloadThread() {
    if (!IsTerminated.load()) {
        IsTerminated = true;
    }
    if (DownloadWorker.joinable()) {
        DownloadWorker.join();
    }
}

void TDownloadThread::CancelDownload() {
    IsTerminated = true;
}

void TDownloadThread::DoProgress(void* Sender, const int64_t ContentLength, const int64_t CurrentPos) {
    if ((ContentLength > MAX_DL_SIZE) || (CurrentPos > MAX_DL_SIZE)) {
        FStatus = 3;  // Indicate file too large
        IsTerminated = true;
        return;
    }
    
    if (ContentLength > 0) {
        uint8_t oldProgress = FProgress;
        FProgress = static_cast<uint8_t>(std::round((static_cast<double>(CurrentPos) / ContentLength) * 100));
        FDownloadSize = ContentLength;
        FDownloadPos = CurrentPos;
        
        if (FProgress != oldProgress) {
            // In a real implementation, we'd synchronize to main thread to call SetStatus
            // For now, just call directly
            SetStatus();
        }
    }
}