/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_system_includes.h"

#if A_PLATFORM_LIB_SDL
#include "a2x_pack_platform_sdl.v.h"

#include <SDL.h>

#include "a2x_pack_out.v.h"
#include "a2x_pack_platform_sdl_input.v.h"
#include "a2x_pack_platform_sdl_sound.v.h"
#include "a2x_pack_platform_sdl_video.v.h"
#include "a2x_pack_settings.v.h"

static uint32_t g_sdlFlags;

void a_platform_sdl__init(void)
{
    g_sdlFlags = 0;

    #if A_PLATFORM_LIB_SDL_TIME
        g_sdlFlags |= SDL_INIT_TIMER;
    #endif

    if(SDL_Init(g_sdlFlags) != 0) {
        a_out__fatal("SDL_Init: %s", SDL_GetError());
    }

    a_platform_sdl_input__init();
    a_platform_sdl_video__init();

    if(a_settings_getBool("sound.on")) {
        a_platform_sdl_sound__init();
    }
}

void a_platform_sdl__uninit(void)
{
    a_platform_sdl_input__uninit();
    a_platform_sdl_video__uninit();

    if(a_settings_getBool("sound.on")) {
        a_platform_sdl_sound__uninit();
    }

    SDL_QuitSubSystem(g_sdlFlags);
    SDL_Quit();
}

#if A_PLATFORM_LIB_SDL_TIME
uint32_t a_platform__getMs(void)
{
    return SDL_GetTicks();
}
#endif
#endif // A_PLATFORM_LIB_SDL
