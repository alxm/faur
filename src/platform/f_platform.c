/*
    Copyright 2017-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_platform.v.h"
#include <faur.v.h>

static void f_platform__init(void)
{
    #if F_CONFIG_SYSTEM_EMSCRIPTEN
        f_platform_emscripten__init();
    #endif

    #if F_CONFIG_SYSTEM_LINUX
        f_platform_linux__init();
    #endif

    #if F_CONFIG_SYSTEM_PANDORA
        f_platform_pandora__init();
    #endif

    #if F_CONFIG_LIB_SDL
        f_platform_sdl__init();
    #endif

    #if F_CONFIG_SYSTEM_GP2X
        f_platform_gp2x__init();
    #elif F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        f_platform_wiz__init();
    #endif

    f_platform_api__screenInit();

    #if F_CONFIG_LIB_RENDER_SOFTWARE
        #if F_CONFIG_SCREEN_ALLOCATE
            f_out__info("Software graphics (virtual buffer)");
        #else
            f_out__info("Software graphics (raw buffer)");
        #endif

        f_platform_software_blit__init();
    #elif F_CONFIG_LIB_RENDER_SDL
        f_out__info("SDL graphics");
    #endif
}

static void f_platform__uninit(void)
{
    #if F_CONFIG_LIB_RENDER_SOFTWARE
        f_platform_software_blit__uninit();
    #endif

    f_platform_api__screenUninit();

    #if F_CONFIG_SYSTEM_GP2X
        f_platform_gp2x__uninit();
    #elif F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        f_platform_wiz__uninit();
    #elif F_CONFIG_SYSTEM_PANDORA
        f_platform_pandora__uninit();
    #endif

    #if F_CONFIG_LIB_SDL
        f_platform_sdl__uninit();
    #endif
}

const FPack f_pack__platform = {
    "Platform",
    {
        [0] = f_platform__init,
    },
    {
        [0] = f_platform__uninit,
    },
};
