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

#ifndef A2X_PACK_PIXEL_VH
#define A2X_PACK_PIXEL_VH

#include "a2x_pack_blit.v.h"
#include "a2x_pack_draw.v.h"

#define a_pixel__plain(dst, p) \
({                             \
    *dst = p;                  \
})

#define a_pixel__rgba(dst, r, g, b, a)  \
({                                      \
    const Pixel p = *dst;               \
                                        \
    const uint8_t R = a_pixel_red(p);   \
    const uint8_t G = a_pixel_green(p); \
    const uint8_t B = a_pixel_blue(p);  \
                                        \
    *dst = a_pixel_make(                \
        R + ((((r) - R) * (a)) >> 8),   \
        G + ((((g) - G) * (a)) >> 8),   \
        B + ((((b) - B) * (a)) >> 8)    \
    );                                  \
})

#define a_pixel__rgb25(dst, r, g, b)    \
({                                      \
    const Pixel p = *dst;               \
                                        \
    const uint8_t R = a_pixel_red(p);   \
    const uint8_t G = a_pixel_green(p); \
    const uint8_t B = a_pixel_blue(p);  \
                                        \
    *dst = a_pixel_make(                \
        (R >> 1) + ((R + (r)) >> 2),    \
        (G >> 1) + ((G + (g)) >> 2),    \
        (B >> 1) + ((B + (b)) >> 2)     \
    );                                  \
})

#define a_pixel__rgb50(dst, r, g, b)    \
({                                      \
    const Pixel p = *dst;               \
                                        \
    const uint8_t R = a_pixel_red(p);   \
    const uint8_t G = a_pixel_green(p); \
    const uint8_t B = a_pixel_blue(p);  \
                                        \
    *dst = a_pixel_make(                \
        (R + (r)) >> 1,                 \
        (G + (g)) >> 1,                 \
        (B + (b)) >> 1                  \
    );                                  \
})

#define a_pixel__rgb75(dst, r, g, b)        \
({                                          \
    const Pixel p = *dst;                   \
                                            \
    const uint8_t R = a_pixel_red(p);       \
    const uint8_t G = a_pixel_green(p);     \
    const uint8_t B = a_pixel_blue(p);      \
                                            \
    *dst = a_pixel_make(                    \
        (R >> 2) + ((r) >> 2) + ((r) >> 1), \
        (G >> 2) + ((g) >> 2) + ((g) >> 1), \
        (B >> 2) + ((b) >> 2) + ((b) >> 1)  \
    );                                      \
})

#define a_pixel__inverse(dst) \
({                            \
    *dst = ~*dst;             \
})

extern void a_pixel__set(void);

#endif // A2X_PACK_PIXEL_VH
