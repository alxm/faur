/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_software_blit.v.h"
#include <faur.v.h>

#if F_CONFIG_SCREEN_RENDER == F_SCREEN_RENDER_SOFTWARE
#define F__SCANLINES_MALLOC (F_CONFIG_SCREEN_SIZE_HEIGHT < 0)

typedef struct {
    #if F__SCANLINES_MALLOC
        int* screen; // one end of the horizontal screen span to draw to
        FVecFix* sprite; // one end of the sprite line to interpolate along
    #else
        int screen[F_CONFIG_SCREEN_SIZE_HEIGHT];
        FVecFix sprite[F_CONFIG_SCREEN_SIZE_HEIGHT];
    #endif
} FScanlineEdge;

typedef struct {
    FSpriteWord framesNum;
    FSpriteWord* spans[1]; // [framesNum]
} FTexture;

typedef void (*FCallBlitter)(const FTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
typedef void (*FCallBlitterEx)(const FPixels* Pixels, unsigned Frame, int TopY, int BottomY);

static FScanlineEdge g_edges[2];

// Interpolate sprite side (SprP1, SprP2) along screen line (ScrP1, ScrP2).
// ScrP1.y <= ScrP2.y and at least part of this range is on screen.
static void scan_line(FScanlineEdge* Edge, FVecInt ScrP1, FVecInt ScrP2, FVecFix SprP1, FVecFix SprP2)
{
    // Happens when sprite angle is a multiple of 90deg,
    // and 2 of the sprite's opposite sides are 0-height in screen space.
    if(ScrP1.y == ScrP2.y
        || ScrP1.y >= f__screen.clipEnd.y || ScrP2.y < f__screen.clipStart.y) {

        return;
    }

    FFix scrX = f_fix_fromInt(ScrP1.x);
    int scrDY = ScrP2.y - ScrP1.y + 1;
    FFix scrIncX = f_fix_fromInt(ScrP2.x - ScrP1.x + 1) / scrDY;

    FFix sprX = SprP1.x;
    FFix sprY = SprP1.y;
    FFix sprDX = SprP2.x - SprP1.x + (SprP2.x > SprP1.x) - (SprP2.x < SprP1.x);
    FFix sprDY = SprP2.y - SprP1.y + (SprP2.y > SprP1.y) - (SprP2.y < SprP1.y);
    int sprIncX = sprDX / scrDY;
    int sprIncY = sprDY / scrDY;

    if(ScrP1.y < 0) {
        scrX += scrIncX * -ScrP1.y;

        sprX += sprIncX * -ScrP1.y;
        sprY += sprIncY * -ScrP1.y;

        ScrP1.y = 0;
    }

    if(ScrP2.y >= f__screen.pixels->size.y) {
        ScrP2.y = f__screen.pixels->size.y - 1;
    }

    int* sideScreen = Edge->screen;
    FVecFix* sideSprite = Edge->sprite;

    for(int scrY = ScrP1.y; scrY <= ScrP2.y; scrY++, scrX += scrIncX) {
        sideScreen[scrY] = f_fix_toInt(scrX);

        sideSprite[scrY].x = sprX;
        sideSprite[scrY].y = sprY;

        sprX += sprIncX;
        sprY += sprIncY;
    }
}

#define F__FUNC_NAME(ColorKey, Clip) F_GLUE5(f_blit__, F__BLEND, F__FILL, ColorKey, Clip)
#define F__FUNC_NAME_EX F_GLUE4(f_blitEx__, F__BLEND, F__FILL, F__COLORKEY)
#define F__PIXEL_DRAW(Dst) F_GLUE2(f_color__draw_, F__BLEND)(Dst F__PIXEL_PARAMS)

#define F__BLEND solid
#define F__FILL Data
#define F__BLEND_SETUP
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , *src
#include "f_software_blit.inc.c"

#define F__BLEND solid
#define F__FILL Flat
#define F__BLEND_SETUP const FColorPixel color = f__color.pixel;
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , color
#include "f_software_blit.inc.c"

#define F__BLEND alpha
#define F__FILL Data
#define F__BLEND_SETUP \
    const int alpha = f__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
#define F__PIXEL_PARAMS , &rgb, alpha
#include "f_software_blit.inc.c"

#define F__BLEND alpha
#define F__FILL Flat
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb, alpha
#include "f_software_blit.inc.c"

#if F__OPTIMIZE_ALPHA
    #define F__BLEND alpha25
    #define F__FILL Data
    #define F__BLEND_SETUP
    #define F__PIXEL_USE_SRC
    #define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"

    #define F__BLEND alpha25
    #define F__FILL Flat
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_SETUP
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"

    #define F__BLEND alpha50
    #define F__FILL Data
    #define F__BLEND_SETUP
    #define F__PIXEL_USE_SRC
    #define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"

    #define F__BLEND alpha50
    #define F__FILL Flat
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_SETUP
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"

    #define F__BLEND alpha75
    #define F__FILL Data
    #define F__BLEND_SETUP
    #define F__PIXEL_USE_SRC
    #define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"

    #define F__BLEND alpha75
    #define F__FILL Flat
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_SETUP
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_blit.inc.c"
#endif // F__OPTIMIZE_ALPHA

#define F__BLEND alphaMask
#define F__FILL Data
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha;
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb, alpha, f_color_pixelToRgbAny(*src)
#include "f_software_blit.inc.c"

#define F__BLEND alphaMask
#define F__FILL Flat
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha;
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb, alpha, f_color_pixelToRgbAny(*src)
#include "f_software_blit.inc.c"

#define F__BLEND inverse
#define F__FILL Data
#define F__BLEND_SETUP
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS
#include "f_software_blit.inc.c"

#define F__BLEND inverse
#define F__FILL Flat
#define F__BLEND_SETUP
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS
#include "f_software_blit.inc.c"

#define F__BLEND mod
#define F__FILL Data
#define F__BLEND_SETUP
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__BLEND mod
#define F__FILL Flat
#define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__BLEND add
#define F__FILL Data
#define F__BLEND_SETUP
#define F__PIXEL_USE_SRC
#define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__BLEND add
#define F__FILL Flat
#define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__INIT_BLEND(Index, Name)                      \
    [Index][0][0][0] = f_blit__##Name##DataBlockNoClip, \
    [Index][0][0][1] = f_blit__##Name##DataBlockDoClip, \
    [Index][0][1][0] = f_blit__##Name##DataKeyedNoClip, \
    [Index][0][1][1] = f_blit__##Name##DataKeyedDoClip, \
    [Index][1][0][0] = f_blit__##Name##FlatBlockNoClip, \
    [Index][1][0][1] = f_blit__##Name##FlatBlockDoClip, \
    [Index][1][1][0] = f_blit__##Name##FlatKeyedNoClip, \
    [Index][1][1][1] = f_blit__##Name##FlatKeyedDoClip, \

// [Blend][Fill][ColorKey][Clip]
static const FCallBlitter g_blitters[F_COLOR_BLEND_NUM][2][2][2] = {
    F__INIT_BLEND(F_COLOR_BLEND_SOLID, solid)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA, alpha)
    #if F__OPTIMIZE_ALPHA
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_25, alpha25)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_50, alpha50)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_75, alpha75)
    #else
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_25, alpha)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_50, alpha)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_75, alpha)
    #endif
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA_MASK, alphaMask)
    F__INIT_BLEND(F_COLOR_BLEND_INVERSE, inverse)
    F__INIT_BLEND(F_COLOR_BLEND_MOD, mod)
    F__INIT_BLEND(F_COLOR_BLEND_ADD, add)
};

#define F__INIT_BLEND_EX(Index, Name)            \
    [Index][0][0] = f_blitEx__##Name##DataBlock, \
    [Index][0][1] = f_blitEx__##Name##DataKeyed, \
    [Index][1][0] = f_blitEx__##Name##FlatBlock, \
    [Index][1][1] = f_blitEx__##Name##FlatKeyed, \

// [Blend][Fill][ColorKey]
static const FCallBlitterEx g_blittersEx[F_COLOR_BLEND_NUM][2][2] = {
    F__INIT_BLEND_EX(F_COLOR_BLEND_SOLID, solid)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA, alpha)
    #if F__OPTIMIZE_ALPHA
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_25, alpha25)
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_50, alpha50)
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_75, alpha75)
    #else
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_25, alpha)
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_50, alpha)
        F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_75, alpha)
    #endif
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_MASK, alphaMask)
    F__INIT_BLEND_EX(F_COLOR_BLEND_INVERSE, inverse)
    F__INIT_BLEND_EX(F_COLOR_BLEND_MOD, mod)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ADD, add)
};

void f_platform_software_blit__init(void)
{
    #if F__SCANLINES_MALLOC
        unsigned height = (unsigned)f_platform_api__screenSizeGet().y;

        for(int i = 2; i--; ) {
            g_edges[i].screen =
                f_mem_malloc((unsigned)height * sizeof(int));

            g_edges[i].sprite =
                f_mem_malloc((unsigned)height * sizeof(FVecFix));
        }
    #endif
}

void f_platform_software_blit__uninit(void)
{
    #if F__SCANLINES_MALLOC
        for(int i = 2; i--; ) {
            f_mem_free(g_edges[i].screen);
            f_mem_free(g_edges[i].sprite);
        }
    #endif
}

static FSpriteWord* spansNew(const FPixels* Pixels, unsigned Frame)
{
    const FColorPixel* bufferStart = f_pixels__bufferGetStart(Pixels, Frame);
    const FColorPixel* buffer = bufferStart;
    bool transparent = false;

    for(unsigned i = Pixels->bufferLen; i--; ) {
        if(*buffer++ == f_color__key) {
            transparent = true;
            break;
        }
    }

    if(!transparent) {
        return NULL;
    }

    // Spans format for each scanline:
    // (NumSpans << 1 | start draw/transparent), len0, len1, ...

    size_t bytesNeeded = 0;
    buffer = bufferStart;

    for(int y = Pixels->size.y; y--; ) {
        bytesNeeded += sizeof(FSpriteWord); // NumSpans, initial state
        bool doDraw = *buffer != f_color__key; // initial state

        for(int x = Pixels->size.x; x--; ) {
            if((*buffer++ != f_color__key) != doDraw) {
                bytesNeeded += sizeof(FSpriteWord); // length of span
                doDraw = !doDraw;
            }
        }

        bytesNeeded += sizeof(FSpriteWord); // length of last span
    }

    FSpriteWord* spansStart = f_mem_malloc(bytesNeeded);
    FSpriteWord* spans = spansStart;
    buffer = bufferStart;

    for(int y = Pixels->size.y; y--; ) {
        FSpriteWord* lineStart = spans;
        FSpriteWord spanLength = 0;

        bool doDraw = *buffer != f_color__key; // initial state
        *spans++ = doDraw;

        for(int x = Pixels->size.x; x--; ) {
            if((*buffer++ != f_color__key) == doDraw) {
                spanLength++; // keep growing current span
            } else {
                *spans++ = spanLength; // record the just-ended span length
                spanLength = 1; // start a new span from this pixel
                doDraw = !doDraw;
            }
        }

        *lineStart |= (FSpriteWord)((spans - lineStart) << 1); // # of spans
        *spans++ = spanLength; // record the last span's length
    }

    return spansStart;
}

FPlatformTexture* f_platform_api_software__textureNew(const FPixels* Pixels)
{
    FTexture* t = f_mem_malloc(
                    sizeof(FTexture)
                        + (Pixels->framesNum - 1) * sizeof(FSpriteWord*));

    t->framesNum = (FSpriteWord)Pixels->framesNum;

    for(unsigned f = Pixels->framesNum; f--; ) {
        t->spans[f] = spansNew(Pixels, f);
    }

    return t;
}

FPlatformTexture* f_platform_api_software__textureDup(const FPlatformTexture* Texture, const FPixels* Pixels)
{
    F_UNUSED(Pixels);

    const FTexture* texSrc = Texture;
    FTexture* texDst = f_mem_mallocz(
                        sizeof(FTexture)
                            + Pixels->framesNum * sizeof(FSpriteWord*));

    texDst->framesNum = texSrc->framesNum;

    for(unsigned f = texSrc->framesNum; f--; ) {
        if(texSrc->spans[f]) {
            FSpriteWord num = texSrc->spans[f][0] >> 1;

            texDst->spans[f] = f_mem_dup(texSrc->spans[f],
                                         (num + 1u) * sizeof(FSpriteWord));
        }
    }

    return texDst;
}

void f_platform_api_software__textureFree(FPlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    FTexture* texture = Texture;

    for(unsigned f = texture->framesNum; f--; ) {
        f_mem_free(texture->spans[f]);
    }

    f_mem_free(texture);
}

void f_platform_api_software__textureUpdate(FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame)
{
    FTexture* texture = Texture;

    f_mem_free(texture->spans[Frame]);

    texture->spans[Frame] = spansNew(Pixels, Frame);
}

void f_platform_api_software__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    if(!f_screen_boxOnClip(X, Y, Pixels->size.x, Pixels->size.y)) {
        return;
    }

    g_blitters
        [f__color.blend]
        [f__color.fillBlit]
        [((FTexture*)Texture)->spans[Frame] != NULL]
        [!f_screen_boxInsideClip(X, Y, Pixels->size.x, Pixels->size.y)]
            (Texture, Pixels, Frame, X, Y);
}

void f_platform_api_software__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    const FVecInt size = Pixels->size;
    const FVecFix sizeScaled = {size.x * Scale, size.y * Scale};
    const FVecFix sizeScaledHalf = {sizeScaled.x / 2, sizeScaled.y / 2};

    /*
         Counter-clockwise rotations:

                  p0 --- p1           p1 --- p2
                  |       |           |       |
                  p3 --- p2           p0 --- p3

                    0 deg               90 deg
    */

    const FFix sin = f_fix_sin(Angle);
    const FFix cos = f_fix_cos(Angle);

    const FFix wLeft = sizeScaledHalf.x + f_fix_mul(CenterX, sizeScaledHalf.x);
    const FFix wRight = sizeScaled.x - wLeft;
    const FFix hTop = sizeScaledHalf.y + f_fix_mul(CenterY, sizeScaledHalf.y);
    const FFix hDown = sizeScaled.y - hTop;

    const FFix xMns = -wLeft;
    const FFix xPls = wRight - 1;
    const FFix yMns = -hTop;
    const FFix yPls = hDown - 1;

    #define ROTATE_X(x, y) f_fix_toInt(f_fix_mul(x,  cos) + f_fix_mul(y, sin))
    #define ROTATE_Y(x, y) f_fix_toInt(f_fix_mul(x, -sin) + f_fix_mul(y, cos))

    const FVecInt
        p0 = {X + ROTATE_X(xMns, yMns), Y + ROTATE_Y(xMns, yMns)},
        p1 = {X + ROTATE_X(xPls, yMns), Y + ROTATE_Y(xPls, yMns)},
        p2 = {X + ROTATE_X(xPls, yPls), Y + ROTATE_Y(xPls, yPls)},
        p3 = {X + ROTATE_X(xMns, yPls), Y + ROTATE_Y(xMns, yPls)};

    FVecInt screenTop, screenBottom, screenLeft, screenRight;
    FVecFix spriteTop, spriteBottom, spriteMidleft, spriteMidright;

    FVecFix
        sprite0 = {0, 0},
        sprite1 = {f_fix_fromInt(size.x) - 1, 0},
        sprite2 = {f_fix_fromInt(size.x) - 1, f_fix_fromInt(size.y) - 1},
        sprite3 = {0, f_fix_fromInt(size.y) - 1};

    // Based on Angle ranges, determine the top and bottom y coords
    // of the rotated sprite and the sides to interpolate.
    if(Angle < F_DEG_090_INT) {
        screenTop = p1;
        screenBottom = p3;
        screenLeft = p0;
        screenRight = p2;

        spriteTop = sprite1;
        spriteBottom = sprite3;
        spriteMidleft = sprite0;
        spriteMidright = sprite2;
    } else if(Angle >= F_DEG_090_INT && Angle < F_DEG_180_INT) {
        screenTop = p2;
        screenBottom = p0;
        screenLeft = p1;
        screenRight = p3;

        spriteTop = sprite2;
        spriteBottom = sprite0;
        spriteMidleft = sprite1;
        spriteMidright = sprite3;
    } else if(Angle >= F_DEG_180_INT && Angle < F_DEG_270_INT) {
        screenTop = p3;
        screenBottom = p1;
        screenLeft = p2;
        screenRight = p0;

        spriteTop = sprite3;
        spriteBottom = sprite1;
        spriteMidleft = sprite2;
        spriteMidright = sprite0;
    } else { // if(Angle >= F_DEG_270_INT) {
        screenTop = p0;
        screenBottom = p2;
        screenLeft = p3;
        screenRight = p1;

        spriteTop = sprite0;
        spriteBottom = sprite2;
        spriteMidleft = sprite3;
        spriteMidright = sprite1;
    }

    if(!f_screen_boxOnClip(screenLeft.x,
                           screenTop.y,
                           screenRight.x - screenLeft.x + 1,
                           screenBottom.y - screenTop.y + 1)) {

        return;
    }

    scan_line(
        &g_edges[0], screenTop, screenLeft, spriteTop, spriteMidleft);
    scan_line(
        &g_edges[0], screenLeft, screenBottom, spriteMidleft, spriteBottom);
    scan_line(
        &g_edges[1], screenTop, screenRight, spriteTop, spriteMidright);
    scan_line(
        &g_edges[1], screenRight, screenBottom, spriteMidright, spriteBottom);

    int yTop = f_math_max(screenTop.y, f__screen.clipStart.y);
    int yBottom = f_math_min(screenBottom.y, f__screen.clipEnd.y - 1);

    g_blittersEx
        [f__color.blend]
        [f__color.fillBlit]
        [Texture != NULL]
            (Pixels, Frame, yTop, yBottom);
}
#endif // F_CONFIG_SCREEN_RENDER == F_SCREEN_RENDER_SOFTWARE
