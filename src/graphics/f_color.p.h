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

#include "../general/f_system_includes.h"

#define F__C_RGBA (1 << 30)
#define F__C_ARGB (1 << 29)
#define F__C_ABGR (1 << 28)

#define F__C_565  (1 << 20)
#define F__C_5551 (1 << 19)
#define F__C_8888 (1 << 18)

#define F__C_16 (1 << 10)
#define F__C_32 (1 << 9)

#define F__C_ENDIAN (1 << 0)

#define F_COLOR_FORMAT_RGB_565   (F__C_RGBA | F__C_565 | F__C_16)
#define F_COLOR_FORMAT_RGB_565_B (F__C_RGBA | F__C_565 | F__C_16 | F__C_ENDIAN)
#define F_COLOR_FORMAT_RGBA_5551 (F__C_RGBA | F__C_5551 | F__C_16)
#define F_COLOR_FORMAT_RGBA_8888 (F__C_RGBA | F__C_8888 | F__C_32)
#define F_COLOR_FORMAT_ARGB_8888 (F__C_ARGB | F__C_8888 | F__C_32)
#define F_COLOR_FORMAT_ABGR_8888 (F__C_ABGR | F__C_8888 | F__C_32)

#if F_CONFIG_SCREEN_FORMAT & F__C_16
    #define F_COLOR_BPP 16
    typedef uint16_t FColorPixel;
#elif F_CONFIG_SCREEN_FORMAT & F__C_32
    #define F_COLOR_BPP 32
    typedef uint32_t FColorPixel;
#else
    #error Invalid F_CONFIG_SCREEN_FORMAT value
#endif

typedef struct {
    int r, g, b;
} FColorRgb;

#include "../graphics/f_palette.p.h"

#if F_CONFIG_SCREEN_FORMAT & F__C_565
    #define F__PX_BITS_R 5
    #define F__PX_BITS_G 6
    #define F__PX_BITS_B 5
    #define F__PX_BITS_A 0
#elif F_CONFIG_SCREEN_FORMAT & F__C_5551
    #define F__PX_BITS_R 5
    #define F__PX_BITS_G 5
    #define F__PX_BITS_B 5
    #define F__PX_BITS_A 1
#elif F_CONFIG_SCREEN_FORMAT & F__C_8888
    #define F__PX_BITS_R 8
    #define F__PX_BITS_G 8
    #define F__PX_BITS_B 8
    #define F__PX_BITS_A 8
#else
    #error Invalid F_CONFIG_SCREEN_FORMAT value
#endif

#if F_CONFIG_SCREEN_FORMAT & F__C_RGBA
    #define F__PX_SHIFT_R (F__PX_BITS_A + F__PX_BITS_B + F__PX_BITS_G)
    #define F__PX_SHIFT_G (F__PX_BITS_A + F__PX_BITS_B)
    #define F__PX_SHIFT_B (F__PX_BITS_A)
    #define F__PX_SHIFT_A (0)
#elif F_CONFIG_SCREEN_FORMAT & F__C_ARGB
    #define F__PX_SHIFT_A (F__PX_BITS_B + F__PX_BITS_G + F__PX_BITS_R)
    #define F__PX_SHIFT_R (F__PX_BITS_B + F__PX_BITS_G)
    #define F__PX_SHIFT_G (F__PX_BITS_B)
    #define F__PX_SHIFT_B (0)
#elif F_CONFIG_SCREEN_FORMAT & F__C_ABGR
    #define F__PX_SHIFT_A (F__PX_BITS_R + F__PX_BITS_G + F__PX_BITS_B)
    #define F__PX_SHIFT_B (F__PX_BITS_R + F__PX_BITS_G)
    #define F__PX_SHIFT_G (F__PX_BITS_R)
    #define F__PX_SHIFT_R (0)
#else
    #error Invalid F_CONFIG_SCREEN_FORMAT value
#endif

#define F__PX_PACK_R (8 - F__PX_BITS_R)
#define F__PX_PACK_G (8 - F__PX_BITS_G)
#define F__PX_PACK_B (8 - F__PX_BITS_B)

#if F_CONFIG_SCREEN_RENDER_SOFTWARE
    #define F_COLOR_ALPHA_MAX 256
#else
    #define F_COLOR_ALPHA_MAX 255
#endif

#if F_CONFIG_SCREEN_FORMAT & F__C_ENDIAN
static inline FColorPixel f__color_flipEndianness(FColorPixel Pixel)
{
    #if F_CONFIG_SCREEN_FORMAT & F__C_16
        return (FColorPixel)((Pixel << 8) | (Pixel >> 8));
    #else
        #error Invalid F_CONFIG_SCREEN_FORMAT value
    #endif
}
#endif

static inline FColorPixel f_color_pixelFromRgb3(int Red, int Green, int Blue)
{
    FColorPixel p = (FColorPixel)
        (((((unsigned)Red   & 0xff) >> F__PX_PACK_R) << F__PX_SHIFT_R) |
         ((((unsigned)Green & 0xff) >> F__PX_PACK_G) << F__PX_SHIFT_G) |
         ((((unsigned)Blue  & 0xff) >> F__PX_PACK_B) << F__PX_SHIFT_B));

    #if F_CONFIG_SCREEN_FORMAT & F__C_ENDIAN
        p = f__color_flipEndianness(p);
    #endif

    return p;
}

static inline FColorPixel f_color_pixelFromRgb(FColorRgb Rgb)
{
    return f_color_pixelFromRgb3(Rgb.r, Rgb.g, Rgb.b);
}

static inline FColorPixel f_color_pixelFromHex(uint32_t Hexcode)
{
    #if F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_RGBA_8888
        return (FColorPixel)((Hexcode & 0xffffff) << F__PX_BITS_A);
    #elif F_CONFIG_SCREEN_FORMAT == F_COLOR_FORMAT_ARGB_8888
        return (FColorPixel)(Hexcode & 0xffffff);
    #else
        return f_color_pixelFromRgb3(
                (int)(Hexcode >> 16), (int)(Hexcode >> 8), (int)(Hexcode));
    #endif
}

static inline FColorRgb f_color_pixelToRgb(FColorPixel Pixel)
{
    #if F_CONFIG_SCREEN_FORMAT & F__C_ENDIAN
        Pixel = f__color_flipEndianness(Pixel);
    #endif

    FColorRgb rgb = {
        (int)((Pixel >> F__PX_SHIFT_R) << F__PX_PACK_R) & 0xff,
        (int)((Pixel >> F__PX_SHIFT_G) << F__PX_PACK_G) & 0xff,
        (int)((Pixel >> F__PX_SHIFT_B) << F__PX_PACK_B) & 0xff
    };

    return rgb;
}

static inline int f_color_pixelToRgbAny(FColorPixel Pixel)
{
    #if F_CONFIG_SCREEN_FORMAT & F__C_ENDIAN
        Pixel = f__color_flipEndianness(Pixel);
    #endif

    return (int)((Pixel >> F__PX_SHIFT_B) << F__PX_PACK_B) & 0xff;
}

typedef enum {
    F_COLOR_BLEND_INVALID = -1,
    F_COLOR_BLEND_SOLID,
    F_COLOR_BLEND_ALPHA,
    F_COLOR_BLEND_ALPHA_25,
    F_COLOR_BLEND_ALPHA_50,
    F_COLOR_BLEND_ALPHA_75,
    F_COLOR_BLEND_ALPHA_MASK,
    F_COLOR_BLEND_INVERSE,
    F_COLOR_BLEND_MOD,
    F_COLOR_BLEND_ADD,
    F_COLOR_BLEND_NUM
} FColorBlend;

extern void f_color_push(void);
extern void f_color_pop(void);
extern void f_color_reset(void);

extern void f_color_paletteSet(const FPalette* Palette);

extern void f_color_blendSet(FColorBlend Blend);
extern void f_color_alphaSet(int Alpha);

extern void f_color_colorSetRgb(int Red, int Green, int Blue);
extern void f_color_colorSetRgba(int Red, int Green, int Blue, int Alpha);
extern void f_color_colorSetHex(uint32_t Hexcode);
extern void f_color_colorSetPixel(FColorPixel Pixel);
extern void f_color_colorSetIndex(unsigned ColorIndex);

extern void f_color_fillBlitSet(bool Fill);
extern void f_color_fillDrawSet(bool Fill);

#endif // F_INC_GRAPHICS_COLOR_P_H
