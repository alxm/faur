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

#include "a2x_pack_draw.p.h"
#include "a2x_pack_draw.v.h"

static int lineClipper(int* error, int* x1, int* y1, int* x2, int* y2);

void a_draw_line(int x1, int y1, int x2, int y2, const Pixel p)
{
    int error = 0;
    while(lineClipper(&error, &x1, &y1, &x2, &y2)) continue;
    if(error) return;

    int xmin, xmax, ymin, ymax;
    if(x1 < x2) {
        xmin = x1;
        xmax = x2;
    } else {
        xmin = x2;
        xmax = x1;
    }
    if(y1 < y2) {
        ymin = y1;
        ymax = y2;
    } else {
        ymin = y2;
        ymax = y1;
    }

    if(x1 == x2) {
        a_draw_vline(x1, ymin, ymax, p);
    } else if(y1 == y2) {
        a_draw_hline(xmin, xmax, y1, p);
    } else {
        const int deltax = xmax - xmin;
        const int deltay = ymax - ymin;

        const int denominator = a_math_max(deltax, deltay);
        int numerator = denominator >> 1;
        const int numeratorinc = a_math_min(deltax, deltay);

        const int xinct = (x1 <= x2) ? 1 : -1;
        const int yinct = (y1 <= y2) ? 1 : -1;

        const int xinc1 = (denominator == deltax) ? xinct : 0;
        const int yinc1 = (denominator == deltax) ? 0 : yinct;

        const int xinc2 = (denominator == deltax) ? 0 : xinct;
        const int yinc2 = (denominator == deltax) ? yinct : 0;

        for(int i = denominator + 1; i--; x1 += xinc1, y1 += yinc1) {
            a_pixel_put(x1, y1, p);
            numerator += numeratorinc;
            if(numerator >= denominator) {
                numerator -= denominator;
                x1 += xinc2;
                y1 += yinc2;
            }
        }
    }
}

#define CIRCLE_APPROX 2

void a_draw_roughCircle(const int x, const int y, const int r, const Pixel p)
{
    for(int i = 0; i < A_ANGLES_NUM / 8; i += A_ANGLES_NUM / (8 * CIRCLE_APPROX)) {
        const int cos_i = a_fix8_fixtoi(r * a_fix8_cos(i));
        const int sin_i = a_fix8_fixtoi(r * a_fix8_sin(i));
        const int cos_n = a_fix8_fixtoi(r * a_fix8_cos(i + A_ANGLES_NUM / (8 * CIRCLE_APPROX)));
        const int sin_n = a_fix8_fixtoi(r * a_fix8_sin(i + A_ANGLES_NUM / (8 * CIRCLE_APPROX)));

        a_draw_line(x + cos_i, y + sin_i, x + cos_n, y + sin_n, p);
        a_draw_line(x + sin_i, y + cos_i, x + sin_n, y + cos_n, p);

        a_draw_line(x - cos_i, y + sin_i, x - cos_n, y + sin_n, p);
        a_draw_line(x - sin_i, y + cos_i, x - sin_n, y + cos_n, p);

        a_draw_line(x - cos_i, y - sin_i, x - cos_n, y - sin_n, p);
        a_draw_line(x - sin_i, y - cos_i, x - sin_n, y - cos_n, p);

        a_draw_line(x + cos_i, y - sin_i, x + cos_n, y - sin_n, p);
        a_draw_line(x + sin_i, y - cos_i, x + sin_n, y - cos_n, p);
    }
}

void a_draw_fill(const Pixel c)
{
    Uint32* a_pixels32 = (Uint32*)a_pixels;
    const Uint32 col = (c << 16) | c;

    for(int i = a_width * a_height >> 2; i--; ) {
        *a_pixels32++ = col;
        *a_pixels32++ = col;
    }
}

void a_draw_rectangle(const int x1, const int y1, const int x2, const int y2, const Pixel c)
{
    Pixel* a_pixels2 = a_pixels + y1 * a_width + x1;
    const int w = x2 - x1 + 1;
    const int o1 = x1 & 1;
    const int o2 = 1 - (x2 & 1);
    const int W = (w - o1 - o2) >> 1;
    const Uint32 C = (c << 16) | c;

    for(int i = y2 - y1 + 1; i--; a_pixels2 += a_width) {
        if(o1) {
            *a_pixels2 = c;
        }
        if(o2) {
            *(a_pixels2 + w - 1) = c;
        }

        Uint32* a_pixels32 = (Uint32*)(a_pixels2 + o1);
        for(int j = W; j--; ) {
            *a_pixels32++ = C;
        }
    }
}

void a_draw_rectangleAlpha(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    Pixel* dst = a_pixels + y1 * a_width + x1;
    const int offset = a_width - (x2 - x1 + 1);

    for(int i = y2 - y1 + 1; i--; ) {
        for(int j = x2 - x1 + 1; j--; ) {
            a_pixel__putAlpha(dst, r, g, b, a);
            dst++;
        }

        dst += offset;
    }
}

void a_draw_rectangle25(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y1 * a_width + x1;
    const int offset = a_width - (x2 - x1 + 1);

    for(int i = y2 - y1 + 1; i--; ) {
        for(int j = x2 - x1 + 1; j--; ) {
            a_pixel__put25(dst, r, g, b);
            dst++;
        }

        dst += offset;
    }
}

void a_draw_rectangle50(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y1 * a_width + x1;
    const int offset = a_width - (x2 - x1 + 1);

    for(int i = y2 - y1 + 1; i--; ) {
        for(int j = x2 - x1 + 1; j--; ) {
            a_pixel__put50(dst, r, g, b);
            dst++;
        }

        dst += offset;
    }
}

void a_draw_rectangle75(const int x1, const int y1, const int x2, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y1 * a_width + x1;
    const int offset = a_width - (x2 - x1 + 1);

    for(int i = y2 - y1 + 1; i--; ) {
        for(int j = x2 - x1 + 1; j--; ) {
            a_pixel__put75(dst, r, g, b);
            dst++;
        }

        dst += offset;
    }
}

void a_draw_rectangleSafe(int x1, int y1, int x2, int y2, const Pixel c)
{
    if(y1 < 0) y1 = 0;
    if(y2 < 0) y2 = 0;
    if(y1 > a_height - 1) y1 = a_height - 1;
    if(y2 > a_height - 1) y2 = a_height - 1;

    if(x1 < 0) x1 = 0;
    if(x2 < 0) x2 = 0;
    if(x1 > a_width - 1) x1 = a_width - 1;
    if(x2 > a_width - 1) x2 = a_width - 1;

    a_draw_rectangle(x1, y1, x2, y2, c);
}

void a_draw_borderRectangle(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c)
{
    a_draw_rectangle(x1, y1, x2, y1 + t, c);         // top
    a_draw_rectangle(x1, y2 - t, x2, y2, c);         // bottom
    a_draw_rectangle(x1, y1 + t, x1 + t, y2 - t, c); // left
    a_draw_rectangle(x2 - t, y1 + t, x2, y2 - t, c); // right
}

void a_draw_borderRectangleSafe(const int x1, const int y1, const int x2, const int y2, const int t, const Pixel c)
{
    a_draw_rectangleSafe(x1, y1, x2, y1 + t, c);         // top
    a_draw_rectangleSafe(x1, y2 - t, x2, y2, c);         // bottom
    a_draw_rectangleSafe(x1, y1 + t, x1 + t, y2 - t, c); // left
    a_draw_rectangleSafe(x2 - t, y1 + t, x2, y2 - t, c); // right
}

static int lineClipper(int* error, int* x1, int* y1, int* x2, int* y2)
{
    const int xx1 = *x1;
    const int yy1 = *y1;
    const int xx2 = *x2;
    const int yy2 = *y2;

    if(    xx1 >= 0 && xx1 < a_width
        && xx2 >= 0 && xx2 < a_width
        && yy1 >= 0 && yy1 < a_height
        && yy2 >= 0 && yy2 < a_height) return 0;

    int xmin, xmax, ymin, ymax;

    if(xx1 < xx2) {
        xmin = xx1;
        xmax = xx2;
    } else {
        xmin = xx2;
        xmax = xx1;
    }
    if(yy1 < yy2) {
        ymin = yy1;
        ymax = yy2;
    } else {
        ymin = yy2;
        ymax = yy1;
    }

    if(xmin >= a_width || xmax < 0 || ymin >= a_height || ymax < 0) {
        *error = 1;
        return 0;
    }

    if(xx1 == xx2) {
        if(ymin < 0) {
            if(ymin == yy1) *y1 = 0;
            if(ymin == yy2) *y2 = 0;
        }

        if(ymax >= a_height) {
            if(ymax == yy1) *y1 = a_height - 1;
            if(ymax == yy2) *y2 = a_height - 1;
        }

        return 0;
    }

    if(yy1 == yy2) {
        if(xmin < 0) {
            if(xmin == xx1) *x1 = 0;
            if(xmin == xx2) *x2 = 0;
        }

        if(xmax >= a_width) {
            if(xmax == xx1) *x1 = a_width - 1;
            if(xmax == xx2) *x2 = a_width - 1;
        }

        return 0;
    }

    const fix8 m = a_fix8_div(a_fix8_itofix(yy1 - yy2), a_fix8_itofix(xx1 - xx2));

    if(xmin < 0) {
        if(xmin == xx1) {
            *y1 = a_fix8_fixtoi(m * (0 - xx1)) + yy1;
            *x1 = 0;
        } else {
            *y2 = a_fix8_fixtoi(m * (0 - xx2)) + yy2;
            *x2 = 0;
        }

        return 1;
    } else if(xmax >= a_width) {
        if(xmax == xx1) {
            *y1 = a_fix8_fixtoi(m * (a_width - 1 - xx1)) + yy1;
            *x1 = a_width - 1;
        } else {
            *y2 = a_fix8_fixtoi(m * (a_width - 1 - xx2)) + yy2;
            *x2 = a_width - 1;
        }

        return 1;
    } else if(ymin < 0) {
        if(ymin == yy1) {
            *x1 = a_fix8_fixtoi(a_fix8_div(a_fix8_itofix(0 - yy1), m)) + xx1;
            *y1 = 0;
        } else {
            *x2 = a_fix8_fixtoi(a_fix8_div(a_fix8_itofix(0 - yy2), m)) + xx2;
            *y2 = 0;
        }

        return 1;
    } else if(ymax >= a_height) {
        if(ymax == yy1) {
            *x1 = a_fix8_fixtoi(a_fix8_div(a_fix8_itofix(a_height - 1 - yy1), m)) + xx1;
            *y1 = a_height - 1;
        } else {
            *x2 = a_fix8_fixtoi(a_fix8_div(a_fix8_itofix(a_height - 1 - yy2), m)) + xx2;
            *y2 = a_height - 1;
        }

        return 1;
    }

    return 0;
}

void a_draw_hline(const int x1, const int x2, const int y, const Pixel c)
{
    Pixel* p = a_pixels + y * a_width + x1;

    for(int i = x2 - x1 + 1; i--; ) {
        *p++ = c;
    }
}

void a_draw_hlineAlpha(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    Pixel* dst = a_pixels + y * a_width + x1;

    for(int i = x2 - x1 + 1; i--; ) {
        a_pixel__putAlpha(dst, r, g, b, a);
        dst++;
    }
}

void a_draw_hline25(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y * a_width + x1;

    for(int i = x2 - x1 + 1; i--; ) {
        a_pixel__put25(dst, r, g, b);
        dst++;
    }
}

void a_draw_hline50(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y * a_width + x1;

    for(int i = x2 - x1 + 1; i--; ) {
        a_pixel__put50(dst, r, g, b);
        dst++;
    }
}

void a_draw_hline75(const int x1, const int x2, const int y, const uint8_t r, const uint8_t g, const uint8_t b)
{
    Pixel* dst = a_pixels + y * a_width + x1;

    for(int i = x2 - x1 + 1; i--; ) {
        a_pixel__put75(dst, r, g, b);
        dst++;
    }
}

void a_draw_vline(const int x, const int y1, const int y2, const Pixel c)
{
    const int width = a_width;
    Pixel* p = a_pixels + y1 * width + x;

    for(int i = y2 - y1 + 1; i--; ) {
        *p = c;
        p += width;
    }
}

void a_draw_vlineAlpha(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    const int width = a_width;
    Pixel* dst = a_pixels + y1 * width + x;

    for(int i = y2 - y1 + 1; i--; ) {
        a_pixel__putAlpha(dst, r, g, b, a);
        dst += width;
    }
}

void a_draw_vline25(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    const int width = a_width;
    Pixel* dst = a_pixels + y1 * width + x;

    for(int i = y2 - y1 + 1; i--; ) {
        a_pixel__put25(dst, r, g, b);
        dst += width;
    }
}

void a_draw_vline50(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    const int width = a_width;
    Pixel* dst = a_pixels + y1 * width + x;

    for(int i = y2 - y1 + 1; i--; ) {
        a_pixel__put50(dst, r, g, b);
        dst += width;
    }
}

void a_draw_vline75(const int x, const int y1, const int y2, const uint8_t r, const uint8_t g, const uint8_t b)
{
    const int width = a_width;
    Pixel* dst = a_pixels + y1 * width + x;

    for(int i = y2 - y1 + 1; i--; ) {
        a_pixel__put75(dst, r, g, b);
        dst += width;
    }
}
