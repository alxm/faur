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

#include "a2x_pack_pixel.v.h"

PixelPut a_pixel_put;
static PixelPut pixels[A_PIXEL_TYPE_NUM][2];

static PixelBlend_t blend;
static bool clip;

static uint8_t alpha;
static uint8_t red, green, blue;
static Pixel pixel;

#define PIXEL_DST (a_pixels + y * a_width + x)

#define pixelMake(blend, args)                                \
                                                              \
    void a_pixel__noclip_##blend(const int x, const int y)    \
    {                                                         \
        a_pixel__##blend args;                                \
    }                                                         \
                                                              \
    void a_pixel__clip_##blend(const int x, const int y)      \
    {                                                         \
        if(x >= 0 && x < a_width && y >= 0 && y < a_height) { \
            a_pixel__##blend args;                            \
        }                                                     \
    }

pixelMake(plain, (PIXEL_DST, pixel))
pixelMake(rgba, (PIXEL_DST, red, green, blue, alpha))
pixelMake(rgb25, (PIXEL_DST, red, green, blue))
pixelMake(rgb50, (PIXEL_DST, red, green, blue))
pixelMake(rgb75, (PIXEL_DST, red, green, blue))
pixelMake(inverse, (PIXEL_DST))

void a_pixel__set(void)
{
    #define pixelInit(index, blend)                 \
    ({                                              \
        pixels[index][0] = a_pixel__noclip_##blend; \
        pixels[index][1] = a_pixel__clip_##blend;   \
    })

    pixelInit(A_PIXEL_PLAIN, plain);
    pixelInit(A_PIXEL_RGBA, rgba);
    pixelInit(A_PIXEL_RGB25, rgb25);
    pixelInit(A_PIXEL_RGB50, rgb50);
    pixelInit(A_PIXEL_RGB75, rgb75);
    pixelInit(A_PIXEL_INVERSE, inverse);

    blend = A_PIXEL_PLAIN;
    clip = true;

    a_pixel_put = pixels[blend][clip];
}

void a_pixel_setBlend(const PixelBlend_t b)
{
    blend = b;

    a_blit__setBlend(blend);
    a_draw__setBlend(blend);

    a_pixel_put = pixels[blend][clip];
}

void a_pixel_setClip(const bool c)
{
    clip = c;

    a_blit__setClip(clip);
    a_draw__setClip(clip);

    a_pixel_put = pixels[blend][clip];
}

void a_pixel_setAlpha(const uint8_t a)
{
    alpha = a;

    a_blit__setAlpha(alpha);
    a_draw__setAlpha(alpha);
}

void a_pixel_setRGB(const uint8_t r, const uint8_t g, const uint8_t b)
{
    red = r;
    green = g;
    blue = b;

    pixel = a_pixel_make(red, green, blue);

    a_blit__setRGB(red, green, blue);
    a_draw__setRGB(red, green, blue);
}

void a_pixel_setPixel(const Pixel p)
{
    pixel = p;

    red = a_pixel_red(pixel);
    green = a_pixel_green(pixel);
    blue = a_pixel_blue(pixel);

    a_blit__setRGB(red, green, blue);
    a_draw__setRGB(red, green, blue);
}
