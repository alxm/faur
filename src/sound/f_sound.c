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

#include "f_sound.v.h"
#include <faur.v.h>

#if F_CONFIG_SOUND_ENABLED
static int g_volume;
static int g_musicVolume;
static int g_samplesVolume;
static int g_volumeMax;

#if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ
    #define F__SOUND_VOLUME_BAR 1
    #define F__VOLUME_STEP 1
    #define F__VOLBAR_SHOW_MS 500
    static ATimer* g_volTimer;
    static AButton* g_volumeUpButton;
    static AButton* g_volumeDownButton;
#endif

#if F_CONFIG_TRAIT_KEYBOARD
    static AButton* g_muteButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = f_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = g_volume * F_CONFIG_SOUND_VOLUME_SCALE_MUSIC / 100;
    g_samplesVolume = g_volume * F_CONFIG_SOUND_VOLUME_SCALE_SAMPLE / 100;

    f_platform_api__soundSampleVolumeSetAll(g_samplesVolume);
    f_platform_api__soundMusicVolumeSet(g_musicVolume);
}

static void f_sound__init(void)
{
    g_volumeMax = f_platform_api__soundVolumeGetMax();

    #if F__SOUND_VOLUME_BAR
        adjustSoundVolume(g_volumeMax / 16);
        g_volTimer = f_timer_new(F_TIMER_MS, F__VOLBAR_SHOW_MS, false);
    #else
        adjustSoundVolume(g_volumeMax);
    #endif

    #if F__SOUND_VOLUME_BAR
        g_volumeUpButton = f_button_new();
        f_button_bindButton(g_volumeUpButton, NULL, F_BUTTON_VOLUP);

        g_volumeDownButton = f_button_new();
        f_button_bindButton(g_volumeDownButton, NULL, F_BUTTON_VOLDOWN);
    #endif

    #if F_CONFIG_TRAIT_KEYBOARD
        g_muteButton = f_button_new();
        f_button_bindKey(g_muteButton, F_KEY_M);
    #endif
}

static void f_sound__uninit(void)
{
    #if F__SOUND_VOLUME_BAR
        f_timer_free(g_volTimer);
    #endif

    #if F_CONFIG_TRAIT_KEYBOARD
        f_button_free(g_muteButton);
    #endif

    f_music_stop();
}

const APack f_pack__sound = {
    "Sound",
    {
        [0] = f_sound__init,
    },
    {
        [0] = f_sound__uninit,
    },
};

void f_sound__tick(void)
{
    #if F__SOUND_VOLUME_BAR
        int adjust = 0;

        if(f_button_pressGet(g_volumeUpButton)) {
            adjust = F__VOLUME_STEP;
        } else if(f_button_pressGet(g_volumeDownButton)) {
            adjust = -F__VOLUME_STEP;
        }

        if(adjust) {
            adjustSoundVolume(g_volume + adjust);
            f_timer_start(g_volTimer);
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
    #if F__SOUND_VOLUME_BAR
        if(!f_timer_isRunning(g_volTimer) || f_timer_expiredGet(g_volTimer)) {
            return;
        }

        f_color_blendSet(F_COLOR_BLEND_PLAIN);

        f_color_baseSetHex(F_CONFIG_COLOR_VOLBAR_BACKGROUND);
        f_draw_rectangle(0, 181, g_volumeMax / F__VOLUME_STEP + 5, 16);

        f_color_baseSetHex(F_CONFIG_COLOR_VOLBAR_BORDER);
        f_draw_hline(0, g_volumeMax / F__VOLUME_STEP + 4, 180);
        f_draw_hline(0, g_volumeMax / F__VOLUME_STEP + 4, 183 + 14);
        f_draw_vline(g_volumeMax / F__VOLUME_STEP + 4 + 1, 181, 183 + 13);

        f_color_baseSetHex(F_CONFIG_COLOR_VOLBAR_FILL);
        f_draw_rectangle(0, 186, g_volume / F__VOLUME_STEP, 6);
    #endif
}

AMusic* f_music_new(const char* Path)
{
    APlatformMusic* m = f_platform_api__soundMusicNew(Path);

    if(m == NULL) {
        F__FATAL("f_music_new(%s): Cannot open file", Path);
    }

    return m;
}

void f_music_free(AMusic* Music)
{
    if(Music) {
        f_platform_api__soundMusicFree(Music);
    }
}

void f_music_play(AMusic* Music)
{
    if(f_platform_api__soundMuteGet()) {
        return;
    }

    f_platform_api__soundMusicPlay(Music);
}

void f_music_stop(void)
{
    f_platform_api__soundMusicStop();
}

ASample* f_sample_new(const char* Path)
{
    APlatformSample* s = NULL;

    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        s = f_platform_api__soundSampleNewFromFile(Path);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        const AEmbeddedFile* e = f_embed__fileGet(Path);

        s = f_platform_api__soundSampleNewFromData(e->buffer, (int)e->size);
    } else {
        F__FATAL("f_sample_new(%s): File does not exist", Path);
    }

    if(s == NULL) {
        F__FATAL("f_sample_new(%s): Cannot open file", Path);
    }

    return s;
}

void f_sample_free(ASample* Sample)
{
    if(Sample) {
        f_platform_api__soundSampleFree(Sample);
    }
}

int f_channel_new(void)
{
    return f_platform_api__soundSampleChannelGet();
}

void f_channel_play(int Channel, ASample* Sample, AChannelFlags Flags)
{
    if(f_platform_api__soundMuteGet()) {
        return;
    }

    if(F_FLAGS_TEST_ANY(Flags, F_CHANNEL_RESTART)) {
        f_platform_api__soundSampleStop(Channel);
    } else if(F_FLAGS_TEST_ANY(Flags, F_CHANNEL_YIELD)
        && f_platform_api__soundSampleIsPlaying(Channel)) {

        return;
    }

    f_platform_api__soundSamplePlay(
        Sample, Channel, F_FLAGS_TEST_ANY(Flags, F_CHANNEL_LOOP));
}

void f_channel_stop(int Channel)
{
    f_platform_api__soundSampleStop(Channel);
}

bool f_channel_isPlaying(int Channel)
{
    return f_platform_api__soundSampleIsPlaying(Channel);
}
#else // !F_CONFIG_SOUND_ENABLED
const APack f_pack__sound;

void f_sound__tick(void)
{
}

void f_sound__draw(void)
{
}

AMusic* f_music_new(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

void f_music_free(AMusic* Music)
{
    F_UNUSED(Music);
}

void f_music_play(AMusic* Music)
{
    F_UNUSED(Music);
}

void f_music_stop(void)
{
}

ASample* f_sample_new(const char* Path)
{
    F_UNUSED(Path);

    return NULL;
}

void f_sample_free(ASample* Sample)
{
    F_UNUSED(Sample);
}

int f_channel_new(void)
{
    return -1;
}

void f_channel_play(int Channel, ASample* Sample, AChannelFlags Flags)
{
    F_UNUSED(Channel);
    F_UNUSED(Sample);
    F_UNUSED(Flags);
}

void f_channel_stop(int Channel)
{
    F_UNUSED(Channel);
}

bool f_channel_isPlaying(int Channel)
{
    F_UNUSED(Channel);

    return false;
}
#endif // !F_CONFIG_SOUND_ENABLED
