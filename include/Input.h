#ifndef INPUT_H
#define INPUT_H

//*******************************************************************************
//                                                                              
//       Input Unit for SOLDAT                                                   
//                                                                              
//       Copyright (c) 2016 Paweł Drzazga               
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>
#include "Constants.h"
#include "Weapons.h"

// Enum for actions
enum class TAction {
    None = 0,
    Left,
    Right,
    Jump,
    Crouch,
    Fire,
    Jet,
    Reload,
    ChangeWeapon,
    VoiceChat,
    ThrowGrenade,
    DropWeapon,
    Prone,
    FlagThrow,
    StatsMenu,
    GameStats,
    MiniMap,
    PlayerName,
    FragsList,
    SniperLine,
    Radio,
    RecordDemo,
    VolumeUp,
    VolumeDown,
    MouseSensitivityUp,
    MouseSensitivityDown,
    Cmd,
    Chat,
    TeamChat,
    Snap,
    Weapons,
    Bind
};

// Key modifier constants
const uint16_t KM_NONE  = 0;
const uint16_t KM_ALT   = 1 << 0;
const uint16_t KM_CTRL  = 1 << 1;
const uint16_t KM_SHIFT = 1 << 2;

struct TBind {
    TAction Action;
    uint32_t KeyId;
    uint16_t KeyMod;
    std::wstring Command;
    
    TBind() : Action(TAction::None), KeyId(0), KeyMod(0), Command(L"") {}
};

// Function declarations
bool BindKey(const std::string& key, const std::string& action, const std::string& command, uint16_t Modifier);
TBind* FindKeyBind(uint16_t KeyMods, SDL_Scancode KeyCode);
void StartInput();
void UnbindAll();

// Global variables
extern std::vector<TBind> Binds;
extern bool KeyStatus[513];  // Array [0..512] of Boolean;
extern SDL_Window* GameWindow;
extern SDL_GLContext GameGLContext;

namespace InputImpl {
    inline bool BindKey(const std::string& key, const std::string& action, const std::string& command, uint16_t Modifier) {
        if (Binds.empty()) {
            Binds.resize(1);
        }
        int id = static_cast<int>(Binds.size()) - 1;

        // Check if it's a mouse button
        if (key.substr(0, 5) == "mouse") {
            // Extract the button number from the key string
            std::string buttonNumStr = key.substr(5);  // Get characters after "mouse"
            int buttonNum = 1;
            try {
                buttonNum = std::stoi(buttonNumStr);
            } catch (...) {
                // If conversion fails, default to 1
                buttonNum = 1;
            }
            Binds[id].KeyId = 300 + buttonNum;
        } else {
            // Use SDL function to get scan code from name
            SDL_Scancode scanCode = SDL_GetScancodeFromName(key.c_str());
            Binds[id].KeyId = static_cast<uint32_t>(scanCode);
        }

        if (Binds[id].KeyId == 0) {
            // Debug message about invalid key
            std::string debugMsg = "[INPUT] Key " + key + " is invalid";
            // Debug(debugMsg); // Assuming Debug is defined elsewhere
            return false;
        }

        // Check if this key is already bound
        TBind* existingBind = FindKeyBind(Modifier, static_cast<SDL_Scancode>(Binds[id].KeyId));
        if (existingBind != nullptr) {
            // Debug("[INPUT] Key " + key + " is already bound");
            return false;
        }

        // Determine the action from the string
        std::string lowerAction = action;
        std::transform(lowerAction.begin(), lowerAction.end(), lowerAction.begin(), ::tolower);
        
        if (lowerAction == "+left") Binds[id].Action = TAction::Left;
        else if (lowerAction == "+right") Binds[id].Action = TAction::Right;
        else if (lowerAction == "+jump") Binds[id].Action = TAction::Jump;
        else if (lowerAction == "+crouch") Binds[id].Action = TAction::Crouch;
        else if (lowerAction == "+fire") Binds[id].Action = TAction::Fire;
        else if (lowerAction == "+jet") Binds[id].Action = TAction::Jet;
        else if (lowerAction == "+reload") Binds[id].Action = TAction::Reload;
        else if (lowerAction == "+changeweapon") Binds[id].Action = TAction::ChangeWeapon;
        else if (lowerAction == "+voicechat") Binds[id].Action = TAction::VoiceChat;
        else if (lowerAction == "+throwgrenade") Binds[id].Action = TAction::ThrowGrenade;
        else if (lowerAction == "+dropweapon") Binds[id].Action = TAction::DropWeapon;
        else if (lowerAction == "+prone") Binds[id].Action = TAction::Prone;
        else if (lowerAction == "+flagthrow") Binds[id].Action = TAction::FlagThrow;
        else if (lowerAction == "+statsmenu") Binds[id].Action = TAction::StatsMenu;
        else if (lowerAction == "+gamestats") Binds[id].Action = TAction::GameStats;
        else if (lowerAction == "+minimap") Binds[id].Action = TAction::MiniMap;
        else if (lowerAction == "+playername") Binds[id].Action = TAction::PlayerName;
        else if (lowerAction == "+fragslist") Binds[id].Action = TAction::FragsList;
        else if (lowerAction == "+sniperline") Binds[id].Action = TAction::SniperLine;
        else if (lowerAction == "+radio") Binds[id].Action = TAction::Radio;
        else if (lowerAction == "+recorddemo") Binds[id].Action = TAction::RecordDemo;
        else if (lowerAction == "+volumeup") Binds[id].Action = TAction::VolumeUp;
        else if (lowerAction == "+volumedown") Binds[id].Action = TAction::VolumeDown;
        else if (lowerAction == "+mousesensitivityup") Binds[id].Action = TAction::MouseSensitivityUp;
        else if (lowerAction == "+mousesensitivitydown") Binds[id].Action = TAction::MouseSensitivityDown;
        else if (lowerAction == "+cmd") Binds[id].Action = TAction::Cmd;
        else if (lowerAction == "+chat") Binds[id].Action = TAction::Chat;
        else if (lowerAction == "+teamchat") Binds[id].Action = TAction::TeamChat;
        else if (lowerAction == "+snap") Binds[id].Action = TAction::Snap;
        else if (lowerAction == "+weapons") Binds[id].Action = TAction::Weapons;
        else if (lowerAction == "+bind") Binds[id].Action = TAction::Bind;
        else {
            // Unknown action
            Binds[id].Action = TAction::None;
        }

        Binds[id].Command = std::wstring(command.begin(), command.end());
        Binds[id].KeyMod = Modifier;

        // Debug message about binding
        std::string debugMsg = "[INPUT] BindKey id: " + std::to_string(id) + 
                              " Key: " + key + " (" + std::to_string(Binds[id].KeyId) + 
                              "), Mod: " + std::to_string(Binds[id].KeyMod) + 
                              " Command: " + command;
        // Debug(debugMsg); // Assuming Debug is defined elsewhere

        Binds.resize(Binds.size() + 1);  // Increase size by 1
        return true;
    }

    inline TBind* FindKeyBind(uint16_t KeyMods, SDL_Scancode KeyCode) {
        for (size_t i = 0; i < Binds.size() - 1; i++) {  // -1 to match original Pascal behavior
            if ((Binds[i].KeyId == static_cast<uint32_t>(KeyCode)) && 
                ((Binds[i].KeyMod & KeyMods) != 0 || Binds[i].KeyMod == KeyMods)) {
                return &Binds[i];
            }
        }
        return nullptr;
    }

    inline void UnbindAll() {
        Binds.clear();
    }

    inline void StartInput() {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_StopTextInput();
    }
}

// Using declarations to bring into global namespace
using InputImpl::TAction;
using InputImpl::TBind;
using InputImpl::BindKey;
using InputImpl::FindKeyBind;
using InputImpl::StartInput;
using InputImpl::UnbindAll;

// Global variables
extern std::vector<TBind> Binds;
extern bool KeyStatus[513];
extern SDL_Window* GameWindow;
extern SDL_GLContext GameGLContext;

// Initialize globals
namespace InputImpl {
    inline void InitializeInputGlobals() {
        // Initialize KeyStatus array to false
        for (int i = 0; i <= 512; i++) {
            KeyStatus[i] = false;
        }
        
        Binds = std::vector<TBind>();
        GameWindow = nullptr;
        GameGLContext = nullptr;
    }
}

#endif // INPUT_H