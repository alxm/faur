/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <a2x.v.h>

#if A_CONFIG_LIB_RENDER_SOFTWARE
enum {LEFT, RIGHT};

typedef struct {
    int screenX[2]; // screen horizontal span to draw to
    AVectorFix sprite[2]; // sprite line to interpolate and fill the span with
} AScanline;

struct APlatformTexture {
    const APixels* pixels;
    unsigned spansSize;
    unsigned spans[];
};

typedef void (*ABlitter)(const APlatformTexture* Texture, int X, int Y);
typedef void (*ABlitterEx)(const APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY);

static ABlitter g_blitters[A_COLOR_BLEND_NUM][2][2][2]; // [Blend][Fill][ColorKey][Clip]
static ABlitterEx g_blittersEx[A_COLOR_BLEND_NUM][2][2]; // [Blend][Fill][ColorKey]

#if A_CONFIG_SCREEN_HEIGHT < 0
    #define A__SCANLINES_MALLOC 1
#endif

#if A__SCANLINES_MALLOC
    static AScanline* g_scanlines;
#else
    static AScanline g_scanlines[A_CONFIG_SCREEN_HEIGHT];
#endif

// Interpolate sprite side (SprP1, SprP2) along screen line (ScrP1, ScrP2).
// ScrP1.y <= ScrP2.y and at least part of this range is on screen.
void scan_line(int Index, AVectorInt ScrP1, AVectorInt ScrP2, AVectorFix SprP1, AVectorFix SprP2)
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

#define A__FUNC_NAME_EXPAND2(Blend, Fill, ColorKey, Clip) a_blit__##Blend##_##Fill##_##ColorKey##_##Clip
#define A__FUNC_NAME_EXPAND(Blend, Fill, ColorKey, Clip) A__FUNC_NAME_EXPAND2(Blend, Fill, ColorKey, Clip)
#define A__FUNC_NAME(ColorKey, Clip) A__FUNC_NAME_EXPAND(A__BLEND, A__FILL, ColorKey, Clip)

#define A__FUNC_NAME_EX_EXPAND2(Blend, Fill, ColorKey) a_blitEx__##Blend##_##Fill##_##ColorKey
#define A__FUNC_NAME_EX_EXPAND(Blend, Fill, ColorKey) A__FUNC_NAME_EX_EXPAND2(Blend, Fill, ColorKey)
#define A__FUNC_NAME_EX A__FUNC_NAME_EX_EXPAND(A__BLEND, A__FILL, A__COLORKEY)

#define A__PIXEL_DRAW_EXPAND2(Blend) a_color__draw_##Blend
#define A__PIXEL_DRAW_EXPAND(Blend, Params) A__PIXEL_DRAW_EXPAND2(Blend)(Params)
#define A__PIXEL_DRAW(Dst) A__PIXEL_DRAW_EXPAND(A__BLEND, Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , *src
#include "platform/a_software_blit.inc.c"

#define A__BLEND plain
#define A__FILL flat
#define A__BLEND_SETUP const APixel color = a__color.pixel;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , color
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL data
#define A__BLEND_SETUP \
    const int alpha = a__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb, alpha
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL flat
#define A__BLEND_SETUP \
    const ARgb rgb = a__color.rgb; \
    const int alpha = a__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb, alpha
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "platform/a_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL flat
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "platform/a_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND add
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP const ARgb rgb = a_pixel_toRgb(*src);
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

#define A__BLEND add
#define A__FILL flat
#define A__BLEND_SETUP const ARgb rgb = a__color.rgb;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , &rgb
#include "platform/a_software_blit.inc.c"

void a_platform_software_blit__init(void)
{
    #define initRoutines(Index, Blend)                                    \
        g_blitters[Index][0][0][0] = a_blit__##Blend##_data_block_noclip; \
        g_blitters[Index][0][0][1] = a_blit__##Blend##_data_block_doclip; \
        g_blitters[Index][0][1][0] = a_blit__##Blend##_data_keyed_noclip; \
        g_blitters[Index][0][1][1] = a_blit__##Blend##_data_keyed_doclip; \
        g_blitters[Index][1][0][0] = a_blit__##Blend##_flat_block_noclip; \
        g_blitters[Index][1][0][1] = a_blit__##Blend##_flat_block_doclip; \
        g_blitters[Index][1][1][0] = a_blit__##Blend##_flat_keyed_noclip; \
        g_blitters[Index][1][1][1] = a_blit__##Blend##_flat_keyed_doclip; \
        g_blittersEx[Index][0][0] = a_blitEx__##Blend##_data_block;       \
        g_blittersEx[Index][0][1] = a_blitEx__##Blend##_data_keyed;       \
        g_blittersEx[Index][1][0] = a_blitEx__##Blend##_flat_block;       \
        g_blittersEx[Index][1][1] = a_blitEx__##Blend##_flat_keyed;       \

    initRoutines(A_COLOR_BLEND_PLAIN, plain);
    initRoutines(A_COLOR_BLEND_RGBA, rgba);
    initRoutines(A_COLOR_BLEND_RGB25, rgb25);
    initRoutines(A_COLOR_BLEND_RGB50, rgb50);
    initRoutines(A_COLOR_BLEND_RGB75, rgb75);
    initRoutines(A_COLOR_BLEND_INVERSE, inverse);
    initRoutines(A_COLOR_BLEND_MOD, mod);
    initRoutines(A_COLOR_BLEND_ADD, add);

    #if A__SCANLINES_MALLOC
        g_scanlines = a_mem_malloc((unsigned)
                        a_platform_api__screenSizeGet().y * sizeof(AScanline));
    #endif
}

void a_platform_software_blit__uninit(void)
{
    #if A__SCANLINES_MALLOC
        free(g_scanlines);
    #endif
}

static bool hasTransparency(const APixels* Pixels)
{
    const APixel* buffer = Pixels->buffer;

    for(int i = Pixels->w * Pixels->h; i--; ) {
        if(*buffer++ == a_color__key) {
            return true;
        }
    }

    return false;
}

static size_t spansBytesNeeded(const APixels* Pixels)
{
    // Spans format for each scanline:
    // [NumSpans << 1 | 1 (draw) / 0 (transparent)][len0][len1]...

    size_t bytesNeeded = 0;
    const APixel* buffer = Pixels->buffer;

    for(int y = Pixels->h; y--; ) {
        bytesNeeded += sizeof(unsigned); // total size and initial state
        bool lastState = *buffer != a_color__key; // initial state

        for(int x = Pixels->w; x--; ) {
            bool newState = *buffer++ != a_color__key;

            if(newState != lastState) {
                bytesNeeded += sizeof(unsigned); // length of new span
                lastState = newState;
            }
        }

        bytesNeeded += sizeof(unsigned); // line's last span length
    }

    return bytesNeeded;
}

static void spansUpdate(const APixels* Pixels, APlatformTexture* Texture)
{
    unsigned* spans = Texture->spans;
    const APixel* buffer = Pixels->buffer;

    for(int y = Pixels->h; y--; ) {
        unsigned* lineStart = spans;
        unsigned numSpans = 1; // line has at least 1 span
        unsigned spanLength = 0;

        bool lastState = *buffer != a_color__key; // initial state
        *spans++ = lastState;

        for(int x = Pixels->w; x--; ) {
            bool newState = *buffer++ != a_color__key;

            if(newState == lastState) {
                spanLength++; // keep growing current span
            } else {
                *spans++ = spanLength; // record the just-ended span length
                numSpans++;
                spanLength = 1; // start a new span from this pixel
                lastState = newState;
            }
        }

        *spans++ = spanLength; // record the last span's length
        *lineStart |= numSpans << 1; // record line's number of spans
    }
}

APlatformTexture* a_platform_api__textureNew(const APixels* Pixels)
{
    if(A_FLAG_TEST_ANY(Pixels->flags, A_PIXELS__SCREEN)) {
        return NULL;
    }

    APlatformTexture* texture = Pixels->texture;
    size_t bytesNeeded = hasTransparency(Pixels) ? spansBytesNeeded(Pixels) : 0;

    if(texture == NULL || bytesNeeded > (texture->spansSize >> 1)) {
        a_platform_api__textureFree(texture);
        texture = a_mem_malloc(sizeof(APlatformTexture) + bytesNeeded);

        texture->pixels = Pixels;
        texture->spansSize = (unsigned)bytesNeeded << 1;
    }

    if(bytesNeeded > 0) {
        texture->spansSize |= 1;
        spansUpdate(Pixels, texture);
    } else {
        texture->spansSize &= ~1u;
    }

    return texture;
}

void a_platform_api__textureFree(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    free(Texture);
}

void a_platform_api__textureBlit(const APlatformTexture* Texture, int X, int Y)
{
    const APixels* pixels = Texture->pixels;

    if(!a_screen_boxOnClip(X, Y, pixels->w, pixels->h)) {
        return;
    }

    g_blitters
        [a__color.blend]
        [a__color.fillBlit]
        [Texture->spansSize & 1]
        [!a_screen_boxInsideClip(X, Y, pixels->w, pixels->h)]
            (Texture, X, Y);
}

void a_platform_api__textureBlitEx(const APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY)
{
    g_blittersEx
        [a__color.blend]
        [a__color.fillBlit]
        [Texture->spansSize & 1]
            (Texture, X, Y, Scale, Angle, CenterX, CenterY);
}
#endif // A_CONFIG_LIB_RENDER_SOFTWARE
