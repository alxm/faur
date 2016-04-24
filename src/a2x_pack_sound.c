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
int a__volumeMax;
int a__volumeAdjust = -2 * A_MILIS_VOLUME;

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static Input* a__volUp;
    static Input* a__volDown;
#elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
    static Input* a__musicOnOff;
#endif

void a_sound__init(void)
{
    if(a2x_bool("sound.on")) {
        musicList = a_list_new();
        sfxList = a_list_new();

        a_sdl__sound_init();
        a__volumeMax = a_sdl__sound_volumeMax();

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a__volume = a__volumeMax / 16;
        #else
            a__volume = a__volumeMax;
        #endif

        a__volumeAdjust = -2 * A_MILIS_VOLUME;

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a__volUp = a_input_new("gp2x.VolUp, wiz.VolUp");
            a__volDown = a_input_new("gp2x.VolDown, wiz.VolDown");
        #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
            a__musicOnOff = a_input_new("pc.m, pandora.m");
        #endif
    }
}

void a_sound__uninit(void)
{
    if(a2x_bool("sound.on")) {
        a_music_stop();

        A_LIST_ITERATE(sfxList, Sound, s) {
            a_sfx__free(s);
        }

        A_LIST_ITERATE(musicList, Music, m) {
            a_music__free(m);
        }

        a_list_free(sfxList);
        a_list_free(musicList);

        a_sdl__sound_free();
    }
}

Music* a_music_load(const char* path)
{
    if(a2x_bool("sound.on")) {
        Music* m = a_sdl__music_load(path);
        a_list_addLast(musicList, m);
        a_sdl__music_setVolume();
        return m;
    } else {
        return NULL;
    }
}

void a_music__free(Music* m)
{
    if(a2x_bool("sound.on")) {
        a_sdl__music_free(m);
    }
}

void a_music_play(Music* m)
{
    if(a2x_bool("sound.on") && m) {
        a_sdl__music_play(m);
    }
}

void a_music_stop(void)
{
    if(a2x_bool("sound.on")) {
        a_sdl__music_stop();
    }
}

Sound* a_sfx_fromFile(const char* path)
{
    if(a2x_bool("sound.on")) {
        Sound* s = a_sdl__sfx_loadFromFile(path);
        a_sdl__sfx_setVolume(s, (float)a2x_int("sound.sfx.scale") / 100 * a__volume);

        a_list_addLast(sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

Sound* a_sfx__fromData(const uint16_t* data, int size)
{
    if(a2x_bool("sound.on")) {
        Sound* s = a_sdl__sfx_loadFromData(data, size);
        a_sdl__sfx_setVolume(s, (float)a2x_int("sound.sfx.scale") / 100 * a__volume);

        a_list_addLast(sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

void a_sfx__free(Sound* s)
{
    if(a2x_bool("sound.on")) {
        a_sdl__sfx_free(s);
    }
}

void a_sfx_play(Sound* s)
{
    if(a2x_bool("sound.on")) {
        a_sdl__sfx_play(s);
    }
}

void a_sfx_volume(int v)
{
    if(a2x_bool("sound.on")) {
        A_LIST_ITERATE(sfxList, Sound, s) {
            a_sdl__sfx_setVolume(s, v);
        }
    }
}

void a_sound_adjustVolume(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a2x_bool("sound.on")) {
            int adjust = 0;

            if(a_button_get(a__volUp)) adjust = 1;
            if(a_button_get(a__volDown)) adjust = -1;

            if(adjust) {
                a__volume += adjust * A_VOLUME_STEP;

                if(a__volume > a__volumeMax) a__volume = a__volumeMax;
                else if(a__volume < 0) a__volume = 0;

                if(a_list_size(musicList) > 0) {
                    a_sdl__music_setVolume();
                }

                a_sfx_volume((float)a2x_int("sound.sfx.scale") / 100 * a__volume);
                a__volumeAdjust = a_time_getMilis();
            }
        }
    #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
        if(a2x_bool("sound.on")) {
            if(a_button_getAndUnpress(a__musicOnOff)) {
                a_sdl__music_toggle();
            }
        }
    #endif
}
