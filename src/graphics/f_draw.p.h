/*
    Copyright 2010, 2016 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_DRAW_P_H
#define F_INC_GRAPHICS_DRAW_P_H

#include "general/f_system_includes.h"

extern void f_draw_fill(void);
extern void f_draw_pixel(int X, int Y);
extern void f_draw_line(int X1, int Y1, int X2, int Y2);
extern void f_draw_hline(int X1, int X2, int Y);
extern void f_draw_vline(int X, int Y1, int Y2);
extern void f_draw_rectangle(int X, int Y, int Width, int Height);
extern void f_draw_circle(int X, int Y, int Radius);

#endif // F_INC_GRAPHICS_DRAW_P_H
