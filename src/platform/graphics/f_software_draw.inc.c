/*
    Copyright 2010, 2016-2017 Alex Margarit <alex@alxm.org>
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

#include <faur.h>

#ifdef F_INC_FAUR_V_H
static void F__FUNC_NAME(pixel)(int X, int Y)
{
    F__BLEND_SETUP;

    F__PIXEL_DRAW(f_screen__bufferGetFrom(X, Y));
}

static void F__FUNC_NAME(hline)(int X1, int X2, int Y)
{
    F__BLEND_SETUP;

    FColorPixel* dst = f_screen__bufferGetFrom(X1, Y);

    for(int i = X2 - X1 + 1; i--; dst++) {
        F__PIXEL_DRAW(dst);
    }
}

static void F__FUNC_NAME(vline)(int X, int Y1, int Y2)
{
    F__BLEND_SETUP;

    const int screenw = f__screen.pixels->size.x;
    FColorPixel* dst = f_screen__bufferGetFrom(X, Y1);

    for(int i = Y2 - Y1 + 1; i--; dst += screenw) {
        F__PIXEL_DRAW(dst);
    }
}

static void F__FUNC_NAME(line)(int X1, int Y1, int X2, int Y2)
{
    F__BLEND_SETUP;

    const int xmin = f_math_min(X1, X2);
    const int xmax = f_math_max(X1, X2);
    const int ymin = f_math_min(Y1, Y2);
    const int ymax = f_math_max(Y1, Y2);

    if(X1 == X2) {
        F__FUNC_NAME(vline)(X1, ymin, ymax);
    } else if(Y1 == Y2) {
        F__FUNC_NAME(hline)(xmin, xmax, Y1);
    } else {
        const int deltax = xmax - xmin;
        const int deltay = ymax - ymin;

        const int denominator = f_math_max(deltax, deltay);
        const int numeratorinc = f_math_min(deltax, deltay);
        int numerator = denominator / 2;

        const int xinct = (X1 <= X2) ? 1 : -1;
        const int yinct = (Y1 <= Y2) ? 1 : -1;

        const int xinc1 = (denominator == deltax) ? xinct : 0;
        const int yinc1 = (denominator == deltax) ? 0 : yinct;

        const int xinc2 = (denominator == deltax) ? 0 : xinct;
        const int yinc2 = (denominator == deltax) ? yinct : 0;

        const int screenw = f__screen.pixels->size.x;
        FColorPixel* dst1 = f_screen__bufferGetFrom(X1, Y1);
        FColorPixel* dst2 = f_screen__bufferGetFrom(X2, Y2);

        for(int i = (denominator + 1) / 2; i--; ) {
            F__PIXEL_DRAW(dst1);
            F__PIXEL_DRAW(dst2);

            numerator += numeratorinc;

            if(numerator >= denominator) {
                numerator -= denominator;

                dst1 += xinc2;
                dst1 += yinc2 * screenw;

                dst2 -= xinc2;
                dst2 -= yinc2 * screenw;
            }

            dst1 += xinc1;
            dst1 += yinc1 * screenw;

            dst2 -= xinc1;
            dst2 -= yinc1 * screenw;
        }

        if((denominator & 1) == 0) {
            F__PIXEL_DRAW(dst1);
        }
    }
}

static void F__FUNC_NAME(rectangle_nofill)(int X, int Y, int Width, int Height)
{
    F__FUNC_NAME(hline)(X, X + Width - 1, Y);

    if(Height > 1) {
        F__FUNC_NAME(hline)(X, X + Width - 1, Y + Height - 1);

        if(Height > 2) {
            F__FUNC_NAME(vline)(X, Y + 1, Y + Height - 2);

            if(Width > 1) {
                F__FUNC_NAME(vline)(X + Width - 1, Y + 1, Y + Height - 2);
            }
        }
    }
}

static void F__FUNC_NAME(rectangle_fill)(int X, int Y, int Width, int Height)
{
    F__BLEND_SETUP;

    FColorPixel* pixels = f_screen__bufferGetFrom(X, Y);
    const int screenw = f__screen.pixels->size.x;

    for(int i = Height; i--; pixels += screenw) {
        FColorPixel* dst = pixels;

        for(int j = Width; j--; dst++) {
            F__PIXEL_DRAW(dst);
        }
    }
}

static void F__FUNC_NAME(circle_noclip_nofill)(int X, int Y, int Radius)
{
    F__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        f_draw_pixel(X,     Y);
        f_draw_pixel(X - 1, Y);
        f_draw_pixel(X,     Y - 1);
        f_draw_pixel(X - 1, Y - 1);
        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const int width = f__screen.pixels->size.x;
    FColorPixel* const pixels = f_screen__bufferGetFrom(0, 0);

    FColorPixel* oct1 = pixels + q1Y * width + q1X + Radius;
    FColorPixel* oct2 = pixels + (q1Y - Radius) * width + q1X;
    FColorPixel* oct3 = pixels + (q2Y - Radius) * width + q2X;
    FColorPixel* oct4 = pixels + q2Y * width + q2X - Radius;
    FColorPixel* oct5 = pixels + q3Y * width + q3X - Radius;
    FColorPixel* oct6 = pixels + (q3Y + Radius) * width + q3X;
    FColorPixel* oct7 = pixels + (q4Y + Radius) * width + q4X;
    FColorPixel* oct8 = pixels + q4Y * width + q4X + Radius;

    while(x > y) {
        F__PIXEL_DRAW(oct1);
        F__PIXEL_DRAW(oct2);
        F__PIXEL_DRAW(oct3);
        F__PIXEL_DRAW(oct4);
        F__PIXEL_DRAW(oct5);
        F__PIXEL_DRAW(oct6);
        F__PIXEL_DRAW(oct7);
        F__PIXEL_DRAW(oct8);

        oct1 -= width;
        oct2 += 1;
        oct3 -= 1;
        oct4 -= width;
        oct5 += width;
        oct6 -= 1;
        oct7 += 1;
        oct8 += width;

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            oct1 -= 1;
            oct2 += width;
            oct3 += width;
            oct4 += 1;
            oct5 += 1;
            oct6 -= width;
            oct7 -= width;
            oct8 -= 1;
        }
    }

    if(x == y) {
        F__PIXEL_DRAW(oct1);
        F__PIXEL_DRAW(oct3);
        F__PIXEL_DRAW(oct5);
        F__PIXEL_DRAW(oct7);
    }

    #undef F__PIXEL_DRAW2
}

static void F__FUNC_NAME(circle_noclip_fill)(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        F__FUNC_NAME(rectangle_fill)(X - 1, Y - 1, 2, 2);

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    int x1 = X - 1 - x;
    int x2 = X + x;
    int y1 = Y - 1 - y;
    int y2 = Y + y;
    int x3 = X - 1 - y;
    int x4 = X + y;
    int y3 = Y - 1 - x;
    int y4 = Y + x;

    while(x > y) {
        F__FUNC_NAME(hline)(x1, x2, y1);
        F__FUNC_NAME(hline)(x1, x2, y2);

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            F__FUNC_NAME(hline)(x3, x4, y3);
            F__FUNC_NAME(hline)(x3, x4, y4);

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        F__FUNC_NAME(hline)(x3, x4, y3);
        F__FUNC_NAME(hline)(x3, x4, y4);
    }
}

static void F__FUNC_NAME(circle_clip_nofill)(int X, int Y, int Radius)
{
    F__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        f_draw_pixel(X,     Y);
        f_draw_pixel(X - 1, Y);
        f_draw_pixel(X,     Y - 1);
        f_draw_pixel(X - 1, Y - 1);
        return;
    }

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const int width = f__screen.pixels->size.x;
    FColorPixel* const pixels = f_screen__bufferGetFrom(0, 0);

    const int clipX1 = f__screen.clipStart.x;
    const int clipX2 = f__screen.clipEnd.x;
    const int clipY1 = f__screen.clipStart.y;
    const int clipY2 = f__screen.clipEnd.y;

    FColorPixel* oct1 = pixels + q1Y * width + q1X + Radius;
    FColorPixel* oct2 = pixels + (q1Y - Radius) * width + q1X;
    FColorPixel* oct3 = pixels + (q2Y - Radius) * width + q2X;
    FColorPixel* oct4 = pixels + q2Y * width + q2X - Radius;
    FColorPixel* oct5 = pixels + q3Y * width + q3X - Radius;
    FColorPixel* oct6 = pixels + (q3Y + Radius) * width + q3X;
    FColorPixel* oct7 = pixels + (q4Y + Radius) * width + q4X;
    FColorPixel* oct8 = pixels + q4Y * width + q4X + Radius;

    int midx, midy;
    findMidpoint(Radius, &midx, &midy);

    if(midx - midy == 2) {
        f_draw_pixel(q1X + midx - 1, q1Y - midy - 1); // o1-o2
        f_draw_pixel(q2X - midx + 1, q2Y - midy - 1); // o3-o4
        f_draw_pixel(q3X - midx + 1, q3Y + midy + 1); // o5-o6
        f_draw_pixel(q4X + midx - 1, q4Y + midy + 1); // o7-o8
    }

    // For octant1, reused for rest
    const int boundW = Radius - midx + 1;
    const int boundH = midy + 1;

    // Octant 1
    drawClippedOctant(
        q1X + midx, // BoundX
        q1Y - midy, // BoundY
        boundW, // BoundW
        boundH, // BoundH
        y, // PrimaryCoord
        x, // SecondaryCoord
        oct1, // Buffer
        -width, // PrimaryBufferInc
        -1, // SecondaryBufferInc
        q1Y - y >= clipY2, // YOffScreen
        q1X + x >= clipX2, // XOffScreen
        q1Y - y >= clipY1, // PrimaryOnScreen
        q1X + x >= clipX1 // SecondaryOnScreen
        );

    // Octant 2
    drawClippedOctant(
        q1X, // BoundX
        q1Y - Radius, // BoundY
        boundH, // BoundW
        boundW, // BoundH
        x, // PrimaryCoord
        y, // SecondaryCoord
        oct2, // Buffer
        +1, // PrimaryBufferInc
        +width, // SecondaryBufferInc
        q1Y - y < clipY1, // YOffScreen
        q1X + x < clipX1, // XOffScreen
        q1X + x < clipX2, // PrimaryOnScreen
        q1Y - y < clipY2 // SecondaryOnScreen
        );

    // Octant 3
    drawClippedOctant(
        q2X - midy, // BoundX
        q2Y - Radius, // BoundY
        boundH, // BoundW
        boundW, // BoundH
        x, // PrimaryCoord
        y, // SecondaryCoord
        oct3, // Buffer
        -1, // PrimaryBufferInc
        +width, // SecondaryBufferInc
        q2Y - y < clipY1, // YOffScreen
        q2X - x >= clipX2, // XOffScreen
        q2X - x >= clipX1, // PrimaryOnScreen
        q2Y - y < clipY2 // SecondaryOnScreen
        );

    // Octant 4
    drawClippedOctant(
        q2X - Radius, // BoundX
        q2Y - midy, // BoundY
        boundW, // BoundW
        boundH, // BoundH
        y, // PrimaryCoord
        x, // SecondaryCoord
        oct4, // Buffer
        -width, // PrimaryBufferInc
        +1, // SecondaryBufferInc
        q2Y - y >= clipY2, // YOffScreen
        q2X - x < clipX1, // XOffScreen
        q2Y - y >= clipY1, // PrimaryOnScreen
        q2X - x < clipX2 // SecondaryOnScreen
        );

    // Octant 5
    drawClippedOctant(
        q3X - Radius, // BoundX
        q3Y, // BoundY
        boundW, // BoundW
        boundH, // BoundH
        y, // PrimaryCoord
        x, // SecondaryCoord
        oct5, // Buffer
        +width, // PrimaryBufferInc
        +1, // SecondaryBufferInc
        q3Y + y < clipY1, // YOffScreen
        q3X - x < clipX1, // XOffScreen
        q3Y + y < clipY2, // PrimaryOnScreen
        q3X - x < clipX2 // SecondaryOnScreen
        );

    // Octant 6
    drawClippedOctant(
        q3X - midy, // BoundX
        q3Y + midx, // BoundY
        boundH, // BoundW
        boundW, // BoundH
        x, // PrimaryCoord
        y, // SecondaryCoord
        oct6, // Buffer
        -1, // PrimaryBufferInc
        -width, // SecondaryBufferInc
        q3Y + y >= clipY2, // YOffScreen
        q3X - x >= clipX2, // XOffScreen
        q3X - x >= clipX1, // PrimaryOnScreen
        q3Y + y >= clipY1 // SecondaryOnScreen
        );

    // Octant 7
    drawClippedOctant(
        q4X, // BoundX
        q4Y + midx, // BoundY
        boundH, // BoundW
        boundW, // BoundH
        x, // PrimaryCoord
        y, // SecondaryCoord
        oct7, // Buffer
        +1, // PrimaryBufferInc
        -width, // SecondaryBufferInc
        q4Y + y >= clipY2, // YOffScreen
        q4X + x < clipX1, // XOffScreen
        q4X + x < clipX2, // PrimaryOnScreen
        q4Y + y >= clipY1 // SecondaryOnScreen
        );

    // Octant 8
    drawClippedOctant(
        q4X + midx, // BoundX
        q4Y, // BoundY
        boundW, // BoundW
        boundH, // BoundH
        y, // PrimaryCoord
        x, // SecondaryCoord
        oct8, // Buffer
        +width, // PrimaryBufferInc
        -1, // SecondaryBufferInc
        q4Y + y < clipY1, // YOffScreen
        q4X + x >= clipX2, // XOffScreen
        q4Y + y < clipY2, // PrimaryOnScreen
        q4X + x >= clipX1 // SecondaryOnScreen
        );
}

static void F__FUNC_NAME(circle_clip_fill)(int X, int Y, int Radius)
{
    if(--Radius <= 0) {
        if(Radius == 0) {
            f_draw_rectangle(X - 1, Y - 1, 2, 2);
        }

        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    int x1 = X - 1 - x;
    int x2 = X + x;
    int y1 = Y - 1 - y;
    int y2 = Y + y;
    int x3 = X - 1 - y;
    int x4 = X + y;
    int y3 = Y - 1 - x;
    int y4 = Y + x;

    while(x > y) {
        f_draw_lineh(x1, x2, y1);
        f_draw_lineh(x1, x2, y2);

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            f_draw_lineh(x3, x4, y3);
            f_draw_lineh(x3, x4, y4);

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        f_draw_lineh(x3, x4, y3);
        f_draw_lineh(x3, x4, y4);
    }
}

#undef F__BLEND
#undef F__BLEND_SETUP
#undef F__PIXEL_PARAMS
#endif // F_INC_FAUR_V_H
