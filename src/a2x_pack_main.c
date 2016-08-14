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

#include "a2x_pack_main.v.h"

static int g_argsNum;
static char** g_args;

int main(int Argc, char** Argv)
{
    g_argsNum = Argc;
    g_args = Argv;

    a_console__init();
    a_settings__defaults();
    a_settings__setup();
    a_settings__freeze();

    a_out__message("a2x %s, built %s",
        A__MAKE_CURRENT_GIT_BRANCH,
        A__MAKE_COMPILE_TIME);

    a_out__message("Opening %s %s by %s, built %s",
        a_settings_getString("app.title"),
        a_settings_getString("app.version"),
        a_settings_getString("app.author"),
        a_settings_getString("app.buildtime"));

    a_file__init();
    a_dir__init();
    a_conf__init();
    a_sdl__init();
    a_hw__init();
    a_screen__init();
    a_screenshot__init();
    a_pixel__init();
    a_draw__init();
    a_blit__init();
    a_sprite__init();
    a_fps__init();
    a_input__init();
    a_sound__init();
    a_math__init();
    a_random__init();
    a_fix__init();
    a_font__init();
    a_state__init();
    a_fade__init();

    a_console__init2();

    a_out__message("Opening A_MAIN");
    a_main();
    a_state__run();
    a_out__message("A_MAIN closed");

    a_fade__uninit();
    a_state__uninit();
    a_sound__uninit();
    a_input__uninit();
    a_console__uninit();
    a_font__uninit();
    a_screenshot__uninit();
    a_screen__uninit();
    a_sprite__uninit();
    a_pixel__uninit();
    a_fps__uninit();
    a_hw__uninit();
    a_sdl__uninit();
    a_dir__uninit();
    a_file__uninit();

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        if(a_settings_getBool("app.gp2xMenu")) {
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #endif

    return 0;
}

int a_main_numArgs(void)
{
    return g_argsNum;
}

char* a_main_getArg(unsigned int ArgNum)
{
    if(ArgNum >= g_argsNum) {
        a_out__error("a_main_getArg invalid arg: %u", ArgNum);
        return NULL;
    }

    return g_args[ArgNum];
}
