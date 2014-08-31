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

#include "a2x_pack_sdl.v.h"

static SDL_Surface* screen = NULL;
static bool screen_locked = false;

void a_sdl__init(void)
{
    int ret = 0;
    uint32_t sdlFlags = 0;

    if(a2x_bool("video.window")) {
        sdlFlags |= SDL_INIT_VIDEO;
    }

    if(a2x_bool("sound.on")) {
        sdlFlags |= SDL_INIT_AUDIO;
    }

    sdlFlags |= SDL_INIT_JOYSTICK;

    #if !(A_PLATFORM_WIZ || A_PLATFORM_CAANOO)
        sdlFlags |= SDL_INIT_TIMER;
    #endif

    ret = SDL_Init(sdlFlags);

    if(ret != 0) {
        a_error("SDL: %s", SDL_GetError());
        exit(1);
    }
}

void a_sdl__uninit(void)
{
    SDL_Quit();
}

bool a_sdl__screen_set(void)
{
    static bool first_time = true;

    int bpp = 0;
    int scale = a2x_int("video.scale");
    uint32_t videoFlags = SDL_SWSURFACE;

    if(a2x_bool("video.fullscreen")) {
        videoFlags |= SDL_FULLSCREEN;
    }

    bpp = SDL_VideoModeOK(a_width * scale, a_height * scale, A_BPP, videoFlags);

    if(bpp == 0) {
        if(first_time) {
            a_error("SDL: %dx%d video not available", a_width * scale, a_height * scale);
            exit(1);
        } else {
            a_warning("SDL: %dx%d video not available", a_width * scale, a_height * scale);
            return false;
        }
    }

    first_time = false;
    screen = SDL_SetVideoMode(a_width * scale, a_height * scale, A_BPP, videoFlags);

    if(screen == NULL) {
        a_error("SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_SetClipRect(screen, NULL);

    #if A_PLATFORM_LINUXPC
        char caption[64];
        sprintf(caption, "%s %s", a2x_str("app.title"), a2x_str("app.version"));
        SDL_WM_SetCaption(caption, NULL);
    #else
        SDL_ShowCursor(SDL_DISABLE);
    #endif

    return true;
}

Pixel* a_sdl__screen_pixels(void)
{
    return screen->pixels;
}

void a_sdl__screen_lock(void)
{
    if(SDL_MUSTLOCK(screen) && !screen_locked) {
        SDL_LockSurface(screen);
        screen_locked = true;
    }
}

void a_sdl__screen_unlock(void)
{
    if(SDL_MUSTLOCK(screen) && screen_locked) {
        SDL_UnlockSurface(screen);
        screen_locked = false;
    }
}

void a_sdl__screen_flip(void)
{
    SDL_Flip(screen);
}
