/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_sound.v.h"
#include <a2x.v.h>

static int g_volume;
static int g_musicVolume;
static int g_samplesVolume;
static int g_volumeMax;

#if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ
    #define A__SOUND_VOLUME_BAR 1
    #define A__VOLUME_STEP 1
    #define A__VOLBAR_SHOW_MS 500
    static ATimer* g_volTimer;
    static AButton* g_volumeUpButton;
    static AButton* g_volumeDownButton;
#endif

#if A_CONFIG_TRAIT_KEYBOARD
    static AButton* g_muteButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = a_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = g_volume * A_CONFIG_SOUND_VOLUME_SCALE_MUSIC / 100;
    g_samplesVolume = g_volume * A_CONFIG_SOUND_VOLUME_SCALE_SAMPLE / 100;

    a_platform_api__soundSampleVolumeSetAll(g_samplesVolume);
    a_platform_api__soundMusicVolumeSet(g_musicVolume);
}

static void a_sound__init(void)
{
    g_volumeMax = a_platform_api__soundVolumeGetMax();

    #if A__SOUND_VOLUME_BAR
        adjustSoundVolume(g_volumeMax / 16);
        g_volTimer = a_timer_new(A_TIMER_MS, A__VOLBAR_SHOW_MS, false);
    #else
        adjustSoundVolume(g_volumeMax);
    #endif

    #if A__SOUND_VOLUME_BAR
        g_volumeUpButton = a_button_new();
        a_button_bind(g_volumeUpButton, A_BUTTON_VOLUP);

        g_volumeDownButton = a_button_new();
        a_button_bind(g_volumeDownButton, A_BUTTON_VOLDOWN);
    #endif

    #if A_CONFIG_TRAIT_KEYBOARD
        g_muteButton = a_button_new();
        a_button_bind(g_muteButton, A_KEY_M);
    #endif
}

static void a_sound__uninit(void)
{
    #if A__SOUND_VOLUME_BAR
        a_timer_free(g_volTimer);
    #endif

    #if A_CONFIG_TRAIT_KEYBOARD
        a_button_free(g_muteButton);
    #endif

    a_music_stop();
}

const APack a_pack__sound = {
    "Sound",
    {
        [0] = a_sound__init,
    },
    {
        [0] = a_sound__uninit,
    },
};

void a_sound__tick(void)
{
    #if A__SOUND_VOLUME_BAR
        int adjust = 0;

        if(a_button_pressGet(g_volumeUpButton)) {
            adjust = A__VOLUME_STEP;
        } else if(a_button_pressGet(g_volumeDownButton)) {
            adjust = -A__VOLUME_STEP;
        }

        if(adjust) {
            adjustSoundVolume(g_volume + adjust);
            a_timer_start(g_volTimer);
        }
    #endif

    #if A_CONFIG_TRAIT_KEYBOARD
        if(a_button_pressGetOnce(g_muteButton)) {
            a_platform_api__soundMuteFlip();

            a_out__info("Sound is now %s",
                        a_platform_api__soundMuteGet() ? "off" : "on");
        }
    #endif
}

void a_sound__draw(void)
{
    #if A__SOUND_VOLUME_BAR
        if(!a_timer_isRunning(g_volTimer) || a_timer_expiredGet(g_volTimer)) {
            return;
        }

        a_color_blendSet(A_COLOR_BLEND_PLAIN);

        a_color_baseSetHex(A_CONFIG_COLOR_VOLBAR_BACKGROUND);
        a_draw_rectangle(0, 181, g_volumeMax / A__VOLUME_STEP + 5, 16);

        a_color_baseSetHex(A_CONFIG_COLOR_VOLBAR_BORDER);
        a_draw_hline(0, g_volumeMax / A__VOLUME_STEP + 4, 180);
        a_draw_hline(0, g_volumeMax / A__VOLUME_STEP + 4, 183 + 14);
        a_draw_vline(g_volumeMax / A__VOLUME_STEP + 4 + 1, 181, 183 + 13);

        a_color_baseSetHex(A_CONFIG_COLOR_VOLBAR_FILL);
        a_draw_rectangle(0, 186, g_volume / A__VOLUME_STEP, 6);
    #endif
}

AMusic* a_music_new(const char* Path)
{
    APlatformSoundMusic* m = a_platform_api__soundMusicNew(Path);

    #if A_CONFIG_SOUND_ENABLED
        if(m == NULL) {
            A__FATAL("a_music_new(%s): Cannot open file", Path);
        }
    #endif

    return m;
}

void a_music_free(AMusic* Music)
{
    if(Music) {
        a_platform_api__soundMusicFree(Music);
    }
}

void a_music_play(AMusic* Music)
{
    if(a_platform_api__soundMuteGet()) {
        return;
    }

    a_platform_api__soundMusicPlay(Music);
}

void a_music_stop(void)
{
    a_platform_api__soundMusicStop();
}

ASample* a_sample_new(const char* Path)
{
    APlatformSoundSample* s = NULL;

    if(a_path_exists(Path, A_PATH_FILE | A_PATH_REAL)) {
        s = a_platform_api__soundSampleNewFromFile(Path);
    } else if(a_path_exists(Path, A_PATH_FILE | A_PATH_EMBEDDED)) {
        const AEmbeddedFile* e = a_embed__fileGet(Path);

        s = a_platform_api__soundSampleNewFromData(e->buffer, (int)e->size);
    } else {
        A__FATAL("a_sample_new(%s): File does not exist", Path);
    }

    #if A_CONFIG_SOUND_ENABLED
        if(s == NULL) {
            A__FATAL("a_sample_new(%s): Cannot open file", Path);
        }
    #endif

    return s;
}

void a_sample_free(ASample* Sample)
{
    if(Sample) {
        a_platform_api__soundSampleFree(Sample);
    }
}

int a_channel_new(void)
{
    return a_platform_api__soundSampleChannelGet();
}

void a_channel_play(int Channel, ASample* Sample, AChannelFlags Flags)
{
    if(a_platform_api__soundMuteGet()) {
        return;
    }

    if(A_FLAGS_TEST_ANY(Flags, A_CHANNEL_RESTART)) {
        a_platform_api__soundSampleStop(Channel);
    } else if(A_FLAGS_TEST_ANY(Flags, A_CHANNEL_YIELD)
        && a_platform_api__soundSampleIsPlaying(Channel)) {

        return;
    }

    a_platform_api__soundSamplePlay(
        Sample, Channel, A_FLAGS_TEST_ANY(Flags, A_CHANNEL_LOOP));
}

void a_channel_stop(int Channel)
{
    a_platform_api__soundSampleStop(Channel);
}

bool a_channel_isPlaying(int Channel)
{
    return a_platform_api__soundSampleIsPlaying(Channel);
}
