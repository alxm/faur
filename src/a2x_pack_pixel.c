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

APixelMode a_pixel__mode;
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

pixelMake(plain,   (PIXEL_DST, a_pixel__mode.pixel))
pixelMake(rgba,    (PIXEL_DST, a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue, a_pixel__mode.alpha))
pixelMake(rgb25,   (PIXEL_DST, a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue))
pixelMake(rgb50,   (PIXEL_DST, a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue))
pixelMake(rgb75,   (PIXEL_DST, a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue))
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

    a_pixel__mode.blend = A_PIXEL_PLAIN;
    a_pixel__mode.clip = true;
    g_modeStack = a_list_new();

    a_pixel_put = g_pixelDraw[a_pixel__mode.blend][a_pixel__mode.clip];
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

    *mode = a_pixel__mode;
    a_list_push(g_modeStack, mode);
}

void a_pixel_pop(void)
{
    APixelMode* mode = a_list_pop(g_modeStack);

    if(mode == NULL) {
        a_out__fatal("Cannot pop APixelMode: stack is empty");
    }

    a_pixel__mode = *mode;
    free(mode);

    a_pixel_setClip(a_pixel__mode.clip);
    a_pixel_setBlend(a_pixel__mode.blend);
    a_pixel_setRGBA(a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue, a_pixel__mode.alpha);
}

void a_pixel_setClip(bool DoClip)
{
    a_pixel__mode.clip = DoClip;

    a_blit__updateRoutines();
    a_draw__updateRoutines();

    a_pixel_put = g_pixelDraw[a_pixel__mode.blend][a_pixel__mode.clip];
}

void a_pixel_setBlend(APixelBlend Blend)
{
    a_pixel__mode.blend = Blend;

    a_blit__updateRoutines();
    a_draw__updateRoutines();

    a_pixel_put = g_pixelDraw[Blend][a_pixel__mode.clip];
}

void a_pixel_setAlpha(unsigned int Alpha)
{
    a_pixel__mode.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);
}

void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    a_pixel__mode.red = Red;
    a_pixel__mode.green = Green;
    a_pixel__mode.blue = Blue;

    a_pixel__mode.pixel = a_pixel_make(a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue);
}

void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, unsigned int Alpha)
{
    a_pixel__mode.red = Red;
    a_pixel__mode.green = Green;
    a_pixel__mode.blue = Blue;
    a_pixel__mode.alpha = a_math_min(Alpha, A_PIXEL_ALPHA_MAX);

    a_pixel__mode.pixel = a_pixel_make(a_pixel__mode.red, a_pixel__mode.green, a_pixel__mode.blue);
}

void a_pixel_setPixel(APixel Pixel)
{
    a_pixel__mode.pixel = Pixel;

    a_pixel__mode.red = a_pixel_red(a_pixel__mode.pixel);
    a_pixel__mode.green = a_pixel_green(a_pixel__mode.pixel);
    a_pixel__mode.blue = a_pixel_blue(a_pixel__mode.pixel);
}
