/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
#define F_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H

#if !F_CONFIG_MAKE
    #define F_CONFIG_APP_AUTHOR "Unknown"
    #define F_CONFIG_APP_NAME "Untitled"

    #define F_CONFIG_BUILD_DEBUG 0
    #define F_CONFIG_BUILD_DEBUG_FATAL_SPIN 1
    #define F_CONFIG_BUILD_FAUR_GIT "(commit unknown)"
    #define F_CONFIG_BUILD_FAUR_TIME __DATE__ " " __TIME__
    #define F_CONFIG_BUILD_UID "gamebuino_meta_arduino"

    #define F_CONFIG_COLOR_SPRITE_BORDER 0x00FF00
    #define F_CONFIG_COLOR_SPRITE_KEY 0xFF00FF

    #define F_CONFIG_FILES_PREFIX ""

    #define F_CONFIG_FIX_LUT 1

    #define F_CONFIG_SOUND_ENABLED 1

    #define F_CONFIG_RENDER_SOFTWARE 1

    #define F_CONFIG_SCREEN_FORMAT F_COLOR_FORMAT_RGB_565
    #define F_CONFIG_SCREEN_HARDWARE_HEIGHT 64
    #define F_CONFIG_SCREEN_HARDWARE_WIDTH 80
    #define F_CONFIG_SCREEN_SIZE_HEIGHT 64
    #define F_CONFIG_SCREEN_SIZE_WIDTH 80

    #define F_CONFIG_SYSTEM_ARDUINO 1
    #define F_CONFIG_SYSTEM_GAMEBUINO 1
    #define F_CONFIG_SYSTEM_GAMEBUINO_NO_EXIT 0
#endif // !F_CONFIG_MAKE

// The Gamebuino target also requires the application to provide the file
// `<src>/config-gamebuino.h`, which is used by the Gamebuino_META lib for its
// own build-time settings. `config-gamebuino.h` only has to #include <faur.h>,
// which sets all the Gamebuino_META options it needs.

#undef FOLDER_NAME
#define FOLDER_NAME F_CONFIG_APP_NAME

#undef DISPLAY_MODE
#define DISPLAY_MODE DISPLAY_MODE_RGB565

#undef SOUND_CHANNELS
#define SOUND_CHANNELS 1

#undef SOUND_FREQ
#define SOUND_FREQ 22050

#if F_CONFIG_SYSTEM_GAMEBUINO_NO_EXIT
    #undef HOME_MENU_NO_EXIT
    #define HOME_MENU_NO_EXIT 1
#endif

#endif // F_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
