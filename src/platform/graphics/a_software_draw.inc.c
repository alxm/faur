/*
    Copyright 2010, 2016-2017 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include <faur.v.h>

#if A__COMPILE_INC
static void A__FUNC_NAME(pixel)(int X, int Y)
{
    A__BLEND_SETUP;

    A__PIXEL_DRAW(a_screen__bufferGetFrom(X, Y));
}

static void A__FUNC_NAME(hline)(int X1, int X2, int Y)
{
    A__BLEND_SETUP;

    APixel* dst = a_screen__bufferGetFrom(X1, Y);

    for(int i = X2 - X1 + 1; i--; dst++) {
        A__PIXEL_DRAW(dst);
    }
}

static void A__FUNC_NAME(vline)(int X, int Y1, int Y2)
{
    A__BLEND_SETUP;

    const int screenw = a__screen.pixels->w;
    APixel* dst = a_screen__bufferGetFrom(X, Y1);

    for(int i = Y2 - Y1 + 1; i--; dst += screenw) {
        A__PIXEL_DRAW(dst);
    }
}

static void A__FUNC_NAME(line)(int X1, int Y1, int X2, int Y2)
{
    A__BLEND_SETUP;

    const int xmin = a_math_min(X1, X2);
    const int xmax = a_math_max(X1, X2);
    const int ymin = a_math_min(Y1, Y2);
    const int ymax = a_math_max(Y1, Y2);

    if(X1 == X2) {
        A__FUNC_NAME(vline)(X1, ymin, ymax);
    } else if(Y1 == Y2) {
        A__FUNC_NAME(hline)(xmin, xmax, Y1);
    } else {
        const int deltax = xmax - xmin;
        const int deltay = ymax - ymin;

        const int denominator = a_math_max(deltax, deltay);
        const int numeratorinc = a_math_min(deltax, deltay);
        int numerator = denominator / 2;

        const int xinct = (X1 <= X2) ? 1 : -1;
        const int yinct = (Y1 <= Y2) ? 1 : -1;

        const int xinc1 = (denominator == deltax) ? xinct : 0;
        const int yinc1 = (denominator == deltax) ? 0 : yinct;

        const int xinc2 = (denominator == deltax) ? 0 : xinct;
        const int yinc2 = (denominator == deltax) ? yinct : 0;

        const int screenw = a__screen.pixels->w;
        APixel* dst1 = a_screen__bufferGetFrom(X1, Y1);
        APixel* dst2 = a_screen__bufferGetFrom(X2, Y2);

        for(int i = (denominator + 1) / 2; i--; ) {
            A__PIXEL_DRAW(dst1);
            A__PIXEL_DRAW(dst2);

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
            A__PIXEL_DRAW(dst1);
        }
    }
}

static void A__FUNC_NAME(rectangle_nofill)(int X, int Y, int Width, int Height)
{
    A__FUNC_NAME(hline)(X, X + Width - 1, Y);

    if(Height > 1) {
        A__FUNC_NAME(hline)(X, X + Width - 1, Y + Height - 1);

        if(Height > 2) {
            A__FUNC_NAME(vline)(X, Y + 1, Y + Height - 2);

            if(Width > 1) {
                A__FUNC_NAME(vline)(X + Width - 1, Y + 1, Y + Height - 2);
            }
        }
    }
}

static void A__FUNC_NAME(rectangle_fill)(int X, int Y, int Width, int Height)
{
    A__BLEND_SETUP;

    APixel* pixels = a_screen__bufferGetFrom(X, Y);
    const int screenw = a__screen.pixels->w;

    for(int i = Height; i--; pixels += screenw) {
        APixel* dst = pixels;

        for(int j = Width; j--; dst++) {
            A__PIXEL_DRAW(dst);
        }
    }
}

static void A__FUNC_NAME(circle_noclip_nofill)(int X, int Y, int Radius)
{
    A__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        a_draw_pixel(X,     Y);
        a_draw_pixel(X - 1, Y);
        a_draw_pixel(X,     Y - 1);
        a_draw_pixel(X - 1, Y - 1);
        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const int width = a__screen.pixels->w;
    APixel* const pixels = a_screen__bufferGetFrom(0, 0);

    APixel* oct1 = pixels + q1Y * width + q1X + Radius;
    APixel* oct2 = pixels + (q1Y - Radius) * width + q1X;
    APixel* oct3 = pixels + (q2Y - Radius) * width + q2X;
    APixel* oct4 = pixels + q2Y * width + q2X - Radius;
    APixel* oct5 = pixels + q3Y * width + q3X - Radius;
    APixel* oct6 = pixels + (q3Y + Radius) * width + q3X;
    APixel* oct7 = pixels + (q4Y + Radius) * width + q4X;
    APixel* oct8 = pixels + q4Y * width + q4X + Radius;

    while(x > y) {
        A__PIXEL_DRAW(oct1);
        A__PIXEL_DRAW(oct2);
        A__PIXEL_DRAW(oct3);
        A__PIXEL_DRAW(oct4);
        A__PIXEL_DRAW(oct5);
        A__PIXEL_DRAW(oct6);
        A__PIXEL_DRAW(oct7);
        A__PIXEL_DRAW(oct8);

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
        A__PIXEL_DRAW(oct1);
        A__PIXEL_DRAW(oct3);
        A__PIXEL_DRAW(oct5);
        A__PIXEL_DRAW(oct7);
    }

    #undef A__PIXEL_DRAW2
}

static void A__FUNC_NAME(circle_noclip_fill)(int X, int Y, int Radius)
{
    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        A__FUNC_NAME(rectangle_fill)(X - 1, Y - 1, 2, 2);

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
        A__FUNC_NAME(hline)(x1, x2, y1);
        A__FUNC_NAME(hline)(x1, x2, y2);

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            A__FUNC_NAME(hline)(x3, x4, y3);
            A__FUNC_NAME(hline)(x3, x4, y4);

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        A__FUNC_NAME(hline)(x3, x4, y3);
        A__FUNC_NAME(hline)(x3, x4, y4);
    }
}

static void A__FUNC_NAME(circle_clip_nofill)(int X, int Y, int Radius)
{
    A__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        a_draw_pixel(X,     Y);
        a_draw_pixel(X - 1, Y);
        a_draw_pixel(X,     Y - 1);
        a_draw_pixel(X - 1, Y - 1);
        return;
    }

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const int width = a__screen.pixels->w;
    APixel* const pixels = a_screen__bufferGetFrom(0, 0);

    const int clipX1 = a__screen.clipX;
    const int clipX2 = a__screen.clipX2;
    const int clipY1 = a__screen.clipY;
    const int clipY2 = a__screen.clipY2;

    APixel* oct1 = pixels + q1Y * width + q1X + Radius;
    APixel* oct2 = pixels + (q1Y - Radius) * width + q1X;
    APixel* oct3 = pixels + (q2Y - Radius) * width + q2X;
    APixel* oct4 = pixels + q2Y * width + q2X - Radius;
    APixel* oct5 = pixels + q3Y * width + q3X - Radius;
    APixel* oct6 = pixels + (q3Y + Radius) * width + q3X;
    APixel* oct7 = pixels + (q4Y + Radius) * width + q4X;
    APixel* oct8 = pixels + q4Y * width + q4X + Radius;

    int midx, midy;
    findMidpoint(Radius, &midx, &midy);

    if(midx - midy == 2) {
        a_draw_pixel(q1X + midx - 1, q1Y - midy - 1); // o1-o2
        a_draw_pixel(q2X - midx + 1, q2Y - midy - 1); // o3-o4
        a_draw_pixel(q3X - midx + 1, q3Y + midy + 1); // o5-o6
        a_draw_pixel(q4X + midx - 1, q4Y + midy + 1); // o7-o8
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

static void A__FUNC_NAME(circle_clip_fill)(int X, int Y, int Radius)
{
    if(--Radius <= 0) {
        if(Radius == 0) {
            a_draw_rectangle(X - 1, Y - 1, 2, 2);
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
        a_draw_hline(x1, x2, y1);
        a_draw_hline(x1, x2, y2);

        error += 2 * y + 1; // (y+1)^2 = y^2 + 2y + 1
        y++;

        y1--;
        y2++;
        x3--;
        x4++;

        if(error > 0) { // check if x^2 + y^2 > r^2
            a_draw_hline(x3, x4, y3);
            a_draw_hline(x3, x4, y4);

            error += -2 * x + 1; // (x-1)^2 = x^2 - 2x + 1
            x--;

            x1++;
            x2--;
            y3++;
            y4--;
        }
    }

    if(x == y) {
        a_draw_hline(x3, x4, y3);
        a_draw_hline(x3, x4, y4);
    }
}

#undef A__BLEND
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS
#endif // A__COMPILE_INC
