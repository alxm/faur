/*
    Copyright 2010 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

#include "a2x_pack_pixel.p.h"

typedef void (*ADrawRectangle)(int x1, int y1, int x2, int y2);
typedef void (*ADrawLine)(int x1, int y1, int x2, int y2);
typedef void (*ADrawHLine)(int x1, int x2, int y);
typedef void (*ADrawVLine)(int x, int y1, int y2);
typedef void (*ADrawCircle)(int x, int y, int r);

extern ADrawRectangle a_draw_rectangle;
extern ADrawLine a_draw_line;
extern ADrawHLine a_draw_hline;
extern ADrawVLine a_draw_vline;
extern ADrawCircle a_draw_circle;

#define a_draw_fill() a_draw_rectangle(0, 0, a_width, a_height)

extern void a_draw_rectangle_outline(int x1, int y1, int x2, int y2, int t);
