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

#include "a2x_app_main.h"
#include "a2x_app_settings.h"

#include "a2x_pack_conf.h"
#include "a2x_pack_fix.h"
#include "a2x_pack_font.h"
#include "a2x_pack_fps.h"
#include "a2x_pack_hw.h"
#include "a2x_pack_input.h"
#include "a2x_pack_math.h"
#include "a2x_pack_mem.h"
#include "a2x_pack_screen.h"
#include "a2x_pack_sdl.h"
#include "a2x_pack_sound.h"
#include "a2x_pack_sprite.h"
#include "a2x_pack_time.h"

#define A_PACK_SET(p)     \
({                        \
    a_out("Setting " #p); \
    a__##p##_set();       \
    a_out(#p " set");     \
})

#define A_PACK_FREE(p)    \
({                        \
    a_out("Freeing " #p); \
    a__##p##_free();      \
    a_out(#p " freed");   \
})

int a_argsNum;
char** a_args;

int main(int argc, char** argv)
{
    a_argsNum = argc;
    a_args = argv;

    a2x_defaults();
    a2x();

    a_out("Opening %s %s by %s, compiled %s", a2xSet.title, a2xSet.version, a2xSet.author, a2xSet.compiled);

    A_PACK_SET(conf);
    A_PACK_SET(sdl);
    A_PACK_SET(hw);
    A_PACK_SET(time);
    A_PACK_SET(screen);
    A_PACK_SET(sprite);
    A_PACK_SET(fps);
    A_PACK_SET(input);
    A_PACK_SET(sound);
    A_PACK_SET(math);
    A_PACK_SET(fix);
    A_PACK_SET(font);
    A_PACK_SET(mem);

    a_out("Opening Main()");
    Main();
    a__state_run();
    a_out("Main() closed");

    A_PACK_FREE(mem);
    A_PACK_FREE(sound);
    A_PACK_FREE(input);
    A_PACK_FREE(font);
    A_PACK_FREE(screen);
    A_PACK_FREE(sprite);
    A_PACK_FREE(time);
    A_PACK_FREE(hw);
    A_PACK_FREE(sdl);

    a_out("Closing %s - see you next time.", a2xSet.title);

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a2xSet.gp2xMenu) {
            a_out("Calling gp2xmenu");

            chdir("/usr/gp2x");
            execl("gp2xmenu", "gp2xmenu", NULL);
        }
    #endif

    return 0;
}
