/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_sdl.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_SDL
#if F_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
#elif F_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
#endif

#define F__SDL_TIME !(F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO)

static uint32_t g_sdlFlags;

void f_platform_sdl__init(void)
{
    SDL_version cv, rv;
    SDL_VERSION(&cv);

    #if F_CONFIG_LIB_SDL == 1
        rv = *SDL_Linked_Version();
    #elif F_CONFIG_LIB_SDL == 2
        SDL_GetVersion(&rv);
    #endif

    f_out__info("Built with SDL %d.%d.%d", cv.major, cv.minor, cv.patch);
    f_out__info("Running on SDL %d.%d.%d", rv.major, rv.minor, rv.patch);

    g_sdlFlags = 0;

    #if F__SDL_TIME
        g_sdlFlags |= SDL_INIT_TIMER;
    #endif

    if(SDL_Init(g_sdlFlags) != 0) {
        F__FATAL("SDL_Init: %s", SDL_GetError());
    }

    f_platform_sdl_input__init();
    f_platform_sdl_video__init();

    #if F_CONFIG_FEATURE_SOUND
        f_platform_sdl_sound__init();
    #endif
}

void f_platform_sdl__uninit(void)
{
    f_platform_sdl_input__uninit();
    f_platform_sdl_video__uninit();

    #if F_CONFIG_FEATURE_SOUND
        f_platform_sdl_sound__uninit();
    #endif

    SDL_QuitSubSystem(g_sdlFlags);
    SDL_Quit();
}

#if F__SDL_TIME
uint32_t f_platform_api__timeMsGet(void)
{
    return SDL_GetTicks();
}

void f_platform_api__timeMsWait(uint32_t Ms)
{
    #if F_CONFIG_TRAIT_NOSLEEP
        return;
    #endif

    #if F_CONFIG_SYSTEM_GP2X // too inaccurate
        if(Ms < 10) {
            f_time_spinMs(Ms);
            return;
        }
    #endif

    SDL_Delay(Ms);
}
#endif
#endif // F_CONFIG_LIB_SDL
