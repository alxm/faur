/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

void a_draw_fill(void)
{
    a_pixel_push();
    a_pixel_setFillDraw(true);

    a_draw_rectangle(a__screen.clipX,
                     a__screen.clipY,
                     a__screen.clipWidth,
                     a__screen.clipHeight);

    a_pixel_pop();
}

void a_draw_pixel(int X, int Y)
{
    a_platform__drawPixel(X, Y);
}

void a_draw_line(int X1, int Y1, int X2, int Y2)
{
    a_platform__drawLine(X1, Y1, X2, Y2);
}

void a_draw_hline(int X1, int X2, int Y)
{
    a_platform__drawHLine(a_math_min(X1, X2), a_math_max(X1, X2), Y);
}

void a_draw_vline(int X, int Y1, int Y2)
{
    a_platform__drawVLine(X, a_math_min(Y1, Y2), a_math_max(Y1, Y2));
}

void a_draw_rectangle(int X, int Y, int Width, int Height)
{
    if(a_pixel__state.fillDraw) {
        a_platform__drawRectangleFilled(X, Y, Width, Height);
    } else {
        a_platform__drawRectangleOutline(X, Y, Width, Height);
    }
}

void a_draw_circle(int X, int Y, int Radius)
{
    if(a_pixel__state.fillDraw) {
        a_platform__drawCircleFilled(X, Y, Radius);
    } else {
        a_platform__drawCircleOutline(X, Y, Radius);
    }
}
