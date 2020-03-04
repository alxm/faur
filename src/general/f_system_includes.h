/*
    Copyright 2011, 2016-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_SYSTEM_INCLUDES_H
#define F_INC_GENERAL_SYSTEM_INCLUDES_H

#define F_UNUSED(X) (X = X)
#define F_ARRAY_LEN(A) (sizeof(A) / sizeof(A[0]))

#define F_FLAGS_BIT(N) (1u << (unsigned)(N))
#define F_FLAGS_SET(Value, Mask) ((Value) |= (unsigned)(Mask))
#define F_FLAGS_CLEAR(Value, Mask) ((Value) &= ~(unsigned)(Mask))
#define F_FLAGS_TEST_ANY(Value, Mask) !!((unsigned)(Value) & (unsigned)(Mask))
#define F_FLAGS_TEST_ALL(Value, Mask) (((unsigned)(Value) & (unsigned)(Mask)) == (unsigned)(Mask))

#define F__STRINGIFY_DO_NOT_CALL(X) #X
#define F_STRINGIFY(X) F__STRINGIFY_DO_NOT_CALL(X)

#define F__GLUE_DO_NOT_CALL(A, B) A##B
#define F_GLUE2(A, B) F__GLUE_DO_NOT_CALL(A, B)
#define F_GLUE3(A, B, C) F_GLUE2(A, F_GLUE2(B, C))
#define F_GLUE4(A, B, C, D) F_GLUE2(A, F_GLUE3(B, C, D))
#define F_GLUE5(A, B, C, D, E) F_GLUE2(A, F_GLUE4(B, C, D, E))

#define F__ATTRIBUTE_ALIGN(AlignExp) __attribute__((aligned (1 << (AlignExp))))
#define F__ATTRIBUTE_NORETURN __attribute__((noreturn))

#if F_CONFIG_SYSTEM_MINGW
    #define F__ATTRIBUTE_FORMAT(FormatIndex) \
        __attribute__((format (gnu_printf, FormatIndex, FormatIndex + 1)))
#else
    #define F__ATTRIBUTE_FORMAT(FormatIndex) \
        __attribute__((format (printf, FormatIndex, FormatIndex + 1)))
#endif

#define F__APP_VERSION_STRING \
    F_STRINGIFY(F_CONFIG_APP_VERSION_MAJOR) \
        "." F_STRINGIFY(F_CONFIG_APP_VERSION_MINOR) \
        "." F_STRINGIFY(F_CONFIG_APP_VERSION_MICRO)

typedef void FFree(void* Self);

typedef volatile int FEvent;

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if CHAR_BIT != 8
    #error Code needs CHAR_BIT == 8
#endif

#if __SAMD21G18A__
    #include "../platform/config/f_gamebuino_config.h"
#elif ESP_PLATFORM
    #include "../platform/config/f_odroid_go_config.h"
#endif

#endif // F_INC_GENERAL_SYSTEM_INCLUDES_H
