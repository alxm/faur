/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_draw.v.h"
#include <faur.v.h>

void f_draw_fill(void)
{
    f_platform_api__drawRectangleFilled(f__screen.clipStart.x,
                                        f__screen.clipStart.y,
                                        f__screen.clipSize.x,
                                        f__screen.clipSize.y);
}

void f_draw_pixel(int X, int Y)
{
    f_platform_api__drawPixel(X, Y);
}

void f_draw_line(int X1, int Y1, int X2, int Y2)
{
    f_platform_api__drawLine(X1, Y1, X2, Y2);
}

void f_draw_lineh(int X1, int X2, int Y)
{
    f_platform_api__drawLineH(f_math_min(X1, X2), f_math_max(X1, X2), Y);
}

void f_draw_linev(int X, int Y1, int Y2)
{
    f_platform_api__drawLineV(X, f_math_min(Y1, Y2), f_math_max(Y1, Y2));
}

void f_draw_rectangle(int X, int Y, int Width, int Height)
{
    F__CHECK(Width >= 0);
    F__CHECK(Height >= 0);

    if(f__color.fillDraw) {
        f_platform_api__drawRectangleFilled(X, Y, Width, Height);
    } else {
        f_platform_api__drawRectangleOutline(X, Y, Width, Height);
    }
}

void f_draw_circle(int X, int Y, int Radius)
{
    F__CHECK(Radius >= 0);

    if(f__color.fillDraw) {
        f_platform_api__drawCircleFilled(X, Y, Radius);
    } else {
        f_platform_api__drawCircleOutline(X, Y, Radius);
    }
}
