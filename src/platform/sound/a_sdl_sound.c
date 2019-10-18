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

#include "a_sdl_sound.v.h"
#include <faur.v.h>

#if A_CONFIG_SOUND_ENABLED && A_CONFIG_LIB_SDL
#if A_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
#elif A_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

#if A_CONFIG_SYSTEM_EMSCRIPTEN && A_CONFIG_LIB_SDL == 1
    #define A__SOUND_NO_VOLUME_ADJUSTMENT 1
#endif

static bool g_enabled;
static bool g_mute = A_CONFIG_SOUND_MUTE;
static int g_numSampleChannels;
static int g_numSampleChannelsReserved;
static int g_currentSampleChannel;

void a_platform_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        A__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        a_out__error("Mix_OpenAudio: %s", Mix_GetError());
        return;
    }

    g_enabled = true;

    g_numSampleChannels =
        Mix_AllocateChannels(A_CONFIG_SOUND_SAMPLE_CHANNELS_TOTAL);

    if(g_numSampleChannels < 1) {
        a_out__error("Mix_AllocateChannels: %s", Mix_GetError());
    }

    g_numSampleChannelsReserved =
        Mix_ReserveChannels(A_CONFIG_SOUND_SAMPLE_CHANNELS_RESERVED);

    if(g_numSampleChannelsReserved < 1) {
        a_out__error("Mix_ReserveChannels: %s", Mix_GetError());
    }

    a_out__info("Allocated %d sample channels, reserved %d",
                g_numSampleChannels,
                g_numSampleChannelsReserved);
}

void a_platform_sdl_sound__uninit(void)
{
    if(g_enabled) {
        Mix_CloseAudio();
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool a_platform_api__soundMuteGet(void)
{
    return g_enabled ? g_mute : true;
}

void a_platform_api__soundMuteFlip(void)
{
    if(!g_enabled) {
        return;
    }

    g_mute = !g_mute;

    if(g_mute) {
        Mix_PauseMusic();
    } else if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

int a_platform_api__soundVolumeGetMax(void)
{
    return MIX_MAX_VOLUME;
}

APlatformMusic* a_platform_api__soundMusicNew(const char* Path)
{
    if(!g_enabled) {
        return NULL;
    }

    Mix_Music* m = Mix_LoadMUS(Path);

    if(m == NULL) {
        a_out__error("Mix_LoadMUS(%s): %s", Path, Mix_GetError());
    }

    return m;
}

void a_platform_api__soundMusicFree(APlatformMusic* Music)
{
    Mix_FreeMusic(Music);
}

void a_platform_api__soundMusicVolumeSet(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if A__SOUND_NO_VOLUME_ADJUSTMENT
        A_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void a_platform_api__soundMusicPlay(APlatformMusic* Music)
{
    if(Music == NULL) {
        return;
    }

    if(Mix_PlayMusic(Music, -1) == -1) {
        a_out__error("Mix_PlayMusic: %s", Mix_GetError());
    }
}

void a_platform_api__soundMusicStop(void)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltMusic();
}

APlatformSample* a_platform_api__soundSampleNewFromFile(const char* Path)
{
    if(!g_enabled) {
        return NULL;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(Path);

    if(chunk == NULL) {
        a_out__error("Mix_LoadWAV(%s): %s", Path, Mix_GetError());
    }

    return chunk;
}

APlatformSample* a_platform_api__soundSampleNewFromData(const uint8_t* Data, int Size)
{
    if(!g_enabled) {
        return NULL;
    }

    SDL_RWops* rw = SDL_RWFromMem((void*)Data, Size);

    if(rw == NULL) {
        a_out__error("SDL_RWFromMem: %s", SDL_GetError());
        return NULL;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);

    if(chunk == NULL) {
        a_out__error("Mix_LoadWAV_RW: %s", Mix_GetError());
    }

    SDL_FreeRW(rw);

    return chunk;
}

void a_platform_api__soundSampleFree(APlatformSample* Sample)
{
    Mix_FreeChunk(Sample);
}

void a_platform_api__soundSampleVolumeSet(APlatformSample* Sample, int Volume)
{
    if(Sample == NULL) {
        return;
    }

    #if A__SOUND_NO_VOLUME_ADJUSTMENT
        A_UNUSED(Sample);
        A_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sample, Volume);
    #endif
}

void a_platform_api__soundSampleVolumeSetAll(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if A__SOUND_NO_VOLUME_ADJUSTMENT
        A_UNUSED(Volume);
    #else
        Mix_Volume(-1, Volume);
    #endif
}

void a_platform_api__soundSamplePlay(APlatformSample* Sample, int Channel, bool Loop)
{
    if(Sample == NULL) {
        return;
    }

    if(Mix_PlayChannel(Channel, Sample, Loop ? -1 : 0) == -1) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__error("Mix_PlayChannel(%d): %s", Channel, Mix_GetError());
        #endif
    }
}

void a_platform_api__soundSampleStop(int Channel)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltChannel(Channel);
}

bool a_platform_api__soundSampleIsPlaying(int Channel)
{
    if(!g_enabled) {
        return false;
    }

    return Mix_Playing(Channel);
}

int a_platform_api__soundSampleChannelGet(void)
{
    if(!g_enabled || g_numSampleChannelsReserved < 1) {
        return -1;
    }

    return g_currentSampleChannel++ % g_numSampleChannelsReserved;
}
#endif // A_CONFIG_SOUND_ENABLED && A_CONFIG_LIB_SDL
