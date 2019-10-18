/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_console.v.h"
#include <faur.v.h>

#if A_CONFIG_CONSOLE_ENABLED
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
static bool g_show = A_CONFIG_CONSOLE_SHOW;

static ALine* line_new(AOutSource Source, AOutType Type, const char* Text)
{
    ALine* line = f_mem_malloc(sizeof(ALine));

    line->source = Source;
    line->type = Type;
    line->text = f_str_dup(Text);

    return line;
}

static void line_free(ALine* Line)
{
    f_mem_free(Line->text);
    f_mem_free(Line);
}

static void f_console__init0(void)
{
    g_lines = f_list_new();
    g_linesPerScreen = UINT_MAX;

    g_state = A_CONSOLE__STATE_BASIC;
}

static void f_console__init1(void)
{
    g_linesPerScreen =
        (unsigned)((f_screen_sizeGetHeight() - 2) / f_font_lineHeightGet() - 2);

    // In case messages were logged between init and init2
    while(f_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(f_list_pop(g_lines));
    }

    #if A_CONFIG_LIB_PNG
        g_tags = f_sprite_newFromPng("/faur/consoleTitles", 0, 0, 19, 7);
    #endif

    g_toggle = f_button_new();
    f_button_bindKey(g_toggle, A_KEY_F11);
    f_button_bindCombo(g_toggle,
                       NULL,
                       A_BUTTON_A, A_BUTTON_B, A_BUTTON_X, A_BUTTON_Y,
                       A_BUTTON_L, A_BUTTON_R,
                       A_BUTTON_INVALID);

    g_state = A_CONSOLE__STATE_FULL;
}

static void f_console__uninit1(void)
{
    g_state = A_CONSOLE__STATE_INVALID;

    f_list_freeEx(g_lines, (AFree*)line_free);
    f_sprite_free(g_tags);
    f_button_free(g_toggle);
}

const APack f_pack__console = {
    "Console",
    {
        [0] = f_console__init0,
        [1] = f_console__init1,
    },
    {
        [1] = f_console__uninit1,
    },
};

void f_console__tick(void)
{
    if(f_button_pressGetOnce(g_toggle)) {
        g_show = !g_show;
    }
}

#if A_CONFIG_BUILD_DEBUG_ALLOC
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

    f_font_printf("%.3f %s (%s)\n", value, suffix, Tag);
}
#endif

void f_console__draw(void)
{
    if(!g_show || g_state != A_CONSOLE__STATE_FULL) {
        return;
    }

    f_color_push();
    f_font_push();
    f_screen_clipReset();

    f_color_blendSet(A_COLOR_BLEND_RGB75);
    f_color_baseSetHex(0x1f0f0f);
    f_draw_fill();

    f_color_reset();
    f_font_reset();

    {
        f_font_coordsSet(2, 2);
        f_font_alignSet(A_FONT_ALIGN_LEFT);

        f_font__fontSet(A_FONT__ID_BLUE); f_font_print("a");
        f_font__fontSet(A_FONT__ID_GREEN); f_font_print("2");
        f_font__fontSet(A_FONT__ID_YELLOW); f_font_print("x");

        f_font__fontSet(A_FONT__ID_LIGHT_GRAY);
        f_font_printf(" %s %.8s %s\n",
                      A_CONFIG_BUILD_UID,
                      A_CONFIG_BUILD_GIT_HASH,
                      A_CONFIG_BUILD_TIMESTAMP);

        f_font__fontSet(A_FONT__ID_WHITE);
        f_font_printf("%s %s by %s\n",
                      A_CONFIG_APP_NAME,
                      A_CONFIG_APP_VERSION_STRING,
                      A_CONFIG_APP_AUTHOR);
    }

    {
        int tagWidth = f_sprite_sizeGetWidth(g_tags);

        f_font_coordsSet(1 + tagWidth + 1 + tagWidth + 2, f_font_coordsGetY());
        f_font__fontSet(A_FONT__ID_LIGHT_GRAY);

        A_LIST_ITERATE(g_lines, ALine*, l) {
            f_sprite_blit(g_tags, (unsigned)l->source, 1, f_font_coordsGetY());
            f_sprite_blit(g_tags,
                          (unsigned)(A_OUT__SOURCE_NUM + l->type),
                          1 + tagWidth + 1,
                          f_font_coordsGetY());
            f_font_print(l->text);
            f_font_lineNew();
        }
    }

    {
        f_font_alignSet(A_FONT_ALIGN_RIGHT);
        f_font_coordsSet(f__screen.pixels->w - 1, 2);

        f_font__fontSet(A_FONT__ID_YELLOW);
        f_font_printf("%u tick fps\n", f_fps_rateTickGet());
        f_font_printf("%u draw fps\n", f_fps_rateDrawGet());
        f_font_printf("%u draw max\n", f_fps_rateDrawGetMax());

        f_font__fontSet(A_FONT__ID_GREEN);
        f_font_printf("%dx%d:%d x%d %c\n",
                      f_screen_sizeGetWidth(),
                      f_screen_sizeGetHeight(),
                      A_CONFIG_SCREEN_BPP,
                      f_platform_api__screenZoomGet(),
                      f_platform_api__screenFullscreenGet() ? 'F' : 'W');
        f_font_printf(
            "V-sync %s\n", f_platform_api__screenVsyncGet() ? "on" : "off");

        #if A_CONFIG_LIB_SDL == 1
            f_font_print("SDL 1.2\n");
        #elif A_CONFIG_LIB_SDL == 2
            f_font_print("SDL 2.0\n");
        #endif

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            #if A_CONFIG_SCREEN_ALLOCATE
                f_font_print("S/W Gfx (buffer)\n");
            #else
                f_font_print("S/W Gfx (raw)\n");
            #endif
        #elif A_CONFIG_LIB_RENDER_SDL
            f_font_print("SDL Gfx\n");
        #endif

        #if A_CONFIG_SOUND_ENABLED
            f_font_printf(
                "Sound %s\n", f_platform_api__soundMuteGet() ? "off" : "on");
        #endif

        f_font__fontSet(A_FONT__ID_BLUE);
        f_font_printf("PID %d\n", getpid());
        f_font_printf("%u ticks\n", f_fps_ticksGet());

        #if A_CONFIG_BUILD_DEBUG_ALLOC
            printBytes(f_mem__tally, "now");
            printBytes(f_mem__top, "top");
        #endif

        #if A_CONFIG_ECS_ENABLED
            f_font_printf("%u entities", f_ecs__listGetSum());
        #endif
    }

    f_color_pop();
    f_font_pop();
}

void f_console_showSet(bool Show)
{
    g_show = Show;
}

bool f_console__isInitialized(void)
{
    return g_state == A_CONSOLE__STATE_FULL;
}

void f_console__write(AOutSource Source, AOutType Type, const char* Text)
{
    if(g_state == A_CONSOLE__STATE_INVALID) {
        return;
    }

    f_list_addLast(g_lines, line_new(Source, Type, Text));

    if(f_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(f_list_pop(g_lines));
    }
}
#else // !A_CONFIG_CONSOLE_ENABLED
const APack f_pack__console;

void f_console_showSet(bool Show)
{
    A_UNUSED(Show);
}

void f_console__tick(void)
{
}

void f_console__draw(void)
{
}

bool f_console__isInitialized(void)
{
    return false;
}

void f_console__write(AOutSource Source, AOutType Type, const char* Text)
{
    A_UNUSED(Source);
    A_UNUSED(Type);
    A_UNUSED(Text);
}
#endif // !A_CONFIG_CONSOLE_ENABLED
