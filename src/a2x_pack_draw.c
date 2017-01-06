/*
    Copyright 2010, 2016 Alex Margarit

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

typedef void (*ADrawPixel)(int X, int Y);
typedef void (*ADrawRectangle)(int X, int Y, int Width, int Height);
typedef void (*ADrawLine)(int X1, int Y1, int X2, int Y2);
typedef void (*ADrawHLine)(int X1, int X2, int Y);
typedef void (*ADrawVLine)(int X, int Y1, int Y2);
typedef void (*ADrawCircle)(int X, int Y, int Radius);

static ADrawPixel g_draw_pixel;
static ADrawPixel g_pixel[A_PIXEL_BLEND_NUM];

static ADrawRectangle g_draw_rectangle;
static ADrawRectangle g_rectangle[A_PIXEL_BLEND_NUM];

static ADrawLine g_draw_line;
static ADrawLine g_line[A_PIXEL_BLEND_NUM];

static ADrawHLine g_draw_hline;
static ADrawHLine g_hline[A_PIXEL_BLEND_NUM];

static ADrawVLine g_draw_vline;
static ADrawHLine g_vline[A_PIXEL_BLEND_NUM];

static ADrawCircle g_draw_circle_noclip;
static ADrawCircle g_draw_circle_clip;
static ADrawCircle g_circle[A_PIXEL_BLEND_NUM][2];

static bool cohen_sutherland_clip(int* X1, int* Y1, int* X2, int* Y2)
{
    int x1 = *X1;
    int y1 = *Y1;
    int x2 = *X2;
    int y2 = *Y2;

    const int clipX1 = a_screen__clipX;
    const int clipX2 = a_screen__clipX2;
    const int clipY1 = a_screen__clipY;
    const int clipY2 = a_screen__clipY2;

    #define OUT_LEFT  1
    #define OUT_RIGHT 2
    #define OUT_TOP   4
    #define OUT_DOWN  8

    #define outcode(x, y)                     \
    ({                                        \
        int o = 0;                            \
                                              \
        if(x < clipX1) o |= OUT_LEFT;         \
        else if(x >= clipX2) o |= OUT_RIGHT;  \
                                              \
        if(y < clipY1) o |= OUT_TOP;          \
        else if(y >= clipY2) o |= OUT_DOWN;   \
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
                x = clipX1;
                y = solvey();
            } else if(outcode & OUT_RIGHT) {
                x = clipX2 - 1;
                y = solvey();
            } else if(outcode & OUT_TOP) {
                y = clipY1;
                x = solvex();
            } else { // outcode & OUT_DOWN
                y = clipY2 - 1;
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

static void findMidpoint(int Radius, int* MidX, int* MidY)
{
    int x = Radius;
    int y = 0;
    int error = -Radius / 2;
    int lastx = x, lasty = y;

    while(x > y) {
        lastx = x;
        lasty = y;

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1;
        y++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1;
            x--;
        }
    }

    *MidX = lastx;
    *MidY = lasty;
}

#define drawClippedOctant(BoundX, BoundY, BoundW, BoundH,                   \
                          PrimaryCoord, SecondaryCoord,                     \
                          Buffer, PrimaryBufferInc, SecondaryBufferInc,     \
                          YOffScreen, XOffScreen,                           \
                          PrimaryOnScreen, SecondaryOnScreen)               \
do {                                                                        \
    if(!a_screen_boxOnClip(BoundX, BoundY, BoundW, BoundH)) {               \
        break;                                                              \
    }                                                                       \
                                                                            \
    int PrimaryCoord = 0;                                                   \
    int SecondaryCoord = Radius;                                            \
    int error = -Radius / 2;                                                \
                                                                            \
    while(YOffScreen) {                                                     \
        error += 2 * PrimaryCoord + 1;                                      \
        PrimaryCoord++;                                                     \
                                                                            \
        Buffer += PrimaryBufferInc;                                         \
                                                                            \
        if(error > 0) {                                                     \
            error += -2 * SecondaryCoord + 1;                               \
            SecondaryCoord--;                                               \
                                                                            \
            Buffer += SecondaryBufferInc;                                   \
        }                                                                   \
    }                                                                       \
                                                                            \
    while(XOffScreen) {                                                     \
        error += 2 * PrimaryCoord + 1;                                      \
        PrimaryCoord++;                                                     \
                                                                            \
        Buffer += PrimaryBufferInc;                                         \
                                                                            \
        if(error > 0) {                                                     \
            error += -2 * SecondaryCoord + 1;                               \
            SecondaryCoord--;                                               \
                                                                            \
            Buffer += SecondaryBufferInc;                                   \
        }                                                                   \
    }                                                                       \
                                                                            \
    if((PrimaryOnScreen) && (SecondaryOnScreen)) {                          \
        while(PrimaryCoord < SecondaryCoord && (PrimaryOnScreen)) {         \
            error += 2 * PrimaryCoord + 1;                                  \
            PrimaryCoord++;                                                 \
                                                                            \
            A__PIXEL_DRAW(Buffer);                                          \
            Buffer += PrimaryBufferInc;                                     \
                                                                            \
            if(error > 0) {                                                 \
                error += -2 * SecondaryCoord + 1;                           \
                SecondaryCoord--;                                           \
                                                                            \
                if(!(SecondaryOnScreen)) {                                  \
                    break;                                                  \
                }                                                           \
                                                                            \
                Buffer += SecondaryBufferInc;                               \
            }                                                               \
        }                                                                   \
    }                                                                       \
} while(0)

#define A__CONCAT_WORKER(A, B) A##B
#define A__CONCAT(A, B) A__CONCAT_WORKER(A, B)

#define A__FUNC_NAME(Prefix) A__CONCAT(Prefix##_, A__BLEND)

#define A__PIXEL_DRAW_WORKER(Params) A__CONCAT(a_pixel__, A__BLEND)(Params)
#define A__PIXEL_DRAW(Dst) A__PIXEL_DRAW_WORKER(Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__BLEND_SETUP \
    const APixel a__pass_color = a_pixel__state.pixel;
#define A__PIXEL_PARAMS , a__pass_color
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgba
#define A__BLEND_SETUP                                       \
    const uint8_t a__pass_red = a_pixel__state.red;          \
    const uint8_t a__pass_green = a_pixel__state.green;      \
    const uint8_t a__pass_blue = a_pixel__state.blue;        \
    const unsigned int a__pass_alpha = a_pixel__state.alpha;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue, a__pass_alpha
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb25
#define A__BLEND_SETUP                                  \
    const uint8_t a__pass_red = a_pixel__state.red;     \
    const uint8_t a__pass_green = a_pixel__state.green; \
    const uint8_t a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb50
#define A__BLEND_SETUP                                  \
    const uint8_t a__pass_red = a_pixel__state.red;     \
    const uint8_t a__pass_green = a_pixel__state.green; \
    const uint8_t a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND rgb75
#define A__BLEND_SETUP                                  \
    const uint8_t a__pass_red = a_pixel__state.red;     \
    const uint8_t a__pass_green = a_pixel__state.green; \
    const uint8_t a__pass_blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , a__pass_red, a__pass_green, a__pass_blue
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

#define A__BLEND inverse
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_draw.inc.c"
#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS

void a_draw__init(void)
{
    #define initRoutines(Index, Blend)                                      \
        g_pixel[Index] = a_draw__pixel_##Blend;                             \
        g_rectangle[Index] = a_draw__rectangle_##Blend;                     \
        g_line[Index] = a_draw__line_##Blend;                               \
        g_hline[Index] = a_draw__hline_##Blend;                             \
        g_vline[Index] = a_draw__vline_##Blend;                             \
        g_circle[Index][0] = a_draw__circle_noclip_##Blend;                 \
        g_circle[Index][1] = a_draw__circle_clip_##Blend;                   \

    initRoutines(A_PIXEL_BLEND_PLAIN, plain);
    initRoutines(A_PIXEL_BLEND_RGBA, rgba);
    initRoutines(A_PIXEL_BLEND_RGB25, rgb25);
    initRoutines(A_PIXEL_BLEND_RGB50, rgb50);
    initRoutines(A_PIXEL_BLEND_RGB75, rgb75);
    initRoutines(A_PIXEL_BLEND_INVERSE, inverse);

    a_draw__updateRoutines();
}

void a_draw__updateRoutines(void)
{
    g_draw_pixel = g_pixel[a_pixel__state.blend];
    g_draw_rectangle = g_rectangle[a_pixel__state.blend];
    g_draw_line = g_line[a_pixel__state.blend];
    g_draw_hline = g_hline[a_pixel__state.blend];
    g_draw_vline = g_vline[a_pixel__state.blend];
    g_draw_circle_noclip = g_circle[a_pixel__state.blend][0];
    g_draw_circle_clip = g_circle[a_pixel__state.blend][1];
}

void a_draw_fill(void)
{
    g_draw_rectangle(0, 0, a_screen__width, a_screen__height);
}

void a_draw_rectangleThick(int X, int Y, int Width, int Height, int Thickness)
{
    g_draw_rectangle(X, Y, Width, Thickness); // top
    g_draw_rectangle(X, Y + Height - Thickness, Width, Thickness); // bottom
    g_draw_rectangle(X, Y + Thickness, Thickness, Height - 2 * Thickness); // left
    g_draw_rectangle(X + Width - Thickness, Y + Thickness, Thickness, Height - 2 * Thickness); // right
}

void a_draw_pixel(int X, int Y)
{
    if(a_screen_boxInsideClip(X, Y, 1, 1)) {
        g_draw_pixel(X, Y);
    }
}

void a_draw_rectangle(int X, int Y, int Width, int Height)
{
    if(a_screen_boxInsideClip(X, Y, Width, Height)) {
        g_draw_rectangle(X, Y, Width, Height);
        return;
    }

    if(!a_screen_boxOnClip(X, Y, Width, Height)) {
        return;
    }

    const int x2 = a_math_min(X + Width, a_screen__clipX2);
    const int y2 = a_math_min(Y + Height, a_screen__clipY2);

    X = a_math_max(X, a_screen__clipX);
    Y = a_math_max(Y, a_screen__clipY);
    Width = a_math_min(Width, x2 - X);
    Height = a_math_min(Height, y2 - Y);

    g_draw_rectangle(X, Y, Width, Height);
}

void a_draw_line(int X1, int Y1, int X2, int Y2)
{
    if(!cohen_sutherland_clip(&X1, &Y1, &X2, &Y2)) {
        return;
    }

    g_draw_line(X1, Y1, X2, Y2);
}

void a_draw_hline(int X1, int X2, int Y)
{
    if(X1 >= X2 || !a_screen_boxOnClip(X1, Y, X2 - X1, 1)) {
        return;
    }

    X1 = a_math_max(X1, a_screen__clipX);
    X2 = a_math_min(X2, a_screen__clipX2);

    g_draw_hline(X1, X2, Y);
}

void a_draw_vline(int X, int Y1, int Y2)
{
    if(Y1 >= Y2 || !a_screen_boxOnClip(X, Y1, 1, Y2 - Y1)) {
        return;
    }

    Y1 = a_math_max(Y1, a_screen__clipY);
    Y2 = a_math_min(Y2, a_screen__clipY2);

    g_draw_vline(X, Y1, Y2);
}

void a_draw_circle(int X, int Y, int Radius)
{
    if(a_screen_boxInsideClip(X - Radius, Y - Radius, 2 * Radius, 2 * Radius)) {
        g_draw_circle_noclip(X, Y, Radius);
        return;
    }

    if(a_screen_boxOnClip(X - Radius, Y - Radius, 2 * Radius, 2 * Radius)) {
        g_draw_circle_clip(X, Y, Radius);
    }
}
