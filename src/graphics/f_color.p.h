/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_GRAPHICS_COLOR_P_H
#define F_INC_GRAPHICS_COLOR_P_H

#include "general/f_system_includes.h"

#if F_CONFIG_SCREEN_BPP == 16
    typedef uint16_t FColorPixel;
#elif F_CONFIG_SCREEN_BPP == 32
    typedef uint32_t FColorPixel;
#else
    #error F_CONFIG_SCREEN_BPP must be 16 or 32
#endif

typedef struct {
    int r, g, b;
} FColorRgb;

#if F_CONFIG_SCREEN_BPP == 16
    #if F_CONFIG_LIB_RENDER_SOFTWARE
        // RGB565
        #define F__PX_BITS_R 5
        #define F__PX_BITS_G 6
        #define F__PX_BITS_B 5
        #define F__PX_BITS_A 0
    #elif F_CONFIG_LIB_RENDER_SDL
        // RGBA5551
        #define F__PX_BITS_R 5
        #define F__PX_BITS_G 5
        #define F__PX_BITS_B 5
        #define F__PX_BITS_A 1
    #endif
#elif F_CONFIG_SCREEN_BPP == 32
    #define F__PX_BITS_R 8
    #define F__PX_BITS_G 8
    #define F__PX_BITS_B 8

    #if F_CONFIG_LIB_SDL == 1
        // XRGB8888
        #define F__PX_BITS_A 0
    #elif F_CONFIG_LIB_SDL == 2
        // RGBX8888 / RGBA8888
        #define F__PX_BITS_A 8
    #endif
#endif

#if F_CONFIG_SCREEN_ORDER_RGBA
    #define F__PX_SHIFT_A (0)
    #define F__PX_SHIFT_B (F__PX_BITS_A)
    #define F__PX_SHIFT_G (F__PX_BITS_A + F__PX_BITS_B)
    #define F__PX_SHIFT_R (F__PX_BITS_A + F__PX_BITS_B + F__PX_BITS_G)
#elif F_CONFIG_SCREEN_ORDER_ABGR
    #define F__PX_SHIFT_R (0)
    #define F__PX_SHIFT_G (F__PX_BITS_R)
    #define F__PX_SHIFT_B (F__PX_BITS_R + F__PX_BITS_G)
    #define F__PX_SHIFT_A (F__PX_BITS_R + F__PX_BITS_G + F__PX_BITS_B)
#endif

#define F__PX_MASK_R ((1 << F__PX_BITS_R) - 1)
#define F__PX_MASK_G ((1 << F__PX_BITS_G) - 1)
#define F__PX_MASK_B ((1 << F__PX_BITS_B) - 1)
#define F__PX_MASK_A ((1 << F__PX_BITS_A) - 1)

#define F__PX_PACK_R (8 - F__PX_BITS_R)
#define F__PX_PACK_G (8 - F__PX_BITS_G)
#define F__PX_PACK_B (8 - F__PX_BITS_B)

#if F_CONFIG_LIB_RENDER_SOFTWARE
    #define F_COLOR_ALPHA_MAX 256
#else
    #define F_COLOR_ALPHA_MAX 255
#endif

static inline FColorPixel f_color_pixelFromRgb(int Red, int Green, int Blue)
{
    return (FColorPixel)
        (((((unsigned)Red   & 0xff) >> F__PX_PACK_R) << F__PX_SHIFT_R) |
         ((((unsigned)Green & 0xff) >> F__PX_PACK_G) << F__PX_SHIFT_G) |
         ((((unsigned)Blue  & 0xff) >> F__PX_PACK_B) << F__PX_SHIFT_B));
}

static inline FColorPixel f_color_pixelFromHex(uint32_t Hexcode)
{
    #if F_CONFIG_SCREEN_BPP == 32 && F_CONFIG_SCREEN_ORDER_RGBA
        return (FColorPixel)((Hexcode & 0xffffff) << F__PX_BITS_A);
    #else
        return (FColorPixel)
            (((((Hexcode >> 16) & 0xff) >> F__PX_PACK_R) << F__PX_SHIFT_R) |
             ((((Hexcode >> 8)  & 0xff) >> F__PX_PACK_G) << F__PX_SHIFT_G) |
             ((((Hexcode)       & 0xff) >> F__PX_PACK_B) << F__PX_SHIFT_B));
    #endif
}

static inline FColorRgb f_color_pixelToRgb(FColorPixel Pixel)
{
    FColorRgb rgb = {
        (int)((Pixel >> F__PX_SHIFT_R) & F__PX_MASK_R) << F__PX_PACK_R,
        (int)((Pixel >> F__PX_SHIFT_G) & F__PX_MASK_G) << F__PX_PACK_G,
        (int)((Pixel >> F__PX_SHIFT_B) & F__PX_MASK_B) << F__PX_PACK_B
    };

    return rgb;
}

typedef enum {
    F_COLOR_BLEND_INVALID = -1,
    F_COLOR_BLEND_PLAIN,
    F_COLOR_BLEND_RGBA,
    F_COLOR_BLEND_RGB25,
    F_COLOR_BLEND_RGB50,
    F_COLOR_BLEND_RGB75,
    F_COLOR_BLEND_INVERSE,
    F_COLOR_BLEND_MOD,
    F_COLOR_BLEND_ADD,
    F_COLOR_BLEND_NUM
} FColorBlend;

extern void f_color_push(void);
extern void f_color_pop(void);
extern void f_color_reset(void);

extern void f_color_blendSet(FColorBlend Blend);
extern void f_color_alphaSet(int Alpha);

extern void f_color_baseSetRgb(int Red, int Green, int Blue);
extern void f_color_baseSetRgba(int Red, int Green, int Blue, int Alpha);
extern void f_color_baseSetHex(uint32_t Hexcode);
extern void f_color_baseSetPixel(FColorPixel Pixel);

extern void f_color_fillBlitSet(bool Fill);
extern void f_color_fillDrawSet(bool Fill);

#endif // F_INC_GRAPHICS_COLOR_P_H
