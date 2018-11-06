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

#include "a2x_pack_platform_sdl_sound.v.h"

#if A_BUILD_LIB_SDL
#include <SDL.h>
#include <SDL_mixer.h>

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_settings.v.h"

static bool g_enabled;
static int g_numSampleChannels;
static int g_numSampleChannelsReserved;
static int g_currentSampleChannel;

#if A_BUILD_DEVICE_KEYBOARD
    static void settingMute(ASettingId Setting)
    {
        if(!g_enabled) {
            return;
        }

        if(a_settings_boolGet(Setting)) {
            Mix_PauseMusic();
        } else if(Mix_PausedMusic()) {
            Mix_ResumeMusic();
        }
    }
#endif

void a_platform_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        a_out__error("Mix_OpenAudio: %s", Mix_GetError());
        return;
    }

    g_enabled = true;

    g_numSampleChannels =
        Mix_AllocateChannels(
            a_settings_intGet(A_SETTING_SOUND_SAMPLE_CHANNELS_TOTAL));

    if(g_numSampleChannels <= 0) {
        a_out__error("Mix_AllocateChannels: %s", Mix_GetError());
    }

    g_numSampleChannelsReserved =
        Mix_ReserveChannels(
            a_settings_intGet(A_SETTING_SOUND_SAMPLE_CHANNELS_RESERVED));

    if(g_numSampleChannelsReserved <= 0) {
        a_out__error("Mix_ReserveChannels: %s", Mix_GetError());
    }

    a_out__message("Allocated %d sample channels, reserved %d",
                   g_numSampleChannels,
                   g_numSampleChannelsReserved);

    #if A_BUILD_DEVICE_KEYBOARD
        a_settings__callbackSet(A_SETTING_SOUND_MUTE, settingMute);
    #endif
}

void a_platform_sdl_sound__uninit(void)
{
    if(g_enabled) {
        Mix_CloseAudio();
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int a_platform__volumeGetMax(void)
{
    return MIX_MAX_VOLUME;
}

APlatformMusic* a_platform__musicNew(const char* Path)
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

void a_platform__musicFree(APlatformMusic* Music)
{
    Mix_FreeMusic(Music);
}

void a_platform__musicVolumeSet(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void a_platform__musicPlay(APlatformMusic* Music)
{
    if(Music == NULL) {
        return;
    }

    if(Mix_PlayMusic(Music, -1) == -1) {
        a_out__error("Mix_PlayMusic: %s", Mix_GetError());
    }
}

void a_platform__musicStop(void)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltMusic();
}

APlatformSample* a_platform__sampleNewFromFile(const char* Path)
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

APlatformSample* a_platform__sampleNewFromData(const uint8_t* Data, int Size)
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

void a_platform__sampleFree(APlatformSample* Sample)
{
    Mix_FreeChunk(Sample);
}

void a_platform__sampleVolumeSet(APlatformSample* Sample, int Volume)
{
    if(Sample == NULL) {
        return;
    }

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        A_UNUSED(Sample);
        A_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sample, Volume);
    #endif
}

void a_platform__sampleVolumeSetAll(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        A_UNUSED(Volume);
    #else
        Mix_Volume(-1, Volume);
    #endif
}

void a_platform__samplePlay(APlatformSample* Sample, int Channel, bool Loop)
{
    if(Sample == NULL) {
        return;
    }

    if(Mix_PlayChannel(Channel, Sample, Loop ? -1 : 0) == -1) {
        a_out__errorv("Mix_PlayChannel(%d): %s", Channel, Mix_GetError());
    }
}

void a_platform__sampleStop(int Channel)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltChannel(Channel);
}

bool a_platform__sampleIsPlaying(int Channel)
{
    if(!g_enabled) {
        return false;
    }

    return Mix_Playing(Channel);
}

int a_platform__sampleChannelGet(void)
{
    if(!g_enabled) {
        return -1;
    }

    return g_currentSampleChannel++ % g_numSampleChannelsReserved;
}
#endif // A_BUILD_LIB_SDL
