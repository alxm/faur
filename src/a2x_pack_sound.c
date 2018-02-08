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

#include "a2x_pack_draw.v.h"
#include "a2x_pack_embed.v.h"
#include "a2x_pack_file.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_sound.v.h"
#include "a2x_pack_time.v.h"

static bool g_soundOn;
static int g_volume;
static int g_musicVolume;
static int g_sfxVolume;
static int g_volumeMax;

#if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
    #define A_SFX_LIST 1
    #define A_VOLUME_STEP 1
    #define A_VOLBAR_SHOW_MS 500
    static uint32_t g_lastVolAdjustment;
    static AInputButton* g_volumeUpButton;
    static AInputButton* g_volumeDownButton;
    static APixel g_volbarBackground;
    static APixel g_volbarBorder;
    static APixel g_volbarFill;
    static AList* g_sfxList;
#elif A_DEVICE_HAS_KEYBOARD
    static AInputButton* g_musicOnOffButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = a_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = a_settings_getInt("sound.music.scale") * g_volume / 100;
    g_sfxVolume = a_settings_getInt("sound.sfx.scale") * g_volume / 100;
}

static void inputCallback(void)
{
    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        if(g_soundOn) {
            int adjust = 0;

            if(a_button_getPressed(g_volumeUpButton)) {
                adjust = A_VOLUME_STEP;
            } else if(a_button_getPressed(g_volumeDownButton)) {
                adjust = -A_VOLUME_STEP;
            }

            if(adjust) {
                adjustSoundVolume(g_volume + adjust);
                a_platform__setMusicVolume(g_musicVolume);

                A_LIST_ITERATE(g_sfxList, ASound*, s) {
                    a_platform__setSfxVolume(s, g_sfxVolume);
                }

                g_lastVolAdjustment = a_time_getMs();
            }
        }
    #elif A_DEVICE_HAS_KEYBOARD
        if(g_soundOn) {
            if(a_button_getPressedOnce(g_musicOnOffButton)) {
                a_platform__toggleMusic();
            }
        }
    #endif
}

#if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
    static void screenCallback(void)
    {
        if(g_soundOn) {
            if(a_time_getMs() - g_lastVolAdjustment >= A_VOLBAR_SHOW_MS) {
                return;
            }

            a_pixel_setBlend(A_PIXEL_BLEND_PLAIN);

            a_pixel_setPixel(g_volbarBackground);
            a_draw_rectangle(0, 181, g_volumeMax / A_VOLUME_STEP + 5, 16);

            a_pixel_setPixel(g_volbarBorder);
            a_draw_hline(0, g_volumeMax / A_VOLUME_STEP + 4, 180);
            a_draw_hline(0, g_volumeMax / A_VOLUME_STEP + 4, 183 + 14);
            a_draw_vline(g_volumeMax / A_VOLUME_STEP + 4 + 1, 181, 183 + 13);

            a_pixel_setPixel(g_volbarFill);
            a_draw_rectangle(0, 186, g_volume / A_VOLUME_STEP, 6);
        }
    }
#endif

void a_sound__init(void)
{
    g_soundOn = a_settings_getBool("sound.on");

    if(!g_soundOn) {
        return;
    }

    #if A_SFX_LIST
        g_sfxList = a_list_new();
    #endif

    g_volumeMax = a_platform__getMaxVolome();

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        adjustSoundVolume(g_volumeMax / 16);
        g_lastVolAdjustment = UINT32_MAX - A_VOLBAR_SHOW_MS;
    #else
        adjustSoundVolume(g_volumeMax);
    #endif

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        g_volumeUpButton = a_button_new("gamepad.b.volUp");
        g_volumeDownButton = a_button_new("gamepad.b.volDown");
    #elif A_DEVICE_HAS_KEYBOARD
        g_musicOnOffButton = a_button_new("key.m");
    #endif

    a_input__addCallback(inputCallback);

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        const char* color;

        color = a_settings_getString("sound.volbar.background");
        g_volbarBackground = a_pixel_hex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.border");
        g_volbarBorder = a_pixel_hex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.fill");
        g_volbarFill = a_pixel_hex((uint32_t)strtol(color, NULL, 16));

        a_screen__addOverlay(screenCallback);
    #endif
}

void a_sound__uninit(void)
{
    if(g_soundOn) {
        a_music_stop();

        #if A_SFX_LIST
            a_list_freeEx(g_sfxList, (AFree*)a_sfx_free);
        #endif
    }
}

AMusic* a_music_new(const char* Path)
{
    if(g_soundOn) {
        AMusic* music = a_platform__newMusic(Path);
        a_platform__setMusicVolume(g_musicVolume);

        return music;
    } else {
        return NULL;
    }
}

void a_music_free(AMusic* Music)
{
    if(g_soundOn) {
        a_platform__freeMusic(Music);
    }
}

void a_music_play(AMusic* Music)
{
    if(g_soundOn && Music) {
        a_platform__playMusic(Music);
    }
}

void a_music_stop(void)
{
    if(g_soundOn) {
        a_platform__stopMusic();
    }
}

ASound* a_sfx_new(const char* Path)
{
    if(!g_soundOn) {
        return NULL;
    }

    ASound* s = NULL;

    if(a_file_exists(Path)) {
        s = a_platform__newSfxFromFile(Path);
    } else {
        const uint8_t* data;
        size_t size;

        if(a_embed__get(Path, &data, &size)) {
            s = a_platform__newSfxFromData(data, (int)size);
        }
    }

    if(s) {
        a_platform__setSfxVolume(s, g_sfxVolume);

        #if A_SFX_LIST
            a_list_addLast(g_sfxList, s);
        #endif
    }

    return s;
}

void a_sfx_free(ASound* Sfx)
{
    if(g_soundOn) {
        a_platform__freeSfx(Sfx);
    }
}

void a_sfx_play(ASound* Sfx)
{
    if(g_soundOn) {
        a_platform__stopSfx(Sfx);
        a_platform__playSfx(Sfx);
    }
}

void a_sfx_playOnce(ASound* Sfx)
{
    if(g_soundOn) {
        if(!a_platform__isSfxPlaying(Sfx)) {
            a_platform__playSfx(Sfx);
        }
    }
}
