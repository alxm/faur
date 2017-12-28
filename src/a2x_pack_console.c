/*
    Copyright 2016, 2017 Alex Margarit

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

typedef struct {
    AConsoleOutType type;
    char* text;
} ALine;

bool g_enabled;
bool g_show;
static AList* g_lines;
static unsigned g_linesPerScreen;
static ASprite* g_titles[A_CONSOLE_MAX];
static AInputButton* g_toggle;

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

static void line_set(ALine* Line, AConsoleOutType Type, const char* Text)
{
    free(Line->text);

    Line->type = Type;
    Line->text = a_str_dup(Text);
}

static void inputCallback(void)
{
    if(a_button_getPressedOnce(g_toggle)) {
        g_show = !g_show;
    }
}

static void screenCallback(void)
{
    if(!g_enabled || !g_show) {
        return;
    }

    a_pixel_push();
    a_font_push();
    a_screen_clipReset();

    a_pixel_setBlend(A_PIXEL_BLEND_RGB75);
    a_pixel_setHex(0x1f0f0f);
    a_draw_fill();

    a_pixel_reset();
    a_font_reset();

    {
        a_font_setCoords(2, 2);
        a_font_setAlign(A_FONT_ALIGN_LEFT);

        a_font__setFont(A_FONT_FACE_BLUE); a_font_print("a");
        a_font__setFont(A_FONT_FACE_GREEN); a_font_print("2");
        a_font__setFont(A_FONT_FACE_YELLOW); a_font_print("x");

        a_font__setFont(A_FONT_FACE_WHITE);
        a_font_printf(" %s", A__MAKE_PLATFORM_NAME);
        a_font__setFont(A_FONT_FACE_LIGHT_GRAY);
        a_font_printf(" %s (%s)",
                      A__MAKE_CURRENT_GIT_BRANCH,
                      A__MAKE_COMPILE_TIME);
        a_font_newLine();

        a_font__setFont(A_FONT_FACE_WHITE);
        a_font_printf("%s %s",
                      a_settings_getString("app.title"),
                      a_settings_getString("app.version"));
        a_font__setFont(A_FONT_FACE_LIGHT_GRAY);
        a_font_printf(" by %s (%s)",
                      a_settings_getString("app.author"),
                      a_settings_getString("app.buildtime"));
        a_font_newLine();
    }

    {
        const int xOffset = 1 + g_titles[A_CONSOLE_MESSAGE]->w + 2;

        a_font_setCoords(xOffset, a_font_getY());
        a_font__setFont(A_FONT_FACE_LIGHT_GRAY);

        A_LIST_ITERATE(g_lines, ALine*, line) {
            a_sprite_blit(g_titles[line->type], 1, a_font_getY());
            a_font_print(line->text);
            a_font_newLine();
        }
    }

    {
        a_font_setAlign(A_FONT_ALIGN_RIGHT);
        a_font_setCoords(a__screen.width - 1, 2);

        a_font__setFont(A_FONT_FACE_YELLOW);
        a_font_printf("%u fps", a_fps_getFps());
        a_font_newLine();

        a_font__setFont(A_FONT_FACE_GREEN);
        a_font_printf("%u max", a_fps_getMaxFps());
        a_font_newLine();

        a_font__setFont(A_FONT_FACE_BLUE);
        a_font_printf("%u skip", a_fps_getFrameSkip());
    }

    a_pixel_pop();
    a_font_pop();
}

void a_console__init(void)
{
    g_enabled = true;
    g_show = false;
    g_lines = a_list_new();
    g_linesPerScreen = UINT_MAX;
}

void a_console__init2(void)
{
    if(!a_settings_getBool("video.on")) {
        return;
    }

    ASprite* graphics = a_sprite_newFromFile("/a2x/consoleTitles");
    ASpriteFrames* frames = a_spriteframes_new(graphics, 0, 0, 0);

    for(AConsoleOutType type = 0; type < A_CONSOLE_MAX; type++) {
        g_titles[type] = a_spriteframes_getNext(frames);
    }

    a_spriteframes_free(frames, false);
    a_sprite_free(graphics);

    g_linesPerScreen = (unsigned)
                        (a_screen_getHeight() / a_font_getLineHeight() - 2);

    // In case messages were logged between init and init2
    while(a_list_getSize(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }

    g_show = a_settings_getBool("console.on");
    g_toggle = a_button_new(a_settings_getString("console.button"));

    a_input__addCallback(inputCallback);
    a_screen__addOverlay(screenCallback);
}

void a_console__uninit(void)
{
    g_enabled = false;
    a_list_freeEx(g_lines, (AFree*)line_free);
}

void a_console__write(AConsoleOutType Type, const char* Text)
{
    if(!g_enabled) {
        return;
    }

    a_list_addLast(g_lines, line_new(Type, Text));

    if(a_list_getSize(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }
}

void a_console__overwrite(AConsoleOutType Type, const char* Text)
{
    if(!g_enabled) {
        return;
    }

    if(a_list_isEmpty(g_lines)) {
        a_list_addLast(g_lines, line_new(Type, Text));
    } else {
        line_set(a_list_getLast(g_lines), Type, Text);
    }
}

void a_console__setShow(bool DoShow)
{
    g_show = DoShow;
}
