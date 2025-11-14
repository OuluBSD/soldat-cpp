#ifndef GFX_H
#define GFX_H

//*******************************************************************************
//                                                                              
//       Gfx Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2015 Mariano Cuatrin                                 
//                                                                              
//*******************************************************************************

#include "Vector.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

// Constants
const int GFX_MONOCHROME = 1;

// Enums
enum TGfxTextureWrap {
    GFX_CLAMP,
    GFX_REPEAT
};

enum TGfxVerticalAlign {
    GFX_TOP,
    GFX_BASELINE,
    GFX_BOTTOM
};

enum TGfxTextureFilter {
    GFX_LINEAR,
    GFX_NEAREST,
    GFX_MIPMAP_LINEAR,
    GFX_MIPMAP_NEAREST
};

// Type definitions
typedef float* TGfxMat3;  // Array of 9 floats: [0..8]

struct TGfxColor {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t rgba;
    };
    
    TGfxColor() : r(0), g(0), b(0), a(255) {}
    TGfxColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) 
        : r(red), g(green), b(blue), a(alpha) {}
};

struct TGfxVertex {
    float x, y;
    float u, v;
    TGfxColor Color;
    
    TGfxVertex() : x(0), y(0), u(0), v(0), Color() {}
    TGfxVertex(float _x, float _y, float _u, float _v, const TGfxColor& _Color) 
        : x(_x), y(_y), u(_u), v(_v), Color(_Color) {}
};

struct TGfxRect {
    float Left, Right, Top, Bottom;
    
    TGfxRect() : Left(0), Right(0), Top(0), Bottom(0) {}
    TGfxRect(float l, float r, float t, float b) 
        : Left(l), Right(r), Top(t), Bottom(b) {}
};

// Forward declarations
class TGfxTexture;
class TGfxVertexBuffer;
class TGfxIndexBuffer;
class TGfxSpritesheet;
class TGfxImage;
typedef void* TGfxFont;

// Sprite definition
struct TGfxSprite {
    int x, y;
    int Width, Height;
    float Scale;
    int Delay;
    TGfxRect TexCoords;
    TGfxTexture* Texture;
    TGfxSprite* Next;
    
    TGfxSprite() : x(0), y(0), Width(0), Height(0), Scale(1.0f), Delay(0), 
                   TexCoords(), Texture(nullptr), Next(nullptr) {}
};

typedef std::vector<TGfxSprite*> TGfxSpriteArray;

// Function declarations
bool GfxFramebufferSupported();
bool GfxInitContext(void* Wnd, bool Dithering, bool FixedPipeline);
void GfxDestroyContext();
void GfxPresent(bool Finish);
void GfxTarget(TGfxTexture* RenderTarget);
void GfxBlit(TGfxTexture* Src, TGfxTexture* Dst, const TGfxRect& SrcRect, 
             const TGfxRect& DstRect, TGfxTextureFilter filter);
void GfxClear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GfxClear(const TGfxColor& c);
void GfxDraw(TGfxVertexBuffer* Buffer, int Offset, int Count);
void GfxDraw(TGfxVertexBuffer* Buffer, TGfxIndexBuffer* IndexBuffer, 
             int Offset, int Count);
void GfxDraw(TGfxVertexBuffer* Buffer, struct TGfxDrawCommand* Cmds, 
             int CmdCount);
void GfxDraw(TGfxVertexBuffer* Buffer, TGfxIndexBuffer* IndexBuffer, 
             struct TGfxDrawCommand* Cmds, int CmdCount);
void GfxViewport(int x, int y, int w, int h);
void GfxTransform(const float* t);
void GfxSpriteVertices(TGfxSprite* s, float x, float y, float w, float h, 
                       float sx, float sy, float cx, float cy, float r, 
                       const TGfxColor& Color, TGfxVertex* v);
void GfxSaveScreen(const std::string& Filename, int x, int y, int w, int h, 
                   bool Async = true);
void GfxSetMipmapBias(float Bias);

// Pseudo constructors
TGfxColor ARGB(uint32_t argb);
TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b);
TGfxColor RGBA(uint32_t rgba);
TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b, double a);
TGfxColor RGBA(uint32_t rgb, float a);
TGfxVertex GfxVertex(float x, float y, float u, float v, const TGfxColor& c);

// Texture functions
TGfxTexture* GfxCreateTexture(int w, int h, int c, uint8_t* Data = nullptr);
TGfxTexture* GfxCreateRenderTarget(int w, int h, int c, bool Msaa = false);
void GfxBindTexture(TGfxTexture* Texture);
void GfxTextureWrap(TGfxTexture* Texture, TGfxTextureWrap s, TGfxTextureWrap t);
void GfxTextureFilter(TGfxTexture* Texture, TGfxTextureFilter Min, 
                      TGfxTextureFilter Mag);
void GfxUpdateTexture(TGfxTexture* Texture, int x, int y, int w, int h, 
                      uint8_t* Data);
void GfxGenerateMipmap(TGfxTexture* Texture);
void GfxDeleteTexture(TGfxTexture*& Texture);

// Vertex buffer functions
TGfxVertexBuffer* GfxCreateBuffer(int Capacity, bool Static = false, 
                                  TGfxVertex* Data = nullptr);
void GfxUpdateBuffer(TGfxVertexBuffer* b, int i, int n, TGfxVertex* Data);
void GfxDeleteBuffer(TGfxVertexBuffer*& b);

// Index buffer functions
TGfxIndexBuffer* GfxCreateIndexBuffer(int Capacity, bool Static = false, 
                                      uint16_t* Data = nullptr);
void GfxUpdateIndexBuffer(TGfxIndexBuffer* b, int i, int n, uint16_t* Data);
void GfxDeleteIndexBuffer(TGfxIndexBuffer*& b);

// Font functions
TGfxFont GfxCreateFont(const std::string& Filename, int w = 512, int h = 512);
void GfxDeleteFont(TGfxFont& Font);
int GfxSetFont(TGfxFont Font, float FontSize, uint32_t Flags, float Stretch = 1.0f);
void GfxSetFontTable(TGfxFont Font, int TableIndex);
void GfxTextPixelRatio(const TVector2& PixelRatio);
void GfxTextScale(float s);
void GfxTextColor(const TGfxColor& Color);
void GfxTextShadow(float dx, float dy, const TGfxColor& Color);
void GfxTextVerticalAlign(TGfxVerticalAlign Align);
TGfxRect GfxTextMetrics();
TGfxRect GfxTextMetrics(const std::wstring& Text);
void GfxDrawText(float x, float y);
void GfxDrawText(const std::wstring& Text, float x, float y);
void GfxDrawText(const std::string& Text, float x, float y);

// Batching functions
void GfxBegin();
void GfxEnd();
void GfxDrawQuad(TGfxTexture* Texture, const std::vector<TGfxVertex>& Vertices);
void GfxDrawQuad(TGfxTexture* Texture, const TGfxVertex& a, const TGfxVertex& b, 
                 const TGfxVertex& c, const TGfxVertex& d);

void GfxDrawSprite(TGfxSprite* s, float x, float y);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float scale);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float rx, float ry, float r);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   float rx, float ry, float r);

void GfxDrawSprite(TGfxSprite* s, float x, float y, const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float scale, const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float rx, float ry, float r, 
                   const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   float rx, float ry, float r, const TGfxRect& rc);

void GfxDrawSprite(TGfxSprite* s, float x, float y, const TGfxColor& Color);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float scale, const TGfxColor& Color);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   const TGfxColor& Color);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float rx, float ry, float r, 
                   const TGfxColor& Color);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   float rx, float ry, float r, const TGfxColor& Color);

void GfxDrawSprite(TGfxSprite* s, float x, float y, const TGfxColor& Color, 
                   const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float scale, 
                   const TGfxColor& Color, const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   const TGfxColor& Color, const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float rx, float ry, float r, 
                   const TGfxColor& Color, const TGfxRect& rc);
void GfxDrawSprite(TGfxSprite* s, float x, float y, float sx, float sy, 
                   float rx, float ry, float r, const TGfxColor& Color, 
                   const TGfxRect& rc);

// Matrix functions
void GfxMat3Rot(float r, float* result);
void GfxMat3Ortho(float l, float r, float t, float b, float* result);
void GfxMat3Transform(float tx, float ty, float sx, float sy, float cx, float cy, 
                      float r, float* result);
TVector2 GfxMat3Mul(const float* m, float x, float y);

// Utility functions
int Npot(int x);
float RectWidth(const TGfxRect& Rect);
float RectHeight(const TGfxRect& Rect);

// External variable for logging
extern void (*GfxLog)(const std::string& s);

// Class declarations
class TGfxImage {
private:
    uint8_t* FData;
    int FWidth;
    int FHeight;
    int FComponents;
    int FNumFrames;
    bool FLoadedFromFile;

public:
    TGfxImage(const std::string& Filename, const TGfxColor& ColorKey);
    TGfxImage(int Width, int Height, int Comp = 4);
    ~TGfxImage();
    
    uint8_t* GetImageData(int Frame = 0);
    uint16_t GetFrameDelay(int Frame = 0);
    void Update(int x, int y, int w, int h, uint8_t* Data, int Frame = 0);
    void Premultiply();
    void Resize(int w, int h);
    
    int GetWidth() const { return FWidth; }
    int GetHeight() const { return FHeight; }
    int GetComponents() const { return FComponents; }
    int GetNumFrames() const { return FNumFrames; }
};

class TGfxTexture {
private:
    int FHandle;
    int FFboHandle;
    int FWidth;
    int FHeight;
    int FComponents;
    int FSamples;
    struct {
        int x, y;
        TGfxColor Color;
    } FPixel;
    
    void Update(int x, int y, int w, int h, uint8_t* Data);
    void SetWrap(TGfxTextureWrap s, TGfxTextureWrap t);
    void SetFilter(TGfxTextureFilter Min, TGfxTextureFilter Mag);

public:
    TGfxTexture(int Width, int Height, int Comp, bool Rt, bool Msaa, uint8_t* Data);
    ~TGfxTexture();
    
    int GetWidth() const { return FWidth; }
    int GetHeight() const { return FHeight; }
    int GetComponents() const { return FComponents; }
    int GetSamples() const { return FSamples; }
    
    friend void GfxTextureWrap(TGfxTexture* Texture, TGfxTextureWrap s, TGfxTextureWrap t);
    friend void GfxTextureFilter(TGfxTexture* Texture, TGfxTextureFilter Min, 
                                 TGfxTextureFilter Mag);
    friend void GfxUpdateTexture(TGfxTexture* Texture, int x, int y, int w, int h, 
                                 uint8_t* Data);
};

class TGfxVertexBuffer {
private:
    int FHandle;
    int FCapacity;
    void Update(int Offset, int Count, TGfxVertex* Data);

public:
    TGfxVertexBuffer(int Cap, bool Static, TGfxVertex* Data);
    ~TGfxVertexBuffer();
    
    int GetCapacity() const { return FCapacity; }
    
    friend void GfxUpdateBuffer(TGfxVertexBuffer* b, int i, int n, TGfxVertex* Data);
};

class TGfxIndexBuffer {
private:
    int FHandle;
    int FCapacity;
    void Update(int Offset, int Count, uint16_t* Data);

public:
    TGfxIndexBuffer(int Cap, bool Static, uint16_t* Data);
    ~TGfxIndexBuffer();
    
    int GetCapacity() const { return FCapacity; }
    
    friend void GfxUpdateIndexBuffer(TGfxIndexBuffer* b, int i, int n, uint16_t* Data);
};

struct TGfxDrawCommand {
    TGfxTexture* Texture;
    int Offset;
    int Count;
};

class TGfxSpritesheet {
private:
    std::vector<TGfxTexture*> FTextures;
    std::vector<TGfxSprite> FSprites;
    std::vector<TGfxSprite> FAdditionalSprites;
    void* FLoadData;
    
    TGfxSprite* GetSprite(int Index);
    TGfxTexture* GetTexture(int Index);
    int GetSpriteCount();
    int GetTextureCount();
    bool IsLoading();
    void LoadNextImage();
    void PackRects();
    void UpdateNextSprite();
    void UpdateTexture();
    void CleanUp();

public:
    TGfxSpritesheet(int Count);
    ~TGfxSpritesheet();
    
    void AddImage(const std::string& Path, const TGfxColor& ColorKey, float TargetScale);
    void AddImage(const std::string& Path, const TGfxColor& ColorKey, const TVector2& TargetSize);
    void AddImage(TGfxImage* Image);
    void Load();
    void StartLoading();
    void ContinueLoading();
    void FinishLoading();
    
    int GetSpriteCount() const;
    int GetTextureCount() const;
    TGfxSprite* operator[](int Index) { return GetSprite(Index); }
    bool Loading() const { return IsLoading(); }
};

namespace GfxImpl {

    inline TGfxColor ARGB(uint32_t argb) {
        return RGBA(argb, argb >> 24);
    }

    inline TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        TGfxColor result;
        result.r = r;
        result.g = g;
        result.b = b;
        result.a = a;
        return result;
    }

    inline TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b) {
        TGfxColor result;
        result.r = r;
        result.g = g;
        result.b = b;
        result.a = 255;
        return result;
    }

    inline TGfxColor RGBA(uint32_t rgba) {
        TGfxColor result;
        result.a = 255;
        result.r = (rgba & 0x00FF0000) >> 16;
        result.g = (rgba & 0x0000FF00) >> 8;
        result.b = (rgba & 0x000000FF);
        return result;
    }

    inline TGfxColor RGBA(uint8_t r, uint8_t g, uint8_t b, double a) {
        TGfxColor result;
        result.r = r;
        result.g = g;
        result.b = b;
        result.a = static_cast<uint8_t>(a);
        return result;
    }

    inline TGfxColor RGBA(uint32_t rgb, float a) {
        TGfxColor result;
        result.r = (rgb & 0xFF0000) >> 16;
        result.g = (rgb & 0x00FF00) >> 8;
        result.b = (rgb & 0x0000FF);
        result.a = static_cast<uint8_t>(a);
        return result;
    }

    inline TGfxVertex GfxVertex(float x, float y, float u, float v, const TGfxColor& c) {
        TGfxVertex result;
        result.x = x;
        result.y = y;
        result.u = u;
        result.v = v;
        result.Color = c;
        return result;
    }

    inline int Npot(int x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x < 2 ? 2 : x + 1;
    }

    inline float RectWidth(const TGfxRect& Rect) {
        return std::abs(Rect.Right - Rect.Left);
    }

    inline float RectHeight(const TGfxRect& Rect) {
        return std::abs(Rect.Bottom - Rect.Top);
    }

    // Matrix operations
    inline void GfxMat3Rot(float r, float* result) {
        float c = cosf(r);
        float s = sinf(r);
        
        result[0] = c; result[3] = -s; result[6] = 0;
        result[1] = s; result[4] = c;  result[7] = 0;
        result[2] = 0; result[5] = 0;  result[8] = 1;
    }

    inline void GfxMat3Ortho(float l, float r, float t, float b, float* result) {
        float w = r - l;
        float h = t - b;
        
        result[0] = 2.0f / w; result[3] = 0;       result[6] = -(r + l) / w;
        result[1] = 0;       result[4] = 2.0f / h; result[7] = -(t + b) / h;
        result[2] = 0;       result[5] = 0;       result[8] = 1;
    }

    inline void GfxMat3Transform(float tx, float ty, float sx, float sy, 
                                float cx, float cy, float r, float* result) {
        float c = cosf(r);
        float s = sinf(r);
        
        // m = T(tx,ty) * T(cx,cy) * R(r) * T(-cx,-cy) * S(sx,sy)
        result[0] = c * sx; result[3] = -s * sy; result[6] = tx + cy * s - c * cx + cx;
        result[1] = s * sx; result[4] = c * sy;  result[7] = ty - cx * s - c * cy + cy;
        result[2] = 0;      result[5] = 0;       result[8] = 1;
    }

    inline TVector2 GfxMat3Mul(const float* m, float x, float y) {
        TVector2 result;
        result.x = m[0] * x + m[3] * y + m[6];
        result.y = m[1] * x + m[4] * y + m[7];
        return result;
    }

} // namespace GfxImpl

using GfxImpl::ARGB;
using GfxImpl::RGBA;
using GfxImpl::GfxVertex;
using GfxImpl::Npot;
using GfxImpl::RectWidth;
using GfxImpl::RectHeight;
using GfxImpl::GfxMat3Rot;
using GfxImpl::GfxMat3Ortho;
using GfxImpl::GfxMat3Transform;
using GfxImpl::GfxMat3Mul;

#endif // GFX_H