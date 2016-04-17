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

#define LINE_HEIGHT 10
static int linesPerScreen;

static struct {
    char* text;
    int font;
} titles[ConsoleMax] = {
    {"[ a2x Msg ] ", A_FONT_GREEN},
    {"[ a2x Wrn ] ", A_FONT_YELLOW},
    {"[ a2x Err ] ", A_FONT_RED},
    {"[ a2x Stt ] ", A_FONT_BLUE},
};

static Line* line_new(ConsoleOutType type, const char* text);
static void line_free(Line* line);

void a_console__init(void)
{
    enabled = true;
    lines = a_list_new();
    linesPerScreen = a2x_int("video.height") / LINE_HEIGHT;
}

void a_console__uninit(void)
{
    enabled = false;

    A_LIST_ITERATE(lines, Line, line) {
        line_free(line);
    }

    a_list_free(lines);
}

void a_console__write(ConsoleOutType type, const char* text)
{
    if(!enabled) {
        return;
    }

    a_list_addLast(lines, line_new(type, text));

    if(a_list_size(lines) > linesPerScreen) {
        line_free(a_list_pop(lines));
    }
}

void a_console__draw(void)
{
    if(!enabled || !show) {
        return;
    }

    a_pixel_setBlend(A_PIXEL_RGBA);
    a_pixel_setRGBA(0, 0, 0, 0.8 * A_FIX_ONE);
    a_draw_fill();

    int y = 1;
    a_font_setAlign(A_FONT_ALIGN_LEFT);

    A_LIST_ITERATE(lines, Line, line) {
        a_font_setCoords(1, y);
        a_font_setFace(titles[line->type].font);
        a_font_text(titles[line->type].text);

        a_font_setFace(A_FONT_WHITE);
        a_font_fixed(a_width - a_font_getX(), line->text);

        y += LINE_HEIGHT;
    }
}

void a_console__show(void)
{
    show = !show;
}

static Line* line_new(ConsoleOutType type, const char* text)
{
    Line* line = malloc(sizeof(Line));

    line->type = type;
    line->text = a_str_dup(text);

    return line;
}

static void line_free(Line* line)
{
    free(line->text);
    free(line);
}
