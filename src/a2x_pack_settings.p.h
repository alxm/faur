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

#pragma once

#include "a2x_system_includes.h"

typedef enum {
    A_SETTING_INVALID = -1,

    A_SETTING_APP_TITLE,
    A_SETTING_APP_VERSION,
    A_SETTING_APP_AUTHOR,
    A_SETTING_APP_BUILDTIME,

    A_SETTING_OUTPUT_ON,
    A_SETTING_OUTPUT_VERBOSE,
    A_SETTING_OUTPUT_CONSOLE,

    A_SETTING_FPS_TICK,
    A_SETTING_FPS_DRAW,

    A_SETTING_VIDEO_WIDTH,
    A_SETTING_VIDEO_HEIGHT,
    A_SETTING_VIDEO_VSYNC,
    A_SETTING_VIDEO_DOUBLEBUFFER,
    A_SETTING_VIDEO_FULLSCREEN,

    A_SETTING_COLOR_SCREEN_BORDER,
    A_SETTING_COLOR_VOLBAR_BACKGROUND,
    A_SETTING_COLOR_VOLBAR_BORDER,
    A_SETTING_COLOR_VOLBAR_FILL,
    A_SETTING_COLOR_KEY,
    A_SETTING_COLOR_LIMIT,
    A_SETTING_COLOR_END,

    A_SETTING_SOUND_MUTE,
    A_SETTING_SOUND_SAMPLE_CHANNELS_TOTAL,
    A_SETTING_SOUND_SAMPLE_CHANNELS_RESERVED,
    A_SETTING_SOUND_VOLUME_SCALE_MUSIC,
    A_SETTING_SOUND_VOLUME_SCALE_SAMPLE,

    A_SETTING_INPUT_MOUSE_HIDECURSOR,
    A_SETTING_INPUT_MOUSE_TRACK,
    A_SETTING_INPUT_ANALOG_AXES_SWITCH,
    A_SETTING_INPUT_ANALOG_AXES_INVERT,

    A_SETTING_FILE_CONFIG,
    A_SETTING_FILE_SCREENSHOTS,
    A_SETTING_FILE_GAMEPADMAP,

    A_SETTING_SYSTEM_GP2X_MENU,
    A_SETTING_SYSTEM_GP2X_MHZ,
    A_SETTING_SYSTEM_WIZ_FIXTEARING,

    A_SETTING_NUM
} ASettingId;

#include "a2x_pack_pixel.p.h"

#define A_SETUP void a__settings_application(void)
extern A_SETUP;

extern bool a_settings_isDefault(ASettingId Setting);

extern bool a_settings_boolGet(ASettingId Setting);
extern void a_settings_boolSet(ASettingId Setting, bool Value);
extern bool a_settings_boolFlip(ASettingId Setting);

extern int a_settings_intGet(ASettingId Setting);
extern void a_settings_intSet(ASettingId Setting, int Value);

extern unsigned a_settings_intuGet(ASettingId Setting);
extern void a_settings_intuSet(ASettingId Setting, unsigned Value);

extern const char* a_settings_stringGet(ASettingId Setting);
extern void a_settings_stringSet(ASettingId Setting, const char* Value);

extern APixel a_settings_colorGet(ASettingId Setting);
extern void a_settings_colorSet(ASettingId Setting, uint32_t Hexcode);
