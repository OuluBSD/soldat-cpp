#ifndef CONTROLGAME_H
#define CONTROLGAME_H

//*******************************************************************************
//                                                                              
//       ControlGame Unit for SOLDAT                                                
//                                                                              
//       Copyright (c) 2016 Paweł Drzazga               
//                                                                              
//*******************************************************************************

#include <string>
#include <SDL2/SDL.h>
#include "Constants.h"
#include "Vector.h"
#include "Client.h"
#include "ClientGame.h"
#include "Game.h" 
#include "Sprites.h"
#include "Input.h"
#include "Util.h"
#include "Demo.h"
#include "GameMenus.h"
#include "Command.h"
#include "Cvar.h"
#include "NetworkClientConnection.h"
#include "NetworkClientGame.h"
#include "NetworkClientMessages.h"

// Function declarations
void GameInput();

// Helper functions
void ClearChatText();
std::wstring FilterChatText(const std::wstring& Str);
bool ChatKeyDown(Uint8 KeyMods, SDL_Keycode KeyCode);
bool MenuKeyDown(Uint8 KeyMods, SDL_Scancode KeyCode);
bool KeyDown(SDL_KeyboardEvent& KeyEvent);
bool KeyUp(SDL_KeyboardEvent& KeyEvent);

#endif // CONTROLGAME_H