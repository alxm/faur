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

ADrawRectangle a_draw_rectangle;
static ADrawRectangle g_rectangleDraw[A_PIXEL_TYPE_NUM][2];

ADrawLine a_draw_line;
static ADrawLine g_lineDraw[A_PIXEL_TYPE_NUM][2];

ADrawHLine a_draw_hline;
static ADrawHLine g_hlineDraw[A_PIXEL_TYPE_NUM][2];

ADrawVLine a_draw_vline;
static ADrawHLine g_vlineDraw[A_PIXEL_TYPE_NUM][2];

ADrawCircle a_draw_circle;
//static ADrawCircle g_circleDraw[A_PIXEL_TYPE_NUM][2];

static APixelBlend g_blend;
static bool g_clip;

static uint8_t g_alpha;
static uint8_t g_red, g_green, g_blue;
static APixel g_pixel;

static bool cohen_sutherland_clip(int* X1, int* Y1, int* X2, int* Y2)
{
    int x1 = *X1;
    int y1 = *Y1;
    int x2 = *X2;
    int y2 = *Y2;

    const int screenw = a_screen__width;
    const int screenh = a_screen__height;

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

    #define solvey() (float)(y1 - y2) / (x1 - x2) * (x - x1) + y1;
    #define solvex() (float)(x1 - x2) / (y1 - y2) * (y - y1) + x1;

    while(true) {
        const int outcode1 = outcode(x1, y1);
        const int outcode2 = outcode(x2, y2);

        if((outcode1 | outcode2) == 0) {
            *X1 = x1;
            *Y1 = y1;
            *X2 = x2;
            *Y2 = y2;

            return true;
        } else if(outcode1 & outcode2) {
            return false;
        } else {
            int x, y;
            const int outcode = outcode1 ? outcode1 : outcode2;

            if(outcode & OUT_LEFT) {
                x = 0;
                y = solvey();
            } else if(outcode & OUT_RIGHT) {
                x = screenw - 1;
                y = solvey();
            } else if(outcode & OUT_TOP) {
                y = 0;
                x = solvex();
            } else { // outcode & OUT_DOWN
                y = screenh - 1;
                x = solvex();
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

#define rectangle_noclip(Pixeler)                  \
{                                                  \
    APixel* pixels = a_screen__pixels              \
                     + Y1 * a_screen__width + X1;  \
                                                   \
    const int screenw = a_screen__width;           \
    const int w = X2 - X1;                         \
                                                   \
    for(int i = Y2 - Y1; i--; pixels += screenw) { \
        APixel* a__pass_dst = pixels;              \
                                                   \
        for(int j = w; j--; a__pass_dst++) {       \
            Pixeler;                               \
        }                                          \
    }                                              \
}

#define rectangle_clip(Pixeler)            \
{                                          \
    X1 = a_math_max(X1, 0);                \
    Y1 = a_math_max(Y1, 0);                \
    X2 = a_math_min(X2, a_screen__width);  \
    Y2 = a_math_min(Y2, a_screen__height); \
                                           \
    if(X1 >= X2 || Y1 >= Y2) {             \
        return;                            \
    }                                      \
                                           \
    rectangle_noclip(Pixeler);             \
}

#define line_noclip(Pixeler)                                   \
{                                                              \
    const int xmin = a_math_min(X1, X2);                       \
    const int xmax = a_math_max(X1, X2);                       \
    const int ymin = a_math_min(Y1, Y2);                       \
    const int ymax = a_math_max(Y1, Y2);                       \
                                                               \
    if(X1 == X2) {                                             \
        a_draw_vline(X1, ymin, ymax);                          \
    } else if(Y1 == Y2) {                                      \
        a_draw_hline(xmin, xmax, Y1);                          \
    } else {                                                   \
        const int deltax = xmax - xmin;                        \
        const int deltay = ymax - ymin;                        \
                                                               \
        const int denominator = a_math_max(deltax, deltay);    \
        const int numeratorinc = a_math_min(deltax, deltay);   \
        int numerator = denominator / 2;                       \
                                                               \
        const int xinct = (X1 <= X2) ? 1 : -1;                 \
        const int yinct = (Y1 <= Y2) ? 1 : -1;                 \
                                                               \
        const int xinc1 = (denominator == deltax) ? xinct : 0; \
        const int yinc1 = (denominator == deltax) ? 0 : yinct; \
                                                               \
        const int xinc2 = (denominator == deltax) ? 0 : xinct; \
        const int yinc2 = (denominator == deltax) ? yinct : 0; \
                                                               \
        const int screenw = a_screen__width;                   \
        APixel* a__pass_dst = a_screen__pixels                 \
                              + Y1 * screenw + X1;             \
                                                               \
        for(int i = denominator + 1; i--; ) {                  \
            Pixeler;                                           \
                                                               \
            numerator += numeratorinc;                         \
                                                               \
            if(numerator >= denominator) {                     \
                numerator -= denominator;                      \
                a__pass_dst += xinc2;                          \
                a__pass_dst += yinc2 * screenw;                \
            }                                                  \
                                                               \
            a__pass_dst += xinc1;                              \
            a__pass_dst += yinc1 * screenw;                    \
        }                                                      \
    }                                                          \
}

#define line_clip(Pixeler)                           \
{                                                    \
    if(!cohen_sutherland_clip(&X1, &Y1, &X2, &Y2)) { \
        return;                                      \
    }                                                \
                                                     \
    line_noclip(Pixeler);                            \
}

#define hline_noclip(Pixeler)                          \
{                                                      \
    APixel* a__pass_dst = a_screen__pixels             \
                          + Y * a_screen__width + X1;  \
                                                       \
    for(int i = X2 - X1; i--; a__pass_dst++) {         \
        Pixeler;                                       \
    }                                                  \
}

#define hline_clip(Pixeler)                          \
{                                                    \
    X1 = a_math_max(X1, 0);                          \
    X2 = a_math_min(X2, a_screen__width);            \
                                                     \
    if(X1 >= X2 || Y < 0 || Y >= a_screen__height) { \
        return;                                      \
    }                                                \
                                                     \
    hline_noclip(Pixeler);                           \
}

#define vline_noclip(Pixeler)                           \
{                                                       \
    APixel* a__pass_dst = a_screen__pixels              \
                          + Y1 * a_screen__width + X;   \
    const int screenw = a_screen__width;                \
                                                        \
    for(int i = Y2 - Y1; i--; a__pass_dst += screenw) { \
        Pixeler;                                        \
    }                                                   \
}

#define vline_clip(Pixeler)                         \
{                                                   \
    Y1 = a_math_max(Y1, 0);                         \
    Y2 = a_math_min(Y2, a_screen__height);          \
                                                    \
    if(Y1 >= Y2 || X < 0 || X >= a_screen__width) { \
        return;                                     \
    }                                               \
                                                    \
    vline_noclip(Pixeler);                          \
}

#define shape_setup_plain                 \
    const APixel a__pass_color = g_pixel;

#define shape_setup_rgb25                  \
    const uint8_t a__pass_red = g_red;     \
    const uint8_t a__pass_green = g_green; \
    const uint8_t a__pass_blue = g_blue;

#define shape_setup_rgb50 shape_setup_rgb25
#define shape_setup_rgb75 shape_setup_rgb25

#define shape_setup_rgba                   \
    shape_setup_rgb25                      \
    const uint8_t a__pass_alpha = g_alpha;

#define shape_setup_inverse

#define shapeMake(Shape, FuncArgs, Blend, BlendArgs) \
                                                     \
    void a_draw__##Shape##_noclip_##Blend FuncArgs   \
    {                                                \
        shape_setup_##Blend                          \
        Shape##_noclip(a_pixel__##Blend BlendArgs)   \
    }                                                \
                                                     \
    void a_draw__##Shape##_clip_##Blend FuncArgs     \
    {                                                \
        shape_setup_##Blend                          \
        Shape##_clip(a_pixel__##Blend BlendArgs)     \
    }

#define shapeMakeAll(Blend, BlendArgs)                                       \
    shapeMake(rectangle, (int X1, int Y1, int X2, int Y2), Blend, BlendArgs) \
    shapeMake(line,      (int X1, int Y1, int X2, int Y2), Blend, BlendArgs) \
    shapeMake(hline,     (int X1, int X2, int Y),          Blend, BlendArgs) \
    shapeMake(vline,     (int X,  int Y1, int Y2),         Blend, BlendArgs)

shapeMakeAll(plain,   (a__pass_dst, a__pass_color))
shapeMakeAll(rgba,    (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue, a__pass_alpha))
shapeMakeAll(rgb25,   (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue))
shapeMakeAll(rgb50,   (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue))
shapeMakeAll(rgb75,   (a__pass_dst, a__pass_red, a__pass_green, a__pass_blue))
shapeMakeAll(inverse, (a__pass_dst))

#define drawSet(Shape)                                 \
({                                                     \
    a_draw_##Shape = g_##Shape##Draw[g_blend][g_clip]; \
})

#define drawSetAll()    \
({                      \
    drawSet(rectangle); \
    drawSet(line);      \
    drawSet(hline);     \
    drawSet(vline);     \
})

void a_draw__init(void)
{
    #define shapeInit(Shape, Index, Blend)                            \
    ({                                                                \
        g_##Shape##Draw[Index][0] = a_draw__##Shape##_noclip_##Blend; \
        g_##Shape##Draw[Index][1] = a_draw__##Shape##_clip_##Blend;   \
    })

    #define shapeInitAll(Index, Blend)      \
    ({                                      \
        shapeInit(rectangle, Index, Blend); \
        shapeInit(line, Index, Blend);      \
        shapeInit(hline, Index, Blend);     \
        shapeInit(vline, Index, Blend);     \
    })

    shapeInitAll(A_PIXEL_PLAIN, plain);
    shapeInitAll(A_PIXEL_RGBA, rgba);
    shapeInitAll(A_PIXEL_RGB25, rgb25);
    shapeInitAll(A_PIXEL_RGB50, rgb50);
    shapeInitAll(A_PIXEL_RGB75, rgb75);
    shapeInitAll(A_PIXEL_INVERSE, inverse);

    g_blend = A_PIXEL_PLAIN;
    g_clip = true;

    drawSetAll();
}

void a_draw__setBlend(APixelBlend Blend)
{
    g_blend = Blend;
    drawSetAll();
}

void a_draw__setClip(bool DoClip)
{
    g_clip = DoClip;
    drawSetAll();
}

void a_draw__setAlpha(uint8_t Alpha)
{
    g_alpha = Alpha;
}

void a_draw__setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    g_red = Red;
    g_green = Green;
    g_blue = Blue;

    g_pixel = a_pixel_make(g_red, g_green, g_blue);
}

void a_draw_rectangleBorder(int X1, int Y1, int X2, int Y2, int Border)
{
    a_draw_rectangle(X1,          Y1,          X2,          Y1 + Border); // top
    a_draw_rectangle(X1,          Y2 - Border, X2,          Y2);          // bottom
    a_draw_rectangle(X1,          Y1 + Border, X1 + Border, Y2 - Border); // left
    a_draw_rectangle(X2 - Border, Y1 + Border, X2,          Y2 - Border); // right
}
