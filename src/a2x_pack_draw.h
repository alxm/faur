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

#ifndef A2X_PACK_DRAW_H
#define A2X_PACK_DRAW_H

#include "a2x_pack_fix.h"
#include "a2x_pack_screen.h"

#define a_draw_putPixel(x, y, p)     (*(a_pixels + (y) * a_width + (x)) = (p))
#define a_draw_putRGB(x, y, r, g, b) a_draw_putPixel(x, y, a_screen_makePixel((r), (g), (b)))
#define a_draw_getPixel(x, y)        (*(a_pixels + (y) * a_width + (x)))

#define _a_draw_pixelAlpha(r, g, b, a)           \
({                                               \
    const int R = a_screen_pixelRed(a_colour);   \
    const int G = a_screen_pixelGreen(a_colour); \
    const int B = a_screen_pixelBlue(a_colour);  \
    *a_pixels2++ = a_screen_makePixel(           \
        R + a_fix8_fixtoi(((r) - R) * (a)),      \
        G + a_fix8_fixtoi(((g) - G) * (a)),      \
        B + a_fix8_fixtoi(((b) - B) * (a))       \
    );                                           \
})

#define _a_draw_pixel25(r, g, b)                                                             \
({                                                                                           \
    *a_pixels2++ = a_screen_makePixel(                                                       \
        (a_screen_pixelRed(a_colour)   >> 1) + ((a_screen_pixelRed(a_colour)   + (r)) >> 2), \
        (a_screen_pixelGreen(a_colour) >> 1) + ((a_screen_pixelGreen(a_colour) + (g)) >> 2), \
        (a_screen_pixelBlue(a_colour)  >> 1) + ((a_screen_pixelBlue(a_colour)  + (b)) >> 2)  \
    );                                                                                       \
})

#define _a_draw_pixel50(r, g, b)                      \
({                                                    \
    *a_pixels2++ = a_screen_makePixel(                \
        ((a_screen_pixelRed(a_colour)   + (r)) >> 1), \
        ((a_screen_pixelGreen(a_colour) + (g)) >> 1), \
        ((a_screen_pixelBlue(a_colour)  + (b)) >> 1)  \
    );                                                \
})

#define _a_draw_pixel75(r, g, b)                                        \
({                                                                      \
    *a_pixels2++ = a_screen_makePixel(                                  \
        (a_screen_pixelRed(a_colour)   >> 2) + ((r) >> 2) + ((r) >> 1), \
        (a_screen_pixelGreen(a_colour) >> 2) + ((g) >> 2) + ((g) >> 1), \
        (a_screen_pixelBlue(a_colour)  >> 2) + ((b) >> 2) + ((b) >> 1)  \
    );                                                                  \
})

#define a_draw_putPixelAlpha(x, y, r, g, b, a)         \
({                                                     \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x); \
    const Pixel a_colour = *a_pixels2;                 \
    _a_draw_pixelAlpha((r), (g), (b), (a));            \
})

#define a_draw_putPixel25(x, y, r, g, b)               \
({                                                     \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x); \
    const Pixel a_colour = *a_pixels2;                 \
    _a_draw_pixel25((r), (g), (b));                    \
})

#define a_draw_putPixel50(x, y, r, g, b)               \
({                                                     \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x); \
    const Pixel a_colour = *a_pixels2;                 \
    _a_draw_pixel50((r), (g), (b));                    \
})

#define a_draw_putPixel75(x, y, r, g, b)               \
({                                                     \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x); \
    const Pixel a_colour = *a_pixels2;                 \
    _a_draw_pixel75((r), (g), (b));                    \
})

extern void a_draw_rectangle(const int x1, const int y1, const int x2, const int y2, const Pixel c);
#define a_draw_rectangleRGB(x1, y1, x2, y2, r, g, b) a_draw_rectangle((x1), (y1), (x2), (y2), a_screen_makePixel((r), (g), (b)))

extern void a_draw_rectangleAlpha(const int x1, const int y1, const int x2, const int y2, const int r, const int g, const int b, const fix8 a);
extern void a_draw_rectangle25(const int x1, const int y1, const int x2, const int y2, const int r, const int g, const int b);
extern void a_draw_rectangle50(const int x1, const int y1, const int x2, const int y2, const int r, const int g, const int b);
extern void a_draw_rectangle75(const int x1, const int y1, const int x2, const int y2, const int r, const int g, const int b);

extern void a_draw_rectangleSafe(int x1, int y1, int x2, int y2, const Pixel c);
#define a_draw_rectangleSafeRGB(x1, y1, x2, y2, r, g, b) a_draw_rectangleSafe((x1), (y1), (x2), (y2), a_screen_makePixel((r), (g), (b)))

extern void a_draw_borderRectangle(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c);
#define a_draw_borderRectangleRGB(x1, y1, x2, y2, t, r, g, b) a_draw_borderRectangle((x1), (y1), (x2), (y2), (t), a_screen_makePixel((r), (g), (b)))

extern void a_draw_borderRectangleSafe(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c);
#define a_draw_borderRectangleSafeRGB(x1, y1, x2, y2, t, r, g, b) a_draw_borderRectangleSafe((x1), (y1), (x2), (y2), (t), a_screen_makePixel((r), (g), (b)))

extern void a_draw_fill(const Pixel c);
#define a_draw_fillRGB(r, g, b) a_draw_fill(a_screen_makePixel((r), (g), (b)))

#define a_draw_fillBlack() a_memset(a_pixels, 0, SCREEN_SIZE)
#define a_draw_fillWhite() a_memset(a_pixels, 0xff, SCREEN_SIZE)

#define a_draw_fillAlpha(r, g, b, a) a_draw_rectangleAlpha(0, 0, a_width - 1, a_height - 1, (r), (g), (b), (a))
#define a_draw_fill25(r, g, b)       a_draw_rectangle25(0, 0, a_width - 1, a_height - 1, (r), (g), (b))
#define a_draw_fill50(r, g, b)       a_draw_rectangle50(0, 0, a_width - 1, a_height - 1, (r), (g), (b))
#define a_draw_fill75(r, g, b)       a_draw_rectangle75(0, 0, a_width - 1, a_height - 1, (r), (g), (b))

extern void a_draw_line(int x1, int y1, int x2, int y2, const Pixel p);
#define a_draw_lineRGB(x1, y1, x2, y2, r, g, b) a_draw_line((x1), (y1), (x2), (y2), a_screen_makePixel((r), (g), (b)))

#define _a_draw_hline2(x1, x2)                \
({                                            \
    for(int a_i = (x2) - (x1) + 1; a_i--; ) { \
        *a_pixels2++ = a_colour;              \
    }                                         \
})

#define a_draw_hline(x1, x2, y, c)                      \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x1); \
    const Pixel a_colour = (c);                         \
    _a_draw_hline2((x1), (x2));                         \
})

#define a_draw_hlineRGB(x1, x2, y, r, g, b)                          \
    a_draw_hline((x1), (x2), (y), a_screen_makePixel((r), (g), (b)))

#define a_draw_hlineAlpha(x1, x2, y, r, g, b, a)        \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x1); \
    for(int a_i = (x2) - (x1) + 1; a_i--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixelAlpha((r), (g), (b), (a));         \
    }                                                   \
})

#define a_draw_hline25(x1, x2, y, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x1); \
    for(int a_i = (x2) - (x1) + 1; a_i--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel25((r), (g), (b));                 \
    }                                                   \
})

#define a_draw_hline50(x1, x2, y, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x1); \
    for(int a_i = (x2) - (x1) + 1; a_i--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel50((r), (g), (b));                 \
    }                                                   \
})

#define a_draw_hline75(x1, x2, y, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y) * a_width + (x1); \
    for(int a_i = (x2) - (x1) + 1; a_i--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel75((r), (g), (b));                 \
    }                                                   \
})

#define a_draw_vline(x, y1, y2, c)                      \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y1) * a_width + (x); \
    const Pixel a_colour = (c);                         \
    for(int a_j = (y2) - (y1) + 1; a_j--; ) {           \
        *a_pixels2 = a_colour;                          \
        a_pixels2 += a_width;                           \
    }                                                   \
})

#define a_draw_vlineRGB(x, y1, y2, r, g, b)                          \
    a_draw_vline((x), (y1), (y2), a_screen_makePixel((r), (g), (b)))

#define a_draw_vlineAlpha(x, y1, y2, r, g, b, a)        \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y1) * a_width + (x); \
    for(int a_j = (y2) - (y1) + 1; a_j--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixelAlpha((r), (g), (b), (a));         \
        a_pixels2 += a_width - 1;                       \
    }                                                   \
})

#define a_draw_vline25(x, y1, y2, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y1) * a_width + (x); \
    for(int a_j = (y2) - (y1) + 1; a_j--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel25((r), (g), (b));                 \
        a_pixels2 += a_width - 1;                       \
    }                                                   \
})

#define a_draw_vline50(x, y1, y2, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y1) * a_width + (x); \
    for(int a_j = (y2) - (y1) + 1; a_j--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel50((r), (g), (b));                 \
        a_pixels2 += a_width - 1;                       \
    }                                                   \
})

#define a_draw_vline75(x, y1, y2, r, g, b)              \
({                                                      \
    Pixel* a_pixels2 = a_pixels + (y1) * a_width + (x); \
    for(int a_j = (y2) - (y1) + 1; a_j--; ) {           \
        const Pixel a_colour = *a_pixels2;              \
        _a_draw_pixel75((r), (g), (b));                 \
        a_pixels2 += a_width - 1;                       \
    }                                                   \
})

extern void a_draw_roughCircle(const int x, const int y, const int r, const Pixel p);
#define a_draw_roughCircleRGB(x, y, rad, r, g, b) a_draw_roughCircle((x), (y), (rad), a_screen_makePixel((r), (g), (b)))

#endif // A2X_PACK_DRAW_H
