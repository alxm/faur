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
static APixelPut g_pixelDraw[A_PIXEL_TYPE_NUM][2];

static APixelBlend g_blend;
static bool g_clip;

static uint8_t g_red, g_green, g_blue, g_alpha;
static APixel g_pixel;

#define PIXEL_DST (a_screen__pixels + Y * a_screen__width + X)

#define pixelMake(Blend, Args)                                \
                                                              \
    void a_pixel__noclip_##Blend(int X, int Y)                \
    {                                                         \
        a_pixel__##Blend Args;                                \
    }                                                         \
                                                              \
    void a_pixel__clip_##Blend(int X, int Y)                  \
    {                                                         \
        if(X >= 0 && X < a_screen__width                      \
           && Y >= 0 && Y < a_screen__height) {               \
            a_pixel__##Blend Args;                            \
        }                                                     \
    }

pixelMake(plain,   (PIXEL_DST, g_pixel))
pixelMake(rgba,    (PIXEL_DST, g_red, g_green, g_blue, g_alpha))
pixelMake(rgb25,   (PIXEL_DST, g_red, g_green, g_blue))
pixelMake(rgb50,   (PIXEL_DST, g_red, g_green, g_blue))
pixelMake(rgb75,   (PIXEL_DST, g_red, g_green, g_blue))
pixelMake(inverse, (PIXEL_DST))

void a_pixel__init(void)
{
    #define pixelInit(Index, Blend)                 \
    ({                                              \
        g_pixelDraw[Index][0] = a_pixel__noclip_##Blend; \
        g_pixelDraw[Index][1] = a_pixel__clip_##Blend;   \
    })

    pixelInit(A_PIXEL_PLAIN, plain);
    pixelInit(A_PIXEL_RGBA, rgba);
    pixelInit(A_PIXEL_RGB25, rgb25);
    pixelInit(A_PIXEL_RGB50, rgb50);
    pixelInit(A_PIXEL_RGB75, rgb75);
    pixelInit(A_PIXEL_INVERSE, inverse);

    g_blend = A_PIXEL_PLAIN;
    g_clip = true;

    a_pixel_put = g_pixelDraw[g_blend][g_clip];
}

void a_pixel_setBlend(APixelBlend Blend)
{
    g_blend = Blend;

    a_blit__setBlend(Blend);
    a_draw__setBlend(Blend);

    a_pixel_put = g_pixelDraw[Blend][g_clip];
}

void a_pixel_setClip(bool DoClip)
{
    g_clip = DoClip;

    a_blit__setClip(g_clip);
    a_draw__setClip(g_clip);

    a_pixel_put = g_pixelDraw[g_blend][g_clip];
}

void a_pixel_setAlpha(uint8_t Alpha)
{
    g_alpha = Alpha;

    a_blit__setAlpha(g_alpha);
    a_draw__setAlpha(g_alpha);
}

void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    g_red = Red;
    g_green = Green;
    g_blue = Blue;

    g_pixel = a_pixel_make(g_red, g_green, g_blue);

    a_blit__setRGB(g_red, g_green, g_blue);
    a_draw__setRGB(g_red, g_green, g_blue);
}

void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
{
    g_red = Red;
    g_green = Green;
    g_blue = Blue;
    g_alpha = Alpha;

    g_pixel = a_pixel_make(g_red, g_green, g_blue);

    a_blit__setAlpha(g_alpha);
    a_blit__setRGB(g_red, g_green, g_blue);

    a_draw__setAlpha(g_alpha);
    a_draw__setRGB(g_red, g_green, g_blue);
}

void a_pixel_setPixel(APixel Pixel)
{
    g_pixel = Pixel;

    g_red = a_pixel_red(g_pixel);
    g_green = a_pixel_green(g_pixel);
    g_blue = a_pixel_blue(g_pixel);

    a_blit__setRGB(g_red, g_green, g_blue);
    a_draw__setRGB(g_red, g_green, g_blue);
}
