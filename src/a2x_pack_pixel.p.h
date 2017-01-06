/*
    Copyright 2010, 2016 Alex Margarit

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
    #define A_PIXEL_PAD_BITS   0
#elif A_PIXEL_BPP == 32
    #define A_PIXEL_RED_BITS   8
    #define A_PIXEL_GREEN_BITS 8
    #define A_PIXEL_BLUE_BITS  8

    #if A_USE_LIB_SDL == 1
        // XRGB8888
        #define A_PIXEL_PAD_BITS 0
    #elif A_USE_LIB_SDL == 2
        // RGBX8888
        #define A_PIXEL_PAD_BITS 8
    #endif
#else
    #error Invalid A_PIXEL_BPP value
#endif

#define A_PIXEL_RED_SHIFT   (A_PIXEL_GREEN_BITS + A_PIXEL_BLUE_BITS + A_PIXEL_PAD_BITS)
#define A_PIXEL_GREEN_SHIFT (A_PIXEL_BLUE_BITS + A_PIXEL_PAD_BITS)
#define A_PIXEL_BLUE_SHIFT  (A_PIXEL_PAD_BITS)

#define A_PIXEL_RED_MASK   ((1 << A_PIXEL_RED_BITS) - 1)
#define A_PIXEL_GREEN_MASK ((1 << A_PIXEL_GREEN_BITS) - 1)
#define A_PIXEL_BLUE_MASK  ((1 << A_PIXEL_BLUE_BITS) - 1)

#define A_PIXEL_RED_PACK   (8 - A_PIXEL_RED_BITS)
#define A_PIXEL_GREEN_PACK (8 - A_PIXEL_GREEN_BITS)
#define A_PIXEL_BLUE_PACK  (8 - A_PIXEL_BLUE_BITS)

#define a_pixel_make(Red, Green, Blue)                               \
    (APixel)(                                                        \
        ((((Red)   >> A_PIXEL_RED_PACK))   << A_PIXEL_RED_SHIFT)   | \
        ((((Green) >> A_PIXEL_GREEN_PACK)) << A_PIXEL_GREEN_SHIFT) | \
        ((((Blue)  >> A_PIXEL_BLUE_PACK))  << A_PIXEL_BLUE_SHIFT))

#define a_pixel_red(Pixel)   ((((Pixel) >> A_PIXEL_RED_SHIFT)   & A_PIXEL_RED_MASK)   << A_PIXEL_RED_PACK)
#define a_pixel_green(Pixel) ((((Pixel) >> A_PIXEL_GREEN_SHIFT) & A_PIXEL_GREEN_MASK) << A_PIXEL_GREEN_PACK)
#define a_pixel_blue(Pixel)  ((((Pixel) >> A_PIXEL_BLUE_SHIFT)  & A_PIXEL_BLUE_MASK)  << A_PIXEL_BLUE_PACK)

#define A_PIXEL_ALPHA_MAX 256u

typedef enum {
    A_PIXEL_BLEND_PLAIN,
    A_PIXEL_BLEND_RGBA,
    A_PIXEL_BLEND_RGB25,
    A_PIXEL_BLEND_RGB50,
    A_PIXEL_BLEND_RGB75,
    A_PIXEL_BLEND_INVERSE,
    A_PIXEL_BLEND_NUM
} APixelBlend;

extern void a_pixel_push(void);
extern void a_pixel_pop(void);

extern void a_pixel_reset(void);

extern void a_pixel_setBlend(APixelBlend Blend);
extern void a_pixel_setAlpha(unsigned int Alpha);
extern void a_pixel_setRGB(uint8_t Red, uint8_t Green, uint8_t Blue);
extern void a_pixel_setRGBA(uint8_t Red, uint8_t Green, uint8_t Blue, unsigned int Alpha);
extern void a_pixel_setPixel(APixel Pixel);
