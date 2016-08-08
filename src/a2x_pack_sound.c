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

static AList* g_musicList;
static AList* g_sfxList;

int a__volume;
int a__volumeMax;
int a__volumeAdjust = -2 * A_MILIS_VOLUME;

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static AInput* g_volumeUpButton;
    static AInput* g_volumeDownButton;
#elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
    static AInput* g_musicOnOffButton;
#endif

void a_sound__init(void)
{
    if(a_settings_getBool("sound.on")) {
        g_musicList = a_list_new();
        g_sfxList = a_list_new();

        a__volumeMax = a_sdl__sound_volumeMax();

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            a__volume = a__volumeMax / 16;
        #else
            a__volume = a__volumeMax;
        #endif

        a__volumeAdjust = -2 * A_MILIS_VOLUME;

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            g_volumeUpButton = a_input_new("gp2x.VolUp, wiz.VolUp");
            g_volumeDownButton = a_input_new("gp2x.VolDown, wiz.VolDown");
        #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
            g_musicOnOffButton = a_input_new("pc.Music, pandora.Music");
        #endif
    }
}

void a_sound__uninit(void)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s;
        AMusic* m;

        a_music_stop();

        A_LIST_ITERATE(g_sfxList, s) {
            a_sfx__free(s);
        }

        A_LIST_ITERATE(g_musicList, m) {
            a_music__free(m);
        }

        a_list_free(g_sfxList);
        a_list_free(g_musicList);
    }
}

AMusic* a_music_load(const char* Path)
{
    if(a_settings_getBool("sound.on")) {
        AMusic* Music = a_sdl__music_load(Path);
        a_list_addLast(g_musicList, Music);
        a_sdl__music_setVolume();
        return Music;
    } else {
        return NULL;
    }
}

void a_music__free(AMusic* Music)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl__music_free(Music);
    }
}

void a_music_play(AMusic* Music)
{
    if(a_settings_getBool("sound.on") && Music) {
        a_sdl__music_play(Music);
    }
}

void a_music_stop(void)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl__music_stop();
    }
}

ASound* a_sfx_fromFile(const char* Path)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s = a_sdl__sfx_loadFromFile(Path);
        a_sdl__sfx_setVolume(s, (float)a_settings_getInt("sound.sfx.scale") / 100 * a__volume);

        a_list_addLast(g_sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

ASound* a_sfx__fromData(const uint8_t* Data, int Size)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s = a_sdl__sfx_loadFromData(Data, Size);
        a_sdl__sfx_setVolume(s, (float)a_settings_getInt("sound.sfx.scale") / 100 * a__volume);

        a_list_addLast(g_sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

void a_sfx__free(ASound* Sfx)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl__sfx_free(Sfx);
    }
}

void a_sfx_play(ASound* Sfx)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl__sfx_play(Sfx);
    }
}

void a_sfx_volume(int Volume)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s;

        A_LIST_ITERATE(g_sfxList, s) {
            a_sdl__sfx_setVolume(s, Volume);
        }
    }
}

void a_sound_adjustVolume(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a_settings_getBool("sound.on")) {
            int adjust = 0;

            if(a_button_get(g_volumeUpButton)) adjust = 1;
            if(a_button_get(g_volumeDownButton)) adjust = -1;

            if(adjust) {
                a__volume += adjust * A_VOLUME_STEP;

                if(a__volume > a__volumeMax) a__volume = a__volumeMax;
                else if(a__volume < 0) a__volume = 0;

                if(a_list_size(g_musicList) > 0) {
                    a_sdl__music_setVolume();
                }

                a_sfx_volume((float)a_settings_getInt("sound.sfx.scale") / 100 * a__volume);
                a__volumeAdjust = a_time_getMilis();
            }
        }
    #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
        if(a_settings_getBool("sound.on")) {
            if(a_button_getAndUnpress(g_musicOnOffButton)) {
                a_sdl__music_toggle();
            }
        }
    #endif
}
