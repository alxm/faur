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


#include <sketch/config-faur.h>

#ifndef F_CONFIG_APP_AUTHOR
#define F_CONFIG_APP_AUTHOR "you"
#endif

#ifndef F_CONFIG_APP_DIR
#define F_CONFIG_APP_DIR "untitled"
#endif

#ifndef F_CONFIG_APP_NAME
#define F_CONFIG_APP_NAME "Untitled"
#endif

#ifndef F_CONFIG_APP_VERSION_MAJOR
#define F_CONFIG_APP_VERSION_MAJOR 1
#endif

#ifndef F_CONFIG_APP_VERSION_MINOR
#define F_CONFIG_APP_VERSION_MINOR 0
#endif

#ifndef F_CONFIG_APP_VERSION_MICRO
#define F_CONFIG_APP_VERSION_MICRO 0
#endif

#ifndef F_CONFIG_APP_VERSION_STRING
#define F_CONFIG_APP_VERSION_STRING \
    F_STRINGIFY(F_GLUE5(F_CONFIG_APP_VERSION_MAJOR, ., \
                        F_CONFIG_APP_VERSION_MINOR, ., \
                        F_CONFIG_APP_VERSION_MICRO))
#endif

#ifndef F_CONFIG_BUILD_DEBUG_FATAL_SPIN
#define F_CONFIG_BUILD_DEBUG_FATAL_SPIN 1
#endif

#ifndef F_CONFIG_FILES_PREFIX
#define F_CONFIG_FILES_PREFIX ""
#endif

#ifndef F_CONFIG_FIX_LUT
#define F_CONFIG_FIX_LUT 1
#endif

#ifndef F_CONFIG_BUILD_FAUR_GIT
#define F_CONFIG_BUILD_FAUR_GIT "(commit unknown)"
#endif

#ifndef F_CONFIG_BUILD_FAUR_TIME
#define F_CONFIG_BUILD_FAUR_TIME __DATE__ " " __TIME__
#endif

#ifndef F_CONFIG_BUILD_UID
#define F_CONFIG_BUILD_UID "gamebuino_arduino"
#endif

#ifndef F_CONFIG_COLOR_SPRITE_BORDER
#define F_CONFIG_COLOR_SPRITE_BORDER 0x00FF00
#endif

#ifndef F_CONFIG_COLOR_SPRITE_KEY
#define F_CONFIG_COLOR_SPRITE_KEY 0xFF00FF
#endif

#ifndef F_CONFIG_FEATURE_OUTPUT
#define F_CONFIG_FEATURE_OUTPUT 1
#endif

#ifndef F_CONFIG_FEATURE_SOUND
#define F_CONFIG_FEATURE_SOUND 1
#endif

#ifndef F_CONFIG_FILES_LIB_GAMEBUINO
#define F_CONFIG_FILES_LIB_GAMEBUINO 1
#endif

#ifndef F_CONFIG_FPS_HISTORY
#define F_CONFIG_FPS_HISTORY 1
#endif

#ifndef F_CONFIG_FPS_RATE_DRAW
#define F_CONFIG_FPS_RATE_DRAW 30
#endif

#ifndef F_CONFIG_FPS_RATE_TICK
#define F_CONFIG_FPS_RATE_TICK 30
#endif

#if F_CONFIG_FPS_RATE_TICK < 1
    #undef F_CONFIG_FPS_RATE_TICK
    #define F_CONFIG_FPS_RATE_TICK 1
#endif

#if F_CONFIG_FPS_RATE_DRAW < 1
    #undef F_CONFIG_FPS_RATE_DRAW
    #define F_CONFIG_FPS_RATE_DRAW 1
#endif

#if F_CONFIG_FPS_RATE_DRAW < F_CONFIG_FPS_RATE_TICK
    #undef F_CONFIG_FPS_RATE_DRAW
    #define F_CONFIG_FPS_RATE_DRAW F_CONFIG_FPS_RATE_TICK
#endif

#if F_CONFIG_FPS_HISTORY < 1
    #undef F_CONFIG_FPS_HISTORY
    #define F_CONFIG_FPS_HISTORY 1
#endif

#ifndef F_CONFIG_LIB_RENDER_SOFTWARE
#define F_CONFIG_LIB_RENDER_SOFTWARE 1
#endif

#ifndef F_CONFIG_SCREEN_FORMAT
#define F_CONFIG_SCREEN_FORMAT F_COLOR_FORMAT_RGB_565
#endif

#ifndef F_CONFIG_SCREEN_HARDWARE_HEIGHT
#define F_CONFIG_SCREEN_HARDWARE_HEIGHT 64
#endif

#ifndef F_CONFIG_SCREEN_HARDWARE_WIDTH
#define F_CONFIG_SCREEN_HARDWARE_WIDTH 80
#endif

#ifndef F_CONFIG_SCREEN_SIZE_HEIGHT
#define F_CONFIG_SCREEN_SIZE_HEIGHT 64
#endif

#ifndef F_CONFIG_SCREEN_SIZE_WIDTH
#define F_CONFIG_SCREEN_SIZE_WIDTH 80
#endif

#ifndef F_CONFIG_SYSTEM_ARDUINO
#define F_CONFIG_SYSTEM_ARDUINO 1
#endif

#ifndef F_CONFIG_SYSTEM_GAMEBUINO
#define F_CONFIG_SYSTEM_GAMEBUINO 1
#endif

// Application must supply a `/config-gamebuino.h` file that includes <faur.h>
// (for use by Gamebuino_META), and a `/config-faur.h` file (for use by Faur).
#undef FOLDER_NAME
#define FOLDER_NAME F_CONFIG_APP_DIR

#undef DISPLAY_MODE
#define DISPLAY_MODE DISPLAY_MODE_RGB565

#undef SOUND_CHANNELS
#define SOUND_CHANNELS 1

#undef SOUND_FREQ
#define SOUND_FREQ 22050

#endif // F_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
