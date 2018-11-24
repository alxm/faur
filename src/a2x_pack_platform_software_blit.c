/*
    Copyright 2010, 2016-2018 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_platform_software_blit.v.h"

#if A_BUILD_RENDER_SOFTWARE
#include "a2x_pack_mem.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"

struct APlatformTexture {
    ASprite* spr;
    bool colorKeyed;
    size_t spansSize;
    unsigned spans[];
};

typedef void (*ABlitter)(const APlatformTexture* Sprite, int X, int Y);

// [Blend][Fill][ColorKey][Clip]
static ABlitter g_blitters[A_PIXEL_BLEND_NUM][2][2][2];

#define A__FUNC_NAME_EXPAND2(Blend, Fill, ColorKey, Clip) a_blit__##Blend##_##Fill##_##ColorKey##_##Clip
#define A__FUNC_NAME_EXPAND(Blend, Fill, ColorKey, Clip) A__FUNC_NAME_EXPAND2(Blend, Fill, ColorKey, Clip)
#define A__FUNC_NAME(ColorKey, Clip) A__FUNC_NAME_EXPAND(A__BLEND, A__FILL, ColorKey, Clip)

#define A__PIXEL_DRAW_EXPAND2(Blend) a_pixel__##Blend
#define A__PIXEL_DRAW_EXPAND(Blend, Params) A__PIXEL_DRAW_EXPAND2(Blend)(Params)
#define A__PIXEL_DRAW(Dst) A__PIXEL_DRAW_EXPAND(A__BLEND, Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , *src
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND plain
#define A__FILL flat
#define A__BLEND_SETUP                         \
    const APixel color = a_pixel__state.pixel;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , color
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL data
#define A__BLEND_SETUP                      \
    int r, g, b;                            \
    const int alpha = a_pixel__state.alpha; \
    if(alpha == 0) {                        \
        return;                             \
    }
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b, alpha
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgba
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;   \
    const int alpha = a_pixel__state.alpha; \
    if(alpha == 0) {                        \
        return;                             \
    }
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue, alpha
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL data
#define A__BLEND_SETUP int r, g, b;
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb25
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL data
#define A__BLEND_SETUP int r, g, b;
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb50
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL data
#define A__BLEND_SETUP int r, g, b;
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND rgb75
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND inverse
#define A__FILL flat
#define A__BLEND_SETUP
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL data
#define A__BLEND_SETUP int r, g, b;
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND mod
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND add
#define A__FILL data
#define A__BLEND_SETUP int r, g, b;
#define A__PIXEL_SETUP a_pixel_toRgb(*src, &r, &g, &b);
#define A__PIXEL_PARAMS , r, g, b
#include "a2x_pack_platform_software_blit.inc.c"

#define A__BLEND add
#define A__FILL flat
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_SETUP
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_blit.inc.c"

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
        g_blitters[Index][1][1][1] = a_blit__##Blend##_flat_keyed_doclip;

    initRoutines(A_PIXEL_BLEND_PLAIN, plain);
    initRoutines(A_PIXEL_BLEND_RGBA, rgba);
    initRoutines(A_PIXEL_BLEND_RGB25, rgb25);
    initRoutines(A_PIXEL_BLEND_RGB50, rgb50);
    initRoutines(A_PIXEL_BLEND_RGB75, rgb75);
    initRoutines(A_PIXEL_BLEND_INVERSE, inverse);
    initRoutines(A_PIXEL_BLEND_MOD, mod);
    initRoutines(A_PIXEL_BLEND_ADD, add);
}

static size_t spanBytesNeeded(const APixel* Pixels, int Width, int Height)
{
    // Spans format for each graphic line:
    // [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]

    size_t bytesNeeded = 0;

    for(int y = Height; y--; ) {
        bytesNeeded += sizeof(unsigned); // total size and initial state
        bool lastState = *Pixels != a_sprite__colorKey; // initial state

        for(int x = Width; x--; ) {
            bool newState = *Pixels++ != a_sprite__colorKey;

            if(newState != lastState) {
                bytesNeeded += sizeof(unsigned); // length of new span
                lastState = newState;
            }
        }

        bytesNeeded += sizeof(unsigned); // line's last span length
    }

    return bytesNeeded;
}

static bool hasTransparency(const APixel* Pixels, int Width, int Height)
{
    for(int i = Width * Height; i--; ) {
        if(*Pixels++ == a_sprite__colorKey) {
            return true;
        }
    }

    return false;
}

APlatformTexture* a_platform__textureScreenNew(int Width, int Height)
{
    A_UNUSED(Width);
    A_UNUSED(Height);

    return NULL;
}

void a_platform__textureSpriteCommit(ASprite* Sprite)
{
    APlatformTexture* texture = Sprite->texture;
    const APixel* pixels = Sprite->pixels;
    int width = Sprite->w;
    int height = Sprite->h;

    bool colorKeyed = hasTransparency(pixels, width, height);
    size_t bytesNeeded = colorKeyed
                            ? spanBytesNeeded(pixels, width, height)
                            : 0;

    if(texture == NULL || bytesNeeded > texture->spansSize) {
        a_platform__textureFree(texture);
        texture = a_mem_malloc(sizeof(APlatformTexture) + bytesNeeded);

        Sprite->texture = texture;

        texture->spr = Sprite;
        texture->spansSize = bytesNeeded;
    }

    texture->colorKeyed = colorKeyed;

    if(texture->spansSize == 0) {
        return;
    }

    unsigned* spans = texture->spans;

    for(int y = height; y--; ) {
        unsigned* lineStart = spans;
        unsigned numSpans = 1; // line has at least 1 span
        unsigned spanLength = 0;

        bool lastState = *pixels != a_sprite__colorKey; // initial draw state
        *spans++ = lastState;

        for(int x = width; x--; ) {
            bool newState = *pixels++ != a_sprite__colorKey;

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

void a_platform__textureFree(APlatformTexture* Texture)
{
    if(Texture == NULL) {
        return;
    }

    free(Texture);
}

void a_platform__textureBlit(APlatformTexture* Texture, int X, int Y, bool FillFlat)
{
    A_UNUSED(FillFlat);

    if(!a_screen_boxOnClip(X, Y, Texture->spr->w , Texture->spr->h)) {
        return;
    }

    g_blitters
        [a_pixel__state.blend]
        [a_pixel__state.fillBlit]
        [Texture->colorKeyed]
        [!a_screen_boxInsideClip(X, Y, Texture->spr->w, Texture->spr->h)]
            (Texture, X, Y);
}

void a_platform__textureBlitEx(APlatformTexture* Texture, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY, bool FillFlat)
{
    A_UNUSED(Scale);
    A_UNUSED(Angle);
    A_UNUSED(CenterX);
    A_UNUSED(CenterY);

    a_platform__textureBlit(Texture, X, Y, FillFlat);
}
#endif // A_BUILD_RENDER_SOFTWARE
