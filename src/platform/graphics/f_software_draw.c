/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_RENDER_SOFTWARE
typedef void (*FCallDrawPixel)(int X, int Y);
typedef void (*FCallDrawHLine)(int X1, int X2, int Y);
typedef void (*FCallDrawVLine)(int X, int Y1, int Y2);
typedef void (*FCallDrawLine)(int X1, int Y1, int X2, int Y2);
typedef void (*FCallDrawRectangle)(int X, int Y, int Width, int Height);
typedef void (*FCallDrawCircle)(int X, int Y, int Radius);

static bool cohen_sutherland_clip(int* X1, int* Y1, int* X2, int* Y2)
{
    int x1 = *X1;
    int y1 = *Y1;
    int x2 = *X2;
    int y2 = *Y2;

    const int clipX1 = f__screen.clipStart.x;
    const int clipX2 = f__screen.clipEnd.x;
    const int clipY1 = f__screen.clipStart.y;
    const int clipY2 = f__screen.clipEnd.y;

    #define F__OUT_LEFT  1
    #define F__OUT_RIGHT 2
    #define F__OUT_TOP   4
    #define F__OUT_DOWN  8

    #define outcode(o, x, y)                     \
    {                                            \
        if(x < clipX1) o |= F__OUT_LEFT;         \
        else if(x >= clipX2) o |= F__OUT_RIGHT;  \
                                                 \
        if(y < clipY1) o |= F__OUT_TOP;          \
        else if(y >= clipY2) o |= F__OUT_DOWN;   \
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

            if(outcode & F__OUT_LEFT) {
                x = clipX1;
                y = solvey();
            } else if(outcode & F__OUT_RIGHT) {
                x = clipX2 - 1;
                y = solvey();
            } else if(outcode & F__OUT_TOP) {
                y = clipY1;
                x = solvex();
            } else { // outcode & F__OUT_DOWN
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
    FColorPixel* dst = Buffer;                                              \
                                                                            \
    if((PrimaryOnScreen) && (SecondaryOnScreen)) {                          \
        while(PrimaryCoord < SecondaryCoord && (PrimaryOnScreen)) {         \
            error += 2 * PrimaryCoord + 1;                                  \
            PrimaryCoord++;                                                 \
                                                                            \
            F__PIXEL_DRAW(dst);                                             \
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

#define F__FUNC_NAME(Name) F_GLUE4(f_draw__, Name, _, F__BLEND)
#define F__PIXEL_DRAW(Dst) F_GLUE2(f_color__draw_, F__BLEND)(Dst F__PIXEL_PARAMS)

#define F__BLEND solid
#define F__BLEND_SETUP const FColorPixel color = f__color.pixel;
#define F__PIXEL_PARAMS , color
#include "f_software_draw.inc.c"

#define F__BLEND alpha
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define F__PIXEL_PARAMS , &rgb, alpha
#include "f_software_draw.inc.c"

#if F__OPTIMIZE_ALPHA
    #define F__BLEND alpha25
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_draw.inc.c"

    #define F__BLEND alpha50
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_draw.inc.c"

    #define F__BLEND alpha75
    #define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
    #define F__PIXEL_PARAMS , &rgb
    #include "f_software_draw.inc.c"
#endif // F__OPTIMIZE_ALPHA

#define F__BLEND alphaMask
#define F__BLEND_SETUP \
    const FColorRgb rgb = f__color.rgb; \
    const int alpha = f__color.alpha; \
    if(alpha == 0) { \
        return; \
    }
#define F__PIXEL_PARAMS , &rgb, alpha, F_COLOR_ALPHA_MAX
#include "f_software_draw.inc.c"

#define F__BLEND inverse
#define F__BLEND_SETUP
#define F__PIXEL_PARAMS
#include "f_software_draw.inc.c"

#define F__BLEND mod
#define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
#define F__PIXEL_PARAMS , &rgb
#include "f_software_draw.inc.c"

#define F__BLEND add
#define F__BLEND_SETUP const FColorRgb rgb = f__color.rgb;
#define F__PIXEL_PARAMS , &rgb
#include "f_software_draw.inc.c"

#define F__INIT_BLEND(Index, Name)                           \
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
    FCallDrawPixel pixel;
    FCallDrawHLine hline;
    FCallDrawVLine vline;
    FCallDrawLine line;
    FCallDrawRectangle rectangle[2]; // [Fill]
    FCallDrawCircle circle[2][2]; // [Clip][Fill]
} g_draw[F_COLOR_BLEND_NUM] = {
    F__INIT_BLEND(F_COLOR_BLEND_SOLID, solid)
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA, alpha)
    #if F__OPTIMIZE_ALPHA
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_25, alpha25)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_50, alpha50)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_75, alpha75)
    #else
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_25, alpha)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_50, alpha)
        F__INIT_BLEND(F_COLOR_BLEND_ALPHA_75, alpha)
    #endif
    F__INIT_BLEND(F_COLOR_BLEND_ALPHA_MASK, alphaMask)
    F__INIT_BLEND(F_COLOR_BLEND_INVERSE, inverse)
    F__INIT_BLEND(F_COLOR_BLEND_MOD, mod)
    F__INIT_BLEND(F_COLOR_BLEND_ADD, add)
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

void f_platform_api__drawLineH(int X1, int X2, int Y)
{
    if(!f_screen_boxOnClip(X1, Y, X2 - X1 + 1, 1)) {
        return;
    }

    X1 = f_math_max(X1, f__screen.clipStart.x);
    X2 = f_math_min(X2, f__screen.clipEnd.x - 1);

    g_draw[f__color.blend].hline(X1, X2, Y);
}

void f_platform_api__drawLineV(int X, int Y1, int Y2)
{
    if(!f_screen_boxOnClip(X, Y1, 1, Y2 - Y1 + 1)) {
        return;
    }

    Y1 = f_math_max(Y1, f__screen.clipStart.y);
    Y2 = f_math_min(Y2, f__screen.clipEnd.y - 1);

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

    const int x2 = f_math_min(X + Width, f__screen.clipEnd.x);
    const int y2 = f_math_min(Y + Height, f__screen.clipEnd.y);

    X = f_math_max(X, f__screen.clipStart.x);
    Y = f_math_max(Y, f__screen.clipStart.y);
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
#endif // F_CONFIG_RENDER_SOFTWARE
