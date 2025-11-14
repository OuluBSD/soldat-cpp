#ifndef INTERFACEGRAPHICS_H
#define INTERFACEGRAPHICS_H

//*******************************************************************************
//                                                                              
//       Interface Graphics Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Vector.h"
#include "Sprites.h"
#include "Constants.h"
#include "Gfx.h"
#include "GameRendering.h"
#include "Client.h"
#include <string>
#include <vector>

// External variables for chat messages
extern std::wstring ChatMessage[MAX_SPRITES + 1];  // 1-indexed
extern bool ChatTeam[MAX_SPRITES + 1];             // 1-indexed
extern int ChatDelay[MAX_SPRITES + 1];             // 1-indexed

// Big text messages
extern std::wstring BigText[MAX_BIG_MESSAGES + 1]; // 0-indexed
extern int BigDelay[MAX_BIG_MESSAGES + 1];         // 0-indexed
extern int BigX[MAX_BIG_MESSAGES + 1];             // 0-indexed
extern float BigScale[MAX_BIG_MESSAGES + 1];       // 0-indexed
extern uint32_t BigColor[MAX_BIG_MESSAGES + 1];    // 0-indexed
extern float BigPosX[MAX_BIG_MESSAGES + 1];        // 0-indexed
extern float BigPosY[MAX_BIG_MESSAGES + 1];        // 0-indexed

// World text messages
extern std::wstring WorldText[MAX_BIG_MESSAGES + 1]; // 0-indexed
extern int WorldDelay[MAX_BIG_MESSAGES + 1];         // 0-indexed
extern int WorldX[MAX_BIG_MESSAGES + 1];             // 0-indexed
extern float WorldScale[MAX_BIG_MESSAGES + 1];       // 0-indexed
extern int32_t WorldColor[MAX_BIG_MESSAGES + 1];     // 0-indexed
extern float WorldPosX[MAX_BIG_MESSAGES + 1];        // 0-indexed
extern float WorldPosY[MAX_BIG_MESSAGES + 1];        // 0-indexed

// Cursor variables
extern std::string CursorText;
extern int CursorTextLength;
extern bool CursorFriendly;

// Menu visibility flags
extern bool FragsMenuShow, StatsMenuShow, ConInfoShow, PlayerNamesShow, MiniMapShow, NoobShow;

// Frags scrolling
extern uint8_t FragsScrollMax;
extern uint8_t FragsScrollLev;

// Scaling factors
extern TVector2 _RScala;
extern TVector2 _IScala;
extern int fragx, fragy;

// Function declarations
void LoadInterfaceArchives(const std::string& Path, bool FirstOnly = false);
bool LoadInterfaceData(const std::string& InterfaceName);
void RenderInterface(float TimeElapsed, float Width, float Height);
void RenderActionSnapText(double t);
bool IsDefaultInterface(const std::string& InterfaceName);

namespace InterfaceGraphicsImpl {

    struct TInterfaceRelInfo {
        int HealthBar_Rel_X, HealthBar_Rel_Y;
        int JetBar_Rel_X, JetBar_Rel_Y;
        int AmmoBar_Rel_X, AmmoBar_Rel_Y;
        int FireBar_Rel_X, FireBar_Rel_Y;
        int NadesBar_Rel_X, NadesBar_Rel_Y;
    };

    struct TInterface {
        uint8_t Alpha;
        bool Health, Ammo, Vest, Jet, Nades, Bullets, Weapon, Fire, Team, Ping, Status;
        int HealthIco_X, HealthIco_Y;
        int HealthIco_Rotate;
        int HealthBar_X, HealthBar_Y;
        int HealthBar_Width, HealthBar_Height;
        uint8_t HealthBar_Pos;
        int HealthBar_Rotate;
        int AmmoIco_X, AmmoIco_Y;
        int AmmoIco_Rotate;
        int AmmoBar_X, AmmoBar_Y;
        int AmmoBar_Width, AmmoBar_Height;
        uint8_t AmmoBar_Pos;
        int AmmoBar_Rotate;
        int JetIco_X, JetIco_Y;
        int JetIco_Rotate;
        int JetBar_X, JetBar_Y;
        int JetBar_Width, JetBar_Height;
        uint8_t JetBar_Pos;
        int JetBar_Rotate;
        int VestBar_X, VestBar_Y;
        int VestBar_Width, VestBar_Height;
        uint8_t VestBar_Pos;
        int VestBar_Rotate;
        int Nades_X, Nades_Y;
        int Nades_Width, Nades_Height;
        uint8_t Nades_Pos;
        int Bullets_X, Bullets_Y;
        int Weapon_X, Weapon_Y;
        int FireIco_X, FireIco_Y;
        int FireIco_Rotate;
        int FireBar_X, FireBar_Y;
        int FireBar_Width, FireBar_Height;
        uint8_t FireBar_Pos;
        int FireBar_Rotate;
        int TeamBox_X, TeamBox_Y;
        int Ping_X, Ping_Y;
        int Status_X, Status_Y;
    };

    struct InterfaceAlign {
        uint8_t Weapon;
        uint8_t Bullets;
        uint8_t HealthBar;
        uint8_t AmmoBar;
        uint8_t ReloadBar;
        uint8_t FireBar;
        uint8_t JetBar;
        uint8_t VestBar;
    };

    struct TAttr {
        float Cur;
        float Def;
        std::wstring Des;
    };

    // Function implementations
    inline void LoadDefaultInterfaceData() {
        // Initialize default interface data
        InterfaceAlign IntAlign;
        IntAlign.Weapon    = 1;
        IntAlign.Bullets   = 1;
        IntAlign.HealthBar = 0;
        IntAlign.AmmoBar   = 0;
        IntAlign.ReloadBar = 0;
        IntAlign.FireBar   = 1;
        IntAlign.JetBar    = 0;
        IntAlign.VestBar   = 0;

        TInterface Int;
        Int.Alpha = 255;
        Int.Health = true;
        Int.Ammo = true;
        Int.Vest = true;
        Int.Jet = true;
        Int.Nades = true;
        Int.Bullets = true;
        Int.Weapon = true;
        Int.Fire = true;
        Int.Team = true;
        Int.Ping = true;
        Int.Status = true;
        Int.HealthBar_Width = 115;
        Int.HealthBar_Height = 9;
        Int.HealthBar_Pos = HORIZONTAL;
        Int.HealthBar_Rotate = 0;
        Int.AmmoBar_Width = 120;
        Int.AmmoBar_Height = 9;
        Int.AmmoBar_Pos = HORIZONTAL;
        Int.AmmoBar_Rotate = 0;
        Int.JetBar_Width = 115;
        Int.JetBar_Height = 9;
        Int.JetBar_Pos = HORIZONTAL;
        Int.JetBar_Rotate = 0;
        Int.VestBar_Width = 115;
        Int.VestBar_Height = 9;
        Int.VestBar_Pos = HORIZONTAL;
        Int.VestBar_Rotate = 0;
        Int.FireBar_Width = 57;
        Int.FireBar_Height = 4;
        Int.FireBar_Pos = HORIZONTAL;
        Int.FireBar_Rotate = 0;
        Int.HealthIco_X = 5;
        Int.HealthIco_Y = 445 - 6;
        Int.HealthIco_Rotate = 0;
        Int.AmmoIco_X = 285 - 10;
        Int.AmmoIco_Y = 445 - 6;
        Int.AmmoIco_Rotate = 0;
        Int.JetIco_X = 480;
        Int.JetIco_Y = 445 - 6;
        Int.JetIco_Rotate = 0;
        Int.HealthBar_X = 45;
        Int.HealthBar_Y = 455 - 6;
        Int.AmmoBar_X = 352;
        Int.AmmoBar_Y = 455 - 6;
        Int.Bullets_X = 348;
        Int.Bullets_Y = 451;
        Int.JetBar_X = 520;
        Int.JetBar_Y = 455 - 6;
        Int.FireBar_X = 402;
        Int.FireBar_Y = 464;
        Int.FireBar_Rotate = 0;
        Int.FireIco_X = 409;
        Int.FireIco_Y = 464;
        Int.FireIco_Rotate = 0;
        Int.Nades_X = 305 - 7 + 10;
        Int.Nades_Y = 468 - 6;
        Int.Nades_Height = 10;
        Int.Nades_Width = 10;
        Int.Nades_Pos = HORIZONTAL;
        Int.VestBar_X = 45;
        Int.VestBar_Y = 465 - 6;
        Int.TeamBox_X = 575;
        Int.TeamBox_Y = 330;
        Int.Status_X = 575;
        Int.Status_Y = 421;
        Int.Ping_X = 600;
        Int.Ping_Y = 18;
        Int.Weapon_X = 285;
        Int.Weapon_Y = 454;

        TInterfaceRelInfo relinfo;
        relinfo.HealthBar_Rel_X = Int.HealthIco_X;
        relinfo.HealthBar_Rel_Y = Int.HealthIco_Y;
        relinfo.JetBar_Rel_X = Int.JetIco_X;
        relinfo.JetBar_Rel_Y = Int.JetIco_Y;
        relinfo.AmmoBar_Rel_X = Int.AmmoIco_X;
        relinfo.AmmoBar_Rel_Y = Int.AmmoIco_Y;
        relinfo.FireBar_Rel_X = Int.AmmoIco_X;
        relinfo.FireBar_Rel_Y = Int.AmmoIco_Y;
        relinfo.NadesBar_Rel_X = Int.AmmoIco_X;
        relinfo.NadesBar_Rel_Y = Int.AmmoIco_Y;
    }

    inline bool IsInteractiveInterface() {
        return Sprite[MySprite].IsNotSpectator ||
            ((CameraFollowSprite > 0) && (sv_advancedspectator.Value));
    }

    inline float PixelAlignX(float x) {
        TVector2 PixelSize = {RenderWidth / RenderWidth, RenderHeight / RenderHeight};
        return PixelSize.x * floorf(x / PixelSize.x);
    }

    inline float PixelAlignY(float y) {
        TVector2 PixelSize = {RenderWidth / RenderWidth, RenderHeight / RenderHeight}; 
        return PixelSize.y * floorf(y / PixelSize.y);
    }

    inline void DrawLine(float x, float y, float w, const TGfxColor& Color) {
        float x0 = PixelAlignX(x);
        float y0 = PixelAlignY(y);
        float x1 = PixelAlignX(x0 + w);
        float y1 = y0 + 1; // PixelSize.y would be 1 for standard pixel

        GfxDrawQuad(nullptr,
            GfxVertex(x0, y0, 0, 0, Color),
            GfxVertex(x1, y0, 0, 0, Color),
            GfxVertex(x1, y1, 0, 0, Color),
            GfxVertex(x0, y1, 0, 0, Color)
        );
    }

    inline TVector2 ToMinimap(const TVector2& Pos, float Scale = 1.0f) {
        TVector2 Result = {0, 0};
        // Placeholder for WorldToMinimap function
        // WorldToMinimap(Pos.x, Pos.y, Result.x, Result.y);

        Scale *= Textures[GFX_INTERFACE_SMALLDOT].Scale;

        Result.x = PixelAlignX(ui_minimap_posx.Value * _rscala.x + Result.x -
            Scale * Textures[GFX_INTERFACE_SMALLDOT].Width / 2);

        Result.y = PixelAlignY(ui_minimap_posy.Value + Result.y -
            Scale * Textures[GFX_INTERFACE_SMALLDOT].Height / 2);

        return Result;
    }

    inline void RenderBar(int t, uint8_t PosType, int x, int rx, int y, int ry, 
                         int w, int h, int r, float p, bool LeftAlign = true) {
        if (PosType == TEXTSTYLE) {
            return;
        }

        p = std::max(0.0f, std::min(1.0f, p));
        w = Textures[t].Width;
        h = Textures[t].Height;

        float px = PixelAlignX(rx * _iscala.x) + (x - rx);
        float py = PixelAlignY(ry * _iscala.y) + (y - ry);
        TGfxRect rc = {0, 0, 0, 0};
        rc.Top = 0;
        rc.Bottom = h;

        if (LeftAlign) {
            rc.Left = 0;
            rc.Right = w * p;

            if (PosType == VERTICAL) {
                rc.Right = w;
                rc.Top = h * (1 - p);
                py += rc.Top * Textures[t].Scale;
            }
        } else {
            rc.Left = w * (1 - p);
            rc.Right = w;

            if (PosType == VERTICAL) {
                rc.Left = 0;
                rc.Bottom = h * p;
                py += (h * (1 - p)) * Textures[t].Scale;
            }
        }

        GfxDrawSprite(Textures[t], px, py, 0, 0, DegToRad(r), RGBA(0xFFFFFF, Int.Alpha), rc);
    }

    inline void GetWeaponAttribs(int i, std::vector<TAttr>& Attrs) {
        // Implementation would get weapon attributes
        // This is a simplified version
    }

    // More functions would be implemented here...
} // namespace InterfaceGraphicsImpl

using InterfaceGraphicsImpl::LoadInterfaceArchives;
using InterfaceGraphicsImpl::LoadInterfaceData;
using InterfaceGraphicsImpl::RenderInterface;
using InterfaceGraphicsImpl::RenderActionSnapText;
using InterfaceGraphicsImpl::IsDefaultInterface;

#endif // INTERFACEGRAPHICS_H