/*
    Copyright 2010, 2016 Alex Margarit

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

static int g_volume;
static int g_musicVolume;
static int g_sfxVolume;
static int g_volumeMax;

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    #define A_VOLUME_STEP 1
    #define A_VOLBAR_SHOW_MS 500
    static uint32_t g_lastVolAdjustment;
    static AInputButton* g_volumeUpButton;
    static AInputButton* g_volumeDownButton;
    static APixel g_volbarBackground;
    static APixel g_volbarBorder;
    static APixel g_volbarFill;
#elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
    static AInputButton* g_musicOnOffButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = a_math_constrain(Volume, 0, g_volumeMax);
    g_musicVolume = a_settings_getInt("sound.music.scale") * g_volume / 100;
    g_sfxVolume = a_settings_getInt("sound.sfx.scale") * g_volume / 100;
}

static void inputCallback(void)
{
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        if(a_settings_getBool("sound.on")) {
            int adjust = 0;

            if(a_button_get(g_volumeUpButton)) {
                adjust = A_VOLUME_STEP;
            } else if(a_button_get(g_volumeDownButton)) {
                adjust = -A_VOLUME_STEP;
            }

            if(adjust) {
                adjustSoundVolume(g_volume + adjust);

                if(!a_list_empty(g_musicList)) {
                    a_sdl_sound__musicSetVolume(g_musicVolume);
                }

                A_LIST_ITERATE(g_sfxList, ASound*, s) {
                    a_sdl_sound__sfxSetVolume(s, g_sfxVolume);
                }

                g_lastVolAdjustment = a_time_getMs();
            }
        }
    #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
        if(a_settings_getBool("sound.on")) {
            if(a_button_getOnce(g_musicOnOffButton)) {
                a_sdl_sound__musicToggle();
            }
        }
    #endif
}

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ
    static void screenCallback(void)
    {
        if(a_settings_getBool("sound.on")) {
            if(a_time_getMs() - g_lastVolAdjustment >= A_VOLBAR_SHOW_MS) {
                return;
            }

            a_pixel_setBlend(A_PIXEL_BLEND_PLAIN);

            a_pixel_setPixel(g_volbarBackground);
            a_draw_rectangle(0, 181, g_volumeMax / A_VOLUME_STEP + 5, 16);

            a_pixel_setPixel(g_volbarBorder);
            a_draw_hline(0, g_volumeMax / A_VOLUME_STEP + 4 + 1, 180);
            a_draw_hline(0, g_volumeMax / A_VOLUME_STEP + 4 + 1, 183 + 14);
            a_draw_vline(g_volumeMax / A_VOLUME_STEP + 4 + 1, 181, 183 + 14);

            a_pixel_setPixel(g_volbarFill);
            a_draw_rectangle(0, 186, g_volume / A_VOLUME_STEP, 6);
        }
    }
#endif

void a_sound__init(void)
{
    if(a_settings_getBool("sound.on")) {
        g_musicList = a_list_new();
        g_sfxList = a_list_new();

        g_volumeMax = a_sdl_sound__getMaxVolome();

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            adjustSoundVolume(g_volumeMax / 16);
            g_lastVolAdjustment = UINT32_MAX - A_VOLBAR_SHOW_MS;
        #else
            adjustSoundVolume(g_volumeMax);
        #endif

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            g_volumeUpButton = a_button_new("gp2x.volup, wiz.volup");
            g_volumeDownButton = a_button_new("gp2x.voldown, wiz.voldown");
        #elif A_PLATFORM_LINUXPC || A_PLATFORM_PANDORA
            g_musicOnOffButton = a_button_new("key.m");
        #endif

        a_input__addCallback(inputCallback);

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
            char* end;
            const char* color;
            int r, g, b;

            color = a_settings_getString("sound.volbar.background");
            r = (int)strtol(color, &end, 0);
            g = (int)strtol(end, &end, 0);
            b = (int)strtol(end, NULL, 0);
            g_volbarBackground = a_pixel_rgb(r, g, b);

            color = a_settings_getString("sound.volbar.border");
            r = (int)strtol(color, &end, 0);
            g = (int)strtol(end, &end, 0);
            b = (int)strtol(end, NULL, 0);
            g_volbarBorder = a_pixel_rgb(r, g, b);

            color = a_settings_getString("sound.volbar.fill");
            r = (int)strtol(color, &end, 0);
            g = (int)strtol(end, &end, 0);
            b = (int)strtol(end, NULL, 0);
            g_volbarFill = a_pixel_rgb(r, g, b);

            a_screen__addOverlay(screenCallback);
        #endif
    }
}

void a_sound__uninit(void)
{
    if(a_settings_getBool("sound.on")) {
        a_music_stop();

        A_LIST_ITERATE(g_sfxList, ASound*, s) {
            a_sfx__free(s);
        }

        A_LIST_ITERATE(g_musicList, AMusic*, m) {
            a_music__free(m);
        }

        a_list_free(g_sfxList);
        a_list_free(g_musicList);
    }
}

AMusic* a_music_load(const char* Path)
{
    if(a_settings_getBool("sound.on")) {
        AMusic* music = a_sdl_sound__musicLoad(Path);
        a_sdl_sound__musicSetVolume(g_musicVolume);

        a_list_addLast(g_musicList, music);
        return music;
    } else {
        return NULL;
    }
}

void a_music__free(AMusic* Music)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__musicFree(Music);
    }
}

void a_music_play(AMusic* Music)
{
    if(a_settings_getBool("sound.on") && Music) {
        a_sdl_sound__musicPlay(Music);
    }
}

void a_music_stop(void)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__musicStop();
    }
}

ASound* a_sfx_fromFile(const char* Path)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s = a_sdl_sound__sfxLoadFromFile(Path);
        a_sdl_sound__sfxSetVolume(s, g_sfxVolume);

        a_list_addLast(g_sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

ASound* a_sfx__fromData(const uint8_t* Data, int Size)
{
    if(a_settings_getBool("sound.on")) {
        ASound* s = a_sdl_sound__sfxLoadFromData(Data, Size);
        a_sdl_sound__sfxSetVolume(s, g_sfxVolume);

        a_list_addLast(g_sfxList, s);
        return s;
    } else {
        return NULL;
    }
}

void a_sfx__free(ASound* Sfx)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__sfxFree(Sfx);
    }
}

void a_sfx_play(ASound* Sfx)
{
    if(a_settings_getBool("sound.on")) {
        a_sdl_sound__sfxPlay(Sfx);
    }
}
