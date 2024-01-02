/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_sound.v.h"
#include <faur.v.h>

#if F_CONFIG_SOUND_VOLUME_ADJUSTABLE
    static int g_volume;
    static int g_musicVolume;
    static int g_samplesVolume;
    static int g_volumeMax;

    #if F_CONFIG_SOUND_VOLUME_BAR
        #define F__VOLUME_STEP 1
        #define F__VOLBAR_SHOW_MS 500

        static FTimer* g_volTimer;
        static FButton* g_volumeUpButton;
        static FButton* g_volumeDownButton;
    #endif
#endif

#if F_CONFIG_TRAIT_KEYBOARD
    static FButton* g_muteButton;
#endif

#if F_CONFIG_SOUND_VOLUME_ADJUSTABLE
static void adjustSoundVolume(int Volume)
{
    g_volume = f_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = g_volume * F_CONFIG_SOUND_VOLUME_SCALE_MUSIC / 100;
    g_samplesVolume = g_volume * F_CONFIG_SOUND_VOLUME_SCALE_SAMPLE / 100;

    f_platform_api__soundVolumeSet(g_musicVolume, g_samplesVolume);
}
#endif

static void f_sound__init(void)
{
    #if F_CONFIG_SOUND_VOLUME_ADJUSTABLE
        g_volumeMax = f_platform_api__soundVolumeGetMax();

        #if F_CONFIG_SOUND_VOLUME_BAR
            adjustSoundVolume(g_volumeMax / 16);
            g_volTimer = f_timer_new(F__VOLBAR_SHOW_MS, false);

            g_volumeUpButton = f_button_new();
            f_button_bindButton(g_volumeUpButton, F_BUTTON_VOLUP);

            g_volumeDownButton = f_button_new();
            f_button_bindButton(g_volumeDownButton, F_BUTTON_VOLDOWN);
        #else
            adjustSoundVolume(g_volumeMax);
        #endif
    #endif

    #if F_CONFIG_TRAIT_KEYBOARD
        g_muteButton = f_button_new();

        f_button_bindKey(g_muteButton, F_KEY_M);
    #endif
}

static void f_sound__uninit(void)
{
    #if F_CONFIG_SOUND_VOLUME_BAR
        f_timer_free(g_volTimer);
    #endif

    #if F_CONFIG_TRAIT_KEYBOARD
        f_button_free(g_muteButton);
    #endif

    f_music_stop();
}

const FPack f_pack__sound = {
    "Sound",
    f_sound__init,
    f_sound__uninit,
};

void f_sound__tick(void)
{
    #if F_CONFIG_SOUND_VOLUME_BAR
        int adjust = 0;

        if(f_button_pressGet(g_volumeUpButton)) {
            adjust = F__VOLUME_STEP;
        } else if(f_button_pressGet(g_volumeDownButton)) {
            adjust = -F__VOLUME_STEP;
        }

        if(adjust) {
            adjustSoundVolume(g_volume + adjust);
            f_timer_runStart(g_volTimer);
        }
    #endif

    #if F_CONFIG_TRAIT_KEYBOARD
        if(f_button_pressGetOnce(g_muteButton)) {
            f_platform_api__soundMuteFlip();

            f_out__info("Sound is now %s",
                        f_platform_api__soundMuteGet() ? "off" : "on");
        }
    #endif
}

void f_sound__draw(void)
{
    #if F_CONFIG_SOUND_VOLUME_BAR
        if(!f_timer_runGet(g_volTimer) || f_timer_expiredGet(g_volTimer)) {
            return;
        }

        #define F__VOL_Y 181
        #define F__VOL_W 128
        #define F__VOL_H 16

        f_color_push();
        f_color_blendSet(F_COLOR_BLEND_ALPHA_75);

        f_color__colorSetInternal(F_COLOR__PAL_BROWN2);
        f_draw_rectangle(0, F__VOL_Y, F__VOL_W, F__VOL_H);

        f_color__colorSetInternal(F_COLOR__PAL_BLUE1);
        f_draw_lineh(0, F__VOL_W - 1, F__VOL_Y - 1);
        f_draw_lineh(0, F__VOL_W - 1, F__VOL_Y + F__VOL_H);
        f_draw_linev(F__VOL_W, F__VOL_Y, F__VOL_Y + F__VOL_H - 1);

        f_color__colorSetInternal(F_COLOR__PAL_CHARTREUSE1);
        f_draw_rectangle(
            0, F__VOL_Y + 4, (F__VOL_W - 4) * g_volume / g_volumeMax, 8);

        f_color_pop();
    #endif
}
