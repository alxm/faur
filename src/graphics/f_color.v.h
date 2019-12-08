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

#ifndef F_INC_GRAPHICS_COLOR_V_H
#define F_INC_GRAPHICS_COLOR_V_H

#include "f_color.p.h"

#define F__OPTIMIZE_ALPHA (F_CONFIG_RENDER_SOFTWARE && F_CONFIG_TRAIT_SLOW_MUL)

typedef struct {
    const FPalette* palette;
    FColorBlend blend;
    #if F__OPTIMIZE_ALPHA
        FColorBlend canonicalBlend;
    #endif
    FColorRgb rgb;
    FColorPixel pixel;
    int alpha;
    bool fillBlit;
    bool fillDraw;
} FColorState;

typedef enum {
    F_COLOR__PAL_INVALID = -1,
    F_COLOR__PAL_BROWN1,
    F_COLOR__PAL_BROWN2,
    F_COLOR__PAL_GRAY1,
    F_COLOR__PAL_GRAY2,
    F_COLOR__PAL_BLUE1,
    F_COLOR__PAL_BLUE2,
    F_COLOR__PAL_GREEN1,
    F_COLOR__PAL_GREEN2,
    F_COLOR__PAL_CHARTREUSE1,
    F_COLOR__PAL_CHARTREUSE2,
    F_COLOR__PAL_NUM
} FColorPaletteInternal;

#include "../general/f_init.v.h"
#include "../math/f_math.v.h"

extern const FPack f_pack__color;

extern FColorState f__color;

extern FColorPixel f_color__key;
extern FColorPixel f_color__limit;

extern void f_color__colorSetInternal(FColorPaletteInternal ColorIndex);

static inline void f_color__draw_solid(FColorPixel* Dst, FColorPixel Pixel)
{
    *Dst = Pixel;
}

static inline void f_color__draw_alpha(FColorPixel* Dst, const FColorRgb* Rgb, int Alpha)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3(rgb.r + (((Rgb->r - rgb.r) * Alpha) >> 8),
                                 rgb.g + (((Rgb->g - rgb.g) * Alpha) >> 8),
                                 rgb.b + (((Rgb->b - rgb.b) * Alpha) >> 8));
}

static inline void f_color__draw_alpha25(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3(rgb.r - (rgb.r >> 2) + (Rgb->r >> 2),
                                 rgb.g - (rgb.g >> 2) + (Rgb->g >> 2),
                                 rgb.b - (rgb.b >> 2) + (Rgb->b >> 2));
}

static inline void f_color__draw_alpha50(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3((rgb.r + Rgb->r) >> 1,
                                 (rgb.g + Rgb->g) >> 1,
                                 (rgb.b + Rgb->b) >> 1);
}

static inline void f_color__draw_alpha75(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3((rgb.r >> 2) + Rgb->r - (Rgb->r >> 2),
                                 (rgb.g >> 2) + Rgb->g - (Rgb->g >> 2),
                                 (rgb.b >> 2) + Rgb->b - (Rgb->b >> 2));
}

static inline void f_color__draw_alphaMask(FColorPixel* Dst, const FColorRgb* Rgb, int GlobalAlpha, int PixelAlpha)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);
    int alpha = GlobalAlpha * PixelAlpha;

    *Dst = f_color_pixelFromRgb3(rgb.r + (((Rgb->r - rgb.r) * alpha) >> 16),
                                 rgb.g + (((Rgb->g - rgb.g) * alpha) >> 16),
                                 rgb.b + (((Rgb->b - rgb.b) * alpha) >> 16));
}

static inline void f_color__draw_inverse(FColorPixel* Dst)
{
    *Dst = (FColorPixel)~*Dst;
}

static inline void f_color__draw_mod(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3((rgb.r * Rgb->r) >> 8,
                                 (rgb.g * Rgb->g) >> 8,
                                 (rgb.b * Rgb->b) >> 8);
}

static inline void f_color__draw_add(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb3(f_math_min(rgb.r + Rgb->r, 255),
                                 f_math_min(rgb.g + Rgb->g, 255),
                                 f_math_min(rgb.b + Rgb->b, 255));
}

#endif // F_INC_GRAPHICS_COLOR_V_H
