/*
    Copyright 2011, 2016, 2017 Alex Margarit

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

#ifndef A_PLATFORM_LINUXPC
    #define A_PLATFORM_LINUXPC 0
#endif

#ifndef A_PLATFORM_MINGW
    #define A_PLATFORM_MINGW 0
#endif

#ifndef A_PLATFORM_CAANOO
    #define A_PLATFORM_CAANOO 0
#endif

#ifndef A_PLATFORM_WIZ
    #define A_PLATFORM_WIZ 0
#endif

#ifndef A_PLATFORM_GP2X
    #define A_PLATFORM_GP2X 0
#endif

#ifndef A_PLATFORM_PANDORA
    #define A_PLATFORM_PANDORA 0
#endif

#ifndef A_PLATFORM_GP32
    #define A_PLATFORM_GP32 0
#endif

#ifndef A_PLATFORM_LINUX
    #if A_PLATFORM_LINUXPC || A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO || A_PLATFORM_PANDORA
        #define A_PLATFORM_LINUX 1
    #else
        #define A_PLATFORM_LINUX 0
    #endif
#endif

#ifndef A_CONFIG_LIB_SDL
    #define A_CONFIG_LIB_SDL 0
#endif

#ifndef A_CONFIG_RENDER_SOFTWARE
    #define A_CONFIG_RENDER_SOFTWARE 0
#endif

#ifndef A_CONFIG_RENDER_SDL2
    #define A_CONFIG_RENDER_SDL2 0
#endif

// For putenv
#define _XOPEN_SOURCE

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    #include <sys/ioctl.h>
    #include <sys/mman.h>
#endif
