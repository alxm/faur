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

#include "a2x_pack_sound.v.h"

#include "a2x_pack_draw.v.h"
#include "a2x_pack_embed.v.h"
#include "a2x_pack_file.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_timer.v.h"

static int g_volume;
static int g_musicVolume;
static int g_samplesVolume;
static int g_volumeMax;

#if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
    #define A__VOLUME_STEP 1
    #define A__VOLBAR_SHOW_MS 500
    static ATimer* g_volTimer;
    static AButton* g_volumeUpButton;
    static AButton* g_volumeDownButton;
    static APixel g_volbarBackground;
    static APixel g_volbarBorder;
    static APixel g_volbarFill;
#endif

#if A_BUILD_DEVICE_KEYBOARD
    static AButton* g_musicOnOffButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = a_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = a_settings_getInt("sound.music.scale") * g_volume / 100;
    g_samplesVolume = a_settings_getInt("sound.sample.scale") * g_volume / 100;

    a_platform__sampleVolumeSetAll(g_samplesVolume);
    a_platform__musicVolumeSet(g_musicVolume);
}

void a_sound__init(void)
{
    g_volumeMax = a_platform__volumeGetMax();

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
        adjustSoundVolume(g_volumeMax / 16);
        g_volTimer = a_timer_new(A_TIMER_MS, A__VOLBAR_SHOW_MS, false);
    #else
        adjustSoundVolume(g_volumeMax);
    #endif

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
        g_volumeUpButton = a_button_new();
        a_button_bind(g_volumeUpButton, "gamepad.b.volUp");

        g_volumeDownButton = a_button_new();
        a_button_bind(g_volumeDownButton, "gamepad.b.volDown");
    #endif

    #if A_BUILD_DEVICE_KEYBOARD
        g_musicOnOffButton = a_button_new();
        a_button_bind(g_musicOnOffButton, "key.m");
    #endif

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
        const char* color;

        color = a_settings_getString("sound.volbar.background");
        g_volbarBackground = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.border");
        g_volbarBorder = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.fill");
        g_volbarFill = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));
    #endif
}

void a_sound__uninit(void)
{
    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
        a_timer_free(g_volTimer);
    #endif

    #if A_BUILD_DEVICE_KEYBOARD
        a_button_free(g_musicOnOffButton);
    #endif

    a_music_stop();
}

void a_sound__tick(void)
{
    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
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

    #if A_BUILD_DEVICE_KEYBOARD
        if(a_button_pressGetOnce(g_musicOnOffButton)) {
            a_platform__musicToggle();
        }
    #endif
}

void a_sound__draw(void)
{
    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
        if(!a_timer_isRunning(g_volTimer) || a_timer_expiredGet(g_volTimer)) {
            return;
        }

        a_pixel_blendSet(A_PIXEL_BLEND_PLAIN);

        a_pixel_colorSetPixel(g_volbarBackground);
        a_draw_rectangle(0, 181, g_volumeMax / A__VOLUME_STEP + 5, 16);

        a_pixel_colorSetPixel(g_volbarBorder);
        a_draw_hline(0, g_volumeMax / A__VOLUME_STEP + 4, 180);
        a_draw_hline(0, g_volumeMax / A__VOLUME_STEP + 4, 183 + 14);
        a_draw_vline(g_volumeMax / A__VOLUME_STEP + 4 + 1, 181, 183 + 13);

        a_pixel_colorSetPixel(g_volbarFill);
        a_draw_rectangle(0, 186, g_volume / A__VOLUME_STEP, 6);
    #endif
}

AMusic* a_music_new(const char* Path)
{
    return a_platform__musicNew(Path);
}

void a_music_free(AMusic* Music)
{
    if(Music) {
        a_platform__musicFree(Music);
    }
}

void a_music_play(AMusic* Music)
{
    a_platform__musicPlay(Music);
}

void a_music_stop(void)
{
    a_platform__musicStop();
}

ASample* a_sample_new(const char* Path)
{
    APlatformSample* s = NULL;

    if(a_file_exists(Path)) {
        s = a_platform__sampleNewFromFile(Path);
    } else {
        const uint8_t* data;
        size_t size;

        if(a_embed__get(Path, &data, &size)) {
            s = a_platform__sampleNewFromData(data, (int)size);
        }
    }

    return s;
}

void a_sample_free(ASample* Sample)
{
    if(Sample) {
        a_platform__sampleFree(Sample);
    }
}

int a_channel_new(void)
{
    return a_platform__sampleChannelGet();
}

void a_channel_play(int Channel, ASample* Sample, AChannelFlags Flags)
{
    if(Flags & A_CHANNEL_RESTART) {
        a_platform__sampleStop(Channel);
    } else if((Flags & A_CHANNEL_YIELD)
        && a_platform__sampleIsPlaying(Channel)) {

        return;
    }

    a_platform__samplePlay(Sample, Channel, Flags & A_CHANNEL_LOOP);
}

void a_channel_stop(int Channel)
{
    a_platform__sampleStop(Channel);
}

bool a_channel_isPlaying(int Channel)
{
    return a_platform__sampleIsPlaying(Channel);
}
