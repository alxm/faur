/*
    Copyright 2016 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_console.v.h"

typedef struct Line {
    ConsoleOutType type;
    char* text;
} Line;

static List* lines;
bool enabled;
bool show;

void a_console__init(void)
{
    enabled = true;
    lines = a_list_new();
}

void a_console__uninit(void)
{
    enabled = false;

    A_LIST_ITERATE(lines, Line, line) {
        free(line);
    }

    a_list_free(lines);
}

void a_console__write(ConsoleOutType type, char* str)
{
    if(!enabled) {
        return;
    }

    Line* line = malloc(sizeof(Line));

    line->type = type;
    line->text = a_str_dup(str);

    a_list_addLast(lines, line);
}

void a_console__draw(void)
{
    if(!enabled || !show) {
        return;
    }

    a_pixel_setBlend(A_PIXEL_RGBA);
    a_pixel_setRGBA(0, 0, 0, A_FIX_ONE / 2);
    a_draw_fill();
}

void a_console__show(void)
{
    show = !show;
}
