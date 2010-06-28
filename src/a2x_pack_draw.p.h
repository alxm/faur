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

#ifndef A2X_PACK_DRAW_PH
#define A2X_PACK_DRAW_PH

#include "a2x_pack_fix.p.h"
#include "a2x_pack_pixel.p.h"
#include "a2x_pack_screen.p.h"

extern void a_draw_rectangle(const int x1, const int y1, const int x2, const int y2, const Pixel c);
#define a_draw_rectangleRGB(x1, y1, x2, y2, r, g, b) a_draw_rectangle((x1), (y1), (x2), (y2), a_pixel_make((r), (g), (b)))

extern void a_draw_rectangleAlpha(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_draw_rectangle25(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_rectangle50(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_rectangle75(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);

extern void a_draw_rectangleSafe(int x1, int y1, int x2, int y2, const Pixel c);
#define a_draw_rectangleSafeRGB(x1, y1, x2, y2, r, g, b) a_draw_rectangleSafe((x1), (y1), (x2), (y2), a_pixel_make((r), (g), (b)))

extern void a_draw_borderRectangle(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c);
#define a_draw_borderRectangleRGB(x1, y1, x2, y2, t, r, g, b) a_draw_borderRectangle((x1), (y1), (x2), (y2), (t), a_pixel_make((r), (g), (b)))

extern void a_draw_borderRectangleSafe(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c);
#define a_draw_borderRectangleSafeRGB(x1, y1, x2, y2, t, r, g, b) a_draw_borderRectangleSafe((x1), (y1), (x2), (y2), (t), a_pixel_make((r), (g), (b)))

extern void a_draw_fill(const Pixel c);
#define a_draw_fillRGB(r, g, b) a_draw_fill(a_pixel_make((r), (g), (b)))

#define a_draw_fillAlpha(r, g, b, a) a_draw_rectangleAlpha(0, 0, a_width - 1, a_height - 1, (r), (g), (b), (a))
#define a_draw_fill25(r, g, b)       a_draw_rectangle25(0, 0, a_width - 1, a_height - 1, (r), (g), (b))
#define a_draw_fill50(r, g, b)       a_draw_rectangle50(0, 0, a_width - 1, a_height - 1, (r), (g), (b))
#define a_draw_fill75(r, g, b)       a_draw_rectangle75(0, 0, a_width - 1, a_height - 1, (r), (g), (b))

extern void a_draw_line(int x1, int y1, int x2, int y2, const Pixel p);
#define a_draw_lineRGB(x1, y1, x2, y2, r, g, b) a_draw_line((x1), (y1), (x2), (y2), a_pixel_make((r), (g), (b)))

extern void a_draw_roughCircle(const int x, const int y, const int r, const Pixel p);
#define a_draw_roughCircleRGB(x, y, rad, r, g, b) a_draw_roughCircle((x), (y), (rad), a_pixel_make((r), (g), (b)))

extern void a_draw_hline(const int x1, const int x2, const int y, const Pixel c);
#define a_draw_hlineRGB(x1, x2, y, r, g, b) a_draw_hline((x1), (x2), (y), a_pixel_make((r), (g), (b)))

extern void a_draw_hlineAlpha(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_draw_hline25(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_hline50(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_hline75(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b);

extern void a_draw_vline(const int x, const int y1, const int y2, const Pixel c);
#define a_draw_vlineRGB(x, y1, y2, r, g, b) a_draw_vline((x), (y1), (y2), a_pixel_make((r), (g), (b)))

extern void a_draw_vlineAlpha(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_draw_vline25(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_vline50(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);
extern void a_draw_vline75(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b);

#endif // A2X_PACK_DRAW_PH
