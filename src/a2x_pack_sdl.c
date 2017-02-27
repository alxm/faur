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

#include "a2x_pack_sdl.v.h"

#include <SDL.h>

static uint32_t g_sdlFlags;

void a_sdl__init(void)
{
    g_sdlFlags = 0;

    #if !(A_PLATFORM_WIZ || A_PLATFORM_CAANOO)
        g_sdlFlags |= SDL_INIT_TIMER;
    #endif

    if(SDL_Init(g_sdlFlags) != 0) {
        a_out__fatal("SDL_Init: %s", SDL_GetError());
    }

    if(a_settings_getBool("video.window")) {
        a_sdl_video__init();
    }

    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__init();
    }

    a_sdl_input__init();
}

void a_sdl__uninit(void)
{
    a_sdl_input__uninit();

    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__uninit();
    }

    if(a_settings_getBool("video.window")) {
        a_sdl_video__uninit();
    }

    SDL_QuitSubSystem(g_sdlFlags);
    SDL_Quit();
}

uint32_t a_sdl_time__getTicks(void)
{
    return SDL_GetTicks();
}

void a_sdl_time__delay(uint32_t Ms)
{
    SDL_Delay(Ms);
}
