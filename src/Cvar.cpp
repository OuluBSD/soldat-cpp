#include "Cvar.h"

// Define global variables
std::unordered_map<std::string, std::unique_ptr<TCvarBase>> Cvars;
std::unordered_map<std::string, std::unique_ptr<TCvarBase>> CvarsSync;
bool CvarsNeedSyncing = false;
bool CvarsInitialized = false;

void CvarInit() {
    // Initialize cvar system
    Cvars.clear();
    CvarsSync.clear();
    CvarsNeedSyncing = false;
    CvarsInitialized = false;
}

std::string DumpFlags(TCvarBase* Cvar) {
    std::string CvarFlags = "";

    if (Cvar->GetFlags().count(TCvarFlag::CVAR_IMMUTABLE)) CvarFlags += " I";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_ARCHIVE)) CvarFlags += " A";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_SPONLY)) CvarFlags += " SP";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_NOTIFY)) CvarFlags += " N";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_MODIFIED)) CvarFlags += " M";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_CLIENT)) CvarFlags += " CL";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_SERVER)) CvarFlags += " SV";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_SYNC)) CvarFlags += " SYNC";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_SCRIPT)) CvarFlags += " SC";
    if (Cvar->GetFlags().count(TCvarFlag::CVAR_INITONLY)) CvarFlags += " INITONLY";

    return CvarFlags;
}

void ResetSyncCvars() {
    for (auto& pair : CvarsSync) {
        pair.second->Reset();
    }
}