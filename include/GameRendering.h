#ifndef GAMERENDERING_H
#define GAMERENDERING_H

//*******************************************************************************
//                                                                              
//       Game Rendering Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2002 Michal Marcinkowski                                 
//                                                                              
//*******************************************************************************

#include "Vector.h"
#include "Constants.h"
#include "Gfx.h"
#include "Sprites.h"
#include "Parts.h"
#include "Game.h"
#include "Weapons.h"
#include "PolyMap.h"
#include "MapFile.h"
#include "Util.h"
#include "InterfaceGraphics.h"
#include "ClientGame.h"
#include "GameStrings.h"
#include "GostekGraphics.h"
#include "Input.h"
#include "MapGraphics.h"
#include "TraceLog.h"
#include "Client.h"
#include "Cvar.h"
#include "Version.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cmath>

// Font style constants
const int FONT_BIG          = 0;
const int FONT_SMALL        = 1;
const int FONT_SMALL_BOLD   = 2;
const int FONT_SMALLEST     = 3;
const int FONT_MENU         = 4;
const int FONT_WEAPONS_MENU = 5;
const int FONT_WORLD        = 6;
const int FONT_LAST         = FONT_WORLD;

// Structure definitions
struct TGameRenderingParams {
    std::string InterfaceName;
};

// External variables
extern TGameRenderingParams GameRenderingParams;
extern TGfxSpriteArray Textures;

// Function declarations
bool InitGameGraphics();
void ReloadGraphics();
void DestroyGameGraphics();
void RenderFrame(double TimeElapsed, double FramePercent, bool Paused);
void RenderGameInfo(const std::wstring& TextString);
bool DoTextureLoading(bool FinishLoading = false);
void SetFontStyle(int Style);
void SetFontStyle(int Style, float Scale);
float FontStyleSize(int Style);
void TakeScreenshot(const std::string& Filename, bool Async = true);
std::string PngOverride(const std::string& Filename);

namespace GameRenderingImpl {

    // Structure definitions
    struct TTextureLoadData {
        int ID;
        int Group;
        std::string Path;
        uint32_t ColorKey;
    };

    struct TInterpolationState {
        TVector2 Camera;
        TVector2 Mouse;
        TVector2 SpritePos[MAX_SPRITES][24];
        TVector2 BulletPos[MAX_BULLETS];
        TVector2 BulletVel[MAX_BULLETS];
        float BulletHitMul[MAX_BULLETS];
        TVector2 SparkPos[MAX_SPARKS];
        TVector2 ThingPos[MAX_THINGS][4];
    };

    struct TFontStyle {
        TGfxFont Font;
        int TableIndex;
        float Size;
        float Stretch;
        uint32_t Flags;
    };

    // Internal variables
    extern bool Initialized;
    extern std::string LoadedInterfaceName;
    extern TGfxSpritesheet* MainSpritesheet;
    extern TGfxSpritesheet* InterfaceSpritesheet;
    extern TGfxFont Fonts[2];
    extern TFontStyle FontStyles[FONT_LAST + 1];
    extern TGfxTexture ActionSnapTexture;
    extern TGfxTexture RenderTarget;
    extern TGfxTexture RenderTargetAA;
    extern std::string ScreenshotPath;
    extern bool ScreenshotAsync;
    extern float ImageScale[GFXID_END + 1];
    extern TStringList* GostekData;
    extern struct ScaleData {
        TStringList* Root;
        TStringList* CurrentMod;
        TStringList* CustomInterface;
    } ScaleData;

    inline void LoadModInfo() {
        // Load required ini files and process them
        // Implementation would handle reading mod info, scale data, etc.
    }

    inline float GetImageScale(const std::string& ImagePath) {
        // Implementation for getting image scale from data files
        // This would process the image path and return appropriate scale value
        std::string intDir = "custom-interfaces/" + Util::LowerCase(GameRenderingParams.InterfaceName) + "/";
        std::string path = Util::LowerCase(ImagePath);
        
        // Process path and return appropriate scale
        return 1.0f; // Default value
    }

    inline void TakeScreenshot(const std::string& Filename, bool Async = true) {
        ScreenshotPath = Filename;
        ScreenshotAsync = Async;
    }

    inline std::string PngOverride(const std::string& Filename) {
        // Replace extension with .png and normalize path separators
        std::string result = Util::StringReplace(Filename, "\\", "/", Util::TReplaceFlags::rfReplaceAll);
        size_t lastDot = result.find_last_of('.');
        if (lastDot != std::string::npos) {
            result = result.substr(0, lastDot);
        }
        result += ".png";
        return result;
    }

    inline void LoadMainTextures() {
        // Implementation to load main textures
        // This would process the LOAD_DATA array and create spritesheets
    }

    inline void LoadInterfaceTextures(const std::string& InterfaceName) {
        // Implementation to load interface textures
        // This would handle custom interface loading
    }

    inline void LoadInterface() {
        if (LoadInterfaceData(GameRenderingParams.InterfaceName)) {
            LoadInterfaceTextures(GameRenderingParams.InterfaceName);
        } else {
            LoadInterfaceTextures("");
        }
        LoadedInterfaceName = GameRenderingParams.InterfaceName;
    }

    inline std::string GetFontPath(const std::string& FontFile) {
        std::string result = "";
        if (Util::FileExists(BaseDirectory + FontFile)) {
            result = BaseDirectory + FontFile;
        }
        return result;
    }

    inline std::string GetFontPath(const std::string& Fallback, std::string& FontFile) {
        std::string result = "";

        if (FontFile != "") {
            result = GetFontPath(FontFile);
        }

        if (result == "") {
            result = GetFontPath(FontFile);
        }

        if (result == "") {
            FontFile = Fallback;
            result = GetFontPath(FontFile);
        }
        return result;
    }

    inline void LoadFonts() {
        std::string FontFile[3]; // 1-indexed
        FontFile[1] = font_1_filename.Value;
        FontFile[2] = font_2_filename.Value;

        std::string FontPath[3];
        FontPath[1] = GetFontPath(DEFAULT_FONT, FontFile[1]);
        FontPath[2] = GetFontPath(DEFAULT_FONT, FontFile[2]);

        if ((FontPath[1] == "") || (FontPath[2] == "")) {
            // Show error message and shutdown
            // ShowMessage(_("One of the fonts cannot be found. Please check your installation directory."));
            // Shutdown();
        }

        int w = RenderWidth;
        int h = RenderHeight;
        float s = r_scaleinterface.Value ? (float)RenderHeight / GameHeight : 1.0f;

        // Create fonts
        Fonts[1] = GfxCreateFont(FontPath[1].c_str(), Npot(w / 2), Npot(h / 2));
        Fonts[2] = GfxCreateFont(FontPath[2].c_str(), Npot(w / 3), Npot(h / 3));

        // Set up font styles
        FontStyles[FONT_SMALL].Font = Fonts[2];
        FontStyles[FONT_SMALL].Size = s * font_consolesize.Value;
        FontStyles[FONT_SMALL].Stretch = font_2_scale.Value / 100;
        FontStyles[FONT_SMALL].Flags = 0;

        // bold not supported for now so same as FONT_SMALL
        FontStyles[FONT_SMALL_BOLD].Font = Fonts[2];
        FontStyles[FONT_SMALL_BOLD].Size = s * font_consolesize.Value;
        FontStyles[FONT_SMALL_BOLD].Stretch = font_2_scale.Value / 100;
        FontStyles[FONT_SMALL_BOLD].Flags = 0;

        FontStyles[FONT_SMALLEST].Font = Fonts[2];
        FontStyles[FONT_SMALLEST].Size = s * font_consolesmallsize.Value;
        FontStyles[FONT_SMALLEST].Stretch = font_2_scale.Value / 100;
        FontStyles[FONT_SMALLEST].Flags = 0;

        FontStyles[FONT_BIG].Font = Fonts[1];
        FontStyles[FONT_BIG].Size = font_bigsize.Value;
        FontStyles[FONT_BIG].Stretch = font_1_scale.Value / 100;
        FontStyles[FONT_BIG].Flags = 0;

        FontStyles[FONT_MENU].Font = Fonts[1];
        FontStyles[FONT_MENU].Size = s * font_menusize.Value;
        FontStyles[FONT_MENU].Stretch = font_1_scale.Value / 100;
        FontStyles[FONT_MENU].Flags = 0;

        FontStyles[FONT_WEAPONS_MENU].Font = Fonts[2];
        FontStyles[FONT_WEAPONS_MENU].Size = s * font_weaponmenusize.Value;
        FontStyles[FONT_WEAPONS_MENU].Stretch = font_2_scale.Value / 100;
        FontStyles[FONT_WEAPONS_MENU].Flags = 0;

        FontStyles[FONT_WORLD].Font = Fonts[1];
        FontStyles[FONT_WORLD].Size = 128 * ((float)RenderHeight / GameHeight);
        FontStyles[FONT_WORLD].Stretch = font_1_scale.Value / 100;
        FontStyles[FONT_WORLD].Flags = 0;

        for (int i = 0; i <= FONT_LAST; i++) {
            if (FontStyles[i].Size < 10) {
                FontStyles[i].Flags = GFX_MONOCHROME;
            }

            FontStyles[i].TableIndex = GfxSetFont(FontStyles[i].Font,
                FontStyles[i].Size, FontStyles[i].Flags, FontStyles[i].Stretch);
        }
    }

    inline bool InitGameGraphics() {
        if (Initialized) {
            if (GameRenderingParams.InterfaceName != LoadedInterfaceName) {
                if (ScaleData.CustomInterface != nullptr) {
                    delete ScaleData.CustomInterface;
                    ScaleData.CustomInterface = nullptr;
                }

                LoadModInfo();
                LoadInterface();
            }

            return true;
        }

        uint32_t WindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

        if (r_fullscreen.Value == 2) {
            WindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else if (r_fullscreen.Value == 1) {
            WindowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        // Set OpenGL attributes for MSAA if enabled
        if (r_msaa.Value > 0) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, r_msaa.Value);
        }

        GameWindow = SDL_CreateWindow("Soldat",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WindowWidth, WindowHeight, WindowFlags);

        // Load icon file
        // FileBuffer := PHYSFS_readBuffer('icon.bmp');
        // IconFile := SDL_RWFromMem(FileBuffer, Length(FileBuffer));
        // SDL_SetWindowIcon(GameWindow, SDL_LoadBMP_RW(IconFile, 1));

        if (GameWindow == nullptr) {
            // ShowMessage('Error creating sdl2 window');
            return false;
        }

        if (!GfxInitContext(GameWindow, r_dithering.Value, r_compatibility.Value)) {
            return false;
        }

        StartInput();

        if (SDL_GL_SetSwapInterval(r_swapeffect.Value) == -1) {
            GfxLog("Error while setting SDL_GL_SetSwapInterval: " + std::string(SDL_GetError()));
        }

        GfxViewport(0, 0, WindowWidth, WindowHeight);

        Textures.resize(GFXID_END + 1);
        LoadModInfo();
        LoadMainTextures();
        LoadInterface();
        LoadFonts();

        // Map.LoadGraphics := @LoadMapGraphics;
        if (!GfxFramebufferSupported) {
            cl_actionsnap.SetValue(false);
        }

        if (cl_actionsnap.Value) {
            ActionSnapTexture = GfxCreateRenderTarget(RenderWidth, RenderHeight, 4, true);
        }

        if (GfxFramebufferSupported) {
            if ((WindowWidth != RenderWidth) || (WindowHeight != RenderHeight)) {
                RenderTarget = GfxCreateRenderTarget(RenderWidth, RenderHeight, 4, true);

                if (RenderTarget.Samples > 0) {
                    RenderTargetAA = GfxCreateRenderTarget(RenderWidth, RenderHeight, 4, false);

                    if (r_resizefilter.Value >= 2) {
                        GfxTextureFilter(RenderTargetAA, GFX_LINEAR, GFX_LINEAR);
                    } else {
                        GfxTextureFilter(RenderTargetAA, GFX_NEAREST, GFX_NEAREST);
                    }
                } else {
                    if (r_resizefilter.Value >= 2) {
                        GfxTextureFilter(RenderTarget, GFX_LINEAR, GFX_LINEAR);
                    } else {
                        GfxTextureFilter(RenderTarget, GFX_NEAREST, GFX_NEAREST);
                    }
                }
            }
        }

        Initialized = true;
        return true;
    }

    inline void ReloadGraphics() {
        // Implementation to reload graphics resources
        // This would handle reloading textures, fonts, etc.
    }

    inline void DestroyGameGraphics() {
        if (!Initialized) {
            return;
        }

        delete MainSpritesheet;
        MainSpritesheet = nullptr;
        
        delete InterfaceSpritesheet;
        InterfaceSpritesheet = nullptr;

        for (int i = 1; i < 3; i++) { // Using 1-indexed like Pascal
            GfxDeleteFont(Fonts[i]);
        }

        if (ActionSnapTexture.ptr != nullptr) {
            GfxDeleteTexture(ActionSnapTexture);
        }

        if (RenderTarget.ptr != nullptr) {
            GfxDeleteTexture(RenderTarget);
        }

        if (RenderTargetAA.ptr != nullptr) {
            GfxDeleteTexture(RenderTargetAA);
        }

        DestroyMapGraphics();
        GfxDestroyContext();

        Initialized = false;
    }

    inline float Lerp(float a, float b, float x) {
        return a + (b - a) * x;
    }

    inline TVector2 Lerp(const TVector2& a, const TVector2& b, float x) {
        TVector2 result;
        result.x = a.x + (b.x - a.x) * x;
        result.y = a.y + (b.y - a.y) * x;
        return result;
    }

    inline void InterpolateState(double p, TInterpolationState& s, bool Paused) {
        // Implementation for interpolating game state for smooth rendering
        // This would handle camera, mouse, sprite, bullet, spark, and thing interpolation
    }

    inline void RestoreState(TInterpolationState& s) {
        // Implementation for restoring game state after interpolation
        // This would restore camera, mouse, sprite, bullet, spark, and thing positions
    }

    inline void RenderFrame(double TimeElapsed, double FramePercent, bool Paused) {
        // Implementation for rendering the game frame
        // This would handle the complete rendering pipeline
    }

    inline void RenderGameInfo(const std::wstring& TextString) {
        // Implementation for rendering game info screen
        // This would draw the text in the center of the screen with background
    }

    inline bool ArrayContains(const std::vector<int>& List, int x) {
        for (int val : List) {
            if (val == x) {
                return true;
            }
        }
        return false;
    }

    inline bool GetSizeConstraint(int id, int& w, int& h) {
        // Array of weapon texture IDs
        const std::vector<int> WEAPONS_LIST = {
            GFX_WEAPONS_AK74,        GFX_WEAPONS_AK74_2,
            GFX_WEAPONS_AK74_FIRE,   GFX_WEAPONS_MINIMI,
            GFX_WEAPONS_MINIMI_2,    GFX_WEAPONS_RUGER,
            GFX_WEAPONS_RUGER_2,     GFX_WEAPONS_MP5,
            GFX_WEAPONS_MP5_2,       GFX_WEAPONS_SPAS,
            GFX_WEAPONS_SPAS_2,      GFX_WEAPONS_M79,
            GFX_WEAPONS_M79_2,       GFX_WEAPONS_DEAGLES,
            GFX_WEAPONS_DEAGLES_2,   GFX_WEAPONS_N_DEAGLES,
            GFX_WEAPONS_N_DEAGLES_2, GFX_WEAPONS_STEYR,
            GFX_WEAPONS_STEYR_2,     GFX_WEAPONS_BARRETT,
            GFX_WEAPONS_BARRETT_2,   GFX_WEAPONS_MINIGUN,
            GFX_WEAPONS_MINIGUN_2,   GFX_WEAPONS_SOCOM,
            GFX_WEAPONS_SOCOM_2,     GFX_WEAPONS_N_SOCOM,
            GFX_WEAPONS_N_SOCOM_2,   GFX_WEAPONS_BOW,
            GFX_WEAPONS_BOW_S,       GFX_WEAPONS_FLAMER,
            GFX_WEAPONS_FLAMER_2,    GFX_WEAPONS_KNIFE,
            GFX_WEAPONS_KNIFE2,      GFX_WEAPONS_CHAINSAW,
            GFX_WEAPONS_CHAINSAW2
        };

        if ((id >= GFX_GOSTEK_STOPA) && (id <= GFX_GOSTEK_TEAM2_LECISTOPA2)) {
            w = GOS_RESTRICT_WIDTH;
            h = GOS_RESTRICT_HEIGHT;
            return true;
        } else if (ArrayContains(WEAPONS_LIST, id)) {
            w = WEP_RESTRICT_WIDTH;
            h = WEP_RESTRICT_HEIGHT;
            return true;
        }
        return false;
    }

    inline bool DoTextureLoading(bool FinishLoading = false) {
        // Implementation for texture loading
        // This would handle the loading of textures from spritesheets
        return true;
    }

} // namespace GameRenderingImpl

using GameRenderingImpl::TGameRenderingParams;
using GameRenderingImpl::GameRenderingParams;
using GameRenderingImpl::Textures;
using GameRenderingImpl::InitGameGraphics;
using GameRenderingImpl::ReloadGraphics;
using GameRenderingImpl::DestroyGameGraphics;
using GameRenderingImpl::RenderFrame;
using GameRenderingImpl::RenderGameInfo;
using GameRenderingImpl::DoTextureLoading;
using GameRenderingImpl::SetFontStyle;
using GameRenderingImpl::FontStyleSize;
using GameRenderingImpl::TakeScreenshot;
using GameRenderingImpl::PngOverride;

#endif // GAMERENDERING_H