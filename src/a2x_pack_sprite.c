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

// [Blend][Fill][ColorKey][Clip]
static ABlitter g_blitters[A_PIXEL_BLEND_NUM][2][2][2];

static ABlitter g_blitter_block_noclip;
static ABlitter g_blitter_block_doclip;
static ABlitter g_blitter_keyed_noclip;
static ABlitter g_blitter_keyed_doclip;

static bool g_fillFlat;
static AList* g_spritesList;
static void a_sprite__free(ASprite* Sprite);

#define A__FUNC_NAME_JOIN_WORKER(A, B, C, D) a_blit__##A##_##B##_##C##_##D
#define A__FUNC_NAME_JOIN(A, B, C, D) A__FUNC_NAME_JOIN_WORKER(A, B, C, D)
#define A__FUNC_NAME(Blend, Fill, ColorKey, Clip) A__FUNC_NAME_JOIN(Blend, Fill, ColorKey, Clip)

#define A__PIXEL_DRAW_JOIN_WORKER(A) a_pixel__##A
#define A__PIXEL_DRAW_JOIN(A) A__PIXEL_DRAW_JOIN_WORKER(A)
#define A__PIXEL_DRAW_WORKER(Params) A__PIXEL_DRAW_JOIN(A__BLEND)(Params)
#define A__PIXEL_DRAW(Dst) A__PIXEL_DRAW_WORKER(Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS , *a__pass_src
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND plain
#define A__FILL flat
#define A__BLEND_SETUP                                \
    const APixel a__pass_color = a_pixel__state.pixel;
#define A__PIXEL_PARAMS , a__pass_color
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgba
#define A__FILL data
#define A__BLEND_SETUP \
    const int a__pass_alpha = a_pixel__state.alpha;
#define A__PIXEL_PARAMS , a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src), a__pass_alpha
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgba
#define A__FILL flat
#define A__BLEND_SETUP                              \
    const int a__pass_red = a_pixel__state.red;     \
    const int a__pass_green = a_pixel__state.green; \
    const int a__pass_blue = a_pixel__state.blue;   \
    const int a__pass_alpha = a_pixel__state.alpha;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue, a__pass_alpha
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb25
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS , a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb25
#define A__FILL flat
#define A__BLEND_SETUP                              \
    const int a__pass_red = a_pixel__state.red;     \
    const int a__pass_green = a_pixel__state.green; \
    const int a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb50
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS , a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb50
#define A__FILL flat
#define A__BLEND_SETUP                              \
    const int a__pass_red = a_pixel__state.red;     \
    const int a__pass_green = a_pixel__state.green; \
    const int a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb75
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS , a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb75
#define A__FILL flat
#define A__BLEND_SETUP                              \
    const int a__pass_red = a_pixel__state.red;     \
    const int a__pass_green = a_pixel__state.green; \
    const int a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND inverse
#define A__FILL data
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND inverse
#define A__FILL flat
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_sprite.inc.c"
#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

void a_sprite__init(void)
{
    #define initRoutines(Index, Blend)                                         \
        g_blitters[Index][0][0][0] = A__FUNC_NAME(Blend, data, block, noclip); \
        g_blitters[Index][0][0][1] = A__FUNC_NAME(Blend, data, block, doclip); \
        g_blitters[Index][0][1][0] = A__FUNC_NAME(Blend, data, keyed, noclip); \
        g_blitters[Index][0][1][1] = A__FUNC_NAME(Blend, data, keyed, doclip); \
        g_blitters[Index][1][0][0] = A__FUNC_NAME(Blend, flat, block, noclip); \
        g_blitters[Index][1][0][1] = A__FUNC_NAME(Blend, flat, block, doclip); \
        g_blitters[Index][1][1][0] = A__FUNC_NAME(Blend, flat, keyed, noclip); \
        g_blitters[Index][1][1][1] = A__FUNC_NAME(Blend, flat, keyed, doclip); \

    initRoutines(A_PIXEL_BLEND_PLAIN, plain);
    initRoutines(A_PIXEL_BLEND_RGBA, rgba);
    initRoutines(A_PIXEL_BLEND_RGB25, rgb25);
    initRoutines(A_PIXEL_BLEND_RGB50, rgb50);
    initRoutines(A_PIXEL_BLEND_RGB75, rgb75);
    initRoutines(A_PIXEL_BLEND_INVERSE, inverse);

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
    g_blitter_block_noclip = g_blitters[a_pixel__state.blend][g_fillFlat][0][0];
    g_blitter_block_doclip = g_blitters[a_pixel__state.blend][g_fillFlat][0][1];
    g_blitter_keyed_noclip = g_blitters[a_pixel__state.blend][g_fillFlat][1][0];
    g_blitter_keyed_doclip = g_blitters[a_pixel__state.blend][g_fillFlat][1][1];
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
    bool foundColorKey = false;

    for(int i = Width * Height; i--; ) {
        if(Pixels[i] == A_SPRITE_COLORKEY) {
            foundColorKey = true;
            break;
        }
    }

    ASprite* s = a_sprite_blank(Width, Height, foundColorKey);

    memcpy(s->pixels,
           Pixels,
           (unsigned)Width * (unsigned)Height * sizeof(APixel));

    a_sprite__refreshSpans(s);

    return s;
}

ASprite* a_sprite_fromSprite(const ASprite* Sheet, int X, int Y)
{
    ASprite* sprite;
    int spriteWidth = 0;
    int spriteHeight = 0;
    const int sheetWidth = Sheet->w;
    const int sheetHeight = Sheet->h;

    for(int endX = X; endX < sheetWidth; endX++) {
        APixel p = a_sprite__getPixel(Sheet, endX, Y);

        if(p != A_SPRITE_LIMIT && p != A_SPRITE_END) {
            continue;
        }

        for(int endY = Y; endY < sheetHeight; endY++) {
            p = a_sprite__getPixel(Sheet, X, endY);

            if(p != A_SPRITE_LIMIT) {
                continue;
            }

            bool foundRightEdge = true;
            bool foundBottomEdge = true;

            for(int x = X; x < endX; x++) {
                p = a_sprite__getPixel(Sheet, x, endY);

                if(p != A_SPRITE_LIMIT) {
                    foundBottomEdge = false;
                    break;
                }
            }

            if(foundBottomEdge) {
                for(int y = Y; y < endY; y++) {
                    p = a_sprite__getPixel(Sheet, endX, y);

                    if(p != A_SPRITE_LIMIT && p != A_SPRITE_END) {
                        foundRightEdge = false;
                        break;
                    }
                }

                if(foundRightEdge) {
                    spriteWidth = endX - X;
                    spriteHeight = endY - Y;
                    goto doneEdges;
                }
            }
        }
    }

doneEdges:
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

    bool foundColorKey = false;
    const APixel* pixels = Sheet->pixels + Y * sheetWidth + X;

    for(int i = spriteHeight; i--; pixels += sheetWidth - spriteWidth) {
        for(int j = spriteWidth; j--; ) {
            if(*pixels++ == A_SPRITE_COLORKEY) {
                foundColorKey = true;
                goto doneColorKey;
            }
        }
    }

doneColorKey:
    sprite = a_sprite_blank(spriteWidth, spriteHeight, foundColorKey);

    const APixel* src = Sheet->pixels + Y * sheetWidth + X;
    APixel* dst = sprite->pixels;

    for(int i = spriteHeight; i--; ) {
        memcpy(dst, src, (unsigned)spriteWidth * sizeof(APixel));
        src += sheetWidth;
        dst += spriteWidth;
    }

    a_sprite__refreshSpans(sprite);

    return sprite;
}

ASprite* a_sprite_blank(int Width, int Height, bool ColorKeyed)
{
    ASprite* s = a_mem_malloc(
        sizeof(ASprite) + (unsigned)Width * (unsigned)Height * sizeof(APixel));

    s->w = Width;
    s->wLog2 = (int)log2f((float)Width);
    s->h = Height;
    s->spans = ColorKeyed ? NULL : (uint16_t*)1;
    s->spansSize = 0;

    if(ColorKeyed) {
        APixel* pixels = s->pixels;

        for(int i = Width * Height; i--; ) {
            *pixels++ = A_SPRITE_COLORKEY;
        }
    } else {
        memset(s->pixels,
               0,
               (unsigned)Width * (unsigned)Height * sizeof(APixel));
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
    if(Sprite->spansSize > 0) {
        free(Sprite->spans);
    }

    free(Sprite);
}

void a_sprite_blit(const ASprite* Sprite, int X, int Y)
{
    if(a_screen_boxInsideClip(X, Y, Sprite->w, Sprite->h)) {
        if(Sprite->spansSize > 0) {
            g_blitter_keyed_noclip(Sprite, X, Y);
        } else {
            g_blitter_block_noclip(Sprite, X, Y);
        }
    } else if(a_screen_boxOnClip(X, Y, Sprite->w, Sprite->h)) {
        if(Sprite->spansSize > 0) {
            g_blitter_keyed_doclip(Sprite, X, Y);
        } else {
            g_blitter_block_doclip(Sprite, X, Y);
        }
    }
}

void a_sprite_blitCenter(const ASprite* Sprite)
{
    a_sprite_blit(Sprite,
                  (a_screen__width - Sprite->w) / 2,
                  (a_screen__height - Sprite->h) / 2);
}

void a_sprite_blitCenterX(const ASprite* Sprite, int Y)
{
    a_sprite_blit(Sprite,
                  (a_screen__width - Sprite->w) / 2,
                  Y);
}

void a_sprite_blitCenterY(const ASprite* Sprite, int X)
{
    a_sprite_blit(Sprite,
                  X,
                  (a_screen__height - Sprite->h) / 2);
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

void a_sprite__refreshSpans(ASprite* Sprite)
{
    if(Sprite->spansSize == 0 && Sprite->spans != NULL) {
        return;
    }

    const int spriteWidth = Sprite->w;
    const int spriteHeight = Sprite->h;
    const APixel* const dst = Sprite->pixels;

    // Spans format for each graphic line:
    // [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]

    size_t bytesNeeded = 0;
    const APixel* dest = dst;

    for(int y = spriteHeight; y--; ) {
        bytesNeeded += sizeof(uint16_t); // total spans size and initial state
        bool lastState = *dest != A_SPRITE_COLORKEY; // initial state

        for(int x = spriteWidth; x--; ) {
            bool newState = *dest++ != A_SPRITE_COLORKEY;

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

        bool lastState = *dest != A_SPRITE_COLORKEY; // initial draw state
        *spans++ = lastState;

        for(int x = spriteWidth; x--; ) {
            bool newState = *dest++ != A_SPRITE_COLORKEY;

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
        *lineStart |= (uint16_t)(numSpans << 1); // record line's number of spans
    }
}

ASprite* a_sprite_clone(const ASprite* Sprite)
{
    ASprite* s = a_sprite_blank(Sprite->w, Sprite->h, Sprite->spansSize > 0);

    memcpy(s->pixels,
           Sprite->pixels,
           (unsigned)Sprite->w * (unsigned)Sprite->h * sizeof(APixel));

    a_sprite__refreshSpans(s);

    return s;
}
