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

Blitter a_blit;
static Blitter blitters[A_PIXEL_TYPE_NUM][2][2];

static PixelBlend_t blend;
static bool clip;
static bool usePixel;

static uint8_t alpha;
static uint8_t red, green, blue;
static Pixel pixel;

// Spans format:
// [1 (draw) / 0 (transp)][[len]...][0 (end line)]

#define blitter_noclip(pixeler)                          \
{                                                        \
    const int scrw = a_width;                            \
                                                         \
    Pixel* dst2 = a_pixels + y * scrw + x;               \
    const Pixel* src = s->data;                          \
                                                         \
    const uint16_t* sp = s->spans;                       \
                                                         \
    for(int i = s->h; i--; dst2 += scrw) {               \
        bool draw = *sp++ == 1;                          \
        Pixel* dst = dst2;                               \
                                                         \
        for(uint16_t len = *sp; *sp++ != 0; len = *sp) { \
            if(draw) {                                   \
                for(int j = len; j--; dst++, src++) {    \
                    pixeler;                             \
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

#define blitter_clip(pixeler)                                            \
{                                                                        \
    const int scrw = a_width;                                            \
    const int scrh = a_height;                                           \
                                                                         \
    const int w = s->w;                                                  \
    const int h = s->h;                                                  \
                                                                         \
    if(y + h <= 0 || y >= scrh || x + w <= 0 || x >= scrw) {             \
        return;                                                          \
    }                                                                    \
                                                                         \
    const int yClipUp = a_math_max(0, -y);                               \
    const int yClipDown = a_math_max(0, y + h - scrh);                   \
    const int xClipLeft = a_math_max(0, -x);                             \
    const int xClipRight = a_math_max(0, x + w - scrw);                  \
                                                                         \
    const int rows = h - yClipUp - yClipDown;                            \
    const int columns = w - xClipLeft - xClipRight;                      \
                                                                         \
    Pixel* startDst = a_pixels + (y + yClipUp) * scrw + (x + xClipLeft); \
    const Pixel* startSrc = s->data + yClipUp * w + xClipLeft;           \
                                                                         \
    const uint16_t* sp = s->spans;                                       \
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
        Pixel* dst = startDst;                                           \
        const Pixel* src = startSrc;                                     \
                                                                         \
        /* draw visible columns */                                       \
        for(int c = columns; c > 0; len = *++sp, draw ^= 1) {            \
            if(draw) {                                                   \
                for(int d = len; d-- && c > 0; dst++, src++, c--) {      \
                    pixeler;                                             \
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

#define blitterMake(blend, argsBlit, argsPixel)                    \
                                                                   \
    void a_blit__noclip_##blend(const Sprite* s, int x, int y)     \
    {                                                              \
        blitter_##blend##_setup                                    \
        blitter_noclip(a_pixel__##blend argsBlit)                  \
    }                                                              \
                                                                   \
    void a_blit__noclip_##blend##_p(const Sprite* s, int x, int y) \
    {                                                              \
        blitter_##blend##_setup                                    \
        blitter_noclip(a_pixel__##blend argsPixel)                 \
    }                                                              \
                                                                   \
    void a_blit__clip_##blend(const Sprite* s, int x, int y)       \
    {                                                              \
        blitter_##blend##_setup                                    \
        blitter_clip(a_pixel__##blend argsBlit)                    \
    }                                                              \
                                                                   \
    void a_blit__clip_##blend##_p(const Sprite* s, int x, int y)   \
    {                                                              \
        blitter_##blend##_setup                                    \
        blitter_clip(a_pixel__##blend argsPixel)                   \
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
    #define blitterInit(index, blend)                       \
    ({                                                      \
        blitters[index][0][0] = a_blit__noclip_##blend;     \
        blitters[index][0][1] = a_blit__noclip_##blend##_p; \
        blitters[index][1][0] = a_blit__clip_##blend;       \
        blitters[index][1][1] = a_blit__clip_##blend##_p;   \
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

void a_blit__setBlend(PixelBlend_t b)
{
    blend = b;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setClip(bool c)
{
    clip = c;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit_pixel(bool p)
{
    usePixel = p;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setAlpha(uint8_t a)
{
    alpha = a;
}

void a_blit__setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    red = r;
    green = g;
    blue = b;

    pixel = a_pixel_make(red, green, blue);
}

void a_blit_c(const Sprite* s)
{
    a_blit(s, (a_width - s->w) / 2, (a_height - s->h) / 2);
}

void a_blit_ch(const Sprite* s, int y)
{
    a_blit(s, (a_width - s->w) / 2, y);
}

void a_blit_cv(const Sprite* s, int x)
{
    a_blit(s, x, (a_height - s->h) / 2);
}
