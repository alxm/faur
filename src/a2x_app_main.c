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

#include "a2x_app_main.v.h"

static int numArgs;
static char** args;

int main(int argc, char** argv)
{
    numArgs = argc;
    args = argv;

    a_str__init();

    a_settings__defaults();
    a_setup();
    a_settings__freeze();

    a_out("a2x %s, compiled on %s", A__VERSION, A_COMPILE_TIME);

    a_out("Opening %s %s by %s, compiled %s",
        a2x_str("app.title"), a2x_str("app.version"),
        a2x_str("app.author"), a2x_str("app.compiled"));

    a_conf__init();
    a_sdl__init();
    a_hw__init();
    a_time__init();
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

    a_out("Opening A_MAIN");
    a_main();
    a_state__run();
    a_out("A_MAIN closed");

    a_state__uninit();
    a_sound__uninit();
    a_input__uninit();
    a_font__uninit();
    a_screen__uninit();
    a_sprite__uninit();
    a_fps__uninit();
    a_time__uninit();
    a_hw__uninit();
    a_sdl__uninit();

    a_out("Closing %s - see you next time.", a2x_str("app.title"));

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        if(a2x_bool("app.gp2xMenu")) {
            a_out("Calling gp2xmenu");
            a_str__uninit();

            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #else
        a_str__uninit();
    #endif

    return 0;
}

int a_main_numArgs(void)
{
    return numArgs;
}

char* a_main_getArg(unsigned int n)
{
    if(n >= numArgs) {
        a_out__error("a_main_getArg invalid arg: %u", n);
        return NULL;
    }

    return args[n];
}
