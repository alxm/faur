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

#ifndef F_CONFIG_BUILD_FIX_LUT
#define F_CONFIG_BUILD_FIX_LUT 1
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

#ifndef F_CONFIG_FPS_HISTORY
#define F_CONFIG_FPS_HISTORY 1
#endif

#ifndef F_CONFIG_FPS_RATE_DRAW
#define F_CONFIG_FPS_RATE_DRAW 30
#endif

#ifndef F_CONFIG_FPS_RATE_TICK
#define F_CONFIG_FPS_RATE_TICK 30
#endif

#ifndef F_CONFIG_LIB_RENDER_SOFTWARE
#define F_CONFIG_LIB_RENDER_SOFTWARE 1
#endif

#ifndef F_CONFIG_PATH_STORAGE_PREFIX
#define F_CONFIG_PATH_STORAGE_PREFIX "./"
#endif

#ifndef F_CONFIG_SCREEN_BPP
#define F_CONFIG_SCREEN_BPP 16
#endif

#ifndef F_CONFIG_SCREEN_FORMAT_RGBA
#define F_CONFIG_SCREEN_FORMAT_RGBA 1
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

#ifndef F_CONFIG_SOUND_ENABLED
#define F_CONFIG_SOUND_ENABLED 0
#endif

#ifndef F_CONFIG_SYSTEM_GAMEBUINO
#define F_CONFIG_SYSTEM_GAMEBUINO 1
#endif

#endif // F_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
