/*
    Copyright 2017 Alex Margarit

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

#include "a2x_pack_platform.v.h"

#include "a2x_pack_platform_gp2x.v.h"
#include "a2x_pack_platform_pandora.v.h"
#include "a2x_pack_platform_sdl.v.h"
#include "a2x_pack_platform_software_blit.v.h"
#include "a2x_pack_platform_software_draw.v.h"
#include "a2x_pack_platform_wiz.v.h"

void a_platform__init(void)
{
    #if A_BUILD_SYSTEM_PANDORA
        a_platform_pandora__init();
    #endif

    #if A_BUILD_LIB_SDL
        a_platform_sdl__init();
    #endif

    #if A_BUILD_SYSTEM_GP2X
        a_platform_gp2x__init();
    #elif A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
        a_platform_wiz__init();
    #endif
}

void a_platform__init2(void)
{
    #if A_BUILD_RENDER_SOFTWARE
        a_platform_software_blit__init();
        a_platform_software_draw__init();
    #endif
}

void a_platform__uninit(void)
{
    #if A_BUILD_SYSTEM_GP2X
        a_platform_gp2x__uninit();
    #elif A_BUILD_SYSTEM_PANDORA
        a_platform_pandora__uninit();
    #elif A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
        a_platform_wiz__uninit();
    #endif

    a_platform_sdl__uninit();
}
