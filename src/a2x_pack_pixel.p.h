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

#include <stdint.h>

#define A_BPP 16
typedef uint16_t Pixel;

#include "a2x_pack_screen.p.h"

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
#define a_pixel_put(x, y, p) (*(a_pixels + (y) * a_width + (x)) = (p))

#define a_pixel_putRGB(x, y, r, g, b) a_pixel_put(x, y, a_pixel_make((r), (g), (b)))

extern void a_pixel_putAlpha(const int x, const int y, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_pixel_put25(const int x, const int y, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_pixel_put50(const int x, const int y, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_pixel_put75(const int x, const int y, const uint8_t r, const uint8_t g, const uint8_t b);

#endif // A2X_PACK_PIXEL_PH
