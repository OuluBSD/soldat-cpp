#ifndef GAMESTRINGS_H
#define GAMESTRINGS_H

//*******************************************************************************
//                                                                              
//       GameStrings Unit for SOLDAT                                             
//                                                                              
//       Copyright (c) 2003 Michal Marcinkowski          
//                                                                              
//*******************************************************************************

#include <string>
#include <memory>
#include "GetText.h"  // Assuming GetText library functionality

// Forward declarations
class TMOFile;

// Global variables
extern std::unique_ptr<TMOFile> TranslationFile;

// Function declarations
bool InitTranslation(const std::string& Filename);
std::wstring _(const std::wstring& InputText); // overload
std::wstring _(const std::string& InputText);  // overload
void DeInitTranslation();

namespace GameStringsImpl {
    inline bool InitTranslation(const std::string& Filename) {
        // Note: This function would require PhysFS for file access and GetText functionality for translations
        // Creating a stream from the physical file system
        // TStream* translationStream = PHYSFS_readAsStream(Filename.c_str());
        // if (!translationStream) {
        //     return false;
        // }
        
        // try {
        //     TranslationFile = std::make_unique<TMOFile>(translationStream);
        //     delete translationStream; // Free the stream after creating TMOFile
        //     return true;
        // } catch (...) {
        //     delete translationStream;
        //     return false;
        // }
        
        // For now, returning true as a placeholder
        return true;
    }

    inline std::wstring _(const std::wstring& InputText) {
        std::wstring translation = InputText;
        
        if (TranslationFile) {
            // Get translation from MO file
            // std::string translated = TranslationFile->Translate(std::string(InputText.begin(), InputText.end()));
            // if (!translated.empty()) {
            //     translation = std::wstring(translated.begin(), translated.end());
            // }
        }
        
        return translation;
    }

    inline std::wstring _(const std::string& InputText) {
        std::wstring translation = std::wstring(InputText.begin(), InputText.end());
        
        if (TranslationFile) {
            // Get translation from MO file
            // std::string translated = TranslationFile->Translate(InputText);
            // if (!translated.empty()) {
            //     translation = std::wstring(translated.begin(), translated.end());
            // }
        }
        
        return translation;
    }

    inline void DeInitTranslation() {
        if (TranslationFile) {
            TranslationFile.reset(); // This will call destructor
        }
    }
}

// Using declarations to bring into global namespace
using GameStringsImpl::TranslationFile;
using GameStringsImpl::InitTranslation;
using GameStringsImpl::_;
using GameStringsImpl::DeInitTranslation;

// Global variable definitions
extern std::unique_ptr<TMOFile> TranslationFile = nullptr;

#endif // GAMESTRINGS_H