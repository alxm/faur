/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_PIXEL_PH
#define A2X_PACK_PIXEL_PH

#include "a2x_app_includes.h"

#define A_BPP 16
typedef uint16_t Pixel;

typedef enum {
    A_PIXEL_PLAIN,
    A_PIXEL_RGBA, A_PIXEL_RGB25, A_PIXEL_RGB50, A_PIXEL_RGB75,
    A_PIXEL_INVERSE,
    A_PIXEL_TYPE_NUM
} PixelBlend_t;

typedef void (*PixelPut)(const int x, const int y);

#define a_mask(x) ((1 << (x)) - 1)

// RGB565
#define a_pixel_make(r, g, b) \
(                             \
    (                         \
        (((r) >> 3) << 11)    \
      | (((g) >> 2) << 5 )    \
      | (((b) >> 3) << 0 )    \
    ) & a_mask(16)            \
)

#define a_pixel_red(p)   ((((p) >> 11) & a_mask(5)) << 3)
#define a_pixel_green(p) ((((p) >> 5 ) & a_mask(6)) << 2)
#define a_pixel_blue(p)  ((((p) >> 0 ) & a_mask(5)) << 3)

#define a_pixel_get(x, y)    (*(a_pixels + (y) * a_width + (x)))

extern void a_pixel_setBlend(const PixelBlend_t b);
extern void a_pixel_setClip(const bool clip);

extern void a_pixel_setAlpha(const uint8_t a);
extern void a_pixel_setRGB(const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_pixel_setPixel(const Pixel p);

extern PixelPut a_pixel_put;

#endif // A2X_PACK_PIXEL_PH
