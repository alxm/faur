/*
    Copyright 2017-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_platform.v.h"
#include <a2x.v.h>

static void a_platform__init(void)
{
    #if A_CONFIG_SYSTEM_EMSCRIPTEN
        a_platform_emscripten__init();
    #endif

    #if A_CONFIG_SYSTEM_LINUX
        a_platform_linux__init();
    #endif

    #if A_CONFIG_SYSTEM_PANDORA
        a_platform_pandora__init();
    #endif

    #if A_CONFIG_LIB_SDL
        a_platform_sdl__init();
    #endif

    #if A_CONFIG_SYSTEM_GP2X
        a_platform_gp2x__init();
    #elif A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
        a_platform_wiz__init();
    #endif

    a_platform_api__screenInit();

    #if A_CONFIG_LIB_RENDER_SOFTWARE
        #if A_CONFIG_SCREEN_ALLOCATE
            a_out__info("Software graphics (virtual buffer)");
        #else
            a_out__info("Software graphics (raw buffer)");
        #endif

        a_platform_software_blit__init();
        a_platform_software_draw__init();
    #elif A_CONFIG_LIB_RENDER_SDL
        a_out__info("SDL graphics");
    #endif
}

static void a_platform__uninit(void)
{
    #if A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_software_blit__uninit();
    #endif

    a_platform_api__screenUninit();

    #if A_CONFIG_SYSTEM_GP2X
        a_platform_gp2x__uninit();
    #elif A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
        a_platform_wiz__uninit();
    #elif A_CONFIG_SYSTEM_PANDORA
        a_platform_pandora__uninit();
    #endif

    a_platform_sdl__uninit();
}

const APack a_pack__platform = {
    "Platform",
    {
        [0] = a_platform__init,
    },
    {
        [0] = a_platform__uninit,
    },
};
