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

typedef struct ALine {
    AConsoleOutType type;
    char* text;
} ALine;

static AList* g_lines;
bool enabled;
bool show;

#define LINE_HEIGHT 10
static int g_linesPerScreen;

static struct {
    char* text;
    int font;
} g_titles[A_CONSOLE_MAX] = {
    {"[ a2x Msg ] ", A_FONT_GREEN},
    {"[ a2x Wrn ] ", A_FONT_YELLOW},
    {"[ a2x Err ] ", A_FONT_RED},
    {"[ a2x Stt ] ", A_FONT_BLUE},
};

static ALine* line_new(AConsoleOutType Type, const char* Text)
{
    ALine* line = a_mem_malloc(sizeof(ALine));

    line->type = Type;
    line->text = a_str_dup(Text);

    return line;
}

static void line_free(ALine* Line)
{
    free(Line->text);
    free(Line);
}

void a_console__init(void)
{
    enabled = true;
    g_lines = a_list_new();
    g_linesPerScreen = a_settings_getInt("video.height") / LINE_HEIGHT;
}

void a_console__uninit(void)
{
    enabled = false;

    A_LIST_ITERATE(g_lines, ALine*, line) {
        line_free(line);
    }

    a_list_free(g_lines);
}

void a_console__write(AConsoleOutType Type, const char* Text)
{
    if(!enabled) {
        return;
    }

    a_list_addLast(g_lines, line_new(Type, Text));

    if(a_list_size(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }
}

void a_console__draw(void)
{
    if(!enabled || !show) {
        return;
    }

    a_pixel_setBlend(A_PIXEL_RGBA);
    a_pixel_setRGBA(0, 0, 0, 0.8 * A_PIXEL_ALPHA_MAX);
    a_draw_fill();

    int y = 1;
    a_font_setAlign(A_FONT_ALIGN_LEFT);

    A_LIST_ITERATE(g_lines, ALine*, line) {
        a_font_setCoords(1, y);
        a_font_setFace(g_titles[line->type].font);
        a_font_text(g_titles[line->type].text);

        a_font_setFace(A_FONT_WHITE);
        a_font_fixed(a_screen__width - a_font_getX(), line->text);

        y += LINE_HEIGHT;
    }
}

void a_console__show(void)
{
    show = !show;
}
