/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_pack_sprite.v.h"

typedef void (*ABlitter)(const ASprite* Sprite, int X, int Y);

static ABlitter g_blitter;
static ABlitter g_blitters[A_PIXEL_BLEND_NUM][2][2];
static bool g_fillFlat;

static AList* g_spritesList;
static void a_sprite__free(ASprite* Sprite);

#define A__BLEND plain
#define A__SUFFIX normal
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst, *a__pass_src)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND plain
#define A__SUFFIX flat
#define A__BLEND_SETUP                                \
    const APixel a__pass_color = a_pixel__mode.pixel;
#define A__PIXEL_PARAMS (a__pass_dst, a__pass_color)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgba
#define A__SUFFIX normal
#define A__BLEND_SETUP \
    const unsigned int a__pass_alpha = a_pixel__mode.alpha;
#define A__PIXEL_PARAMS (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src), a__pass_alpha)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgba
#define A__SUFFIX flat
#define A__BLEND_SETUP                                      \
    const uint8_t a__pass_red = a_pixel__mode.red;          \
    const uint8_t a__pass_green = a_pixel__mode.green;      \
    const uint8_t a__pass_blue = a_pixel__mode.blue;        \
    const unsigned int a__pass_alpha = a_pixel__mode.alpha;
#define A__PIXEL_PARAMS (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue, a__pass_alpha)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb25
#define A__SUFFIX normal
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src))
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb25
#define A__SUFFIX flat
#define A__BLEND_SETUP                                 \
    const uint8_t a__pass_red = a_pixel__mode.red;     \
    const uint8_t a__pass_green = a_pixel__mode.green; \
    const uint8_t a__pass_blue = a_pixel__mode.blue;
#define A__PIXEL_PARAMS (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb50
#define A__SUFFIX normal
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src))
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb50
#define A__SUFFIX flat
#define A__BLEND_SETUP                                 \
    const uint8_t a__pass_red = a_pixel__mode.red;     \
    const uint8_t a__pass_green = a_pixel__mode.green; \
    const uint8_t a__pass_blue = a_pixel__mode.blue;
#define A__PIXEL_PARAMS (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb75
#define A__SUFFIX normal
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src))
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb75
#define A__SUFFIX flat
#define A__BLEND_SETUP                                 \
    const uint8_t a__pass_red = a_pixel__mode.red;     \
    const uint8_t a__pass_green = a_pixel__mode.green; \
    const uint8_t a__pass_blue = a_pixel__mode.blue;
#define A__PIXEL_PARAMS (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND inverse
#define A__SUFFIX normal
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND inverse
#define A__SUFFIX flat
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS (a__pass_dst)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__SUFFIX
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

void a_sprite__init(void)
{
    #define blitterInit(Index, Blend)                              \
    ({                                                             \
        g_blitters[Index][0][0] = a_blit__noclip_##Blend##_normal; \
        g_blitters[Index][0][1] = a_blit__noclip_##Blend##_flat;   \
        g_blitters[Index][1][0] = a_blit__clip_##Blend##_normal;   \
        g_blitters[Index][1][1] = a_blit__clip_##Blend##_flat;     \
    })

    blitterInit(A_PIXEL_BLEND_PLAIN, plain);
    blitterInit(A_PIXEL_BLEND_RGBA, rgba);
    blitterInit(A_PIXEL_BLEND_RGB25, rgb25);
    blitterInit(A_PIXEL_BLEND_RGB50, rgb50);
    blitterInit(A_PIXEL_BLEND_RGB75, rgb75);
    blitterInit(A_PIXEL_BLEND_INVERSE, inverse);

    g_fillFlat = false;
    a_sprite__updateRoutines();
    g_spritesList = a_list_new();
}

void a_sprite__uninit(void)
{
    A_LIST_ITERATE(g_spritesList, ASprite*, s) {
        a_sprite__free(s);
    }

    a_list_free(g_spritesList);
}

void a_sprite__updateRoutines(void)
{
    g_blitter = g_blitters[a_pixel__mode.blend][a_pixel__mode.clip][g_fillFlat];
}

ASprite* a_sprite_fromFile(const char* Path)
{
    int w = 0;
    int h = 0;
    ASprite* s;
    APixel* pixels = NULL;

    a_png_readFile(Path, &pixels, &w, &h);

    if(pixels == NULL) {
        return NULL;
    }

    s = a_sprite_fromPixels(pixels, w, h);
    free(pixels);

    return s;
}

ASprite* a_sprite_fromData(const uint8_t* Data)
{
    int w;
    int h;
    ASprite* s;
    APixel* pixels = NULL;

    a_png_readMemory(Data, &pixels, &w, &h);

    if(pixels == NULL) {
        return NULL;
    }

    s = a_sprite_fromPixels(pixels, w, h);
    free(pixels);

    return s;
}

ASprite* a_sprite_fromPixels(const APixel* Pixels, int Width, int Height)
{
    ASprite* s = a_sprite_blank(Width, Height);

    memcpy(s->pixels, Pixels, Width * Height * sizeof(APixel));
    a_sprite_refresh(s);

    return s;
}

ASprite* a_sprite_fromSprite(const ASprite* Sheet, int X, int Y)
{
    int spriteWidth = 0;
    int spriteHeight = 0;
    const int sheetWidth = Sheet->w;
    const int sheetHeight = Sheet->h;

    for(int sheetx = X; sheetx < sheetWidth; sheetx++) {
        const APixel hPixel = a_sprite__getPixel(Sheet, sheetx, Y);

        // reached right edge
        if(hPixel == A_SPRITE_LIMIT || hPixel == A_SPRITE_END) {
            for(int sheety = Y; sheety < sheetHeight; sheety++) {
                const APixel vPixel = a_sprite__getPixel(Sheet, X, sheety);

                // reached bottom edge
                if(vPixel == A_SPRITE_LIMIT) {
                    spriteWidth = sheetx - X;
                    spriteHeight = sheety - Y;
                    goto Done;
                }
            }
        }
    }

    Done:

    if(spriteWidth == 0 || spriteHeight == 0) {
        if(X == 0 && Y == 0) {
            // no boundary borders for full-image sprites
            spriteWidth = sheetWidth;
            spriteHeight = sheetHeight;
        } else {
            a_out__error("Sprite coords %d, %d are invalid", X, Y);
            return NULL;
        }
    }

    ASprite* const sprite = a_sprite_blank(spriteWidth, spriteHeight);
    const APixel* src = Sheet->pixels + Y * sheetWidth + X;
    APixel* dst = sprite->pixels;

    for(int i = spriteHeight; i--; ) {
        memcpy(dst, src, spriteWidth * sizeof(APixel));
        src += sheetWidth;
        dst += spriteWidth;
    }

    a_sprite_refresh(sprite);

    return sprite;
}

ASprite* a_sprite_blank(int Width, int Height)
{
    ASprite* const s = a_mem_malloc(sizeof(ASprite) + Width * Height * sizeof(APixel));

    s->w = Width;
    s->wLog2 = (int)log2f(Width);
    s->h = Height;
    s->spans = NULL;
    s->spansSize = 0;

    APixel* pixels = s->pixels;

    for(int i = Width * Height; i--; ) {
        *pixels++ = A_SPRITE_TRANSPARENT;
    }

    s->node = a_list_addLast(g_spritesList, s);

    return s;
}

void a_sprite_free(ASprite* Sprite)
{
    a_list_removeNode(Sprite->node);
    a_sprite__free(Sprite);
}

void a_sprite__free(ASprite* Sprite)
{
    free(Sprite->spans);
    free(Sprite);
}

void a_sprite_blit(const ASprite* Sprite, int X, int Y)
{
    g_blitter(Sprite, X, Y);
}

void a_sprite_blitCenter(const ASprite* Sprite)
{
    g_blitter(Sprite, (a_screen__width - Sprite->w) / 2, (a_screen__height - Sprite->h) / 2);
}

void a_sprite_blitCenterX(const ASprite* Sprite, int Y)
{
    g_blitter(Sprite, (a_screen__width - Sprite->w) / 2, Y);
}

void a_sprite_blitCenterY(const ASprite* Sprite, int X)
{
    g_blitter(Sprite, X, (a_screen__height - Sprite->h) / 2);
}

void a_sprite_fillFlat(bool FillFlatColor)
{
    g_fillFlat = FillFlatColor;
    a_sprite__updateRoutines();
}

int a_sprite_w(const ASprite* Sprite)
{
    return Sprite->w;
}

int a_sprite_wLog2(const ASprite* Sprite)
{
    return Sprite->wLog2;
}

int a_sprite_h(const ASprite* Sprite)
{
    return Sprite->h;
}

APixel* a_sprite_pixels(ASprite* Sprite)
{
    return Sprite->pixels;
}

APixel a_sprite_getPixel(const ASprite* Sprite, int X, int Y)
{
    return *(Sprite->pixels + Y * Sprite->w + X);
}

void a_sprite_refresh(ASprite* Sprite)
{
    const int spriteWidth = Sprite->w;
    const int spriteHeight = Sprite->h;
    const APixel* const dst = Sprite->pixels;

    // Spans format for each graphic line:
    // [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]

    size_t bytesNeeded = 0;
    const APixel* dest = dst;

    for(int y = spriteHeight; y--; ) {
        bytesNeeded += sizeof(uint16_t); // total spans size and initial state
        bool lastState = *dest != A_SPRITE_TRANSPARENT; // initial state

        for(int x = spriteWidth; x--; ) {
            bool newState = *dest++ != A_SPRITE_TRANSPARENT;

            if(newState != lastState) {
                bytesNeeded += sizeof(uint16_t); // length of new span
                lastState = newState;
            }
        }

        bytesNeeded += sizeof(uint16_t); // line's last span length
    }

    if(Sprite->spansSize < bytesNeeded) {
        free(Sprite->spans);
        Sprite->spans = a_mem_malloc(bytesNeeded);
        Sprite->spansSize = bytesNeeded;
    }

    dest = dst;
    uint16_t* spans = Sprite->spans;

    for(int y = spriteHeight; y--; ) {
        uint16_t* lineStart = spans;
        uint16_t numSpans = 1; // line has at least 1 span
        uint16_t spanLength = 0;

        bool lastState = *dest != A_SPRITE_TRANSPARENT; // initial draw state
        *spans++ = lastState;

        for(int x = spriteWidth; x--; ) {
            bool newState = *dest++ != A_SPRITE_TRANSPARENT;

            if(newState == lastState) {
                spanLength++; // keep growing current span
            } else {
                *spans++ = spanLength; // record the just-ended span's length
                numSpans++;
                spanLength = 1; // start a new span from this pixel
                lastState = newState;
            }
        }

        *spans++ = spanLength; // record the last span's length
        *lineStart |= numSpans << 1; // record line's number of spans
    }
}

ASprite* a_sprite_clone(const ASprite* Sprite)
{
    ASprite* const s = a_sprite_blank(Sprite->w, Sprite->h);

    memcpy(s->pixels, Sprite->pixels, Sprite->w * Sprite->h * sizeof(APixel));
    a_sprite_refresh(s);

    return s;
}
