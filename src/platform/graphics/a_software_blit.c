/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_software_blit.v.h"
#include <a2x.v.h>

#if A_CONFIG_LIB_RENDER_SOFTWARE
#define A__COMPILE_INC 1

enum {LEFT, RIGHT};

typedef struct {
    int screenX[2]; // screen horizontal span to draw to
    AVectorFix sprite[2]; // sprite line to interpolate and fill the span with
} AScanline;

struct APlatformTexture {
    unsigned spans[1];
};

typedef void (*ABlitter)(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y);
typedef void (*ABlitterEx)(const APixels* Pixels, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, AFix CenterX, AFix CenterY);

#if A_CONFIG_SCREEN_SIZE_HEIGHT < 0
    #define A__SCANLINES_MALLOC 1
#endif

#if A__SCANLINES_MALLOC
    static AScanline* g_scanlines;
#else
    static AScanline g_scanlines[A_CONFIG_SCREEN_SIZE_HEIGHT];
#endif

// Interpolate sprite side (SprP1, SprP2) along screen line (ScrP1, ScrP2).
// ScrP1.y <= ScrP2.y and at least part of this range is on screen.
static void scan_line(int Index, AVectorInt ScrP1, AVectorInt ScrP2, AVectorFix SprP1, AVectorFix SprP2)
{
    // Happens when sprite angle is a multiple of 90deg,
    // and 2 of the sprite's opposite sides are 0-height in screen space.
    if(ScrP1.y == ScrP2.y || ScrP1.y >= a__screen.pixels->h || ScrP2.y < 0) {
        return;
    }

    const int scrDY = ScrP2.y - ScrP1.y + 1;
    AFix scrX = a_fix_fromInt(ScrP1.x);
    const AFix scrXInc = a_fix_fromInt(ScrP2.x - ScrP1.x + 1) / scrDY;

    AFix sprX = SprP1.x;
    AFix sprY = SprP1.y;
    const AFix sprDX = SprP2.x - SprP1.x
                        + (SprP2.x > SprP1.x) - (SprP2.x < SprP1.x);
    const AFix sprDY = SprP2.y - SprP1.y
                        + (SprP2.y > SprP1.y) - (SprP2.y < SprP1.y);
    const AFix sprXInc = sprDX / scrDY;
    const AFix sprYInc = sprDY / scrDY;

    if(ScrP1.y < 0) {
        scrX += scrXInc * -ScrP1.y;

        sprX += sprXInc * -ScrP1.y;
        sprY += sprYInc * -ScrP1.y;

        ScrP1.y = 0;
    }

    if(ScrP2.y >= a__screen.pixels->h) {
        ScrP2.y = a__screen.pixels->h - 1;
    }

    for(int scrY = ScrP1.y; scrY <= ScrP2.y; scrY++, scrX += scrXInc) {
        g_scanlines[scrY].screenX[Index] = a_fix_toInt(scrX);

        g_scanlines[scrY].sprite[Index].x = sprX;
        g_scanlines[scrY].sprite[Index].y = sprY;

        sprX += sprXInc;
        sprY += sprYInc;
    }
}

#define A__FUNC_NAME(ColorKey, Clip) A_GLUE5(a_blit__, A__BLEND, A__FILL, ColorKey, Clip)
#define A__FUNC_NAME_EX A_GLUE4(a_blitEx__, A__BLEND, A__FILL, A__COLORKEY)
#define A__PIXEL_DRAW(Dst) A_GLUE2(a_color__draw_, A__BLEND)(Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , *src
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND plain
#define A__FILL Flat
#define A__BLEND_SETUP const APixel color = a__color.pixel;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , color
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL Data
#define A__BLEND_SETUP \
    const int alpha = a__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb, alpha
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL Flat
#define A__BLEND_SETUP \
    const ARgb rgb = a__color.rgb; \
    const int alpha = a__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb, alpha
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL Flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL Flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL Flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL Flat
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL Flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND add
#define A__FILL Data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__BLEND add
#define A__FILL Flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/a_software_blit.inc.c"

#define A__INIT_BLEND(Index, Name)                      \
    [Index][0][0][0] = a_blit__##Name##DataBlockNoClip, \
    [Index][0][0][1] = a_blit__##Name##DataBlockDoClip, \
    [Index][0][1][0] = a_blit__##Name##DataKeyedNoClip, \
    [Index][0][1][1] = a_blit__##Name##DataKeyedDoClip, \
    [Index][1][0][0] = a_blit__##Name##FlatBlockNoClip, \
    [Index][1][0][1] = a_blit__##Name##FlatBlockDoClip, \
    [Index][1][1][0] = a_blit__##Name##FlatKeyedNoClip, \
    [Index][1][1][1] = a_blit__##Name##FlatKeyedDoClip, \

// [Blend][Fill][ColorKey][Clip]
static const ABlitter g_blitters[A_COLOR_BLEND_NUM][2][2][2] = {
    A__INIT_BLEND(A_COLOR_BLEND_PLAIN, plain)
    A__INIT_BLEND(A_COLOR_BLEND_RGBA, rgba)
    A__INIT_BLEND(A_COLOR_BLEND_RGB25, rgb25)
    A__INIT_BLEND(A_COLOR_BLEND_RGB50, rgb50)
    A__INIT_BLEND(A_COLOR_BLEND_RGB75, rgb75)
    A__INIT_BLEND(A_COLOR_BLEND_INVERSE, inverse)
    A__INIT_BLEND(A_COLOR_BLEND_MOD, mod)
    A__INIT_BLEND(A_COLOR_BLEND_ADD, add)
};

#define A__INIT_BLEND_EX(Index, Name)            \
    [Index][0][0] = a_blitEx__##Name##DataBlock, \
    [Index][0][1] = a_blitEx__##Name##DataKeyed, \
    [Index][1][0] = a_blitEx__##Name##FlatBlock, \
    [Index][1][1] = a_blitEx__##Name##FlatKeyed, \

// [Blend][Fill][ColorKey]
static const ABlitterEx g_blittersEx[A_COLOR_BLEND_NUM][2][2] = {
    A__INIT_BLEND_EX(A_COLOR_BLEND_PLAIN, plain)
    A__INIT_BLEND_EX(A_COLOR_BLEND_RGBA, rgba)
    A__INIT_BLEND_EX(A_COLOR_BLEND_RGB25, rgb25)
    A__INIT_BLEND_EX(A_COLOR_BLEND_RGB50, rgb50)
    A__INIT_BLEND_EX(A_COLOR_BLEND_RGB75, rgb75)
    A__INIT_BLEND_EX(A_COLOR_BLEND_INVERSE, inverse)
    A__INIT_BLEND_EX(A_COLOR_BLEND_MOD, mod)
    A__INIT_BLEND_EX(A_COLOR_BLEND_ADD, add)
};

void a_platform_software_blit__init(void)
{
    #if A__SCANLINES_MALLOC
        g_scanlines = a_mem_malloc((unsigned)
                        a_platform_api__screenSizeGet().y * sizeof(AScanline));
    #endif
}

void a_platform_software_blit__uninit(void)
{
    #if A__SCANLINES_MALLOC
        a_mem_free(g_scanlines);
    #endif
}

static bool hasTransparency(const APixels* Pixels, unsigned Frame)
{
    const APixel* buffer = a_pixels__bufferGetStart(Pixels, Frame);

    for(int i = Pixels->w * Pixels->h; i--; ) {
        if(*buffer++ == a_color__key) {
            return true;
        }
    }

    return false;
}

static size_t spansBytesNeeded(const APixels* Pixels, unsigned Frame)
{
    // Spans format for each scanline:
    // (NumSpans << 1 | start draw/transparent), len0, len1, ...

    size_t bytesNeeded = 0;
    const APixel* buffer = a_pixels__bufferGetStart(Pixels, Frame);

    for(int y = Pixels->h; y--; ) {
        bytesNeeded += sizeof(unsigned); // total size and initial state
        bool doDraw = *buffer != a_color__key; // initial state

        for(int x = Pixels->w; x--; ) {
            if((*buffer++ != a_color__key) != doDraw) {
                bytesNeeded += sizeof(unsigned); // length of new span
                doDraw = !doDraw;
            }
        }

        bytesNeeded += sizeof(unsigned); // line's last span length
    }

    return bytesNeeded;
}

static void spansUpdate(const APixels* Pixels, unsigned Frame, APlatformTexture* Texture)
{
    unsigned* spans = Texture->spans;
    const APixel* buffer = a_pixels__bufferGetStart(Pixels, Frame);

    for(int y = Pixels->h; y--; ) {
        unsigned* lineStart = spans;
        unsigned spanLength = 0;

        bool doDraw = *buffer != a_color__key; // initial state
        *spans++ = doDraw;

        for(int x = Pixels->w; x--; ) {
            if((*buffer++ != a_color__key) == doDraw) {
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

APlatformTexture* a_platform_api__textureNew(const APixels* Pixels, unsigned Frame)
{
    APlatformTexture* texture = NULL;

    if(hasTransparency(Pixels, Frame)) {
        texture = a_mem_malloc(spansBytesNeeded(Pixels, Frame));
        spansUpdate(Pixels, Frame, texture);
    }

    return texture;
}

void a_platform_api__textureFree(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    a_mem_free(Texture);
}

void a_platform_api__textureBlit(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    if(!a_screen_boxOnClip(X, Y, Pixels->w, Pixels->h)) {
        return;
    }

    g_blitters
        [a__color.blend]
        [a__color.fillBlit]
        [Texture != NULL]
        [!a_screen_boxInsideClip(X, Y, Pixels->w, Pixels->h)]
            (Texture, Pixels, Frame, X, Y);
}

void a_platform_api__textureBlitEx(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, AFix CenterX, AFix CenterY)
{
    g_blittersEx
        [a__color.blend]
        [a__color.fillBlit]
        [Texture != NULL]
            (Pixels, Frame, X, Y, Scale, Angle, CenterX, CenterY);
}
#endif // A_CONFIG_LIB_RENDER_SOFTWARE
