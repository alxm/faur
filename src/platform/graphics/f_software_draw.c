/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_software_draw.v.h"
#include <faur.v.h>

#if A_CONFIG_LIB_RENDER_SOFTWARE
#define A__COMPILE_INC 1

typedef void (*ADrawPixel)(int X, int Y);
typedef void (*ADrawHLine)(int X1, int X2, int Y);
typedef void (*ADrawVLine)(int X, int Y1, int Y2);
typedef void (*ADrawLine)(int X1, int Y1, int X2, int Y2);
typedef void (*ADrawRectangle)(int X, int Y, int Width, int Height);
typedef void (*ADrawCircle)(int X, int Y, int Radius);

static bool cohen_sutherland_clip(int* X1, int* Y1, int* X2, int* Y2)
{
    int x1 = *X1;
    int y1 = *Y1;
    int x2 = *X2;
    int y2 = *Y2;

    const int clipX1 = f__screen.clipX;
    const int clipX2 = f__screen.clipX2;
    const int clipY1 = f__screen.clipY;
    const int clipY2 = f__screen.clipY2;

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
    if(!f_screen_boxOnClip(BoundX, BoundY, BoundW, BoundH)) {               \
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

#define A__FUNC_NAME(Name) A_GLUE4(f_draw__, Name, _, A__BLEND)
#define A__PIXEL_DRAW(Dst) A_GLUE2(f_color__draw_, A__BLEND)(Dst A__PIXEL_PARAMS)

#define A__BLEND plain
#define A__BLEND_SETUP const APixel color = f__color.pixel;
#define A__PIXEL_PARAMS , color
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND rgba
#define A__BLEND_SETUP \
    const ARgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define A__PIXEL_PARAMS , &rgb, alpha
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND rgb25
#define A__BLEND_SETUP const ARgb rgb = f__color.rgb;
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND rgb50
#define A__BLEND_SETUP const ARgb rgb = f__color.rgb;
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND rgb75
#define A__BLEND_SETUP const ARgb rgb = f__color.rgb;
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND inverse
#define A__BLEND_SETUP
#define A__PIXEL_PARAMS
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND mod
#define A__BLEND_SETUP const ARgb rgb = f__color.rgb;
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/f_software_draw.inc.c"

#define A__BLEND add
#define A__BLEND_SETUP const ARgb rgb = f__color.rgb;
#define A__PIXEL_PARAMS , &rgb
#include "platform/graphics/f_software_draw.inc.c"

#define A__INIT_BLEND(Index, Name)                           \
    [Index] = {                                              \
        .pixel = f_draw__pixel_##Name,                       \
        .hline = f_draw__hline_##Name,                       \
        .vline = f_draw__vline_##Name,                       \
        .line = f_draw__line_##Name,                         \
        .rectangle[0] = f_draw__rectangle_nofill_##Name,     \
        .rectangle[1] = f_draw__rectangle_fill_##Name,       \
        .circle[0][0] = f_draw__circle_noclip_nofill_##Name, \
        .circle[0][1] = f_draw__circle_noclip_fill_##Name,   \
        .circle[1][0] = f_draw__circle_clip_nofill_##Name,   \
        .circle[1][1] = f_draw__circle_clip_fill_##Name,     \
    },

static const struct {
    ADrawPixel pixel;
    ADrawHLine hline;
    ADrawVLine vline;
    ADrawLine line;
    ADrawRectangle rectangle[2]; // [Fill]
    ADrawCircle circle[2][2]; // [Clip][Fill]
} g_draw[A_COLOR_BLEND_NUM] = {
    A__INIT_BLEND(A_COLOR_BLEND_PLAIN, plain)
    A__INIT_BLEND(A_COLOR_BLEND_RGBA, rgba)
    A__INIT_BLEND(A_COLOR_BLEND_RGB25, rgb25)
    A__INIT_BLEND(A_COLOR_BLEND_RGB50, rgb50)
    A__INIT_BLEND(A_COLOR_BLEND_RGB75, rgb75)
    A__INIT_BLEND(A_COLOR_BLEND_INVERSE, inverse)
    A__INIT_BLEND(A_COLOR_BLEND_MOD, mod)
    A__INIT_BLEND(A_COLOR_BLEND_ADD, add)
};

void f_platform_api__drawPixel(int X, int Y)
{
    if(f_screen_boxInsideClip(X, Y, 1, 1)) {
        g_draw[f__color.blend].pixel(X, Y);
    }
}

void f_platform_api__drawLine(int X1, int Y1, int X2, int Y2)
{
    int x = f_math_min(X1, X2);
    int y = f_math_min(Y1, Y2);
    int w = f_math_abs(X2 - X1) + 1;
    int h = f_math_abs(Y2 - Y1) + 1;

    if(!f_screen_boxOnClip(x, y, w, h)
        || !cohen_sutherland_clip(&X1, &Y1, &X2, &Y2)) {

        return;
    }

    g_draw[f__color.blend].line(X1, Y1, X2, Y2);
}

void f_platform_api__drawHLine(int X1, int X2, int Y)
{
    if(!f_screen_boxOnClip(X1, Y, X2 - X1 + 1, 1)) {
        return;
    }

    X1 = f_math_max(X1, f__screen.clipX);
    X2 = f_math_min(X2, f__screen.clipX2 - 1);

    g_draw[f__color.blend].hline(X1, X2, Y);
}

void f_platform_api__drawVLine(int X, int Y1, int Y2)
{
    if(!f_screen_boxOnClip(X, Y1, 1, Y2 - Y1 + 1)) {
        return;
    }

    Y1 = f_math_max(Y1, f__screen.clipY);
    Y2 = f_math_min(Y2, f__screen.clipY2 - 1);

    g_draw[f__color.blend].vline(X, Y1, Y2);
}

static void drawRectangle(int X, int Y, int Width, int Height)
{
    if(!f_screen_boxOnClip(X, Y, Width, Height)) {
        return;
    }

    if(f_screen_boxInsideClip(X, Y, Width, Height)) {
        g_draw[f__color.blend].rectangle[f__color.fillDraw]
            (X, Y, Width, Height);

        return;
    }

    const int x2 = f_math_min(X + Width, f__screen.clipX2);
    const int y2 = f_math_min(Y + Height, f__screen.clipY2);

    X = f_math_max(X, f__screen.clipX);
    Y = f_math_max(Y, f__screen.clipY);
    Width = f_math_min(Width, x2 - X);
    Height = f_math_min(Height, y2 - Y);

    g_draw[f__color.blend].rectangle[f__color.fillDraw](X, Y, Width, Height);
}

void f_platform_api__drawRectangleFilled(int X, int Y, int Width, int Height)
{
    drawRectangle(X, Y, Width, Height);
}

void f_platform_api__drawRectangleOutline(int X, int Y, int Width, int Height)
{
    drawRectangle(X, Y, Width, Height);
}

static void drawCircle(int X, int Y, int Radius)
{
    int boxX = X - Radius;
    int boxY = Y - Radius;
    int boxDim = 2 * Radius;

    if(f_screen_boxOnClip(boxX, boxY, boxDim, boxDim)) {
        g_draw[f__color.blend].circle
            [!f_screen_boxInsideClip(boxX, boxY, boxDim, boxDim)]
            [f__color.fillDraw]
                (X, Y, Radius);
    }
}

void f_platform_api__drawCircleOutline(int X, int Y, int Radius)
{
    drawCircle(X, Y, Radius);
}

void f_platform_api__drawCircleFilled(int X, int Y, int Radius)
{
    drawCircle(X, Y, Radius);
}
#endif // A_CONFIG_LIB_RENDER_SOFTWARE
