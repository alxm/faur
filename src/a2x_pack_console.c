/*
    Copyright 2016-2018 Alex Margarit

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

#include "a2x_pack_draw.v.h"
#include "a2x_pack_font.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sprite.v.h"
#include "a2x_pack_spriteframes.v.h"
#include "a2x_pack_str.v.h"

typedef struct {
    AOutSource source;
    AOutType type;
    char* text;
} ALine;

typedef enum {
    A_CONSOLE__STATE_INVALID = -1,
    A_CONSOLE__STATE_BASIC,
    A_CONSOLE__STATE_FULL,
    A_CONSOLE__STATE_VISIBLE,
} AConsoleState;

AConsoleState g_state;
static AList* g_lines;
static unsigned g_linesPerScreen;
static ASprite* g_sources[A_OUT__SOURCE_NUM];
static ASprite* g_titles[A_OUT__TYPE_NUM];
static AButton* g_toggle;

static void line_set(ALine* Line, AOutSource Source, AOutType Type, const char* Text)
{
    free(Line->text);

    Line->source = Source;
    Line->type = Type;
    Line->text = a_str_dup(Text);
}

static ALine* line_new(AOutSource Source, AOutType Type, const char* Text)
{
    ALine* line = a_mem_malloc(sizeof(ALine));

    line->text = NULL;
    line_set(line, Source, Type, Text);

    return line;
}

static void line_free(ALine* Line)
{
    free(Line->text);
    free(Line);
}

void a_console__init(void)
{
    g_lines = a_list_new();
    g_linesPerScreen = UINT_MAX;
    g_state = A_CONSOLE__STATE_BASIC;
}

void a_console__init2(void)
{
    ASpriteFrames* frames = a_spriteframes_newFromFile("/a2x/consoleTitles", 0);

    for(AOutSource s = 0; s < A_OUT__SOURCE_NUM; s++) {
        g_sources[s] = a_spriteframes_getNext(frames);
    }

    for(AOutType t = 0; t < A_OUT__TYPE_NUM; t++) {
        g_titles[t] = a_spriteframes_getNext(frames);
    }

    a_spriteframes_free(frames, false);

    g_linesPerScreen =
        (unsigned)(a_screen_heightGet() / a_font_lineHeightGet() - 2);

    // In case messages were logged between init and init2
    while(a_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }

    g_toggle = a_button_new();
    a_button_bind(g_toggle, "key.f11");

    a_button_bindComboStart(g_toggle);
        a_button_bind(g_toggle, "gamepad.b.l");
        a_button_bind(g_toggle, "gamepad.b.r");
        a_button_bind(g_toggle, "gamepad.b.a");
        a_button_bind(g_toggle, "gamepad.b.b");
        a_button_bind(g_toggle, "gamepad.b.x");
        a_button_bind(g_toggle, "gamepad.b.y");
    a_button_bindComboEnd(g_toggle);

    g_state = a_settings_getBool("console.on")
                ? A_CONSOLE__STATE_VISIBLE
                : A_CONSOLE__STATE_FULL;
}

void a_console__uninit(void)
{
    g_state = A_CONSOLE__STATE_INVALID;
    a_list_freeEx(g_lines, (AFree*)line_free);

    for(AOutSource s = 0; s < A_OUT__SOURCE_NUM; s++) {
        a_sprite_free(g_sources[s]);
    }

    for(AOutType t = 0; t < A_OUT__TYPE_NUM; t++) {
        a_sprite_free(g_titles[t]);
    }

    a_button_free(g_toggle);
}

void a_console__tick(void)
{
    if(a_button_pressGetOnce(g_toggle)) {
        a_console__showSet(g_state == A_CONSOLE__STATE_FULL);
    }
}

void a_console__draw(void)
{
    if(g_state != A_CONSOLE__STATE_VISIBLE) {
        return;
    }

    a_pixel_push();
    a_font_push();
    a_screen_clipReset();

    a_pixel_blendSet(A_PIXEL_BLEND_RGB75);
    a_pixel_colorSetHex(0x1f0f0f);
    a_draw_fill();

    a_pixel_reset();
    a_font_reset();

    {
        a_font_coordsSet(2, 2);
        a_font_alignSet(A_FONT_ALIGN_LEFT);

        a_font__fontSet(A_FONT__ID_BLUE); a_font_print("a");
        a_font__fontSet(A_FONT__ID_GREEN); a_font_print("2");
        a_font__fontSet(A_FONT__ID_YELLOW); a_font_print("x");

        a_font__fontSet(A_FONT__ID_WHITE);
        a_font_printf(" %s", A_BUILD__PLATFORM_NAME);
        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);
        a_font_printf(
            " %s (%s)", A_BUILD__CURRENT_GIT_BRANCH, A_BUILD__COMPILE_TIME);
        a_font_newLine();

        a_font__fontSet(A_FONT__ID_WHITE);
        a_font_printf("%s %s",
                      a_settings_getString("app.title"),
                      a_settings_getString("app.version"));
        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);
        a_font_printf(" by %s (%s)",
                      a_settings_getString("app.author"),
                      a_settings_getString("app.buildtime"));
        a_font_newLine();
    }

    {
        int tagWidth = g_sources[A_OUT__SOURCE_A2X]->w;

        a_font_coordsSet(1 + tagWidth + 1 + tagWidth + 2, a_font_coordsGetY());
        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);

        A_LIST_ITERATE(g_lines, ALine*, l) {
            a_sprite_blit(g_sources[l->source], 1, a_font_coordsGetY());
            a_sprite_blit(
                g_titles[l->type], 1 + tagWidth + 1, a_font_coordsGetY());
            a_font_print(l->text);
            a_font_newLine();
        }
    }

    {
        a_font_alignSet(A_FONT_ALIGN_RIGHT);
        a_font_coordsSet(a__screen.width - 1, 2);

        a_font__fontSet(A_FONT__ID_YELLOW);
        a_font_printf("%u tick fps", a_fps_tickRateGet());
        a_font_newLine();
        a_font_printf("%u draw fps", a_fps_drawRateGet());
        a_font_newLine();

        a_font__fontSet(A_FONT__ID_GREEN);
        a_font_printf("%u draw max", a_fps_drawRateGetMax());
        a_font_newLine();

        a_font__fontSet(A_FONT__ID_BLUE);
        a_font_printf(
            "Vsync is %s", a_settings_getBool("video.vsync") ? "on" : "off");
        a_font_newLine();
    }

    a_pixel_pop();
    a_font_pop();
}

bool a_console__isInitialized(void)
{
    return g_state >= A_CONSOLE__STATE_FULL;
}

void a_console__showSet(bool DoShow)
{
    if(g_state < A_CONSOLE__STATE_FULL) {
        return;
    }

    g_state = A_CONSOLE__STATE_FULL + DoShow;
}

void a_console__write(AOutSource Source, AOutType Type, const char* Text, bool Overwrite)
{
    if(g_state == A_CONSOLE__STATE_INVALID) {
        return;
    }

    if(Overwrite) {
        if(a_list_isEmpty(g_lines)) {
            a_list_addLast(g_lines, line_new(Source, Type, Text));
        } else {
            line_set(a_list_getLast(g_lines), Source, Type, Text);
        }
    } else {
        a_list_addLast(g_lines, line_new(Source, Type, Text));

        if(a_list_sizeGet(g_lines) > g_linesPerScreen) {
            line_free(a_list_pop(g_lines));
        }
    }
}
