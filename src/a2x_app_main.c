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

#define A_PACK_INIT(p)         \
({                             \
    a_out("Initing pack " #p); \
    a_##p##__init();           \
})

#define A_PACK_FREE(p)         \
({                             \
    a_out("Freeing pack " #p); \
    a_##p##__free();           \
})

int a_argsNum;
char** a_args;

int main(int argc, char** argv)
{
    a_argsNum = argc;
    a_args = argv;

    a_settings__defaults();
    a2x();
    a_settings__freeze();

    a_out("Opening %s %s by %s, compiled %s",
        a2x_str("app.title"), a2x_str("app.version"),
        a2x_str("app.author"),a2x_str("app.compiled"));

    A_PACK_INIT(conf);
    A_PACK_INIT(sdl);
    A_PACK_INIT(hw);
    A_PACK_INIT(time);
    A_PACK_INIT(screen);
    A_PACK_INIT(screenshot);
    A_PACK_INIT(pixel);
    A_PACK_INIT(draw);
    A_PACK_INIT(blit);
    A_PACK_INIT(sprite);
    A_PACK_INIT(fps);
    A_PACK_INIT(input);
    A_PACK_INIT(sound);
    A_PACK_INIT(math);
    A_PACK_INIT(fix);
    A_PACK_INIT(font);
    A_PACK_INIT(state);

    a_out("Opening Main()");
    Main();
    a_state__run();
    a_out("Main() closed");

    A_PACK_FREE(state);
    A_PACK_FREE(sound);
    A_PACK_FREE(input);
    A_PACK_FREE(font);
    A_PACK_FREE(screen);
    A_PACK_FREE(sprite);
    A_PACK_FREE(fps);
    A_PACK_FREE(time);
    A_PACK_FREE(hw);
    A_PACK_FREE(sdl);

    a_out("Closing %s - see you next time.", a2x_str("app.title"));

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        if(a2x_bool("app.gp2xMenu")) {
            a_out("Calling gp2xmenu");
            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #endif

    return 0;
}
