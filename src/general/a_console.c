/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_console.v.h"
#include <a2x.v.h>

typedef struct {
    AOutSource source;
    AOutType type;
    char* text;
} ALine;

typedef enum {
    A_CONSOLE__STATE_INVALID = -1,
    A_CONSOLE__STATE_BASIC,
    A_CONSOLE__STATE_FULL,
} AConsoleState;

static AConsoleState g_state = A_CONSOLE__STATE_INVALID;
static AList* g_lines;
static unsigned g_linesPerScreen;
static ASprite* g_tags;
static AButton* g_toggle;
static bool g_show = A_CONFIG_OUTPUT_CONSOLE_SHOW;

static ALine* line_new(AOutSource Source, AOutType Type, const char* Text)
{
    ALine* line = a_mem_malloc(sizeof(ALine));

    line->source = Source;
    line->type = Type;
    line->text = a_str_dup(Text);

    return line;
}

static void line_free(ALine* Line)
{
    a_mem_free(Line->text);
    a_mem_free(Line);
}

static void a_console__init0(void)
{
    g_lines = a_list_new();
    g_linesPerScreen = UINT_MAX;

    g_state = A_CONSOLE__STATE_BASIC;
}

static void a_console__init1(void)
{
    g_linesPerScreen =
        (unsigned)((a_screen_sizeGetHeight() - 2) / a_font_lineHeightGet() - 2);

    // In case messages were logged between init and init2
    while(a_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }

    g_tags = a_sprite_newFromPng("/a2x/consoleTitles", 0, 0, 19, 7);
    g_toggle = a_button_new();
    a_button_bindKey(g_toggle, A_KEY_F11);
    a_button_bindCombo(g_toggle,
                       NULL,
                       A_BUTTON_A, A_BUTTON_B, A_BUTTON_X, A_BUTTON_Y,
                       A_BUTTON_L, A_BUTTON_R,
                       A_BUTTON_INVALID);

    g_state = A_CONSOLE__STATE_FULL;
}

static void a_console__uninit1(void)
{
    g_state = A_CONSOLE__STATE_INVALID;

    a_list_freeEx(g_lines, (AFree*)line_free);
    a_sprite_free(g_tags);
    a_button_free(g_toggle);
}

const APack a_pack__console = {
    "Console",
    {
        [0] = a_console__init0,
        [1] = a_console__init1,
    },
    {
        [1] = a_console__uninit1,
    },
};

void a_console__tick(void)
{
    if(a_button_pressGetOnce(g_toggle)) {
        g_show = !g_show;
    }
}

static void printBytes(size_t Bytes, const char* Tag)
{
    float value;
    const char* suffix;

    if(Bytes >= 1000 * 1000) {
        value = (float)Bytes / (1000 * 1000);
        suffix = "MB";
    } else if(Bytes >= 1000) {
        value = (float)Bytes / (1000);
        suffix = "KB";
    } else {
        value = (float)Bytes;
        suffix = "B";
    }

    a_font_printf("%.3f %s (%s)\n", value, suffix, Tag);
}

void a_console__draw(void)
{
    if(!g_show || g_state != A_CONSOLE__STATE_FULL) {
        return;
    }

    a_color_push();
    a_font_push();
    a_screen_clipReset();

    a_color_blendSet(A_COLOR_BLEND_RGB75);
    a_color_baseSetHex(0x1f0f0f);
    a_draw_fill();

    a_color_reset();
    a_font_reset();

    {
        a_font_coordsSet(2, 2);
        a_font_alignSet(A_FONT_ALIGN_LEFT);

        a_font__fontSet(A_FONT__ID_BLUE); a_font_print("a");
        a_font__fontSet(A_FONT__ID_GREEN); a_font_print("2");
        a_font__fontSet(A_FONT__ID_YELLOW); a_font_print("x");

        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);
        a_font_printf(" %s %.8s %s\n",
                      A_CONFIG_BUILD_UID,
                      A_CONFIG_BUILD_GIT_HASH,
                      A_CONFIG_BUILD_TIMESTAMP);

        a_font__fontSet(A_FONT__ID_WHITE);
        a_font_printf("%s %s by %s\n",
                      A_CONFIG_APP_NAME,
                      A_CONFIG_APP_VERSION_STRING,
                      A_CONFIG_APP_AUTHOR);
    }

    {
        int tagWidth = a_sprite_sizeGetWidth(g_tags);

        a_font_coordsSet(1 + tagWidth + 1 + tagWidth + 2, a_font_coordsGetY());
        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);

        A_LIST_ITERATE(g_lines, ALine*, l) {
            a_sprite_blit(g_tags, (unsigned)l->source, 1, a_font_coordsGetY());
            a_sprite_blit(g_tags,
                          (unsigned)(A_OUT__SOURCE_NUM + l->type),
                          1 + tagWidth + 1,
                          a_font_coordsGetY());
            a_font_print(l->text);
            a_font_lineNew();
        }
    }

    {
        a_font_alignSet(A_FONT_ALIGN_RIGHT);
        a_font_coordsSet(a__screen.pixels->w - 1, 2);

        a_font__fontSet(A_FONT__ID_YELLOW);
        a_font_printf("%u tick fps\n", a_fps_rateTickGet());
        a_font_printf("%u draw fps\n", a_fps_rateDrawGet());
        a_font_printf("%u draw max\n", a_fps_rateDrawGetMax());

        a_font__fontSet(A_FONT__ID_GREEN);
        a_font_printf("%dx%d:%d x%d %c\n",
                      a_screen_sizeGetWidth(),
                      a_screen_sizeGetHeight(),
                      A_CONFIG_SCREEN_BPP,
                      a_platform_api__screenZoomGet(),
                      a_platform_api__screenFullscreenGet() ? 'F' : 'W');
        a_font_printf(
            "V-sync %s\n", a_platform_api__screenVsyncGet() ? "on" : "off");

        #if A_CONFIG_LIB_SDL == 1
            a_font_print("SDL 1.2\n");
        #elif A_CONFIG_LIB_SDL == 2
            a_font_print("SDL 2.0\n");
        #endif

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            #if A_CONFIG_SCREEN_ALLOCATE
                a_font_print("S/W Gfx (buffer)\n");
            #else
                a_font_print("S/W Gfx (raw)\n");
            #endif
        #elif A_CONFIG_LIB_RENDER_SDL
            a_font_print("SDL Gfx\n");
        #endif

        a_font_printf(
            "Sound %s\n", a_platform_api__soundMuteGet() ? "off" : "on");

        a_font__fontSet(A_FONT__ID_BLUE);
        a_font_printf("PID %d\n", getpid());
        a_font_printf("%u ticks\n", a_fps_ticksGet());

        printBytes(a_mem__tally, "now");
        printBytes(a_mem__top, "top");

        a_font_printf("%u entities", a_ecs__listGetSum());
    }

    a_color_pop();
    a_font_pop();
}

void a_console_showSet(bool Show)
{
    g_show = Show;
}

bool a_console__isInitialized(void)
{
    return g_state == A_CONSOLE__STATE_FULL;
}

void a_console__write(AOutSource Source, AOutType Type, const char* Text)
{
    if(g_state == A_CONSOLE__STATE_INVALID) {
        return;
    }

    a_list_addLast(g_lines, line_new(Source, Type, Text));

    if(a_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }
}
