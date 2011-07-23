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

#include "a2x_pack_sound.v.h"

static List* musicList;
static List* sfxList;

int a__volume;
int a__volumeAdjust = -2 * A_MILIS_VOLUME;

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static Input* a__volUp;
    static Input* a__volDown;
#endif

void a_sound__set(void)
{
    if(a2x_bool("sound.on")) {
        musicList = a_list_set();
        sfxList = a_list_set();

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a__volume = A_MAX_VOLUME / 16;
        #else
            a__volume = A_MAX_VOLUME;
        #endif

        a__volumeAdjust = -2 * A_MILIS_VOLUME;

        #if A_PLATFORM_LINUXPC || A_PLATFORM_WINDOWS
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
                a2x_set("sound.on", "0");
            }
        #elif A_PLATFORM_GP2X
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
                a2x_set("sound.on", "0");
            }
        #else
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) != 0) {
                a2x_set("sound.on", "0");
            }
        #endif

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a__volUp = a_input_set("gp2x.VolUp, wiz.VolUp");
            a__volDown = a_input_set("gp2x.VolDown, wiz.VolDown");
        #endif
    }
}

void a_sound__free(void)
{
    if(a2x_bool("sound.on")) {
        a_music_stop();

        while(a_list_iterate(sfxList)) {
            a_sfx_free(a_list_current(sfxList));
        }

        while(a_list_iterate(musicList)) {
            a_music_free(a_list_current(musicList));
        }

        a_list_free(sfxList);
        a_list_free(musicList);

        Mix_CloseAudio();

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a_input_free(a__volUp);
            a_input_free(a__volDown);
        #endif
    }
}

Music* a_music_load(const char* const path)
{
    if(a2x_bool("sound.on")) {
        Music* const m = Mix_LoadMUS(path);

        if(!m) {
            a_error(Mix_GetError());
        }

        Mix_VolumeMusic((float)a2x_int("sound.musicScale") / 100 * a__volume);

        a_list_addLast(musicList, m);

        return m;
    } else {
        return NULL;
    }
}

void a_music_free(Music* const m)
{
    if(a2x_bool("sound.on")) {
        Mix_FreeMusic(m);
        a_list_remove(musicList, m);
    }
}

void a_music_play(Music* const m)
{
    if(a2x_bool("sound.on")) {
        if(m) {
            Mix_PlayMusic(m, -1);
        }
    }
}

void a_music_stop(void)
{
    if(a2x_bool("sound.on")) {
        Mix_HaltMusic();
    }
}

Sound* a_sfx_fromFile(const char* const path)
{
    if(a2x_bool("sound.on")) {
        Sound* const s = Mix_LoadWAV(path);

        s->volume = (float)a2x_int("sound.sfxScale") / 100 * a__volume;
        a_list_addLast(sfxList, s);

        return s;
    } else {
        return NULL;
    }
}

Sound* a_sfx__fromData(const uint16_t* const data, const int size)
{
    if(a2x_bool("sound.on")) {
        SDL_RWops* const rw = SDL_RWFromMem((void*)data, size);
        Sound* const s = Mix_LoadWAV_RW(rw, 0);

        s->volume = (float)a2x_int("sound.sfxScale") / 100 * a__volume;
        a_list_addLast(sfxList, s);

        SDL_FreeRW(rw);

        return s;
    } else {
        return NULL;
    }
}

void a_sfx_free(Sound* const s)
{
    if(a2x_bool("sound.on")) {
        Mix_FreeChunk(s);
        a_list_remove(sfxList, s);
    }
}

void a_sfx_play(Sound* const s)
{
    if(a2x_bool("sound.on")) {
        Mix_PlayChannel(-1, s, 0);
    }
}

void a_sfx_volume(const int v)
{
    if(a2x_bool("sound.on")) {
        while(a_list_iterate(sfxList)) {
            ((Sound*)a_list_current(sfxList))->volume = v;
        }
    }
}

void a_sound_adjustVolume(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a2x_bool("sound.on")) {
            int adjust = 0;

            if(a_input_get(a__volUp)) adjust = 1;
            if(a_input_get(a__volDown)) adjust = -1;

            if(adjust) {
                a__volume += adjust * A_VOLUME_STEP;

                if(a__volume > A_MAX_VOLUME) a__volume = A_MAX_VOLUME;
                else if(a__volume < 0) a__volume = 0;

                if(a_list_size(musicList) > 0) {
                    Mix_VolumeMusic((float)a2x_int("sound.musicScale") / 100 * a__volume);
                }

                a_sfx_volume((float)a2x_int("sound.sfxScale") / 100 * a__volume);
                a__volumeAdjust = a_time_getMilis();
            }
        }
    #endif
}
