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

#include "a2x_pack_draw.v.h"

DrawRectangle a_draw_rectangle;
static DrawRectangle rectangles[A_PIXEL_TYPE_NUM][2];

DrawLine a_draw_line;
static DrawLine lines[A_PIXEL_TYPE_NUM][2];

DrawHLine a_draw_hline;
static DrawHLine hlines[A_PIXEL_TYPE_NUM][2];

DrawVLine a_draw_vline;
static DrawHLine vlines[A_PIXEL_TYPE_NUM][2];

DrawCircle a_draw_circle;
//static DrawCircle circles[A_PIXEL_TYPE_NUM][2];

static PixelBlend_t blend;
static bool clip;

static uint8_t alpha;
static uint8_t red, green, blue;
static Pixel pixel;

static bool cohen_sutherland_clip(int* const lx1, int* const ly1, int* const lx2, int* const ly2);

#define rectangle_noclip(pixeler)                  \
{                                                  \
    Pixel* pixels = a_pixels + y1 * a_width + x1;  \
                                                   \
    const int screenw = a_width;                   \
    const int w = x2 - x1;                         \
                                                   \
    for(int i = y2 - y1; i--; pixels += screenw) { \
        Pixel* dst = pixels;                       \
                                                   \
        for(int j = w; j--; dst++) {               \
            pixeler;                               \
        }                                          \
    }                                              \
}

#define rectangle_clip(pixeler)    \
{                                  \
    x1 = a_math_max(x1, 0);        \
    y1 = a_math_max(y1, 0);        \
    x2 = a_math_min(x2, a_width);  \
    y2 = a_math_min(y2, a_height); \
                                   \
    if(x1 >= x2 || y1 >= y2) {     \
        return;                    \
    }                              \
                                   \
    rectangle_noclip(pixeler);     \
}

#define line_noclip(pixeler)                                   \
{                                                              \
    const int xmin = a_math_min(x1, x2);                       \
    const int xmax = a_math_max(x1, x2);                       \
    const int ymin = a_math_min(y1, y2);                       \
    const int ymax = a_math_max(y1, y2);                       \
                                                               \
    if(x1 == x2) {                                             \
        a_draw_vline(x1, ymin, ymax);                          \
    } else if(y1 == y2) {                                      \
        a_draw_hline(xmin, xmax, y1);                          \
    } else {                                                   \
        const int deltax = xmax - xmin;                        \
        const int deltay = ymax - ymin;                        \
                                                               \
        const int denominator = a_math_max(deltax, deltay);    \
        const int numeratorinc = a_math_min(deltax, deltay);   \
        int numerator = denominator / 2;                       \
                                                               \
        const int xinct = (x1 <= x2) ? 1 : -1;                 \
        const int yinct = (y1 <= y2) ? 1 : -1;                 \
                                                               \
        const int xinc1 = (denominator == deltax) ? xinct : 0; \
        const int yinc1 = (denominator == deltax) ? 0 : yinct; \
                                                               \
        const int xinc2 = (denominator == deltax) ? 0 : xinct; \
        const int yinc2 = (denominator == deltax) ? yinct : 0; \
                                                               \
        const int screenw = a_width;                           \
        Pixel* dst = a_pixels + y1 * screenw + x1;             \
                                                               \
        for(int i = denominator + 1; i--; ) {                  \
            pixeler;                                           \
                                                               \
            numerator += numeratorinc;                         \
                                                               \
            if(numerator >= denominator) {                     \
                numerator -= denominator;                      \
                dst += xinc2;                                  \
                dst += yinc2 * screenw;                        \
            }                                                  \
                                                               \
            dst += xinc1;                                      \
            dst += yinc1 * screenw;                            \
        }                                                      \
    }                                                          \
}

#define line_clip(pixeler)                           \
{                                                    \
    if(!cohen_sutherland_clip(&x1, &y1, &x2, &y2)) { \
        return;                                      \
    }                                                \
                                                     \
    line_noclip(pixeler);                            \
}

#define hline_noclip(pixeler)                 \
{                                             \
    Pixel* dst = a_pixels + y * a_width + x1; \
                                              \
    for(int i = x2 - x1; i--; dst++) {        \
        pixeler;                              \
    }                                         \
}

#define hline_clip(pixeler)                  \
{                                            \
    x1 = a_math_max(x1, 0);                  \
    x2 = a_math_min(x2, a_width);            \
                                             \
    if(x1 >= x2 || y < 0 || y >= a_height) { \
        return;                              \
    }                                        \
                                             \
    hline_noclip(pixeler);                   \
}

#define vline_noclip(pixeler)                   \
{                                               \
    Pixel* dst = a_pixels + y1 * a_width + x;   \
    const int screenw = a_width;                \
                                                \
    for(int i = y2 - y1; i--; dst += screenw) { \
        pixeler;                                \
    }                                           \
}

#define vline_clip(pixeler)                 \
{                                           \
    y1 = a_math_max(y1, 0);                 \
    y2 = a_math_min(y2, a_height);          \
                                            \
    if(y1 >= y2 || x < 0 || x >= a_width) { \
        return;                             \
    }                                       \
                                            \
    vline_noclip(pixeler);                  \
}

#define shape_setup_plain  \
    const Pixel c = pixel;

#define shape_setup_rgb25    \
    const uint8_t r = red;   \
    const uint8_t g = green; \
    const uint8_t b = blue;  \

#define shape_setup_rgb50 shape_setup_rgb25
#define shape_setup_rgb75 shape_setup_rgb25

#define shape_setup_rgba     \
    shape_setup_rgb25        \
    const uint8_t a = alpha;

#define shape_setup_inverse

#define shapeMake(shape, func_args, blend, args)    \
                                                    \
    void a_draw__##shape##_noclip_##blend func_args \
    {                                               \
        shape_setup_##blend                         \
        shape##_noclip(a_pixel__##blend args)       \
    }                                               \
                                                    \
    void a_draw__##shape##_clip_##blend func_args   \
    {                                               \
        shape_setup_##blend                         \
        shape##_clip(a_pixel__##blend args)         \
    }

#define shapeMakeAll(blend, args)                                       \
    shapeMake(rectangle, (int x1, int y1, int x2, int y2), blend, args) \
    shapeMake(line, (int x1, int y1, int x2, int y2), blend, args)      \
    shapeMake(hline, (int x1, int x2, int y), blend, args)              \
    shapeMake(vline, (int x, int y1, int y2), blend, args)

shapeMakeAll(plain, (dst, c))
shapeMakeAll(rgba, (dst, r, g, b, a))
shapeMakeAll(rgb25, (dst, r, g, b))
shapeMakeAll(rgb50, (dst, r, g, b))
shapeMakeAll(rgb75, (dst, r, g, b))
shapeMakeAll(inverse, (dst))

#define drawSet(shape)                      \
({                                          \
    a_draw_##shape = shape##s[blend][clip]; \
})

#define drawSetAll()    \
({                      \
    drawSet(rectangle); \
    drawSet(line);      \
    drawSet(hline);     \
    drawSet(vline);     \
})

void a_draw__set(void)
{
    #define shapeInit(shape, index, blend)                     \
    ({                                                         \
        shape##s[index][0] = a_draw__##shape##_noclip_##blend; \
        shape##s[index][1] = a_draw__##shape##_clip_##blend;   \
    })

    #define shapeInitAll(index, blend)      \
    ({                                      \
        shapeInit(rectangle, index, blend); \
        shapeInit(line, index, blend);      \
        shapeInit(hline, index, blend);     \
        shapeInit(vline, index, blend);     \
    })

    shapeInitAll(A_PIXEL_PLAIN, plain);
    shapeInitAll(A_PIXEL_RGBA, rgba);
    shapeInitAll(A_PIXEL_RGB25, rgb25);
    shapeInitAll(A_PIXEL_RGB50, rgb50);
    shapeInitAll(A_PIXEL_RGB75, rgb75);
    shapeInitAll(A_PIXEL_INVERSE, inverse);

    blend = A_PIXEL_PLAIN;
    clip = true;

    drawSetAll();
}

void a_draw__setBlend(const PixelBlend_t b)
{
    blend = b;
    drawSetAll();
}

void a_draw__setClip(const bool c)
{
    clip = c;
    drawSetAll();
}

void a_draw__setAlpha(const uint8_t a)
{
    alpha = a;
}

void a_draw__setRGB(const uint8_t r, const uint8_t g, const uint8_t b)
{
    red = r;
    green = g;
    blue = b;

    pixel = a_pixel_make(red, green, blue);
}

void a_draw_fill_fast(const Pixel c)
{
    uint32_t* pixels = (uint32_t*)a_pixels;
    const uint32_t col = (c << 16) | c;

    for(int i = a_width * a_height / 2; i--; ) {
        *pixels++ = col;
    }
}

void a_draw_rectangle_fast(int x1, int y1, int x2, int y2, const Pixel c)
{
    x1 = a_math_max(x1, 0);
    y1 = a_math_max(y1, 0);
    x2 = a_math_min(x2, a_width);
    y2 = a_math_min(y2, a_height);

    if(x1 >= x2 || y1 >= y2) {
        return;
    }

    Pixel* pixels = a_pixels + y1 * a_width + x1;

    const int w = x2 - x1;
    const int o1 = x1 & 1;
    const int o2 = x2 & 1;
    const int W = (w - o1 - o2) / 2;

    const uint32_t col = (c << 16) | c;

    for(int i = y2 - y1; i--; pixels += a_width) {
        if(o1) {
            *pixels = c;
        }

        if(o2) {
            *(pixels + w - 1) = c;
        }

        uint32_t* pixels32 = (uint32_t*)(pixels + o1);

        for(int j = W; j--; ) {
            *pixels32++ = col;
        }
    }
}

void a_draw_rectangle_outline(const int x1, const int y1, const int x2, const int y2, const int t)
{
    a_draw_rectangle(x1, y1, x2, y1 + t);         // top
    a_draw_rectangle(x1, y2 - t, x2, y2);         // bottom
    a_draw_rectangle(x1, y1 + t, x1 + t, y2 - t); // left
    a_draw_rectangle(x2 - t, y1 + t, x2, y2 - t); // right
}

static bool cohen_sutherland_clip(int* const lx1, int* const ly1, int* const lx2, int* const ly2)
{
    int x1 = *lx1;
    int y1 = *ly1;
    int x2 = *lx2;
    int y2 = *ly2;

    const int screenw = a_width;
    const int screenh = a_height;

    #define OUT_LEFT  1
    #define OUT_RIGHT 2
    #define OUT_TOP   4
    #define OUT_DOWN  8

    #define outcode(x, y)                     \
    ({                                        \
        int o = 0;                            \
                                              \
        if(x < 0) o |= OUT_LEFT;              \
        else if(x >= screenw) o |= OUT_RIGHT; \
                                              \
        if(y < 0) o |= OUT_TOP;               \
        else if(y >= screenh) o |= OUT_DOWN;  \
                                              \
        o;                                    \
    })

    #define solvey(x, x1, y1, x2, y2)                 \
    ({                                                \
        (float)(y1 - y2) / (x1 - x2) * (x - x1) + y1; \
    })

    #define solvex(y, x1, y1, x2, y2)                 \
    ({                                                \
        (float)(x1 - x2) / (y1 - y2) * (y - y1) + x1; \
    })

    while(1) {
        const int outcode1 = outcode(x1, y1);
        const int outcode2 = outcode(x2, y2);

        if((outcode1 | outcode2) == 0) {
            *lx1 = x1;
            *ly1 = y1;
            *lx2 = x2;
            *ly2 = y2;

            return true;
        } else if(outcode1 & outcode2) {
            return false;
        } else {
            int x, y;
            const int outcode = outcode1 ? outcode1 : outcode2;

            if(outcode & OUT_LEFT) {
                x = 0;
                y = solvey(x, x1, y1, x2, y2);
            } else if(outcode & OUT_RIGHT) {
                x = screenw - 1;
                y = solvey(x, x1, y1, x2, y2);
            } else if(outcode & OUT_TOP) {
                y = 0;
                x = solvex(y, x1, y1, x2, y2);
            } else { // outcode & OUT_DOWN
                y = screenh - 1;
                x = solvex(y, x1, y1, x2, y2);
            }

            if(outcode == outcode1) {
                x1 = x;
                y1 = y;
            } else {
                x2 = x;
                y2 = y;
            }
        }
    }
}
