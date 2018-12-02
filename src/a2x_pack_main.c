/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_main.v.h"

#include "a2x_pack_block.v.h"
#include "a2x_pack_conf.v.h"
#include "a2x_pack_console.v.h"
#include "a2x_pack_ecs.v.h"
#include "a2x_pack_embed.v.h"
#include "a2x_pack_fade.v.h"
#include "a2x_pack_fix.v.h"
#include "a2x_pack_font.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_random.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_screenshot.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_timer.v.h"

static int g_argsNum;
static const char** g_args;

static void a__atexit(void)
{
    a_out__message("Running atexit callback");

    a_console__uninit();
    a_font__uninit();
    a_fade__uninit();
    a_ecs__uninit();
    a_state__uninit();
    a_sound__uninit();
    a_screenshot__uninit();
    a_fps__uninit();
    a_pixel__uninit();
    a_screen__uninit();
    a_input__uninit();
    a_timer__uninit();
    a_platform__uninit();
    a_block__uninit();
    a_embed__uninit();

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
        if(a_settings_boolGet(A_SETTING_SYSTEM_GP2X_MENU)) {
            a_settings__uninit();
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #endif

    a_settings__uninit();
}

int main(int Argc, char* Argv[])
{
    g_argsNum = Argc;
    g_args = (const char**)Argv;

    a_console__init();
    a_settings__init();

    a_out__message(
        "a2x %s, %s", A_BUILD__GIT_HASH, A_BUILD__COMPILE_TIME);

    a_out__message("%s %s by %s, %s - PID %d",
                   a_settings_stringGet(A_SETTING_APP_TITLE),
                   a_settings_stringGet(A_SETTING_APP_VERSION),
                   a_settings_stringGet(A_SETTING_APP_AUTHOR),
                   a_settings_stringGet(A_SETTING_APP_BUILDTIME),
                   getpid());

    a_embed__init();
    a_block__init();
    a_conf__init();
    a_platform__init();
    a_timer__init();
    a_input__init();
    a_screen__init();
    a_pixel__init();
    a_platform__init2();
    a_sprite__init();
    a_fps__init();
    a_screenshot__init();
    a_sound__init();
    a_random__init();
    a_fix__init();
    a_state__init();
    a_ecs__init();
    a_fade__init();
    a_font__init();
    a_console__init2();
    a_settings__init2();

    if(atexit(a__atexit)) {
        a_out__error("Cannot register atexit callback");
    }

    a_out__message("Calling A_MAIN");
    a_main();
    a_out__message("A_MAIN returned");

    a_state__run();

    return 0;
}

int a_main_argsGetNum(void)
{
    return g_argsNum;
}

const char* a_main_argsGet(int ArgNum)
{
    if(ArgNum >= g_argsNum) {
        a_out__error("a_main_argsGet: Invalid arg %u", ArgNum);
        return NULL;
    }

    return g_args[ArgNum];
}
