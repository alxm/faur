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

#include "graphics/f_color.p.h"

typedef struct {
    FColorBlend blend, canonicalBlend;
    FColorRgb rgb;
    FColorPixel pixel;
    int alpha;
    bool fillBlit;
    bool fillDraw;
} FPixelState;

#include "general/f_main.v.h"
#include "math/f_math.v.h"

extern const FPack f_pack__color;

extern FPixelState f__color;

extern FColorPixel f_color__key;
extern FColorPixel f_color__limit;

static inline void f_color__draw_plain(FColorPixel* Dst, FColorPixel Pixel)
{
    *Dst = Pixel;
}

static inline void f_color__draw_rgba(FColorPixel* Dst, const FColorRgb* Rgb, int Alpha)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb(rgb.r + (((Rgb->r - rgb.r) * Alpha) >> 8),
                                rgb.g + (((Rgb->g - rgb.g) * Alpha) >> 8),
                                rgb.b + (((Rgb->b - rgb.b) * Alpha) >> 8));
}

static inline void f_color__draw_rgb25(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb(rgb.r - (rgb.r >> 2) + (Rgb->r >> 2),
                                rgb.g - (rgb.g >> 2) + (Rgb->g >> 2),
                                rgb.b - (rgb.b >> 2) + (Rgb->b >> 2));
}

static inline void f_color__draw_rgb50(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb((rgb.r + Rgb->r) >> 1,
                                (rgb.g + Rgb->g) >> 1,
                                (rgb.b + Rgb->b) >> 1);
}

static inline void f_color__draw_rgb75(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb((rgb.r >> 2) + Rgb->r - (Rgb->r >> 2),
                                (rgb.g >> 2) + Rgb->g - (Rgb->g >> 2),
                                (rgb.b >> 2) + Rgb->b - (Rgb->b >> 2));
}

static inline void f_color__draw_inverse(FColorPixel* Dst)
{
    *Dst = (FColorPixel)~*Dst;
}

static inline void f_color__draw_mod(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb((rgb.r * Rgb->r) >> 8,
                                (rgb.g * Rgb->g) >> 8,
                                (rgb.b * Rgb->b) >> 8);
}

static inline void f_color__draw_add(FColorPixel* Dst, const FColorRgb* Rgb)
{
    FColorRgb rgb = f_color_pixelToRgb(*Dst);

    *Dst = f_color_pixelFromRgb(f_math_min(rgb.r + Rgb->r, 255),
                                f_math_min(rgb.g + Rgb->g, 255),
                                f_math_min(rgb.b + Rgb->b, 255));
}

#endif // F_INC_GRAPHICS_COLOR_V_H
