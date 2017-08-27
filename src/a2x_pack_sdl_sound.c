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

#include "a2x_pack_sdl_sound.v.h"

#include <SDL.h>
#include <SDL_mixer.h>

void a_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    #if A_PLATFORM_DESKTOP || A_PLATFORM_EMSCRIPTEN
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
            a_settings__set("sound.on", "0");
        }
    #elif A_PLATFORM_GP2X
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
            a_settings__set("sound.on", "0");
        }
    #else
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) != 0) {
            a_settings__set("sound.on", "0");
        }
    #endif
}

void a_sdl_sound__uninit(void)
{
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int a_sdl_sound__getMaxVolome(void)
{
    return MIX_MAX_VOLUME;
}

void* a_sdl_sound__musicLoad(const char* Path)
{
    Mix_Music* m = Mix_LoadMUS(Path);

    if(!m) {
        a_out__error("Mix_LoadMUS failed: %s", Mix_GetError());
    }

    return m;
}

void a_sdl_sound__musicFree(void* Music)
{
    Mix_FreeMusic(Music);
}

void a_sdl_sound__musicSetVolume(int Volume)
{
    #if A_PLATFORM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void a_sdl_sound__musicPlay(void* Music)
{
    Mix_PlayMusic(Music, -1);
}

void a_sdl_sound__musicStop(void)
{
    Mix_HaltMusic();
}

void a_sdl_sound__musicToggle(void)
{
    if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

void* a_sdl_sound__sfxLoadFromFile(const char* Path)
{
    Mix_Chunk* sfx = Mix_LoadWAV(Path);

    if(sfx == NULL) {
        a_out__error("Mix_LoadWAV(%s) failed: %s", Path, SDL_GetError());
    }

    return sfx;
}

void* a_sdl_sound__sfxLoadFromData(const uint8_t* Data, int Size)
{
    SDL_RWops* rw;
    Mix_Chunk* sfx = NULL;

    rw = SDL_RWFromMem((void*)Data, Size);
    if(rw == NULL) {
        a_out__error("SDL_RWFromMem failed: %s", SDL_GetError());
        goto Done;
    }

    sfx = Mix_LoadWAV_RW(rw, 0);
    if(sfx == NULL) {
        a_out__error("Mix_LoadWAV_RW failed: %s", SDL_GetError());
        goto Done;
    }

Done:
    if(rw) {
        SDL_FreeRW(rw);
    }

    return sfx;
}

void a_sdl_sound__sfxFree(void* Sfx)
{
    Mix_FreeChunk(Sfx);
}

void a_sdl_sound__sfxSetVolume(void* Sfx, int Volume)
{
    #if A_PLATFORM_EMSCRIPTEN
        A_UNUSED(Sfx);
        A_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sfx, Volume);
    #endif
}

void a_sdl_sound__sfxPlay(void* Sfx)
{
    Mix_PlayChannel(-1, Sfx, 0);
}
