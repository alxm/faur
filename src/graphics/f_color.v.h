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
    FRgb rgb;
    FPixel pixel;
    int alpha;
    bool fillBlit;
    bool fillDraw;
} FPixelState;

#include "general/f_main.v.h"
#include "math/f_math.v.h"

extern const FPack f_pack__color;

extern FPixelState f__color;

extern FPixel f_color__key;
extern FPixel f_color__limit;

static inline void f_color__draw_plain(FPixel* Dst, FPixel Pixel)
{
    *Dst = Pixel;
}

static inline void f_color__draw_rgba(FPixel* Dst, const FRgb* Rgb, int Alpha)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb(rgb.r + (((Rgb->r - rgb.r) * Alpha) >> 8),
                           rgb.g + (((Rgb->g - rgb.g) * Alpha) >> 8),
                           rgb.b + (((Rgb->b - rgb.b) * Alpha) >> 8));
}

static inline void f_color__draw_rgb25(FPixel* Dst, const FRgb* Rgb)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb(rgb.r - (rgb.r >> 2) + (Rgb->r >> 2),
                           rgb.g - (rgb.g >> 2) + (Rgb->g >> 2),
                           rgb.b - (rgb.b >> 2) + (Rgb->b >> 2));
}

static inline void f_color__draw_rgb50(FPixel* Dst, const FRgb* Rgb)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb((rgb.r + Rgb->r) >> 1,
                           (rgb.g + Rgb->g) >> 1,
                           (rgb.b + Rgb->b) >> 1);
}

static inline void f_color__draw_rgb75(FPixel* Dst, const FRgb* Rgb)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb((rgb.r >> 2) + Rgb->r - (Rgb->r >> 2),
                           (rgb.g >> 2) + Rgb->g - (Rgb->g >> 2),
                           (rgb.b >> 2) + Rgb->b - (Rgb->b >> 2));
}

static inline void f_color__draw_inverse(FPixel* Dst)
{
    *Dst = (FPixel)~*Dst;
}

static inline void f_color__draw_mod(FPixel* Dst, const FRgb* Rgb)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb((rgb.r * Rgb->r) >> 8,
                           (rgb.g * Rgb->g) >> 8,
                           (rgb.b * Rgb->b) >> 8);
}

static inline void f_color__draw_add(FPixel* Dst, const FRgb* Rgb)
{
    FRgb rgb = f_pixel_toRgb(*Dst);

    *Dst = f_pixel_fromRgb(f_math_min(rgb.r + Rgb->r, 255),
                           f_math_min(rgb.g + Rgb->g, 255),
                           f_math_min(rgb.b + Rgb->b, 255));
}

#endif // F_INC_GRAPHICS_COLOR_V_H
