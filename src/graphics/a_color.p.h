/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "general/a_system_includes.h"

#if A_CONFIG_SCREEN_BPP == 16
    typedef uint16_t APixel;
#elif A_CONFIG_SCREEN_BPP == 32
    typedef uint32_t APixel;
#endif

typedef struct {
    int r, g, b;
} ARgb;

#if A_CONFIG_SCREEN_BPP == 16
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        // RGB565
        #define A__PX_BITS_R 5
        #define A__PX_BITS_G 6
        #define A__PX_BITS_B 5
        #define A__PX_BITS_A 0
    #elif A_CONFIG_LIB_RENDER_SDL
        // RGBA5551
        #define A__PX_BITS_R 5
        #define A__PX_BITS_G 5
        #define A__PX_BITS_B 5
        #define A__PX_BITS_A 1
    #endif
#elif A_CONFIG_SCREEN_BPP == 32
    #define A__PX_BITS_R 8
    #define A__PX_BITS_G 8
    #define A__PX_BITS_B 8

    #if A_CONFIG_LIB_SDL == 1
        // XRGB8888
        #define A__PX_BITS_A 0
    #elif A_CONFIG_LIB_SDL == 2
        // RGBX8888 / RGBA8888
        #define A__PX_BITS_A 8
    #endif
#endif

#if A_CONFIG_SCREEN_FORMAT_RGBA
    #define A__PX_SHIFT_A (0)
    #define A__PX_SHIFT_B (A__PX_BITS_A)
    #define A__PX_SHIFT_G (A__PX_BITS_A + A__PX_BITS_B)
    #define A__PX_SHIFT_R (A__PX_BITS_A + A__PX_BITS_B + A__PX_BITS_G)
#elif A_CONFIG_SCREEN_FORMAT_ABGR
    #define A__PX_SHIFT_R (0)
    #define A__PX_SHIFT_G (A__PX_BITS_R)
    #define A__PX_SHIFT_B (A__PX_BITS_R + A__PX_BITS_G)
    #define A__PX_SHIFT_A (A__PX_BITS_R + A__PX_BITS_G + A__PX_BITS_B)
#endif

#define A__PX_MASK_R ((1 << A__PX_BITS_R) - 1)
#define A__PX_MASK_G ((1 << A__PX_BITS_G) - 1)
#define A__PX_MASK_B ((1 << A__PX_BITS_B) - 1)
#define A__PX_MASK_A ((1 << A__PX_BITS_A) - 1)

#define A__PX_PACK_R (8 - A__PX_BITS_R)
#define A__PX_PACK_G (8 - A__PX_BITS_G)
#define A__PX_PACK_B (8 - A__PX_BITS_B)

#if A_CONFIG_LIB_RENDER_SOFTWARE
    #define A_COLOR_ALPHA_MAX 256
#else
    #define A_COLOR_ALPHA_MAX 255
#endif

static inline APixel a_pixel_fromRgb(int Red, int Green, int Blue)
{
    return (APixel)
        (((((unsigned)Red   & 0xff) >> A__PX_PACK_R) << A__PX_SHIFT_R) |
         ((((unsigned)Green & 0xff) >> A__PX_PACK_G) << A__PX_SHIFT_G) |
         ((((unsigned)Blue  & 0xff) >> A__PX_PACK_B) << A__PX_SHIFT_B));
}

static inline APixel a_pixel_fromHex(uint32_t Hexcode)
{
    #if A_CONFIG_SCREEN_BPP == 16 || A_CONFIG_SCREEN_FORMAT_ABGR
        return (APixel)
            (((((Hexcode >> 16) & 0xff) >> A__PX_PACK_R) << A__PX_SHIFT_R) |
             ((((Hexcode >> 8)  & 0xff) >> A__PX_PACK_G) << A__PX_SHIFT_G) |
             ((((Hexcode)       & 0xff) >> A__PX_PACK_B) << A__PX_SHIFT_B));
    #elif A_CONFIG_SCREEN_BPP == 32 && A_CONFIG_SCREEN_FORMAT_RGBA
        return (APixel)((Hexcode & 0xffffff) << A__PX_BITS_A);
    #endif
}

static inline ARgb a_pixel_toRgb(APixel Pixel)
{
    return (ARgb){
        (int)((Pixel >> A__PX_SHIFT_R) & A__PX_MASK_R) << A__PX_PACK_R,
        (int)((Pixel >> A__PX_SHIFT_G) & A__PX_MASK_G) << A__PX_PACK_G,
        (int)((Pixel >> A__PX_SHIFT_B) & A__PX_MASK_B) << A__PX_PACK_B};
}

typedef enum {
    A_COLOR_BLEND_INVALID = -1,
    A_COLOR_BLEND_PLAIN,
    A_COLOR_BLEND_RGBA,
    A_COLOR_BLEND_RGB25,
    A_COLOR_BLEND_RGB50,
    A_COLOR_BLEND_RGB75,
    A_COLOR_BLEND_INVERSE,
    A_COLOR_BLEND_MOD,
    A_COLOR_BLEND_ADD,
    A_COLOR_BLEND_NUM
} AColorBlend;

extern void a_color_push(void);
extern void a_color_pop(void);
extern void a_color_reset(void);

extern void a_color_blendSet(AColorBlend Blend);
extern void a_color_alphaSet(int Alpha);

extern void a_color_baseSetRgb(int Red, int Green, int Blue);
extern void a_color_baseSetRgba(int Red, int Green, int Blue, int Alpha);
extern void a_color_baseSetHex(uint32_t Hexcode);
extern void a_color_baseSetPixel(APixel Pixel);

extern void a_color_fillBlitSet(bool Fill);
extern void a_color_fillDrawSet(bool Fill);
