/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

#if A_PLATFORM_LINUXPC
    #define A_PIXEL_BPP 32
    typedef uint32_t APixel;
#else
    #define A_PIXEL_BPP 16
    typedef uint16_t APixel;
#endif

#if A_PIXEL_BPP == 16
    // RGB565
    #define A_PIXEL_RED_BITS   5
    #define A_PIXEL_GREEN_BITS 6
    #define A_PIXEL_BLUE_BITS  5
    #define A_PIXEL_ALPHA_BITS 0
#elif A_PIXEL_BPP == 32
    #define A_PIXEL_RED_BITS   8
    #define A_PIXEL_GREEN_BITS 8
    #define A_PIXEL_BLUE_BITS  8

    #if A_CONFIG_LIB_SDL == 1
        // XRGB8888
        #define A_PIXEL_ALPHA_BITS 0
    #elif A_CONFIG_LIB_SDL == 2
        // RGBXA8888 / RGBA8888
        #define A_PIXEL_ALPHA_BITS 8
    #endif
#endif

#define A_PIXEL_RED_SHIFT   (A_PIXEL_GREEN_BITS + A_PIXEL_BLUE_BITS + A_PIXEL_ALPHA_BITS)
#define A_PIXEL_GREEN_SHIFT (A_PIXEL_BLUE_BITS + A_PIXEL_ALPHA_BITS)
#define A_PIXEL_BLUE_SHIFT  (A_PIXEL_ALPHA_BITS)
#define A_PIXEL_ALPHA_SHIFT (0)

#define A_PIXEL_RED_MASK   ((1 << A_PIXEL_RED_BITS) - 1)
#define A_PIXEL_GREEN_MASK ((1 << A_PIXEL_GREEN_BITS) - 1)
#define A_PIXEL_BLUE_MASK  ((1 << A_PIXEL_BLUE_BITS) - 1)
#define A_PIXEL_ALPHA_MASK ((1 << A_PIXEL_ALPHA_BITS) - 1)

#define A_PIXEL_RED_PACK   (8 - A_PIXEL_RED_BITS)
#define A_PIXEL_GREEN_PACK (8 - A_PIXEL_GREEN_BITS)
#define A_PIXEL_BLUE_PACK  (8 - A_PIXEL_BLUE_BITS)

#if A_CONFIG_RENDER_SOFTWARE
    #define A_PIXEL_ALPHA_MAX 256
#elif A_CONFIG_RENDER_SDL2
    #define A_PIXEL_ALPHA_MAX 255
#endif

static inline APixel a_pixel_rgb(int Red, int Green, int Blue)
{
    return (APixel)
        (((((unsigned)Red   & 0xff) >> A_PIXEL_RED_PACK)   << A_PIXEL_RED_SHIFT)   |
         ((((unsigned)Green & 0xff) >> A_PIXEL_GREEN_PACK) << A_PIXEL_GREEN_SHIFT) |
         ((((unsigned)Blue  & 0xff) >> A_PIXEL_BLUE_PACK)  << A_PIXEL_BLUE_SHIFT));
}

static inline APixel a_pixel_hex(uint32_t Hexcode)
{
    #if A_PIXEL_BPP == 16
        return (APixel)
            (((((Hexcode >> 16) & 0xff) >> A_PIXEL_RED_PACK)   << A_PIXEL_RED_SHIFT)   |
             ((((Hexcode >> 8)  & 0xff) >> A_PIXEL_GREEN_PACK) << A_PIXEL_GREEN_SHIFT) |
             ((((Hexcode)       & 0xff) >> A_PIXEL_BLUE_PACK)  << A_PIXEL_BLUE_SHIFT));
    #elif A_PIXEL_BPP == 32
        #if A_CONFIG_LIB_SDL == 1
            return (APixel)(Hexcode & 0xffffff);
        #elif A_CONFIG_LIB_SDL == 2
            return (APixel)(Hexcode << A_PIXEL_ALPHA_BITS);
        #endif
    #endif
}

static inline int a_pixel_red(APixel Pixel)
{
    return (int)((Pixel >> A_PIXEL_RED_SHIFT) & A_PIXEL_RED_MASK) << A_PIXEL_RED_PACK;
}

static inline int a_pixel_green(APixel Pixel)
{
    return (int)((Pixel >> A_PIXEL_GREEN_SHIFT) & A_PIXEL_GREEN_MASK) << A_PIXEL_GREEN_PACK;
}

static inline int a_pixel_blue(APixel Pixel)
{
    return (int)((Pixel >> A_PIXEL_BLUE_SHIFT) & A_PIXEL_BLUE_MASK) << A_PIXEL_BLUE_PACK;
}

typedef enum {
    A_PIXEL_BLEND_PLAIN,
    A_PIXEL_BLEND_RGBA,
    A_PIXEL_BLEND_RGB25,
    A_PIXEL_BLEND_RGB50,
    A_PIXEL_BLEND_RGB75,
    A_PIXEL_BLEND_INVERSE,
    A_PIXEL_BLEND_COLORMOD,
    A_PIXEL_BLEND_NUM
} APixelBlend;

extern void a_pixel_push(void);
extern void a_pixel_pop(void);

extern void a_pixel_reset(void);

extern void a_pixel_setBlend(APixelBlend Blend);
extern void a_pixel_setAlpha(int Alpha);
extern void a_pixel_setRGB(int Red, int Green, int Blue);
extern void a_pixel_setRGBA(int Red, int Green, int Blue, int Alpha);
extern void a_pixel_setHex(uint32_t Hexcode);
extern void a_pixel_setPixel(APixel Pixel);
extern void a_pixel_setBlitFillFlat(bool FillFlat);
