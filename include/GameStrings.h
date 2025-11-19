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

// Basic TMOFile class definition to avoid incomplete type issues
class TMOFile {
public:
    TMOFile() = default;
    virtual ~TMOFile() = default;
    
    // Prevent copying
    TMOFile(const TMOFile&) = delete;
    TMOFile& operator=(const TMOFile&) = delete;
    
    // Allow moving
    TMOFile(TMOFile&&) = default;
    TMOFile& operator=(TMOFile&&) = default;
    
    // Add any required methods here as needed
};

// Global variables
extern std::unique_ptr<TMOFile> TranslationFile;



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

#ifndef SOLDAT_GETTEXT_DEFINED
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
#endif  // SOLDAT_GETTEXT_DEFINED

    inline void DeInitTranslation() {
        if (TranslationFile) {
            TranslationFile.reset(); // This will call destructor
        }
    }
}

// Global variable definition
extern std::unique_ptr<TMOFile> TranslationFile;

// Using declarations to bring into global namespace
using GameStringsImpl::InitTranslation;
#ifndef SOLDAT_GETTEXT_DEFINED
using GameStringsImpl::_;
#endif
using GameStringsImpl::DeInitTranslation;

// Define the global variable (only once, not in multiple header inclusions)
#ifndef GAMESTRINGS_IMPL
#define GAMESTRINGS_IMPL
std::unique_ptr<TMOFile> TranslationFile = nullptr;
#endif

#endif // GAMESTRINGS_H