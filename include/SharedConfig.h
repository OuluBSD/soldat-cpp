#ifndef SHARED_CONFIG_H
#define SHARED_CONFIG_H

//*******************************************************************************
//                                                                              
//       SharedConfig Unit for SOLDAT                                           
//                                                                              
//       Copyright (c) 2012 Daniel Forssten             
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
#include "Sprites.h"  // Assuming this includes the TSprite structure
#include "Util.h"     // For ColorToHex and StringToColor functions
#include "Constants.h" // For various constants

// Helper functions to read configuration values
namespace SharedConfigImpl {
    inline void ReadConfColor(std::map<std::string, std::string>& conf, 
                              const std::string& SectionName, 
                              uint32_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            // Convert string color representation to hex format
            VarName = ColorToHex(StringToColor(it->second));
        } else {
            // Error handling: value not found
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadConfMagicColor(std::map<std::string, std::string>& conf, 
                                   const std::string& SectionName, 
                                   uint32_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            VarName = StringToColor(it->second);
        } else {
            // Error handling: value not found
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadConf(std::map<std::string, std::string>& conf, 
                         const std::string& SectionName, 
                         int& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = std::stoi(it->second);
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadConf(std::map<std::string, std::string>& conf, 
                         const std::string& SectionName, 
                         uint8_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = static_cast<uint8_t>(std::stoi(it->second));
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadConf(std::map<std::string, std::string>& conf, 
                         const std::string& SectionName, 
                         std::string& VarName, 
                         bool AllowBlank = false) {
        auto it = conf.find(SectionName);
        if (it != conf.end() || AllowBlank) {
            VarName = it != conf.end() ? it->second : "";
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadConf(std::map<std::string, std::string>& conf, 
                         const std::string& SectionName, 
                         bool& VarName, 
                         bool AllowBlank = false) {
        auto it = conf.find(SectionName);
        if (it != conf.end() || AllowBlank) {
            VarName = it != conf.end() ? (it->second == "1") : false;
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" not found");
        }
    }

    inline void ReadWMConf(std::map<std::string, std::string>& conf, 
                           const std::string& SectionName, 
                           uint16_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = static_cast<uint16_t>(std::stoi(it->second));
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
        }
    }

    inline void ReadWMConf(std::map<std::string, std::string>& conf, 
                           const std::string& SectionName, 
                           int16_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = static_cast<int16_t>(std::stoi(it->second));
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
        }
    }

    inline void ReadWMConf(std::map<std::string, std::string>& conf, 
                           const std::string& SectionName, 
                           uint8_t& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = static_cast<uint8_t>(std::stoi(it->second));
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
        }
    }

    inline void ReadWMConf(std::map<std::string, std::string>& conf, 
                           const std::string& SectionName, 
                           float& VarName) {
        auto it = conf.find(SectionName);
        if (it != conf.end() && !it->second.empty()) {
            try {
                VarName = std::stof(it->second);
            } catch (...) {
                throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
            }
        } else {
            throw std::runtime_error("Value \"" + SectionName + "\" is not a number");
        }
    }

#ifdef SERVER_CODE
    inline bool LoadBotConfig(const std::string& FilePath, TSprite& SpriteC) {
        // This function would read bot configuration from an INI file
        // Implementation would require proper INI file parser
        
        // Check if file exists
        std::ifstream file(FilePath);
        if (!file.is_open()) {
            // Log error: "Bot file not found"
            return false;
        }

        // Parse INI file content
        std::map<std::string, std::map<std::string, std::string>> iniData;
        std::string line, section;
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            
            // Check for section
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Parse key=value pairs
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                iniData[section][key] = value;
            }
        }
        
        file.close();
        
        // Check if 'BOT' section exists
        auto botSection = iniData.find("BOT");
        if (botSection == iniData.end()) {
            // Log error: Section "[Bot]" not found
            return false;
        }
        
        try {
            auto& botData = botSection->second;
            
            // Read bot configuration values
            std::string favWeaponName;
            if (botData.count("Favourite_Weapon")) {
                favWeaponName = botData["Favourite_Weapon"];
                // SpriteC.Brain.FavWeapon = WeaponNameToNum(favWeaponName);
            }
            
            if (botData.count("Secondary_Weapon")) {
                // ReadConf(botData, "Secondary_Weapon", SpriteC.Player.SecWep);
                uint8_t secWep;
                ReadConf(botData, "Secondary_Weapon", secWep);
                SpriteC.Player.SecWep = secWep;
            }
            
            // Additional bot configuration loading would go here...
            // For brevity, not all fields are implemented
            
            if (botData.count("Accuracy")) {
                int accuracy;
                ReadConf(botData, "Accuracy", accuracy);
                // Apply difficulty adjustment
                accuracy = static_cast<int>(accuracy * (bots_difficulty.Value() / 100.0f));
                SpriteC.Brain.Accuracy = accuracy;
            }
            
            if (botData.count("Name")) {
                std::string name;
                ReadConf(botData, "Name", name);
                SpriteC.Player.Name = name;
                // Truncate to max player name length
                if (SpriteC.Player.Name.length() > PLAYERNAME_CHARS) {
                    SpriteC.Player.Name.resize(PLAYERNAME_CHARS);
                }
            }
            
            // Set bot control method
            SpriteC.Player.ControlMethod = BOT;
            // SpriteC.FreeControls(); // This would need to be implemented
            
            return true;
        } catch (const std::exception& e) {
            // Log error message
            // Extract filename from path
            size_t lastSlash = FilePath.find_last_of("/\\");
            std::string fileName = (lastSlash != std::string::npos) ? 
                                  FilePath.substr(lastSlash + 1) : FilePath;
            // MainConsole.console(fileName + ": " + e.what(), WARNING_MESSAGE_COLOR);
            return false;
        }
    }
#endif

    inline bool LoadWeaponsConfig(const std::string& FilePath) {
        // Check if file exists
        std::ifstream file(FilePath);
        if (!file.is_open()) {
            return false;
        }

        // Parse INI file content
        std::map<std::string, std::map<std::string, std::string>> iniData;
        std::string line, section;
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            
            // Check for section
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.length() - 2);
                continue;
            }
            
            // Parse key=value pairs
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                iniData[section][key] = value;
            }
        }
        
        file.close();
        
        // Check if 'Info' section exists
        auto infoSection = iniData.find("Info");
        if (infoSection == iniData.end()) {
            // Log error: Section "[Info]" not found
            return false;
        }
        
        try {
            auto& infoData = infoSection->second;
            
            // Read weapon mod info
            if (infoData.count("Name")) {
                ReadConf(infoData, "Name", WMName);
            }
            
            if (infoData.count("Version")) {
                ReadConf(infoData, "Version", WMVersion);
            }
            
            // Iterate through weapons and update their properties
            // This would need to access the global Guns array
            // for (int weaponIndex = 1; weaponIndex <= ORIGINAL_WEAPONS; ++weaponIndex) {
            //     TGun* gun = &Guns[weaponIndex];
            //     
            //     auto weaponSection = iniData.find(gun->IniName);
            //     if (weaponSection != iniData.end()) {
            //         auto& weaponData = weaponSection->second;
            //         
            //         if (weaponData.count("Damage")) {
            //             ReadWMConf(weaponData, "Damage", gun->HitMultiply);
            //         }
            //         // Additional weapon property updates would go here...
            //     }
            // }
            
            // BuildWeapons(); // This would need to be implemented
            
            return true;
        } catch (const std::exception& e) {
            // Log error message
            // Extract filename from path
            size_t lastSlash = FilePath.find_last_of("/\\");
            std::string fileName = (lastSlash != std::string::npos) ? 
                                  FilePath.substr(lastSlash + 1) : FilePath;
            // MainConsole.console(fileName + ": " + e.what(), WARNING_MESSAGE_COLOR);
            return false;
        }
    }
}

// Using declarations to bring into global namespace
#ifdef SERVER_CODE
using SharedConfigImpl::LoadBotConfig;
#endif
using SharedConfigImpl::LoadWeaponsConfig;
using SharedConfigImpl::ReadConfColor;
using SharedConfigImpl::ReadConfMagicColor;
using SharedConfigImpl::ReadConf;
using SharedConfigImpl::ReadWMConf;

#endif // SHARED_CONFIG_H