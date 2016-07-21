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

APixelPut a_pixel_put;
static APixelPut pixels[A_PIXEL_TYPE_NUM][2];

static APixelBlend blend;
static bool clip;

static uint8_t alpha;
static uint8_t red, green, blue;
static APixel pixel;

#define PIXEL_DST (a_pixels + Y * a_width + X)

#define pixelMake(Blend, Args)                                \
                                                              \
    void a_pixel__noclip_##Blend(int X, int Y)                \
    {                                                         \
        a_pixel__##Blend Args;                                \
    }                                                         \
                                                              \
    void a_pixel__clip_##Blend(int X, int Y)                  \
    {                                                         \
        if(X >= 0 && X < a_width && Y >= 0 && Y < a_height) { \
            a_pixel__##Blend Args;                            \
        }                                                     \
    }

pixelMake(plain, (PIXEL_DST, pixel))
pixelMake(rgba, (PIXEL_DST, red, green, blue, alpha))
pixelMake(rgb25, (PIXEL_DST, red, green, blue))
pixelMake(rgb50, (PIXEL_DST, red, green, blue))
pixelMake(rgb75, (PIXEL_DST, red, green, blue))
pixelMake(inverse, (PIXEL_DST))

void a_pixel__init(void)
{
    #define pixelInit(Index, Blend)                 \
    ({                                              \
        pixels[Index][0] = a_pixel__noclip_##Blend; \
        pixels[Index][1] = a_pixel__clip_##Blend;   \
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

void a_pixel_setBlend(APixelBlend Blend)
{
    blend = Blend;

    a_blit__setBlend(Blend);
    a_draw__setBlend(Blend);

    a_pixel_put = pixels[Blend][clip];
}

void a_pixel_setClip(bool DoClip)
{
    clip = DoClip;

    a_blit__setClip(clip);
    a_draw__setClip(clip);

    a_pixel_put = pixels[blend][clip];
}

void a_pixel_setAlpha(uint8_t Alpha)
{
    alpha = Alpha;

    a_blit__setAlpha(alpha);
    a_draw__setAlpha(alpha);
}

void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    red = Red;
    green = Green;
    blue = Blue;

    pixel = a_pixel_make(red, green, blue);

    a_blit__setRGB(red, green, blue);
    a_draw__setRGB(red, green, blue);
}

void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
{
    red = Red;
    green = Green;
    blue = Blue;
    alpha = Alpha;

    pixel = a_pixel_make(red, green, blue);

    a_blit__setAlpha(alpha);
    a_blit__setRGB(red, green, blue);

    a_draw__setAlpha(alpha);
    a_draw__setRGB(red, green, blue);
}

void a_pixel_setPixel(APixel Pixel)
{
    pixel = Pixel;

    red = a_pixel_red(pixel);
    green = a_pixel_green(pixel);
    blue = a_pixel_blue(pixel);

    a_blit__setRGB(red, green, blue);
    a_draw__setRGB(red, green, blue);
}
