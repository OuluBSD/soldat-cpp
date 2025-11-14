#ifndef WEATHEREFFECTS_H
#define WEATHEREFFECTS_H

//*******************************************************************************
//                                                                              
//       WeatherEffects Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2003 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Game.h"
#include "Weapons.h"
#include "Vector.h"
#include "Sprites.h"
#include "Net.h"
#include "Sparks.h"
#include "Client.h"
#include "Cvar.h"
#include "Sound.h"
#include "Constants.h"
#include <cstdlib> // for rand()

// Function declarations
void MakeRain();
void MakeSandStorm();
void MakeSnow();

namespace WeatherEffectsImpl {

    // Background animation and sound for rain
    inline void MakeRain() {
        int i;
        TVector2 a, b;
        int Modder;
        
        if (r_maxsparks.Value < (MAX_SPARKS - 10)) {
            Modder = 34;
        } else {
            Modder = 17;
        }

        if (MainTickCounter % Modder == 0) {
            a.x = CameraX - GameWidthHalf - 128;

            b.x = 0;
            b.y = 12;

            for (i = 1; i <= 8; i++) {
                a.x = a.x + 128 - 50 + (rand() % 90);
                a.y = CameraY - GameHeightHalf - 128 - 60 + (rand() % 150);

                CreateSpark(a, b, 38, 255, 60);
            }
        }

        PlaySound(SFX_WIND, CHANNEL_WEATHER);
    }

    // Background animation and sound for sandstorm
    inline void MakeSandStorm() {
        int i;
        TVector2 a, b;
        int Modder;
        
        if (r_maxsparks.Value < (MAX_SPARKS - 10)) {
            Modder = 34;
        } else {
            Modder = 17;
        }

        if (MainTickCounter % Modder == 0) {
            a.x = CameraX - GameWidthHalf - 1.5f * 512;

            b.x = 10;
            b.y = 7;

            for (i = 1; i <= 8; i++) {
                a.x = a.x + 128 - 50 + (rand() % 90);
                a.y = CameraY - GameHeightHalf - 256 - 60 + (rand() % 150);

                CreateSpark(a, b, 39, 255, 80);
            }
        }

        PlaySound(SFX_WIND, CHANNEL_WEATHER);
    }

    // Background animation and sound for snow
    inline void MakeSnow() {
        int i;
        TVector2 a, b;
        int Modder;
        
        if (r_maxsparks.Value < (MAX_SPARKS - 10)) {
            Modder = 34;
        } else {
            Modder = 17;
        }

        if (MainTickCounter % Modder == 0) {
            a.x = CameraX - GameWidthHalf - 256;

            b.x = 1;
            b.y = 2;

            for (i = 1; i <= 8; i++) {
                a.x = a.x + 128 - 50 + (rand() % 90);
                a.y = CameraY - GameHeightHalf - 60 + (rand() % 150);

                CreateSpark(a, b, 53, 255, 80);
            }
        }

        PlaySound(SFX_WIND, CHANNEL_WEATHER);
    }

} // namespace WeatherEffectsImpl

using WeatherEffectsImpl::MakeRain;
using WeatherEffectsImpl::MakeSandStorm;
using WeatherEffectsImpl::MakeSnow;

#endif // WEATHEREFFECTS_H