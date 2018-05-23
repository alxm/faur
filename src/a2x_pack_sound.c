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

#include "a2x_system_includes.h"
#include "a2x_pack_sound.v.h"

#include "a2x_pack_draw.v.h"
#include "a2x_pack_embed.v.h"
#include "a2x_pack_file.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_timer.v.h"

struct AMusic {
    APlatformMusic* platformMusic;
};

struct ASfx {
    APlatformSfx* platformSfx;
    int channel;
};

static bool g_soundOn;
static int g_volume;
static int g_musicVolume;
static int g_sfxVolume;
static int g_volumeMax;

#if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
    #define A_VOLUME_STEP 1
    #define A_VOLBAR_SHOW_MS 500
    static ATimer* g_volTimer;
    static AInputButton* g_volumeUpButton;
    static AInputButton* g_volumeDownButton;
    static APixel g_volbarBackground;
    static APixel g_volbarBorder;
    static APixel g_volbarFill;
#endif

#if A_DEVICE_HAS_KEYBOARD
    static AInputButton* g_musicOnOffButton;
#endif

static void adjustSoundVolume(int Volume)
{
    g_volume = a_math_clamp(Volume, 0, g_volumeMax);
    g_musicVolume = a_settings_getInt("sound.music.scale") * g_volume / 100;
    g_sfxVolume = a_settings_getInt("sound.sfx.scale") * g_volume / 100;

    a_platform__setSfxVolumeAll(g_sfxVolume);
    a_platform__setMusicVolume(g_musicVolume);
}

static void inputCallback(void)
{
    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        if(!g_soundOn) {
            return;
        }

        int adjust = 0;

        if(a_button_getPressed(g_volumeUpButton)) {
            adjust = A_VOLUME_STEP;
        } else if(a_button_getPressed(g_volumeDownButton)) {
            adjust = -A_VOLUME_STEP;
        }

        if(adjust) {
            adjustSoundVolume(g_volume + adjust);
            a_timer_start(g_volTimer);
        }
    #endif

    #if A_DEVICE_HAS_KEYBOARD
        if(g_soundOn && a_button_getPressedOnce(g_musicOnOffButton)) {
            a_platform__toggleMusic();
        }
    #endif
}

#if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
    static void screenCallback(void)
    {
        if(!g_soundOn
            || !a_timer_isRunning(g_volTimer)
            || a_timer_isExpired(g_volTimer)) {

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
#endif

void a_sound__init(void)
{
    g_soundOn = a_settings_getBool("sound.on");

    if(!g_soundOn) {
        return;
    }

    g_volumeMax = a_platform__getMaxVolome();

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        adjustSoundVolume(g_volumeMax / 16);
        g_volTimer = a_timer_new(A_TIMER_MS, A_VOLBAR_SHOW_MS, false);
    #else
        adjustSoundVolume(g_volumeMax);
    #endif

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        g_volumeUpButton = a_button_new("gamepad.b.volUp");
        g_volumeDownButton = a_button_new("gamepad.b.volDown");
    #endif

    #if A_DEVICE_HAS_KEYBOARD
        g_musicOnOffButton = a_button_new("key.m");
    #endif

    a_input__addCallback(inputCallback);

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        const char* color;

        color = a_settings_getString("sound.volbar.background");
        g_volbarBackground = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.border");
        g_volbarBorder = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));

        color = a_settings_getString("sound.volbar.fill");
        g_volbarFill = a_pixel_fromHex((uint32_t)strtol(color, NULL, 16));

        a_screen__addOverlay(screenCallback);
    #endif
}

void a_sound__uninit(void)
{
    if(!g_soundOn) {
        return;
    }

    #if A_PLATFORM_SYSTEM_GP2X || A_PLATFORM_SYSTEM_WIZ
        a_timer_free(g_volTimer);
    #endif

    #if A_DEVICE_HAS_KEYBOARD
        a_button_free(g_musicOnOffButton);
    #endif

    a_music_stop();
}

AMusic* a_music_new(const char* Path)
{
    if(!g_soundOn) {
        return NULL;
    }

    AMusic* m = a_mem_malloc(sizeof(AMusic));

    m->platformMusic = a_platform__newMusic(Path);

    return m;
}

void a_music_free(AMusic* Music)
{
    if(!g_soundOn) {
        return;
    }

    if(Music->platformMusic) {
        a_platform__freeMusic(Music->platformMusic);
    }

    free(Music);
}

void a_music_play(const AMusic* Music)
{
    if(g_soundOn && Music->platformMusic) {
        a_platform__playMusic(Music->platformMusic);
    }
}

void a_music_stop(void)
{
    if(g_soundOn) {
        a_platform__stopMusic();
    }
}

ASfx* a_sfx_new(const char* Path)
{
    if(!g_soundOn) {
        return NULL;
    }

    ASfx* s = a_mem_malloc(sizeof(ASfx));

    if(a_file_exists(Path)) {
        s->platformSfx = a_platform__newSfxFromFile(Path);
    } else {
        const uint8_t* data;
        size_t size;

        if(a_embed__get(Path, &data, &size)) {
            s->platformSfx = a_platform__newSfxFromData(data, (int)size);
        } else {
            s->platformSfx = NULL;
        }
    }

    if(s->platformSfx) {
        s->channel = a_platform__getSfxChannel();
    }

    return s;
}

ASfx* a_sfx_dup(const ASfx* Sfx)
{
    ASfx* s = a_mem_dup(Sfx, sizeof(ASfx));

    if(s->platformSfx) {
        s->channel = a_platform__getSfxChannel();
        a_platform__referenceSfx(s->platformSfx);
    }

    return s;
}

void a_sfx_free(ASfx* Sfx)
{
    if(!g_soundOn) {
        return;
    }

    if(Sfx->platformSfx) {
        a_platform__freeSfx(Sfx->platformSfx);
    }

    free(Sfx);
}

void a_sfx_play(const ASfx* Sfx, ASfxFlags Flags)
{
    if(!g_soundOn || Sfx->platformSfx == NULL) {
        return;
    }

    if(Flags & A_SFX_RESTART) {
        a_platform__stopSfx(Sfx->channel);
    } else if(Flags & A_SFX_YIELD) {
        if(a_platform__isSfxPlaying(Sfx->channel)) {
            return;
        }
    }

    a_platform__playSfx(Sfx->platformSfx,
                        Flags & (A_SFX_RESTART | A_SFX_YIELD)
                            ? Sfx->channel
                            : -1,
                        Flags & A_SFX_LOOP);
}

void a_sfx_stop(const ASfx* Sfx)
{
    if(g_soundOn && Sfx->platformSfx) {
        a_platform__stopSfx(Sfx->channel);
    }
}

bool a_sfx_isPlaying(const ASfx* Sfx)
{
    return g_soundOn && Sfx->platformSfx
        && a_platform__isSfxPlaying(Sfx->channel);
}
