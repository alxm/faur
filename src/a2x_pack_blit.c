/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
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

#define blitter_noclip(pixeler)                                  \
{                                                                \
    const int screenWidth = a_width;                             \
                                                                 \
    const int w = s->w;                                          \
    const int h = s->h;                                          \
                                                                 \
    const Pixel* src2 = s->data;                                 \
    Pixel* dst2 = a_pixels + y * screenWidth + x;                \
                                                                 \
    for(int i = 0; i < h; i++, dst2 += screenWidth, src2 += w) { \
        const int spansNum = s->spansNum[i];                     \
                                                                 \
        for(int j = 0; j < spansNum; j++) {                      \
            const Pixel* src = src2 + s->spans[i][j][0];         \
            Pixel* dst = dst2 + s->spans[i][j][0];               \
                                                                 \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) {  \
                pixeler;                                         \
            }                                                    \
        }                                                        \
    }                                                            \
}

#define blitter_clip(pixeler)                                                       \
{                                                                                   \
    const int screenWidth = a_width;                                                \
    const int screenHeight = a_height;                                              \
                                                                                    \
    const int w = s->w;                                                             \
    const int h = s->h;                                                             \
                                                                                    \
    if(y + h <= 0 || y >= screenHeight || x + w <= 0 || x >= screenWidth) {         \
        return;                                                                     \
    }                                                                               \
                                                                                    \
    const int yTclip = (y < 0) ? (-y) : 0;                                          \
    const int xLclip = (x < 0) ? (-x) : 0;                                          \
                                                                                    \
    const int H = h - a_math_max(0, y + h - screenHeight);                          \
                                                                                    \
    const Pixel* src2 = s->data + yTclip * w;                                       \
    Pixel* dst2 = a_pixels + (y + yTclip) * screenWidth + x;                        \
                                                                                    \
    for(int i = yTclip; i < H; i++, dst2 += screenWidth, src2 += w) {               \
        int j;                                                                      \
        const int spansNum = s->spansNum[i];                                        \
                                                                                    \
        for(j = 0; j < spansNum && s->spans[i][j][1] <= xLclip; j++) {              \
            continue;                                                               \
        }                                                                           \
                                                                                    \
        if(j < spansNum && s->spans[i][j][0] < xLclip) {                            \
            const Pixel* src = src2 + xLclip;                                       \
            Pixel* dst = dst2 + xLclip;                                             \
                                                                                    \
            for(int k = a_math_min(s->spans[i][j][1] - xLclip, screenWidth);        \
                k--; dst++, src++) {                                                \
                pixeler;                                                            \
            }                                                                       \
                                                                                    \
            j++;                                                                    \
        }                                                                           \
                                                                                    \
        for( ; j < spansNum && x + s->spans[i][j][1] <= screenWidth; j++) {         \
            const Pixel* src = src2 + s->spans[i][j][0];                            \
            Pixel* dst = dst2 + s->spans[i][j][0];                                  \
                                                                                    \
            for(int k = s->spans[i][j][2]; k--; dst++, src++) {                     \
                pixeler;                                                            \
            }                                                                       \
        }                                                                           \
                                                                                    \
        if(j < spansNum && x + s->spans[i][j][0] < screenWidth) {                   \
            const Pixel* src = src2 + s->spans[i][j][0];                            \
            Pixel* dst = dst2 + s->spans[i][j][0];                                  \
                                                                                    \
            for(int k = screenWidth - (x + s->spans[i][j][0]); k--; dst++, src++) { \
                pixeler;                                                            \
            }                                                                       \
        }                                                                           \
    }                                                                               \
}

#define blitter_plain_setup

#define blitter_rgba_setup   \
    const uint8_t a = alpha;

#define blitter_rgb25_setup
#define blitter_rgb50_setup
#define blitter_rgb75_setup
#define blitter_inverse_setup

#define blitterMake(blend, argsBlit, argsPixel)                                      \
                                                                                     \
    void a_blit__noclip_##blend(const Sprite* const s, const int x, const int y)     \
    {                                                                                \
        blitter_##blend##_setup                                                      \
        blitter_noclip(a_pixel__##blend argsBlit)                                    \
    }                                                                                \
                                                                                     \
    void a_blit__noclip_##blend##_p(const Sprite* const s, const int x, const int y) \
    {                                                                                \
        blitter_##blend##_setup                                                      \
        blitter_noclip(a_pixel__##blend argsPixel)                                   \
    }                                                                                \
                                                                                     \
    void a_blit__clip_##blend(const Sprite* const s, const int x, const int y)       \
    {                                                                                \
        blitter_##blend##_setup                                                      \
        blitter_clip(a_pixel__##blend argsBlit)                                      \
    }                                                                                \
                                                                                     \
    void a_blit__clip_##blend##_p(const Sprite* const s, const int x, const int y)   \
    {                                                                                \
        blitter_##blend##_setup                                                      \
        blitter_clip(a_pixel__##blend argsPixel)                                     \
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

void a_blit__set(void)
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

void a_blit__setBlend(const PixelBlend_t b)
{
    blend = b;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setClip(const bool c)
{
    clip = c;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit_pixel(const bool p)
{
    usePixel = p;
    a_blit = blitters[blend][clip][usePixel];
}

void a_blit__setAlpha(const uint8_t a)
{
    alpha = a;
}

void a_blit__setRGB(const uint8_t r, const uint8_t g, const uint8_t b)
{
    red = r;
    green = g;
    blue = b;

    pixel = a_pixel_make(red, green, blue);
}

void a_blit_c(const Sprite* const s)
{
    a_blit(s, (a_width - s->w) / 2, (a_height - s->h) / 2);
}

void a_blit_ch(const Sprite* const s, const int y)
{
    a_blit(s, (a_width - s->w) / 2, y);
}

void a_blit_cv(const Sprite* const s, const int x)
{
    a_blit(s, x, (a_height - s->h) / 2);
}
