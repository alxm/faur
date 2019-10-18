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

#ifndef A_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
#define A_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H

#include <sketch/config-faur.h>

#ifndef A_CONFIG_APP_AUTHOR
#define A_CONFIG_APP_AUTHOR "you"
#endif

#ifndef A_CONFIG_APP_NAME
#define A_CONFIG_APP_NAME "Untitled"
#endif

#ifndef A_CONFIG_APP_VERSION_MAJOR
#define A_CONFIG_APP_VERSION_MAJOR 1
#endif

#ifndef A_CONFIG_APP_VERSION_MINOR
#define A_CONFIG_APP_VERSION_MINOR 0
#endif

#ifndef A_CONFIG_APP_VERSION_MICRO
#define A_CONFIG_APP_VERSION_MICRO 0
#endif

#ifndef A_CONFIG_APP_VERSION_STRING
#define A_CONFIG_APP_VERSION_STRING \
    A_STRINGIFY(A_GLUE5(A_CONFIG_APP_VERSION_MAJOR, ., \
                        A_CONFIG_APP_VERSION_MINOR, ., \
                        A_CONFIG_APP_VERSION_MICRO))
#endif

#ifndef A_CONFIG_BUILD_DEBUG_FATAL_SPIN
#define A_CONFIG_BUILD_DEBUG_FATAL_SPIN 1
#endif

#ifndef A_CONFIG_BUILD_FIX_LUT
#define A_CONFIG_BUILD_FIX_LUT 1
#endif

#ifndef A_CONFIG_BUILD_TIMESTAMP
#define A_CONFIG_BUILD_TIMESTAMP __DATE__ " " __TIME__
#endif

#ifndef A_CONFIG_BUILD_UID
#define A_CONFIG_BUILD_UID "gamebuino_arduino"
#endif

#ifndef A_CONFIG_COLOR_SPRITE_BORDER
#define A_CONFIG_COLOR_SPRITE_BORDER 0x00FF00
#endif

#ifndef A_CONFIG_COLOR_SPRITE_KEY
#define A_CONFIG_COLOR_SPRITE_KEY 0xFF00FF
#endif

#ifndef A_CONFIG_FPS_HISTORY
#define A_CONFIG_FPS_HISTORY 1
#endif

#ifndef A_CONFIG_FPS_RATE_DRAW
#define A_CONFIG_FPS_RATE_DRAW 30
#endif

#ifndef A_CONFIG_FPS_RATE_TICK
#define A_CONFIG_FPS_RATE_TICK 30
#endif

#ifndef A_CONFIG_LIB_RENDER_SOFTWARE
#define A_CONFIG_LIB_RENDER_SOFTWARE 1
#endif

#ifndef A_CONFIG_PATH_STORAGE_PREFIX
#define A_CONFIG_PATH_STORAGE_PREFIX "./"
#endif

#ifndef A_CONFIG_SCREEN_BPP
#define A_CONFIG_SCREEN_BPP 16
#endif

#ifndef A_CONFIG_SCREEN_FORMAT_RGBA
#define A_CONFIG_SCREEN_FORMAT_RGBA 1
#endif

#ifndef A_CONFIG_SCREEN_HARDWARE_HEIGHT
#define A_CONFIG_SCREEN_HARDWARE_HEIGHT 64
#endif

#ifndef A_CONFIG_SCREEN_HARDWARE_WIDTH
#define A_CONFIG_SCREEN_HARDWARE_WIDTH 80
#endif

#ifndef A_CONFIG_SCREEN_SIZE_HEIGHT
#define A_CONFIG_SCREEN_SIZE_HEIGHT 64
#endif

#ifndef A_CONFIG_SCREEN_SIZE_WIDTH
#define A_CONFIG_SCREEN_SIZE_WIDTH 80
#endif

#ifndef A_CONFIG_SOUND_ENABLED
#define A_CONFIG_SOUND_ENABLED 0
#endif

#ifndef A_CONFIG_SYSTEM_GAMEBUINO
#define A_CONFIG_SYSTEM_GAMEBUINO 1
#endif

#endif // A_INC_PLATFORM_SYSTEM_GAMEBUINO_CONFIG_H
