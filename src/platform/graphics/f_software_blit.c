/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_LIB_RENDER_SOFTWARE
#define F__COMPILE_INC 1
#define F__SCANLINES_MALLOC (F_CONFIG_SCREEN_SIZE_HEIGHT < 0)

typedef struct {
    #if F__SCANLINES_MALLOC
        int* screen; // one end of the horizontal screen span to draw to
        FVectorFix* sprite; // one end of the sprite line to interpolate along
    #else
        int screen[F_CONFIG_SCREEN_SIZE_HEIGHT];
        FVectorFix sprite[F_CONFIG_SCREEN_SIZE_HEIGHT];
    #endif
} FScanlineEdge;

struct FPlatformTexture {
    unsigned spans[1];
};

typedef void (*FBlitter)(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y);
typedef void (*FBlitterEx)(const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);

static FScanlineEdge g_edges[2];

// Interpolate sprite side (SprP1, SprP2) along screen line (ScrP1, ScrP2).
// ScrP1.y <= ScrP2.y and at least part of this range is on screen.
static void scan_line(FScanlineEdge* Edge, FVectorInt ScrP1, FVectorInt ScrP2, FVectorFix SprP1, FVectorFix SprP2)
{
    // Happens when sprite angle is a multiple of 90deg,
    // and 2 of the sprite's opposite sides are 0-height in screen space.
    if(ScrP1.y == ScrP2.y
        || ScrP1.y >= f__screen.pixels->size.y || ScrP2.y < 0) {

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
    FVectorFix* sideSprite = Edge->sprite;

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

#define F__BLEND plain
#define F__FILL Data
#define F__BLEND_SETUP
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , *src
#include "f_software_blit.inc.c"

#define F__BLEND plain
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

#define F__BLEND alpha25
#define F__FILL Data
#define F__BLEND_SETUP
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
#define F__PIXEL_SETUP const FColorRgb rgb = f_color_pixelToRgb(*src);
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__BLEND alpha75
#define F__FILL Flat
#define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb
#include "f_software_blit.inc.c"

#define F__BLEND alphaMask
#define F__FILL Data
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha;
#define F__PIXEL_SETUP
#define F__PIXEL_PARAMS , &rgb, alpha, f_color_pixelToRgbAny(*src)
#include "f_software_blit.inc.c"

#define F__BLEND alphaMask
#define F__FILL Flat
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha;
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
static const FBlitter g_blitters[F_COLOR_BLEND_NUM][2][2][2] = {
    F__INIT_BLEND(F_COLOR_BLEND_PLAIN, plain)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA, alpha)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA_25, alpha25)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA_50, alpha50)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA_75, alpha75)
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
static const FBlitterEx g_blittersEx[F_COLOR_BLEND_NUM][2][2] = {
    F__INIT_BLEND_EX(F_COLOR_BLEND_PLAIN, plain)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA, alpha)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_25, alpha25)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_50, alpha50)
    F__INIT_BLEND_EX(F_COLOR_BLEND_ALPHA_75, alpha75)
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
                f_mem_malloc((unsigned)height * sizeof(FVectorFix));
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

static bool hasTransparency(const FPixels* Pixels, unsigned Frame)
{
    const FColorPixel* buffer = f_pixels__bufferGetStart(Pixels, Frame);

    for(int i = Pixels->size.x * Pixels->size.y; i--; ) {
        if(*buffer++ == f_color__key) {
            return true;
        }
    }

    return false;
}

static size_t spansBytesNeeded(const FPixels* Pixels, unsigned Frame)
{
    // Spans format for each scanline:
    // (NumSpans << 1 | start draw/transparent), len0, len1, ...

    size_t bytesNeeded = 0;
    const FColorPixel* buffer = f_pixels__bufferGetStart(Pixels, Frame);

    for(int y = Pixels->size.y; y--; ) {
        bytesNeeded += sizeof(unsigned); // total size and initial state
        bool doDraw = *buffer != f_color__key; // initial state

        for(int x = Pixels->size.x; x--; ) {
            if((*buffer++ != f_color__key) != doDraw) {
                bytesNeeded += sizeof(unsigned); // length of new span
                doDraw = !doDraw;
            }
        }

        bytesNeeded += sizeof(unsigned); // line's last span length
    }

    return bytesNeeded;
}

static void spansUpdate(const FPixels* Pixels, unsigned Frame, FPlatformTexture* Texture)
{
    unsigned* spans = Texture->spans;
    const FColorPixel* buffer = f_pixels__bufferGetStart(Pixels, Frame);

    for(int y = Pixels->size.y; y--; ) {
        unsigned* lineStart = spans;
        unsigned spanLength = 0;

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

        *lineStart |= (unsigned)(spans - lineStart) << 1; // line's # of spans
        *spans++ = spanLength; // record the last span's length
    }
}

FPlatformTexture* f_platform_api__textureNew(const FPixels* Pixels, unsigned Frame)
{
    FPlatformTexture* texture = NULL;

    if(hasTransparency(Pixels, Frame)) {
        texture = f_mem_malloc(spansBytesNeeded(Pixels, Frame));
        spansUpdate(Pixels, Frame, texture);
    }

    return texture;
}

void f_platform_api__textureFree(FPlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    f_mem_free(Texture);
}

void f_platform_api__textureBlit(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    if(!f_screen_boxOnClip(X, Y, Pixels->size.x, Pixels->size.y)) {
        return;
    }

    g_blitters
        [f__color.blend]
        [f__color.fillBlit]
        [Texture != NULL]
        [!f_screen_boxInsideClip(X, Y, Pixels->size.x, Pixels->size.y)]
            (Texture, Pixels, Frame, X, Y);
}

void f_platform_api__textureBlitEx(const FPlatformTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    g_blittersEx
        [f__color.blend]
        [f__color.fillBlit]
        [Texture != NULL]
            (Pixels, Frame, X, Y, Scale, Angle, CenterX, CenterY);
}
#endif // F_CONFIG_LIB_RENDER_SOFTWARE
