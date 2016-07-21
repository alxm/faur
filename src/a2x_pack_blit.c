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

ABlitter a_blit;
static ABlitter blitters[A_PIXEL_TYPE_NUM][2][2];

static APixelBlend blend;
static bool clip;
static bool usePixel;

static uint8_t alpha;
static uint8_t red, green, blue;
static APixel pixel;

// Spans format:
// [1 (draw) / 0 (transp)][[len]...][0 (end line)]

#define blitter_noclip(Pixeler)                          \
{                                                        \
    const int scrw = a_width;                            \
                                                         \
    APixel* dst2 = a_pixels + Y * scrw + X;               \
    const APixel* src = Sprite->data;                          \
                                                         \
    const uint16_t* sp = Sprite->spans;                       \
                                                         \
    for(int i = Sprite->h; i--; dst2 += scrw) {               \
        bool draw = *sp++ == 1;                          \
        APixel* dst = dst2;                               \
                                                         \
        for(uint16_t len = *sp; *sp++ != 0; len = *sp) { \
            if(draw) {                                   \
                for(int j = len; j--; dst++, src++) {    \
                    Pixeler;                             \
                }                                        \
            } else {                                     \
                dst += len;                              \
                src += len;                              \
            }                                            \
                                                         \
            draw ^= 1;                                   \
        }                                                \
    }                                                    \
}

#define blitter_clip(Pixeler)                                            \
{                                                                        \
    const int scrw = a_width;                                            \
    const int scrh = a_height;                                           \
                                                                         \
    const int w = Sprite->w;                                                  \
    const int h = Sprite->h;                                                  \
                                                                         \
    if(Y + h <= 0 || Y >= scrh || X + w <= 0 || X >= scrw) {             \
        return;                                                          \
    }                                                                    \
                                                                         \
    const int yClipUp = a_math_max(0, -Y);                               \
    const int yClipDown = a_math_max(0, Y + h - scrh);                   \
    const int xClipLeft = a_math_max(0, -X);                             \
    const int xClipRight = a_math_max(0, X + w - scrw);                  \
                                                                         \
    const int rows = h - yClipUp - yClipDown;                            \
    const int columns = w - xClipLeft - xClipRight;                      \
                                                                         \
    APixel* startDst = a_pixels + (Y + yClipUp) * scrw + (X + xClipLeft); \
    const APixel* startSrc = Sprite->data + yClipUp * w + xClipLeft;           \
                                                                         \
    const uint16_t* sp = Sprite->spans;                                       \
                                                                         \
    /* skip clipped top rows */                                          \
    for(int i = yClipUp; i--; ) {                                        \
        sp++;                                                            \
        while(*sp++ != 0) {                                              \
            continue;                                                    \
        }                                                                \
    }                                                                    \
                                                                         \
    /* draw visible rows */                                              \
    for(int i = rows; i--; startDst += scrw, startSrc += w) {            \
        bool draw = *sp++ == 1;                                          \
        uint16_t len = *sp;                                              \
                                                                         \
        /* skip clipped left columns */                                  \
        while(len <= xClipLeft) {                                        \
            len += *++sp;                                                \
            draw ^= 1;                                                   \
        }                                                                \
        len -= xClipLeft;                                                \
                                                                         \
        APixel* dst = startDst;                                           \
        const APixel* src = startSrc;                                     \
                                                                         \
        /* draw visible columns */                                       \
        for(int c = columns; c > 0; len = *++sp, draw ^= 1) {            \
            if(draw) {                                                   \
                for(int d = len; d-- && c > 0; dst++, src++, c--) {      \
                    Pixeler;                                             \
                }                                                        \
            } else {                                                     \
                dst += len;                                              \
                src += len;                                              \
                c -= len;                                                \
            }                                                            \
        }                                                                \
                                                                         \
        /* skip clipped right columns */                                 \
        while(*sp++ != 0) {                                              \
            continue;                                                    \
        }                                                                \
    }                                                                    \
}

#define blitter_plain_setup

#define blitter_rgba_setup   \
    const uint8_t a = alpha;

#define blitter_rgb25_setup
#define blitter_rgb50_setup
#define blitter_rgb75_setup
#define blitter_inverse_setup

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
        blitter_##Blend##_setup                                          \
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
        blitter_##Blend##_setup                                          \
        blitter_clip(a_pixel__##Blend ArgsPixel)                         \
    }

blitterMake(
    plain,
    (dst, *src),
    (dst, pixel)
)

blitterMake(
    rgba,
    (dst, a_pixel_red(*src), a_pixel_green(*src), a_pixel_blue(*src), a),
    (dst, red, green, blue, a)
)

blitterMake(
    rgb25,
    (dst, a_pixel_red(*src), a_pixel_green(*src), a_pixel_blue(*src)),
    (dst, red, green, blue)
)

blitterMake(
    rgb50,
    (dst, a_pixel_red(*src), a_pixel_green(*src), a_pixel_blue(*src)),
    (dst, red, green, blue)
)

blitterMake(
    rgb75,
    (dst, a_pixel_red(*src), a_pixel_green(*src), a_pixel_blue(*src)),
    (dst, red, green, blue)
)

blitterMake(
    inverse,
    (dst),
    (dst)
)

void a_blit__init(void)
{
    #define blitterInit(Index, Blend)                       \
    ({                                                      \
        blitters[Index][0][0] = a_blit__noclip_##Blend;     \
        blitters[Index][0][1] = a_blit__noclip_##Blend##_p; \
        blitters[Index][1][0] = a_blit__clip_##Blend;       \
        blitters[Index][1][1] = a_blit__clip_##Blend##_p;   \
    })

    blitterInit(A_PIXEL_PLAIN, plain);
    blitterInit(A_PIXEL_RGBA, rgba);
    blitterInit(A_PIXEL_RGB25, rgb25);
    blitterInit(A_PIXEL_RGB50, rgb50);
    blitterInit(A_PIXEL_RGB75, rgb75);
    blitterInit(A_PIXEL_INVERSE, inverse);

    blend = A_PIXEL_PLAIN;
    clip = true;
    usePixel = false;

    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setBlend(APixelBlend Blend)
{
    blend = Blend;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setClip(bool DoClip)
{
    clip = DoClip;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit_pixel(bool UsePixelColor)
{
    usePixel = UsePixelColor;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setAlpha(uint8_t Alpha)
{
    alpha = Alpha;
}

void a_blit__setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    red = Red;
    green = Green;
    blue = Blue;

    pixel = a_pixel_make(red, green, blue);
}

void a_blit_c(const ASprite* Sprite)
{
    a_blit(Sprite, (a_width - Sprite->w) / 2, (a_height - Sprite->h) / 2);
}

void a_blit_ch(const ASprite* Sprite, int Y)
{
    a_blit(Sprite, (a_width - Sprite->w) / 2, Y);
}

void a_blit_cv(const ASprite* Sprite, int X)
{
    a_blit(Sprite, X, (a_height - Sprite->h) / 2);
}
