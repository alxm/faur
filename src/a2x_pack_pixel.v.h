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

#include "a2x_pack_pixel.p.h"

#include "a2x_pack_draw.v.h"
#include "a2x_pack_screen.v.h"

typedef struct APixelState {
    APixelBlend blend, canonicalBlend;
    int red, green, blue, alpha;
    APixel pixel;
    bool blitFillFlat;
} APixelState;

extern APixelState a_pixel__state;

static inline void a_pixel__plain(APixel* Dst, APixel Pixel)
{
    *Dst = Pixel;
}

static inline void a_pixel__rgba(APixel* Dst, int Red, int Green, int Blue, int Alpha)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb(r + (((Red   - r) * Alpha) >> 8),
                       g + (((Green - g) * Alpha) >> 8),
                       b + (((Blue  - b) * Alpha) >> 8));
}

static inline void a_pixel__rgb25(APixel* Dst, int Red, int Green, int Blue)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb(r - (r >> 2) + (Red   >> 2),
                       g - (g >> 2) + (Green >> 2),
                       b - (b >> 2) + (Blue  >> 2));
}

static inline void a_pixel__rgb50(APixel* Dst, int Red, int Green, int Blue)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb((r + Red)   >> 1,
                       (g + Green) >> 1,
                       (b + Blue)  >> 1);
}

static inline void a_pixel__rgb75(APixel* Dst, int Red, int Green, int Blue)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb((r >> 2) + Red   - (Red   >> 2),
                       (g >> 2) + Green - (Green >> 2),
                       (b >> 2) + Blue  - (Blue  >> 2));
}

static inline void a_pixel__inverse(APixel* Dst)
{
    *Dst = (APixel)~*Dst;
}

static inline void a_pixel__mod(APixel* Dst, int Red, int Green, int Blue)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb((r * Red) >> 8, (g * Green) >> 8, (b * Blue) >> 8);
}

static inline void a_pixel__add(APixel* Dst, int Red, int Green, int Blue)
{
    const APixel p = *Dst;

    const int r = a_pixel_red(p);
    const int g = a_pixel_green(p);
    const int b = a_pixel_blue(p);

    *Dst = a_pixel_rgb(a_math_min(r + Red, 255),
                       a_math_min(g + Green, 255),
                       a_math_min(b + Blue, 255));
}

extern void a_pixel__init(void);
extern void a_pixel__uninit(void);
