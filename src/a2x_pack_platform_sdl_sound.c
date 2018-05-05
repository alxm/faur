/*
    Copyright 2010, 2016-2018 Alex Margarit

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
#include "a2x_pack_platform_sdl_sound.v.h"

#include <SDL.h>
#include <SDL_mixer.h>

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_settings.v.h"

struct APlatformSfx {
    Mix_Chunk* chunk;
    int refs;
};

static int g_numSfxChannels;
static int g_numSfxChannelsReserved;
static int g_currentSfxChannel;

void a_platform_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    #if A_PLATFORM_SYSTEM_GP2X
        int frequency = 44100;
    #else
        int frequency = MIX_DEFAULT_FREQUENCY;
    #endif

    if(Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        a_out__error("Mix_OpenAudio failed: %s", Mix_GetError());
        a_settings__set("sound.on", "0");
    } else {
        g_numSfxChannels =
            Mix_AllocateChannels(a_settings_getInt("sound.sfx.chan.total"));
        g_numSfxChannelsReserved =
            Mix_ReserveChannels(a_settings_getInt("sound.sfx.chan.reserved"));

        if(g_numSfxChannelsReserved <= 0) {
            a_out__fatal("Mix_AllocateChannels/Mix_ReserveChannels failed");
        } else {
            a_out__message("Allocated %d sfx channels, reserved %d",
                           g_numSfxChannels,
                           g_numSfxChannelsReserved);
        }
    }
}

void a_platform_sdl_sound__uninit(void)
{
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int a_platform__getMaxVolome(void)
{
    return MIX_MAX_VOLUME;
}

APlatformMusic* a_platform__newMusic(const char* Path)
{
    Mix_Music* m = Mix_LoadMUS(Path);

    if(!m) {
        a_out__error("Mix_LoadMUS failed: %s", Mix_GetError());
    }

    return m;
}

void a_platform__freeMusic(APlatformMusic* Music)
{
    Mix_FreeMusic(Music);
}

void a_platform__setMusicVolume(int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void a_platform__playMusic(APlatformMusic* Music)
{
    if(Mix_PlayMusic(Music, -1) == -1) {
        a_out__error("Mix_PlayMusic failed: %s", Mix_GetError());
    }
}

void a_platform__stopMusic(void)
{
    Mix_HaltMusic();
}

void a_platform__toggleMusic(void)
{
    if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

APlatformSfx* a_platform__newSfxFromFile(const char* Path)
{
    APlatformSfx* sfx = a_mem_malloc(sizeof(APlatformSfx));

    sfx->chunk = Mix_LoadWAV(Path);
    sfx->refs = 0;

    if(sfx->chunk == NULL) {
        a_out__error("Mix_LoadWAV(%s) failed: %s", Path, Mix_GetError());
    }

    return sfx;
}

APlatformSfx* a_platform__newSfxFromData(const uint8_t* Data, int Size)
{
    APlatformSfx* sfx = a_mem_malloc(sizeof(APlatformSfx));
    SDL_RWops* rw = SDL_RWFromMem((void*)Data, Size);

    if(rw) {
        sfx->chunk = Mix_LoadWAV_RW(rw, 0);
        sfx->refs = 0;

        if(sfx->chunk == NULL) {
            a_out__error("Mix_LoadWAV_RW failed: %s", Mix_GetError());
        }

        SDL_FreeRW(rw);
    } else {
        sfx->chunk = NULL;
        a_out__error("SDL_RWFromMem failed: %s", SDL_GetError());
    }

    return sfx;
}

void a_platform__freeSfx(APlatformSfx* Sfx)
{
    if(Sfx->refs--) {
        return;
    }

    if(Sfx->chunk) {
        Mix_FreeChunk(Sfx->chunk);
    }

    free(Sfx);
}

void a_platform__referenceSfx(APlatformSfx* Sfx)
{
    Sfx->refs++;
}

void a_platform__setSfxVolume(APlatformSfx* Sfx, int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Sfx);
        A_UNUSED(Volume);
    #else
        if(Sfx->chunk) {
            Mix_VolumeChunk(Sfx->chunk, Volume);
        }
    #endif
}

void a_platform__setSfxVolumeAll(int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_Volume(-1, Volume);
    #endif
}

void a_platform__playSfx(APlatformSfx* Sfx, int Channel, bool Loop)
{
    if(Sfx->chunk == NULL) {
        return;
    }

    if(Mix_PlayChannel(Channel, Sfx->chunk, Loop ? -1 : 0) == -1) {
        a_out__errorv("Mix_PlayChannel failed: %s", Mix_GetError());
    }
}

void a_platform__stopSfx(int Channel)
{
    Mix_HaltChannel(Channel);
}

bool a_platform__isSfxPlaying(int Channel)
{
    return Mix_Playing(Channel);
}

int a_platform__getSfxChannel(void)
{
    return g_currentSfxChannel++ % g_numSfxChannelsReserved;
}
#endif // A_PLATFORM_LIB_SDL
