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

void A__FUNC_NAME(a_draw__rectangle_noclip_)(int X, int Y, int Width, int Height)
{
    A__BLEND_SETUP;

    APixel* pixels = a_screen__pixels + Y * a_screen__width + X;
    const int screenw = a_screen__width;

    for(int i = Height; i--; pixels += screenw) {
        APixel* a__pass_dst = pixels;

        for(int j = Width; j--; a__pass_dst++) {
            A__PIXEL_DRAW(a__pass_dst);
        }
    }
}

void A__FUNC_NAME(a_draw__rectangle_clip_)(int X, int Y, int Width, int Height)
{
    if(!a_collide_boxOnScreen(X, Y, Width, Height)) {
        return;
    }

    const int x2 = a_math_min(X + Width, a_screen__width);
    const int y2 = a_math_min(Y + Height, a_screen__height);
    X = a_math_max(X, 0);
    Y = a_math_max(Y, 0);
    Width = a_math_min(Width, x2 - X);
    Height = a_math_min(Height, y2 - Y);

    A__FUNC_NAME(a_draw__rectangle_noclip_)(X, Y, Width, Height);
}

void A__FUNC_NAME(a_draw__line_noclip_)(int X1, int Y1, int X2, int Y2)
{
    A__BLEND_SETUP;

    const int xmin = a_math_min(X1, X2);
    const int xmax = a_math_max(X1, X2);
    const int ymin = a_math_min(Y1, Y2);
    const int ymax = a_math_max(Y1, Y2);

    if(X1 == X2) {
        g_draw_vline(X1, ymin, ymax);
    } else if(Y1 == Y2) {
        g_draw_hline(xmin, xmax, Y1);
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

        const int screenw = a_screen__width;
        APixel* a__pass_dst = a_screen__pixels + Y1 * screenw + X1;

        for(int i = denominator + 1; i--; ) {
            A__PIXEL_DRAW(a__pass_dst);

            numerator += numeratorinc;

            if(numerator >= denominator) {
                numerator -= denominator;
                a__pass_dst += xinc2;
                a__pass_dst += yinc2 * screenw;
            }

            a__pass_dst += xinc1;
            a__pass_dst += yinc1 * screenw;
        }
    }
}

void A__FUNC_NAME(a_draw__line_clip_)(int X1, int Y1, int X2, int Y2)
{
    if(!cohen_sutherland_clip(&X1, &Y1, &X2, &Y2)) {
        return;
    }

    A__FUNC_NAME(a_draw__line_noclip_)(X1, Y1, X2, Y2);
}

void A__FUNC_NAME(a_draw__hline_noclip_)(int X1, int X2, int Y)
{
    A__BLEND_SETUP;

    APixel* a__pass_dst = a_screen__pixels + Y * a_screen__width + X1;

    for(int i = X2 - X1; i--; a__pass_dst++) {
        A__PIXEL_DRAW(a__pass_dst);
    }
}

void A__FUNC_NAME(a_draw__hline_clip_)(int X1, int X2, int Y)
{
    if(X1 >= X2 || !a_collide_boxOnScreen(X1, Y, X2 - X1, 1)) {
        return;
    }

    X1 = a_math_max(X1, 0);
    X2 = a_math_min(X2, a_screen__width);

    A__FUNC_NAME(a_draw__hline_noclip_)(X1, X2, Y);
}

void A__FUNC_NAME(a_draw__vline_noclip_)(int X, int Y1, int Y2)
{
    A__BLEND_SETUP;

    const int screenw = a_screen__width;
    APixel* a__pass_dst = a_screen__pixels + Y1 * screenw + X;

    for(int i = Y2 - Y1; i--; a__pass_dst += screenw) {
        A__PIXEL_DRAW(a__pass_dst);
    }
}

void A__FUNC_NAME(a_draw__vline_clip_)(int X, int Y1, int Y2)
{
    if(Y1 >= Y2 || !a_collide_boxOnScreen(X, Y1, 1, Y2 - Y1)) {
        return;
    }

    Y1 = a_math_max(Y1, 0);
    Y2 = a_math_min(Y2, a_screen__height);

    A__FUNC_NAME(a_draw__vline_noclip_)(X, Y1, Y2);
}

void A__FUNC_NAME(a_draw__circle_noclip_)(int X, int Y, int Radius)
{
    A__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        a_pixel_put(X,     Y);
        a_pixel_put(X - 1, Y);
        a_pixel_put(X,     Y - 1);
        a_pixel_put(X - 1, Y - 1);
        return;
    }

    int x = Radius;
    int y = 0;
    int error = -Radius / 2;

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const size_t width = a_screen__width;
    APixel* const pixels = a_screen__pixels;

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
}

void A__FUNC_NAME(a_draw__circle_clip_)(int X, int Y, int Radius)
{
    if(!a_collide_boxOnScreen(X - Radius, Y - Radius, 2 * Radius, 2 * Radius)) {
        return;
    }

    if(a_collide_boxInsideScreen(X - Radius, Y - Radius, 2 * Radius, 2 * Radius)) {
        A__FUNC_NAME(a_draw__circle_noclip_)(X, Y, Radius);
        return;
    }

    A__BLEND_SETUP;

    // Using inclusive coords
    if(--Radius < 0) {
        return;
    }

    if(Radius == 0) {
        a_pixel_put(X,     Y);
        a_pixel_put(X - 1, Y);
        a_pixel_put(X,     Y - 1);
        a_pixel_put(X - 1, Y - 1);
        return;
    }

    const int q1X = X,     q1Y = Y - 1;
    const int q2X = X - 1, q2Y = Y - 1;
    const int q3X = X - 1, q3Y = Y;
    const int q4X = X,     q4Y = Y;

    const int width = a_screen_width();
    const int height = a_screen_height();
    APixel* const pixels = a_screen_pixels();

    APixel* oct1 = pixels + q1Y * width + q1X + Radius;
    APixel* oct2 = pixels + (q1Y - Radius) * width + q1X;
    APixel* oct3 = pixels + (q2Y - Radius) * width + q2X;
    APixel* oct4 = pixels + q2Y * width + q2X - Radius;
    APixel* oct5 = pixels + q3Y * width + q3X - Radius;
    APixel* oct6 = pixels + (q3Y + Radius) * width + q3X;
    APixel* oct7 = pixels + (q4Y + Radius) * width + q4X;
    APixel* oct8 = pixels + q4Y * width + q4X + Radius;

    int midx, midy;
    findMidpoint(X, Y, Radius, &midx, &midy);

    if(midx - midy == 2) {
        a_pixel_put(q1X + midx - 1, q1Y - midy - 1); // o1-o2
        a_pixel_put(q2X - midx + 1, q2Y - midy - 1); // o3-o4
        a_pixel_put(q3X - midx + 1, q3Y + midy + 1); // o5-o6
        a_pixel_put(q4X + midx - 1, q4Y + midy + 1); // o7-o8
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
        q1Y - y >= height, // YOffScreen
        q1X + x >= width, // XOffScreen
        q1Y - y >= 0, // PrimaryOnScreen
        q1X + x >= 0 // SecondaryOnScreen
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
        q1Y - y < 0, // YOffScreen
        q1X + x < 0, // XOffScreen
        q1X + x < width, // PrimaryOnScreen
        q1Y - y < height // SecondaryOnScreen
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
        q2Y - y < 0, // YOffScreen
        q2X - x >= width, // XOffScreen
        q2X - x >= 0, // PrimaryOnScreen
        q2Y - y < height // SecondaryOnScreen
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
        q2Y - y >= height, // YOffScreen
        q2X - x < 0, // XOffScreen
        q2Y - y >= 0, // PrimaryOnScreen
        q2X - x < width // SecondaryOnScreen
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
        q3Y + y < 0, // YOffScreen
        q3X - x < 0, // XOffScreen
        q3Y + y < height, // PrimaryOnScreen
        q3X - x < width // SecondaryOnScreen
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
        q3Y + y >= height, // YOffScreen
        q3X - x >= width, // XOffScreen
        q3X - x >= 0, // PrimaryOnScreen
        q3Y + y >= 0 // SecondaryOnScreen
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
        q4Y + y >= height, // YOffScreen
        q4X + x < 0, // XOffScreen
        q4X + x < width, // PrimaryOnScreen
        q4Y + y >= 0 // SecondaryOnScreen
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
        q4Y + y < 0, // YOffScreen
        q4X + x >= width, // XOffScreen
        q4Y + y < height, // PrimaryOnScreen
        q4X + x >= 0 // SecondaryOnScreen
        );
}
