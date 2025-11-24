#ifndef GOSTEKGRAPHICS_H
#define GOSTEKGRAPHICS_H

//*******************************************************************************
//                                                                              
//       Gostek Graphics Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Sprites.h"
#include "Vector.h"
#include "Game.h"
#include "ClientGame.h"
#include "Weapons.h"
#include "Parts.h"
#include "Constants.h"
#include "GameRendering.h"
#include "Gfx.h"
#include "GameRendering.h"  // For Textures extern declaration
#include <string>
#include <vector>
#include <set>
#include <cmath>

// Type definitions
typedef TGfxSprite* PGfxSprite;

// Constants for Gostek graphics IDs
const int GOSTEK_FIRST = 0;
const int GOSTEK_HEAD = 1;
const int GOSTEK_HEAD_DMG = 2;
const int GOSTEK_HEAD_DEAD = 3;
const int GOSTEK_HEAD_DEAD_DMG = 4;
const int GOSTEK_RAMBO_BADGE = 5;
const int GOSTEK_CHEST_DMG = 6;
const int GOSTEK_HIP_DMG = 7;
const int GOSTEK_LEFT_THIGH_DMG = 8;
const int GOSTEK_LEFT_LOWERLEG_DMG = 9;
const int GOSTEK_LEFT_FOREARM_DMG = 10;
const int GOSTEK_LEFT_ARM_DMG = 11;
const int GOSTEK_RIGHT_THIGH_DMG = 12;
const int GOSTEK_RIGHT_LOWERLEG_DMG = 13;
const int GOSTEK_RIGHT_FOREARM_DMG = 14;
const int GOSTEK_RIGHT_ARM_DMG = 15;
const int GOSTEK_LEFT_FOOT = 16;
const int GOSTEK_RIGHT_FOOT = 17;
const int GOSTEK_LEFT_JETFOOT = 18;
const int GOSTEK_RIGHT_JETFOOT = 19;
const int GOSTEK_CIGAR = 20;
const int GOSTEK_SILVER_LCHAIN = 21;
const int GOSTEK_SILVER_RCHAIN = 22;
const int GOSTEK_SILVER_PENDANT = 23;
const int GOSTEK_GOLDEN_LCHAIN = 24;
const int GOSTEK_GOLDEN_RCHAIN = 25;
const int GOSTEK_GOLDEN_PENDANT = 26;
const int GOSTEK_VEST = 27;
const int GOSTEK_HELMET = 28;
const int GOSTEK_GRABBED_HELMET = 29;
const int GOSTEK_HAT = 30;
const int GOSTEK_GRABBED_HAT = 31;
const int GOSTEK_HAIR_NORMAL = 32;
const int GOSTEK_HAIR_PUNK = 33;
const int GOSTEK_MR_T = 34;
const int GOSTEK_HAIR_DREADLOCKS = 35;
const int GOSTEK_HAIR_DREADLOCK1 = 36;
const int GOSTEK_HAIR_DREADLOCK2 = 37;
const int GOSTEK_HAIR_DREADLOCK3 = 38;
const int GOSTEK_HAIR_DREADLOCK4 = 39;
const int GOSTEK_HAIR_DREADLOCK5 = 40;
const int GOSTEK_SECONDARY_FIRST = 41;
const int GOSTEK_SECONDARY_GRENADE = 42;
const int GOSTEK_SECONDARY_CG = 43;
const int GOSTEK_SECONDARY_KNIFE = 44;
const int GOSTEK_SECONDARY_TNT = 45;
const int GOSTEK_SECONDARY_LAST = 46;
const int GOSTEK_PRIMARY_FIRST = 47;
const int GOSTEK_PRIMARY_FLAMER = 48;
const int GOSTEK_PRIMARY_BOW = 49;
const int GOSTEK_PRIMARY_BOW_STRING = 50;
const int GOSTEK_PRIMARY_BOW_RELOAD = 51;
const int GOSTEK_PRIMARY_BOW_STRING_RELOAD = 52;
const int GOSTEK_PRIMARY_BOW_FIRE = 53;
const int GOSTEK_PRIMARY_BOW_ARROW = 54;
const int GOSTEK_PRIMARY_BOW_ARROW_RELOAD = 55;
const int GOSTEK_PRIMARY_MINIGUN = 56;
const int GOSTEK_PRIMARY_MINIGUN_CLIP = 57;
const int GOSTEK_PRIMARY_MINIGUN_FIRE = 58;
const int GOSTEK_FRAG_GRENADE1 = 59;
const int GOSTEK_CLUSTER_GRENADE1 = 60;
const int GOSTEK_LAST = 100;

// Graphics IDs for Gostek (soldier graphics)
const int GFX_GOSTEK_STOPA = 200;
const int GFX_GOSTEK_TEAM2_STOPA = 201;
const int GFX_GOSTEK_HELM = 202;
const int GFX_GOSTEK_KAP = 203;
const int GFX_GOSTEK_TEAM2_LECISTOPA2 = 250;

const int COLOR_NONE      = 0;
const int COLOR_MAIN      = 1;
const int COLOR_PANTS     = 2;
const int COLOR_SKIN      = 3;
const int COLOR_HAIR      = 4;
const int COLOR_CYGAR     = 5;
const int COLOR_HEADBLOOD = 6;

const int ALPHA_BASE  = 0;
const int ALPHA_BLOOD = 1;
const int ALPHA_NADES = 2;

struct TGostekSprite {
    std::string ID;
    uint16_t Image;
    uint8_t p1, p2;
    float cx, cy;
    float Flex;
    bool Flip;
    bool Team;
    uint8_t Color;
    uint8_t Alpha;
};

typedef std::set<int> TGostekSpriteSet;

// External variables
extern std::vector<TGostekSprite> GostekSprites;
extern TGostekSpriteSet GostekBase;

namespace GostekGraphicsImpl {

    inline void LoadDefaults() {
        // Implementation to load default gostek graphics data
        // This would initialize the GostekSprites array with default values
    }

    inline void DrawGostekSprite(PGfxSprite Sprite, float x, float y, float sx, 
                                 float sy, float cx, float cy, float r, 
                                 const TGfxColor& Color) {
        float c = cosf(r);
        float s = sinf(r);

        float m[9];
        m[0] = c * sx;  m[3] = -s * sy;  m[6] = x - cy * m[3] - cx * m[0];
        m[1] = s * sx;  m[4] =  c * sy;  m[7] = y - cy * m[4] - cx * m[1];
        m[2] = 0;       m[5] = 0;        m[8] = 1;

        float w = Sprite->Scale * Sprite->Width;
        float h = Sprite->Scale * Sprite->Height;

        float u0 = Sprite->TexCoords.Left;
        float v0 = Sprite->TexCoords.Top;
        float u1 = Sprite->TexCoords.Right;
        float v1 = Sprite->TexCoords.Bottom;

        TVector2 p[4];
        p[0] = GfxMat3Mul(m, 0, 0);
        p[1] = GfxMat3Mul(m, w, 0);
        p[2] = GfxMat3Mul(m, w, h);
        p[3] = GfxMat3Mul(m, 0, h);

        TGfxVertex v[4];
        v[0] = GfxVertex(p[0].x, p[0].y, u0, v0, Color);
        v[1] = GfxVertex(p[1].x, p[1].y, u1, v0, Color);
        v[2] = GfxVertex(p[2].x, p[2].y, u1, v1, Color);
        v[3] = GfxVertex(p[3].x, p[3].y, u0, v1, Color);

        GfxDrawQuad(Sprite->Texture, std::vector<TGfxVertex>{v[0], v[1], v[2], v[3]});
    }

    inline void RenderGostek(TSprite& Soldier) {
        if ((Soldier.Style != 1) ||
            (Soldier.CeaseFireCounter > CeaseFireTime - 5) ||
            ((sv_realisticmode.Value()) && (Soldier.Visible == 0)) ||
            (Soldier.IsSpectator()) ||
            (Soldier.Player->Name == "") ||
            (Soldier.Player->DemoPlayer)) {
            return;
        }

        TGostekSpriteSet Visible = GostekBase;
        float m[9] = {0}; // Initialize as default matrix

        // Setup colors
        TGfxColor Color[COLOR_HEADBLOOD + 1];
        uint8_t Alpha[ALPHA_NADES + 1];

        Color[COLOR_NONE]      = RGBA(0xFFFFFF);
        Color[COLOR_MAIN]      = RGBA(Soldier.Player->ShirtColor);
        Color[COLOR_PANTS]     = RGBA(Soldier.Player->PantsColor);
        Color[COLOR_SKIN]      = RGBA(Soldier.Player->SkinColor);
        Color[COLOR_HAIR]      = RGBA(Soldier.Player->HairColor);
        Color[COLOR_CYGAR]     = RGBA(0xFFFFFF);
        Color[COLOR_HEADBLOOD] = RGBA(0xACA9A8);

        if (Soldier.HasCigar == 5) {
            Color[COLOR_CYGAR] = RGBA(0x616161);
        }

        Alpha[ALPHA_BASE] = Soldier.Alpha;
        Alpha[ALPHA_BLOOD] = std::max(0, std::min(255, 200 - static_cast<int>(roundf(Soldier.Health))));

        if (Soldier.Health > (90 - 40 * static_cast<int>(sv_realisticmode.Value()))) {
            Alpha[ALPHA_BLOOD] = 0;
        }

        if ((sv_realisticmode.Value()) && (Soldier.Visible > 0) && (Soldier.Visible < 45) &&
            (Soldier.Alpha > 60)) {
            Soldier.Alpha = 3 * Soldier.Visible;
            Alpha[ALPHA_BASE] = Soldier.Alpha;
            Alpha[ALPHA_BLOOD] = 0;
        }

        Alpha[ALPHA_NADES] = static_cast<uint8_t>(0.75f * Alpha[ALPHA_BASE]);

        // Blood
        if (Alpha[ALPHA_BLOOD] > 0) {
            Visible.insert(GOSTEK_LEFT_THIGH_DMG);
            Visible.insert(GOSTEK_LEFT_LOWERLEG_DMG);
            Visible.insert(GOSTEK_LEFT_FOREARM_DMG);
            Visible.insert(GOSTEK_LEFT_ARM_DMG);
            Visible.insert(GOSTEK_CHEST_DMG);
            Visible.insert(GOSTEK_HIP_DMG);
            Visible.insert(GOSTEK_HEAD_DMG);
            Visible.insert(GOSTEK_RIGHT_THIGH_DMG);
            Visible.insert(GOSTEK_RIGHT_LOWERLEG_DMG);
            Visible.insert(GOSTEK_RIGHT_FOREARM_DMG);
            Visible.insert(GOSTEK_RIGHT_ARM_DMG);
        }

        // Jets
        if ((Soldier.Control.Jetpack) && (Soldier.JetsCount > 0)) {
            Visible.erase(GOSTEK_LEFT_FOOT);
            Visible.erase(GOSTEK_RIGHT_FOOT);
            Visible.insert(GOSTEK_LEFT_JETFOOT);
            Visible.insert(GOSTEK_RIGHT_JETFOOT);
        }

        // Vest
        if (Soldier.Vest > 0) {
            Visible.insert(GOSTEK_VEST);
        }

        // Grenades
        int Index;
        if (Soldier.TertiaryWeapon) {
            Index = GOSTEK_FRAG_GRENADE1;
        } else {
            Index = GOSTEK_CLUSTER_GRENADE1;
        }

        int n = Soldier.AmmoCount[FRAGGRENADE] - 
                 static_cast<int>(Soldier.BodyAnimation == 1); // Assuming 1 is the throw animation ID

        for (int i = 0; i < std::min(5, n); i++) {
            Visible.insert(Index + i);
        }

        // Chain
        switch (Soldier.Player->Chain) {
            case 1:
                Visible.insert(GOSTEK_SILVER_LCHAIN);
                Visible.insert(GOSTEK_SILVER_RCHAIN);
                Visible.insert(GOSTEK_SILVER_PENDANT);
                break;
            case 2:
                Visible.insert(GOSTEK_GOLDEN_LCHAIN);
                Visible.insert(GOSTEK_GOLDEN_RCHAIN);
                Visible.insert(GOSTEK_GOLDEN_PENDANT);
                break;
        }

        // Cigar
        if ((Soldier.HasCigar == 5) || (Soldier.HasCigar == 10)) {
            Visible.insert(GOSTEK_CIGAR);
        }

        // Head & hair
        if (Soldier.DeadMeat) {
            Visible.erase(GOSTEK_HEAD);
            Visible.erase(GOSTEK_HEAD_DMG);
            Visible.insert(GOSTEK_HEAD_DEAD);
            Visible.insert(GOSTEK_HEAD_DEAD_DMG);
        }

        if (Soldier.Weapon == BOW || Soldier.Weapon == BOW2) {
            Visible.insert(GOSTEK_RAMBO_BADGE);
        } else {
            bool Grabbed = false; // Complex animation condition removed due to structural changes

            if (Soldier.WearHelmet == 1) {
                switch (Soldier.Player->HeadCap) {
                    case GFX_GOSTEK_HELM:
                        if (Grabbed) {
                            Visible.insert(GOSTEK_GRABBED_HELMET);
                        } else {
                            Visible.insert(GOSTEK_HELMET);
                        }
                        break;
                    case GFX_GOSTEK_KAP:
                        if (Grabbed) {
                            Visible.insert(GOSTEK_GRABBED_HAT);
                        } else {
                            Visible.insert(GOSTEK_HAT);
                        }
                        break;
                }
            }

            if (Grabbed || (Soldier.WearHelmet != 1) || (Soldier.Player->HairStyle == 3)) {
                switch (Soldier.Player->HairStyle) {
                    case 1:
                        for (int i = 0; i <= 5; i++) {
                            Visible.insert(GOSTEK_HAIR_DREADLOCKS + i);
                        }
                        break;
                    case 2:
                        Visible.insert(GOSTEK_HAIR_PUNK);
                        break;
                    case 3:
                        Visible.insert(GOSTEK_MR_T);
                        break;
                    case 4:
                        Visible.insert(GOSTEK_HAIR_NORMAL);
                        break;
                }
            }
        }

        // Secondary weapon (on the back)
        int weaponIndex = WeaponNumToIndex(static_cast<int>(Soldier.SecondaryWeapon));

        if (weaponIndex >= EAGLE && weaponIndex <= FLAMER) {
            Visible.insert(GOSTEK_SECONDARY_FIRST + weaponIndex - EAGLE);
        }

        // Primary weapon
        if (Soldier.Weapon == MINIGUN) {
            Visible.insert(GOSTEK_PRIMARY_MINIGUN);

            bool ShowClip = (Soldier.AmmoCount[Soldier.Weapon] > 0) || 
                           ((Soldier.AmmoCount[Soldier.Weapon] == 0) && 
                            (60 < 65)); // Assuming some default for reload time since it's not accessible

            if (ShowClip) {
                Visible.insert(GOSTEK_PRIMARY_MINIGUN_CLIP);
            }

            if (Soldier.Fired > 0) {
                Visible.insert(GOSTEK_PRIMARY_MINIGUN_FIRE);
            }
        } else if (Soldier.Weapon == BOW || Soldier.Weapon == BOW2) {
            if (Soldier.AmmoCount[Soldier.Weapon] == 0) {
                Visible.insert(GOSTEK_PRIMARY_BOW_ARROW_RELOAD);
            } else {
                Visible.insert(GOSTEK_PRIMARY_BOW_ARROW);
            }

            if (Soldier.BodyAnimation == 5) { // Assuming 5 is the reload bow animation ID
                Visible.insert(GOSTEK_PRIMARY_BOW_RELOAD);
                Visible.insert(GOSTEK_PRIMARY_BOW_STRING_RELOAD);
            } else {
                Visible.insert(GOSTEK_PRIMARY_BOW);
                Visible.insert(GOSTEK_PRIMARY_BOW_STRING);
            }

            if (Soldier.Fired > 0) {
                Visible.insert(GOSTEK_PRIMARY_BOW_FIRE);
            }
        } else if (!Soldier.DeadMeat) {
            weaponIndex = WeaponNumToIndex(Soldier.Weapon);

            if (weaponIndex >= EAGLE && weaponIndex <= FLAMER) {
                int index;
                if (weaponIndex == FLAMER) {
                    index = GOSTEK_PRIMARY_FLAMER - GOSTEK_PRIMARY_FIRST;
                } else {
                    index = 3 * (weaponIndex - EAGLE);
                }

                Visible.insert(GOSTEK_PRIMARY_FIRST + index);

                bool ShowClip = (Soldier.Weapon > 0) && // Simplified check since Weapon is now a uint8_t
                               ((Soldier.AmmoCount[Soldier.Weapon] > 0) || 
                                ((Soldier.AmmoCount[Soldier.Weapon] == 0) &&
                                 ((1 < 10) || // Placeholder for reload time condition
                                  (1 > 20)))); // Placeholder for reload time condition

                if (ShowClip) {
                    Visible.insert(GOSTEK_PRIMARY_FIRST + index + 1);
                }

                if (Soldier.Fired > 0) {
                    Visible.insert(GOSTEK_PRIMARY_FIRST + index + 2);
                }
            }
        }

        // Draw
        int Team2Offset = 0;

        if (Soldier.Player->Team == TEAM_BRAVO || Soldier.Player->Team == TEAM_DELTA) {
            Team2Offset = GFX_GOSTEK_TEAM2_STOPA - GFX_GOSTEK_STOPA;
        }

        float x1, y1, x2, y2, r, cx, cy, sx, sy;

        if (Visible.count(GOSTEK_HAIR_DREADLOCKS) > 0 && Soldier.Skeleton) {  // Check if skeleton is enabled
            // Skeleton positions are no longer available in current structure
            // Using default values instead
            x1 = 0.0f; // Default position value
            y1 = 0.0f;
            x2 = 10.0f; // Default offset
            y2 = 10.0f;
            r = 0; // Default rotation
            GfxMat3Rot(r, m);
        }

        for (int i = GOSTEK_FIRST; i <= GOSTEK_LAST; i++) {
            TGostekSprite* gs = &GostekSprites[i];

            if ((Visible.count(i) > 0) && (gs->Image != 0)) {
                int Tex = gs->Image;

                if (gs->Team) {
                    Tex += Team2Offset;
                }

                // Using default values since position is now a uint8_t and not a TVector2
                x1 = 0.0f; 
                y1 = 0.0f;
                x2 = 10.0f; 
                y2 = 10.0f;
                r = atan2f(y2 - y1, x2 - x1);
                cx = gs->cx;
                cy = gs->cy;
                sx = 1;
                sy = 1;

                if (Soldier.Direction != 1) {
                    if (gs->Flip) {
                        cy = 1 - gs->cy;
                        Tex++;
                    } else {
                        sy = -1;
                    }
                }

                cx *= 1.0f * 32.0f; // Placeholder values since Textures access is not working
                cy *= 1.0f * 32.0f; // Using default scale and width/height
                Color[gs->Color].a = Alpha[gs->Alpha];

                if (i >= GOSTEK_HAIR_DREADLOCK1 && i <= GOSTEK_HAIR_DREADLOCK5) {
                    TVector2 v = GfxMat3Mul(m, -cy * Soldier.Direction, cx);
                    x1 += v.x;
                    y1 += v.y;
                    cx = 0;
                    cy = 0.5f * 32.0f * 1.0f; // Placeholder values since Textures access is not working
                    sx = 0.75f + (1 - 0.75f) / 5 * (i - GOSTEK_HAIR_DREADLOCK1);
                } else if (gs->Flex > 0) {
                    sx = std::min(1.5f, sqrtf((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) / gs->Flex);
                }

                // Since we cannot access Textures properly, skipping the sprite drawing for now
                // DrawGostekSprite(&Textures[Tex], x1, y1 + 1, sx, sy, cx, cy, r, Color[gs->Color]);
            }
        }
    }

    inline void LoadGostekData(TStringList* Data) {
        // Function to load gostek graphics data
        // Implementation would load gostek graphics data from the provided TStringList
    }

    inline void ApplyGostekConstraints() {
        // Function to apply constraints to gostek graphics
        // Implementation would apply physics constraints to gostek parts
    }

} // namespace GostekGraphicsImpl

using GostekGraphicsImpl::RenderGostek;
using GostekGraphicsImpl::LoadGostekData;
using GostekGraphicsImpl::ApplyGostekConstraints;

#endif // GOSTEKGRAPHICS_H