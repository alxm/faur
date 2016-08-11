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

typedef struct APixelMode {
    bool clip;
    APixelBlend blend;
    uint8_t red, green, blue;
    unsigned int alpha;
    APixel pixel;
} APixelMode;

APixelPut a_pixel_put;
static APixelPut g_pixelDraw[A_PIXEL_TYPE_NUM][2];

static APixelMode g_mode;
static AList* g_modeStack;

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

pixelMake(plain,   (PIXEL_DST, g_mode.pixel))
pixelMake(rgba,    (PIXEL_DST, g_mode.red, g_mode.green, g_mode.blue, g_mode.alpha))
pixelMake(rgb25,   (PIXEL_DST, g_mode.red, g_mode.green, g_mode.blue))
pixelMake(rgb50,   (PIXEL_DST, g_mode.red, g_mode.green, g_mode.blue))
pixelMake(rgb75,   (PIXEL_DST, g_mode.red, g_mode.green, g_mode.blue))
pixelMake(inverse, (PIXEL_DST))

void a_pixel__init(void)
{
    #define pixelInit(Index, Blend)                      \
    ({                                                   \
        g_pixelDraw[Index][0] = a_pixel__noclip_##Blend; \
        g_pixelDraw[Index][1] = a_pixel__clip_##Blend;   \
    })

    pixelInit(A_PIXEL_PLAIN, plain);
    pixelInit(A_PIXEL_RGBA, rgba);
    pixelInit(A_PIXEL_RGB25, rgb25);
    pixelInit(A_PIXEL_RGB50, rgb50);
    pixelInit(A_PIXEL_RGB75, rgb75);
    pixelInit(A_PIXEL_INVERSE, inverse);

    g_mode.blend = A_PIXEL_PLAIN;
    g_mode.clip = true;
    g_modeStack = a_list_new();

    a_pixel_put = g_pixelDraw[g_mode.blend][g_mode.clip];
}

void a_pixel__uninit(void)
{
    APixelMode* mode;

    A_LIST_ITERATE(g_modeStack, mode) {
        free(mode);
    }

    a_list_free(g_modeStack);
}

void a_pixel_push(void)
{
    APixelMode* mode = a_mem_malloc(sizeof(APixelMode));

    *mode = g_mode;
    a_list_push(g_modeStack, mode);
}

void a_pixel_pop(void)
{
    APixelMode* mode = a_list_pop(g_modeStack);

    if(mode == NULL) {
        a_out__fatal("Cannot pop APixelMode: stack is empty");
    }

    g_mode = *mode;
    free(mode);

    a_pixel_setClip(g_mode.clip);
    a_pixel_setBlend(g_mode.blend);
    a_pixel_setRGBA(g_mode.red, g_mode.green, g_mode.blue, g_mode.alpha);
}

void a_pixel_setClip(bool DoClip)
{
    g_mode.clip = DoClip;

    a_blit__setClip(g_mode.clip);
    a_draw__setClip(g_mode.clip);

    a_pixel_put = g_pixelDraw[g_mode.blend][g_mode.clip];
}

void a_pixel_setBlend(APixelBlend Blend)
{
    g_mode.blend = Blend;

    a_blit__setBlend(Blend);
    a_draw__setBlend(Blend);

    a_pixel_put = g_pixelDraw[Blend][g_mode.clip];
}

void a_pixel_setAlpha(unsigned int Alpha)
{
    g_mode.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);

    a_blit__setAlpha(g_mode.alpha);
    a_draw__setAlpha(g_mode.alpha);
}

void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    g_mode.red = Red;
    g_mode.green = Green;
    g_mode.blue = Blue;

    g_mode.pixel = a_pixel_make(g_mode.red, g_mode.green, g_mode.blue);

    a_blit__setRGB(g_mode.red, g_mode.green, g_mode.blue);
    a_draw__setRGB(g_mode.red, g_mode.green, g_mode.blue);
}

void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, unsigned int Alpha)
{
    g_mode.red = Red;
    g_mode.green = Green;
    g_mode.blue = Blue;
    g_mode.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);

    g_mode.pixel = a_pixel_make(g_mode.red, g_mode.green, g_mode.blue);

    a_blit__setAlpha(g_mode.alpha);
    a_blit__setRGB(g_mode.red, g_mode.green, g_mode.blue);

    a_draw__setAlpha(g_mode.alpha);
    a_draw__setRGB(g_mode.red, g_mode.green, g_mode.blue);
}

void a_pixel_setPixel(APixel Pixel)
{
    g_mode.pixel = Pixel;

    g_mode.red = a_pixel_red(g_mode.pixel);
    g_mode.green = a_pixel_green(g_mode.pixel);
    g_mode.blue = a_pixel_blue(g_mode.pixel);

    a_blit__setRGB(g_mode.red, g_mode.green, g_mode.blue);
    a_draw__setRGB(g_mode.red, g_mode.green, g_mode.blue);
}

APixel a_pixel__getPixel(void)
{
    return g_mode.pixel;
}
