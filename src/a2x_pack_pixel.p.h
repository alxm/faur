/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#if A_PLATFORM_SYSTEM_DESKTOP || A_PLATFORM_SYSTEM_EMSCRIPTEN
    #define A_PIXEL__BPP 32
    typedef uint32_t APixel;
#else
    #define A_PIXEL__BPP 16
    typedef uint16_t APixel;
#endif

#if A_PLATFORM_SYSTEM_EMSCRIPTEN
    #define A_PIXEL__ORDER_ABGR 1
#else
    #define A_PIXEL__ORDER_RGBA 1
#endif

#if A_PIXEL__BPP == 16
    #if A_PLATFORM_RENDER_SOFTWARE
        // RGB565
        #define A_PIXEL__BITS_RED   5
        #define A_PIXEL__BITS_GREEN 6
        #define A_PIXEL__BITS_BLUE  5
        #define A_PIXEL__BITS_ALPHA 0
    #elif A_PLATFORM_RENDER_SDL
        // RGBA5551
        #define A_PIXEL__BITS_RED   5
        #define A_PIXEL__BITS_GREEN 5
        #define A_PIXEL__BITS_BLUE  5
        #define A_PIXEL__BITS_ALPHA 1
    #endif
#elif A_PIXEL__BPP == 32
    #define A_PIXEL__BITS_RED   8
    #define A_PIXEL__BITS_GREEN 8
    #define A_PIXEL__BITS_BLUE  8

    #if A_PLATFORM_LIB_SDL == 1
        // XRGB8888
        #define A_PIXEL__BITS_ALPHA 0
    #elif A_PLATFORM_LIB_SDL == 2
        // RGBX8888 / RGBA8888
        #define A_PIXEL__BITS_ALPHA 8
    #endif
#endif

#if A_PIXEL__ORDER_RGBA
    #define A_PIXEL__SHIFT_ALPHA (0)
    #define A_PIXEL__SHIFT_BLUE  (A_PIXEL__BITS_ALPHA)
    #define A_PIXEL__SHIFT_GREEN (A_PIXEL__BITS_ALPHA + A_PIXEL__BITS_BLUE)
    #define A_PIXEL__SHIFT_RED   (A_PIXEL__BITS_ALPHA + A_PIXEL__BITS_BLUE + A_PIXEL__BITS_GREEN)
#elif A_PIXEL__ORDER_ABGR
    #define A_PIXEL__SHIFT_RED   (0)
    #define A_PIXEL__SHIFT_GREEN (A_PIXEL__BITS_RED)
    #define A_PIXEL__SHIFT_BLUE  (A_PIXEL__BITS_RED + A_PIXEL__BITS_GREEN)
    #define A_PIXEL__SHIFT_ALPHA (A_PIXEL__BITS_RED + A_PIXEL__BITS_GREEN + A_PIXEL__BITS_BLUE)
#endif

#define A_PIXEL__MASK_RED   ((1 << A_PIXEL__BITS_RED) - 1)
#define A_PIXEL__MASK_GREEN ((1 << A_PIXEL__BITS_GREEN) - 1)
#define A_PIXEL__MASK_BLUE  ((1 << A_PIXEL__BITS_BLUE) - 1)
#define A_PIXEL__MASK_ALPHA ((1 << A_PIXEL__BITS_ALPHA) - 1)

#define A_PIXEL__PACK_RED   (8 - A_PIXEL__BITS_RED)
#define A_PIXEL__PACK_GREEN (8 - A_PIXEL__BITS_GREEN)
#define A_PIXEL__PACK_BLUE  (8 - A_PIXEL__BITS_BLUE)

#if A_PLATFORM_RENDER_SOFTWARE
    #define A_PIXEL_ALPHA_MAX 256
#else
    #define A_PIXEL_ALPHA_MAX 255
#endif

static inline APixel a_pixel_fromRgb(int Red, int Green, int Blue)
{
    return (APixel)
        (((((unsigned)Red   & 0xff) >> A_PIXEL__PACK_RED)   << A_PIXEL__SHIFT_RED)   |
         ((((unsigned)Green & 0xff) >> A_PIXEL__PACK_GREEN) << A_PIXEL__SHIFT_GREEN) |
         ((((unsigned)Blue  & 0xff) >> A_PIXEL__PACK_BLUE)  << A_PIXEL__SHIFT_BLUE));
}

static inline APixel a_pixel_fromHex(uint32_t Hexcode)
{
    #if A_PIXEL__BPP == 16 || A_PIXEL__ORDER_ABGR
        return (APixel)
            (((((Hexcode >> 16) & 0xff) >> A_PIXEL__PACK_RED)   << A_PIXEL__SHIFT_RED)   |
             ((((Hexcode >> 8)  & 0xff) >> A_PIXEL__PACK_GREEN) << A_PIXEL__SHIFT_GREEN) |
             ((((Hexcode)       & 0xff) >> A_PIXEL__PACK_BLUE)  << A_PIXEL__SHIFT_BLUE));
    #elif A_PIXEL__BPP == 32 && A_PIXEL__ORDER_RGBA
        return (APixel)((Hexcode & 0xffffff) << A_PIXEL__BITS_ALPHA);
    #endif
}

static inline void a_pixel_toRgb(APixel Pixel, int* Red, int* Green, int* Blue)
{
    if(Red) {
        *Red = (int)((Pixel >> A_PIXEL__SHIFT_RED) & A_PIXEL__MASK_RED) << A_PIXEL__PACK_RED;
    }

    if(Green) {
        *Green = (int)((Pixel >> A_PIXEL__SHIFT_GREEN) & A_PIXEL__MASK_GREEN) << A_PIXEL__PACK_GREEN;
    }

    if(Blue) {
        *Blue = (int)((Pixel >> A_PIXEL__SHIFT_BLUE) & A_PIXEL__MASK_BLUE) << A_PIXEL__PACK_BLUE;
    }
}

typedef enum {
    A_PIXEL_BLEND_INVALID = -1,
    A_PIXEL_BLEND_PLAIN,
    A_PIXEL_BLEND_RGBA,
    A_PIXEL_BLEND_RGB25,
    A_PIXEL_BLEND_RGB50,
    A_PIXEL_BLEND_RGB75,
    A_PIXEL_BLEND_INVERSE,
    A_PIXEL_BLEND_MOD,
    A_PIXEL_BLEND_ADD,
    A_PIXEL_BLEND_NUM
} APixelBlend;

extern void a_pixel_push(void);
extern void a_pixel_pop(void);

extern void a_pixel_reset(void);

extern APixelBlend a_pixel_blendGet(void);
extern void a_pixel_blendSet(APixelBlend Blend);

extern int a_pixel_alphaGet(void);
extern void a_pixel_alphaSet(int Alpha);

extern void a_pixel_colorSetRgb(int Red, int Green, int Blue);
extern void a_pixel_colorSetRgba(int Red, int Green, int Blue, int Alpha);
extern void a_pixel_colorSetHex(uint32_t Hexcode);
extern void a_pixel_colorSetPixel(APixel Pixel);

extern void a_pixel_fillBlitSet(bool Fill);
extern void a_pixel_fillDrawSet(bool Fill);
