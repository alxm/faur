/*
    Copyright 2017-2020 Alex Margarit <alex@alxm.org>
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

    #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        #if F_CONFIG_SYSTEM_GP2X
            f_platform_gp2x__init();
        #else
            f_platform_wiz__init();
        #endif

        #if F_CONFIG_SYSTEM_GP2X_MENU
            atexit(f_platform_gp2x__execMenu);
        #endif
    #endif

    #if F_CONFIG_SYSTEM_ODROID_GO
        f_platform_odroid_go__init();
    #endif

    f_platform_api__screenInit();

    #if F_CONFIG_RENDER_SOFTWARE
        f_out__info("Using S/W graphics");
        f_platform_software_blit__init();
    #elif F_CONFIG_RENDER_SDL2
        f_out__info("Using SDL2 graphics");
    #endif
}

static void f_platform__uninit(void)
{
    #if F_CONFIG_RENDER_SOFTWARE
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
    f_platform__init,
    f_platform__uninit,
};
