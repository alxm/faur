/*
    Copyright 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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
} AConsoleState;

static AConsoleState g_state = A_CONSOLE__STATE_INVALID;
static AList* g_lines;
static unsigned g_linesPerScreen;
static ASprite* g_sources[A_OUT__SOURCE_NUM];
static ASprite* g_titles[A_OUT__TYPE_NUM];
static AButton* g_toggle;
static bool g_show = A_CONFIG_OUTPUT_CONSOLE_SHOW;

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
    ASpriteFrames* frames = a_spriteframes_newFromPng(
                                "/a2x/consoleTitles", 19, 7);

    for(int s = 0; s < A_OUT__SOURCE_NUM; s++) {
        g_sources[s] = a_spriteframes_getNext(frames);
    }

    for(int t = 0; t < A_OUT__TYPE_NUM; t++) {
        g_titles[t] = a_spriteframes_getNext(frames);
    }

    a_spriteframes_free(frames, false);

    g_linesPerScreen =
        (unsigned)((a_screen_sizeGetHeight() - 2) / a_font_lineHeightGet() - 2);

    // In case messages were logged between init and init2
    while(a_list_sizeGet(g_lines) > g_linesPerScreen) {
        line_free(a_list_pop(g_lines));
    }

    g_toggle = a_button_new();
    a_button_bind(g_toggle, A_KEY_F11);

    a_button_bindComboStart(g_toggle);
        a_button_bind(g_toggle, A_BUTTON_A);
        a_button_bind(g_toggle, A_BUTTON_B);
        a_button_bind(g_toggle, A_BUTTON_X);
        a_button_bind(g_toggle, A_BUTTON_Y);
        a_button_bind(g_toggle, A_BUTTON_L);
        a_button_bind(g_toggle, A_BUTTON_R);
    a_button_bindComboEnd(g_toggle);

    g_state = A_CONSOLE__STATE_FULL;
}

void a_console__uninit(void)
{
    g_state = A_CONSOLE__STATE_INVALID;

    a_list_freeEx(g_lines, (AFree*)line_free);

    for(int s = 0; s < A_OUT__SOURCE_NUM; s++) {
        a_sprite_free(g_sources[s]);
    }

    for(int t = 0; t < A_OUT__TYPE_NUM; t++) {
        a_sprite_free(g_titles[t]);
    }

    a_button_free(g_toggle);
}

void a_console__tick(void)
{
    if(a_button_pressGetOnce(g_toggle)) {
        g_show = !g_show;
    }
}

void a_console__draw(void)
{
    if(!g_show || g_state != A_CONSOLE__STATE_FULL) {
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

        a_font__fontSet(A_FONT__ID_LIGHT_GRAY);
        a_font_printf(" %s %.8s %s\n",
                      A_CONFIG_BUILD_ID,
                      A_CONFIG_BUILD_GIT_HASH,
                      A_CONFIG_BUILD_TIMESTAMP);

        a_font__fontSet(A_FONT__ID_WHITE);
        a_font_printf("%s %s by %s\n",
                      A_CONFIG_APP_NAME,
                      A_CONFIG_APP_VERSION,
                      A_CONFIG_APP_AUTHOR);
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
        a_font_printf("%u tick fps\n", a_fps_rateTickGet());
        a_font_printf("%u draw fps\n", a_fps_rateDrawGet());
        a_font_printf("%u draw max\n", a_fps_rateDrawGetMax());

        a_font__fontSet(A_FONT__ID_GREEN);
        a_font_printf("%dx%d:%d x%d\n",
                      a_screen_sizeGetWidth(),
                      a_screen_sizeGetHeight(),
                      A_CONFIG_SCREEN_BPP,
                      a_screen__zoomGet());
        a_font_printf("Vsync %s\n",
                      a_platform__screenVsyncGet() ? "on" : "off");

        #if A_CONFIG_LIB_SDL == 1
            a_font_print("SDL 1.2\n");
        #elif A_CONFIG_LIB_SDL == 2
            a_font_print("SDL 2.0\n");
        #endif

        #if A_CONFIG_LIB_RENDER_SOFTWARE
            #if A_CONFIG_LIB_SDL == 1
                #if A_CONFIG_SCREEN_ALLOCATE
                    a_font_print("S/W (Buffer)\n");
                #else
                    a_font_print("S/W (Raw)\n");
                #endif
            #elif A_CONFIG_LIB_SDL == 2
                a_font_print("S/W (Buffer)\n");
            #endif
        #elif A_CONFIG_LIB_RENDER_SDL
            a_font_print("SDL2 Renderer\n");
        #endif

        a_font__fontSet(A_FONT__ID_BLUE);
        a_font_printf("PID %d\n", getpid());
        a_font_printf("%u", a_fps_ticksGet());
    }

    a_pixel_pop();
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
