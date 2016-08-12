/*
    Copyright 2010 Alex Margarit

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

#include "a2x_pack_blit.v.h"

typedef void (*ABlitter)(const ASprite* Sprite, int X, int Y);

ABlitter g_blitter;
static ABlitter g_blitters[A_PIXEL_TYPE_NUM][2][2];

static bool g_fillFlat;

// Spans format:
// [1 (draw) / 0 (transp)][[len]...][0 (end line)]

#define blitter_noclip(Pixeler)                                       \
{                                                                     \
    const int screenW = a_screen__width;                              \
    APixel* dst = a_screen__pixels + Y * screenW + X;                 \
    const APixel* a__pass_src = Sprite->data;                         \
    const uint16_t* spans = Sprite->spans;                            \
                                                                      \
    for(int i = Sprite->h; i--; dst += screenW) {                     \
        bool draw = *spans++ == 1;                                    \
        APixel* a__pass_dst = dst;                                    \
                                                                      \
        for(uint16_t len = *spans; *spans++ != 0; len = *spans) {     \
            if(draw) {                                                \
                for(int j = len; j--; a__pass_dst++, a__pass_src++) { \
                    Pixeler;                                          \
                }                                                     \
            } else {                                                  \
                a__pass_dst += len;                                   \
                a__pass_src += len;                                   \
            }                                                         \
                                                                      \
            draw ^= 1;                                                \
        }                                                             \
    }                                                                 \
}

#define blitter_clip(Pixeler)                                                \
{                                                                            \
    const int screenW = a_screen__width;                                     \
    const int screenH = a_screen__height;                                    \
    const int spriteW = Sprite->w;                                           \
    const int spriteH = Sprite->h;                                           \
                                                                             \
    if(Y + spriteH <= 0 || Y >= screenH                                      \
        || X + spriteW <= 0 || X >= screenW) {                               \
        return;                                                              \
    }                                                                        \
                                                                             \
    const int yClipUp = a_math_max(0, -Y);                                   \
    const int yClipDown = a_math_max(0, Y + spriteH - screenH);              \
    const int xClipLeft = a_math_max(0, -X);                                 \
    const int xClipRight = a_math_max(0, X + spriteW - screenW);             \
                                                                             \
    const int rows = spriteH - yClipUp - yClipDown;                          \
    const int columns = spriteW - xClipLeft - xClipRight;                    \
                                                                             \
    APixel* startDst = a_screen__pixels                                      \
                       + (Y + yClipUp) * screenW + (X + xClipLeft);          \
    const APixel* startSrc = Sprite->data                                    \
                             + yClipUp * spriteW + xClipLeft;                \
    const uint16_t* spans = Sprite->spans;                                   \
                                                                             \
    /* skip clipped top rows */                                              \
    for(int i = yClipUp; i--; ) {                                            \
        spans++;                                                             \
        while (*spans++ != 0) {                                              \
            continue;                                                        \
        }                                                                    \
    }                                                                        \
                                                                             \
    /* draw visible rows */                                                  \
    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {       \
        bool draw = *spans++ == 1;                                           \
        uint16_t len = *spans;                                               \
                                                                             \
        /* skip clipped left columns */                                      \
        while(len <= xClipLeft) {                                            \
            len += *++spans;                                                 \
            draw ^= 1;                                                       \
        }                                                                    \
                                                                             \
        /* account for overclipping */                                       \
        len -= xClipLeft;                                                    \
                                                                             \
        APixel* a__pass_dst = startDst;                                      \
        const APixel* a__pass_src = startSrc;                                \
                                                                             \
        /* draw visible columns */                                           \
        for(int c = columns; c > 0; ) {                                      \
            if(draw) {                                                       \
                for(int d = len; d-- && c--; ) {                             \
                    Pixeler;                                                 \
                    a__pass_dst++;                                           \
                    a__pass_src++;                                           \
                }                                                            \
            } else {                                                         \
                a__pass_dst += len;                                          \
                a__pass_src += len;                                          \
                c -= len;                                                    \
            }                                                                \
                                                                             \
            len = *++spans;                                                  \
            draw ^= 1;                                                       \
        }                                                                    \
                                                                             \
        /* skip clipped right columns */                                     \
        while(*spans++ != 0) {                                               \
            continue;                                                        \
        }                                                                    \
    }                                                                        \
}

#define blitter_plain_setup
#define blitter_plain_setup_p                         \
    const APixel a__pass_pixel = a_pixel__mode.pixel;

#define blitter_rgb25_setup
#define blitter_rgb25_setup_p                          \
    const uint8_t a__pass_red = a_pixel__mode.red;     \
    const uint8_t a__pass_green = a_pixel__mode.green; \
    const uint8_t a__pass_blue = a_pixel__mode.blue;

#define blitter_rgb50_setup
#define blitter_rgb50_setup_p blitter_rgb25_setup_p

#define blitter_rgb75_setup
#define blitter_rgb75_setup_p blitter_rgb25_setup_p

#define blitter_rgba_setup                                  \
    blitter_rgb25_setup                                     \
    const unsigned int a__pass_alpha = a_pixel__mode.alpha;

#define blitter_rgba_setup_p                                \
    blitter_rgb25_setup_p                                   \
    const unsigned int a__pass_alpha = a_pixel__mode.alpha;

#define blitter_inverse_setup
#define blitter_inverse_setup_p

#define blitterMake(Blend, ArgsBlit, ArgsPixel)                          \
                                                                         \
    void a_blit__noclip_##Blend(const ASprite* Sprite, int X, int Y)     \
    {                                                                    \
        blitter_##Blend##_setup                                          \
        blitter_noclip(a_pixel__##Blend ArgsBlit)                        \
    }                                                                    \
                                                                         \
    void a_blit__noclip_##Blend##_p(const ASprite* Sprite, int X, int Y) \
    {                                                                    \
        blitter_##Blend##_setup_p                                        \
        blitter_noclip(a_pixel__##Blend ArgsPixel)                       \
    }                                                                    \
                                                                         \
    void a_blit__clip_##Blend(const ASprite* Sprite, int X, int Y)       \
    {                                                                    \
        blitter_##Blend##_setup                                          \
        blitter_clip(a_pixel__##Blend ArgsBlit)                          \
    }                                                                    \
                                                                         \
    void a_blit__clip_##Blend##_p(const ASprite* Sprite, int X, int Y)   \
    {                                                                    \
        blitter_##Blend##_setup_p                                        \
        blitter_clip(a_pixel__##Blend ArgsPixel)                         \
    }

blitterMake(
    plain,
    (a__pass_dst, *a__pass_src),
    (a__pass_dst, a__pass_pixel)
)

blitterMake(
    rgba,
    (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src), a__pass_alpha),
    (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue, a__pass_alpha)
)

blitterMake(
    rgb25,
    (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)),
    (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
)

blitterMake(
    rgb50,
    (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)),
    (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
)

blitterMake(
    rgb75,
    (a__pass_dst, a_pixel_red(*a__pass_src), a_pixel_green(*a__pass_src), a_pixel_blue(*a__pass_src)),
    (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue)
)

blitterMake(
    inverse,
    (a__pass_dst),
    (a__pass_dst)
)

void a_blit__init(void)
{
    #define blitterInit(Index, Blend)                       \
    ({                                                      \
        g_blitters[Index][0][0] = a_blit__noclip_##Blend;     \
        g_blitters[Index][0][1] = a_blit__noclip_##Blend##_p; \
        g_blitters[Index][1][0] = a_blit__clip_##Blend;       \
        g_blitters[Index][1][1] = a_blit__clip_##Blend##_p;   \
    })

    blitterInit(A_PIXEL_PLAIN, plain);
    blitterInit(A_PIXEL_RGBA, rgba);
    blitterInit(A_PIXEL_RGB25, rgb25);
    blitterInit(A_PIXEL_RGB50, rgb50);
    blitterInit(A_PIXEL_RGB75, rgb75);
    blitterInit(A_PIXEL_INVERSE, inverse);

    g_fillFlat = false;

    a_blit__updateRoutines();
}

void a_blit__updateRoutines(void)
{
    g_blitter = g_blitters[a_pixel__mode.blend][a_pixel__mode.clip][g_fillFlat];
}

void a_blit_fillFlat(bool FillFlatColor)
{
    g_fillFlat = FillFlatColor;
    a_blit__updateRoutines();
}

void a_blit(const ASprite* Sprite, int X, int Y)
{
    g_blitter(Sprite, X, Y);
}

void a_blit_center(const ASprite* Sprite)
{
    g_blitter(Sprite, (a_screen__width - Sprite->w) / 2, (a_screen__height - Sprite->h) / 2);
}

void a_blit_centerX(const ASprite* Sprite, int Y)
{
    g_blitter(Sprite, (a_screen__width - Sprite->w) / 2, Y);
}

void a_blit_centerY(const ASprite* Sprite, int X)
{
    g_blitter(Sprite, X, (a_screen__height - Sprite->h) / 2);
}
