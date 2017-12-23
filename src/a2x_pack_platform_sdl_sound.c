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

#include "a2x_pack_platform_sdl_sound.v.h"

#include <SDL.h>
#include <SDL_mixer.h>

struct ASdlSfx {
    Mix_Chunk* chunk;
    int channel;
};

int g_numSfxChannels;
int g_currentSfxChannel;

void a_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    #if A_PLATFORM_GP2X
        int frequency = 44100;
    #else
        int frequency = MIX_DEFAULT_FREQUENCY;
    #endif

    if(Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        a_out__error("Mix_OpenAudio failed: %s", Mix_GetError());
        a_settings__set("sound.on", "0");
    } else {
        g_numSfxChannels = Mix_AllocateChannels(64);
        a_out__message("Allocated %d sfx channels", g_numSfxChannels);
    }
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
    if(Mix_PlayMusic(Music, -1) == -1) {
        a_out__error("Mix_PlayMusic failed: %s", Mix_GetError());
    }
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

ASdlSfx* a_sdl_sound__sfxLoadFromFile(const char* Path)
{
    ASdlSfx* sfx = a_mem_zalloc(sizeof(ASdlSfx));

    sfx->chunk = Mix_LoadWAV(Path);

    if(sfx->chunk) {
        sfx->channel = g_currentSfxChannel++ % g_numSfxChannels;
    } else {
        a_out__error("Mix_LoadWAV(%s) failed: %s", Path, Mix_GetError());
    }

    return sfx;
}

ASdlSfx* a_sdl_sound__sfxLoadFromData(const uint8_t* Data, int Size)
{
    ASdlSfx* sfx = a_mem_zalloc(sizeof(ASdlSfx));
    SDL_RWops* rw = SDL_RWFromMem((void*)Data, Size);

    if(rw) {
        sfx->chunk = Mix_LoadWAV_RW(rw, 0);

        if(sfx->chunk) {
            sfx->channel = g_currentSfxChannel++ % g_numSfxChannels;
        } else {
            a_out__error("Mix_LoadWAV_RW failed: %s", Mix_GetError());
        }

        SDL_FreeRW(rw);
    } else {
        a_out__error("SDL_RWFromMem failed: %s", SDL_GetError());
    }

    return sfx;
}

void a_sdl_sound__sfxFree(ASdlSfx* Sfx)
{
    if(Sfx->chunk) {
        Mix_FreeChunk(Sfx->chunk);
    }

    free(Sfx);
}

void a_sdl_sound__sfxSetVolume(ASdlSfx* Sfx, int Volume)
{
    #if A_PLATFORM_EMSCRIPTEN
        A_UNUSED(Sfx);
        A_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sfx->chunk, Volume);
    #endif
}

void a_sdl_sound__sfxPlay(ASdlSfx* Sfx)
{
    if(Mix_PlayChannel(Sfx->channel, Sfx->chunk, 0) == -1) {
        a_out__error("Mix_PlayChannel failed: %s", Mix_GetError());
    }
}

void a_sdl_sound__sfxStop(ASdlSfx* Sfx)
{
    Mix_HaltChannel(Sfx->channel);
}

bool a_sdl_sound__sfxIsPlaying(ASdlSfx* Sfx)
{
    return Mix_Playing(Sfx->channel);
}
