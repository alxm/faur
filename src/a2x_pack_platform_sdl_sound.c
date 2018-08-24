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

static int g_numSampleChannels;
static int g_numSampleChannelsReserved;
static int g_currentSampleChannel;

void a_platform_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        a_out__error("Mix_OpenAudio failed: %s", Mix_GetError());
        a_settings__set("sound.on", "0");
    } else {
        g_numSampleChannels =
            Mix_AllocateChannels(
                a_settings_getInt("sound.sample.channels.total"));
        g_numSampleChannelsReserved =
            Mix_ReserveChannels(
                a_settings_getInt("sound.sample.channels.reserved"));

        if(g_numSampleChannelsReserved <= 0) {
            a_out__fatal("Mix_AllocateChannels/Mix_ReserveChannels failed");
        } else {
            a_out__message("Allocated %d sample channels, reserved %d",
                           g_numSampleChannels,
                           g_numSampleChannelsReserved);
        }
    }
}

void a_platform_sdl_sound__uninit(void)
{
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int a_platform__volumeGetMax(void)
{
    return MIX_MAX_VOLUME;
}

APlatformMusic* a_platform__musicNew(const char* Path)
{
    Mix_Music* m = Mix_LoadMUS(Path);

    if(m == NULL) {
        a_out__error("Mix_LoadMUS failed: %s", Mix_GetError());
    }

    return m;
}

void a_platform__musicFree(APlatformMusic* Music)
{
    Mix_FreeMusic(Music);
}

void a_platform__musicVolumeSet(int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void a_platform__musicPlay(APlatformMusic* Music)
{
    if(Mix_PlayMusic(Music, -1) == -1) {
        a_out__error("Mix_PlayMusic failed: %s", Mix_GetError());
    }
}

void a_platform__musicStop(void)
{
    Mix_HaltMusic();
}

void a_platform__musicToggle(void)
{
    if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PauseMusic();
    }
}

APlatformSample* a_platform__sampleNewFromFile(const char* Path)
{
    Mix_Chunk* chunk = Mix_LoadWAV(Path);

    if(chunk == NULL) {
        a_out__error("Mix_LoadWAV(%s) failed: %s", Path, Mix_GetError());
    }

    return chunk;
}

APlatformSample* a_platform__sampleNewFromData(const uint8_t* Data, int Size)
{
    SDL_RWops* rw = SDL_RWFromMem((void*)Data, Size);

    if(rw == NULL) {
        a_out__error("SDL_RWFromMem failed: %s", SDL_GetError());
        return NULL;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);

    if(chunk == NULL) {
        a_out__error("Mix_LoadWAV_RW failed: %s", Mix_GetError());
    }

    SDL_FreeRW(rw);

    return chunk;
}

void a_platform__sampleFree(APlatformSample* Sample)
{
    Mix_FreeChunk(Sample);
}

void a_platform__sampleVolumeSet(APlatformSample* Sample, int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Sample);
        A_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sample, Volume);
    #endif
}

void a_platform__sampleVolumeSetAll(int Volume)
{
    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_Volume(-1, Volume);
    #endif
}

void a_platform__samplePlay(APlatformSample* Sample, int Channel, bool Loop)
{
    if(Mix_PlayChannel(Channel, Sample, Loop ? -1 : 0) == -1) {
        a_out__errorv("Mix_PlayChannel failed: %s", Mix_GetError());
    }
}

void a_platform__sampleStop(int Channel)
{
    Mix_HaltChannel(Channel);
}

bool a_platform__sampleIsPlaying(int Channel)
{
    return Mix_Playing(Channel);
}

int a_platform__sampleChannelGet(void)
{
    return g_currentSampleChannel++ % g_numSampleChannelsReserved;
}
#endif // A_PLATFORM_LIB_SDL
