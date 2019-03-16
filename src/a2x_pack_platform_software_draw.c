/*
    Copyright 2010, 2016-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_platform_software_draw.v.h"

#if A_CONFIG_LIB_RENDER_SOFTWARE
#include "a2x_pack_draw.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"

typedef void (*ADrawPixel)(int X, int Y);
typedef void (*ADrawRectangle)(int X, int Y, int Width, int Height);
typedef void (*ADrawLine)(int X1, int Y1, int X2, int Y2);
typedef void (*ADrawHLine)(int X1, int X2, int Y);
typedef void (*ADrawVLine)(int X, int Y1, int Y2);
typedef void (*ADrawCircle)(int X, int Y, int Radius);

static ADrawPixel g_draw_pixel;
static ADrawPixel g_pixel[A_PIXEL_BLEND_NUM];

static ADrawRectangle g_draw_rectangle;
static ADrawRectangle g_rectangle[A_PIXEL_BLEND_NUM][2]; // [Blend][Fill]

static ADrawLine g_draw_line;
static ADrawLine g_line[A_PIXEL_BLEND_NUM];

static ADrawHLine g_draw_hline;
static ADrawHLine g_hline[A_PIXEL_BLEND_NUM];

static ADrawVLine g_draw_vline;
static ADrawHLine g_vline[A_PIXEL_BLEND_NUM];

static ADrawCircle g_draw_circle_noclip;
static ADrawCircle g_draw_circle_clip;
static ADrawCircle g_circle[A_PIXEL_BLEND_NUM][2][2]; // [Blend][Clip][Fill]

static bool cohen_sutherland_clip(int* X1, int* Y1, int* X2, int* Y2)
{
    int x1 = *X1;
    int y1 = *Y1;
    int x2 = *X2;
    int y2 = *Y2;

    const int clipX1 = a__screen.clipX;
    const int clipX2 = a__screen.clipX2;
    const int clipY1 = a__screen.clipY;
    const int clipY2 = a__screen.clipY2;

    #define A__OUT_LEFT  1
    #define A__OUT_RIGHT 2
    #define A__OUT_TOP   4
    #define A__OUT_DOWN  8

    #define outcode(o, x, y)                  \
    {                                         \
        if(x < clipX1) o |= A__OUT_LEFT;         \
        else if(x >= clipX2) o |= A__OUT_RIGHT;  \
                                              \
        if(y < clipY1) o |= A__OUT_TOP;          \
        else if(y >= clipY2) o |= A__OUT_DOWN;   \
    }

    #define solvex() (x1 + (x1 - x2) * (y - y1) / (y1 - y2))
    #define solvey() (y1 + (y1 - y2) * (x - x1) / (x1 - x2))

    while(true) {
        int outcode1 = 0;
        int outcode2 = 0;

        outcode(outcode1, x1, y1);
        outcode(outcode2, x2, y2);

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

            if(outcode & A__OUT_LEFT) {
                x = clipX1;
                y = solvey();
            } else if(outcode & A__OUT_RIGHT) {
                x = clipX2 - 1;
                y = solvey();
            } else if(outcode & A__OUT_TOP) {
                y = clipY1;
                x = solvex();
            } else { // outcode & A__OUT_DOWN
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
    APixel* dst = Buffer;                                                   \
                                                                            \
    if((PrimaryOnScreen) && (SecondaryOnScreen)) {                          \
        while(PrimaryCoord < SecondaryCoord && (PrimaryOnScreen)) {         \
            error += 2 * PrimaryCoord + 1;                                  \
            PrimaryCoord++;                                                 \
                                                                            \
            A__PIXEL_DRAW(dst);                                             \
            dst += PrimaryBufferInc;                                        \
                                                                            \
            if(error > 0) {                                                 \
                error += -2 * SecondaryCoord + 1;                           \
                SecondaryCoord--;                                           \
                                                                            \
                if(!(SecondaryOnScreen)) {                                  \
                    break;                                                  \
                }                                                           \
                                                                            \
                dst += SecondaryBufferInc;                                  \
            }                                                               \
        }                                                                   \
    }                                                                       \
} while(0)

#define A__FUNC_NAME_EXPAND2(Name, Blend) a_draw__##Name##_##Blend
#define A__FUNC_NAME_EXPAND(Name, Blend) A__FUNC_NAME_EXPAND2(Name, Blend)
#define A__FUNC_NAME(Name) A__FUNC_NAME_EXPAND(Name, A__BLEND)

#define A__PIXEL_DRAW_EXPAND2(Blend) a_pixel__##Blend
#define A__PIXEL_DRAW_EXPAND(Blend, Params) A__PIXEL_DRAW_EXPAND2(Blend)(Params)
#define A__PIXEL_DRAW(Dst) A__PIXEL_DRAW_EXPAND(A__BLEND, Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__BLEND_SETUP \
    const APixel color = a_pixel__state.pixel;
#define A__PIXEL_PARAMS , color
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND rgba
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;   \
    const int alpha = a_pixel__state.alpha; \
    if(alpha == 0) {                        \
        return;                             \
    }
#define A__PIXEL_PARAMS , red, green, blue, alpha
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND rgb25
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND rgb50
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND rgb75
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND inverse
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND mod
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_draw.inc.c"

#define A__BLEND add
#define A__BLEND_SETUP                      \
    const int red = a_pixel__state.red;     \
    const int green = a_pixel__state.green; \
    const int blue = a_pixel__state.blue;
#define A__PIXEL_PARAMS , red, green, blue
#include "a2x_pack_platform_software_draw.inc.c"

void a_platform_software_draw__init(void)
{
    #define initRoutines(Index, Blend)                                \
        g_pixel[Index] = a_draw__pixel_##Blend;                       \
        g_rectangle[Index][0] = a_draw__rectangle_nofill_##Blend;     \
        g_rectangle[Index][1] = a_draw__rectangle_fill_##Blend;       \
        g_line[Index] = a_draw__line_##Blend;                         \
        g_hline[Index] = a_draw__hline_##Blend;                       \
        g_vline[Index] = a_draw__vline_##Blend;                       \
        g_circle[Index][0][0] = a_draw__circle_noclip_nofill_##Blend; \
        g_circle[Index][0][1] = a_draw__circle_noclip_fill_##Blend;   \
        g_circle[Index][1][0] = a_draw__circle_clip_nofill_##Blend;   \
        g_circle[Index][1][1] = a_draw__circle_clip_fill_##Blend;

    initRoutines(A_PIXEL_BLEND_PLAIN, plain);
    initRoutines(A_PIXEL_BLEND_RGBA, rgba);
    initRoutines(A_PIXEL_BLEND_RGB25, rgb25);
    initRoutines(A_PIXEL_BLEND_RGB50, rgb50);
    initRoutines(A_PIXEL_BLEND_RGB75, rgb75);
    initRoutines(A_PIXEL_BLEND_INVERSE, inverse);
    initRoutines(A_PIXEL_BLEND_MOD, mod);
    initRoutines(A_PIXEL_BLEND_ADD, add);

    a_platform_software_draw__updateRoutines();
}

void a_platform_software_draw__updateRoutines(void)
{
    APixelBlend blend = a_pixel__state.blend;
    bool fill = a_pixel__state.fillDraw;

    g_draw_pixel = g_pixel[blend];
    g_draw_rectangle = g_rectangle[blend][fill];
    g_draw_line = g_line[blend];
    g_draw_hline = g_hline[blend];
    g_draw_vline = g_vline[blend];
    g_draw_circle_noclip = g_circle[blend][0][fill];
    g_draw_circle_clip = g_circle[blend][1][fill];
}

void a_platform_api__drawPixel(int X, int Y)
{
    if(a_screen_boxInsideClip(X, Y, 1, 1)) {
        g_draw_pixel(X, Y);
    }
}

void a_platform_api__drawLine(int X1, int Y1, int X2, int Y2)
{
    int x = a_math_min(X1, X2);
    int y = a_math_min(Y1, Y2);
    int w = a_math_abs(X2 - X1) + 1;
    int h = a_math_abs(Y2 - Y1) + 1;

    if(!a_screen_boxOnClip(x, y, w, h)
        || !cohen_sutherland_clip(&X1, &Y1, &X2, &Y2)) {

        return;
    }

    g_draw_line(X1, Y1, X2, Y2);
}

void a_platform_api__drawHLine(int X1, int X2, int Y)
{
    if(!a_screen_boxOnClip(X1, Y, X2 - X1 + 1, 1)) {
        return;
    }

    X1 = a_math_max(X1, a__screen.clipX);
    X2 = a_math_min(X2, a__screen.clipX2 - 1);

    g_draw_hline(X1, X2, Y);
}

void a_platform_api__drawVLine(int X, int Y1, int Y2)
{
    if(!a_screen_boxOnClip(X, Y1, 1, Y2 - Y1 + 1)) {
        return;
    }

    Y1 = a_math_max(Y1, a__screen.clipY);
    Y2 = a_math_min(Y2, a__screen.clipY2 - 1);

    g_draw_vline(X, Y1, Y2);
}

static void drawRectangle(int X, int Y, int Width, int Height)
{
    if(a_screen_boxInsideClip(X, Y, Width, Height)) {
        g_draw_rectangle(X, Y, Width, Height);
        return;
    }

    if(!a_screen_boxOnClip(X, Y, Width, Height)) {
        return;
    }

    const int x2 = a_math_min(X + Width, a__screen.clipX2);
    const int y2 = a_math_min(Y + Height, a__screen.clipY2);

    X = a_math_max(X, a__screen.clipX);
    Y = a_math_max(Y, a__screen.clipY);
    Width = a_math_min(Width, x2 - X);
    Height = a_math_min(Height, y2 - Y);

    g_draw_rectangle(X, Y, Width, Height);
}

void a_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    drawRectangle(X, Y, Width, Height);
}

void a_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    drawRectangle(X, Y, Width, Height);
}

static void drawCircle(int X, int Y, int Radius)
{
    int boxX = X - Radius;
    int boxY = Y - Radius;
    int boxDim = 2 * Radius;

    if(a_screen_boxInsideClip(boxX, boxY, boxDim, boxDim)) {
        g_draw_circle_noclip(X, Y, Radius);
        return;
    }

    if(a_screen_boxOnClip(boxX, boxY, boxDim, boxDim)) {
        g_draw_circle_clip(X, Y, Radius);
    }
}

void a_platform_api__drawCircleOutline(int X, int Y, int Radius)
{
    drawCircle(X, Y, Radius);
}

void a_platform_api__drawCircleFilled(int X, int Y, int Radius)
{
    drawCircle(X, Y, Radius);
}
#endif // A_CONFIG_LIB_RENDER_SOFTWARE
