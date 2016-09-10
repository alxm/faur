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
