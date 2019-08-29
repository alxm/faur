/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_GENERAL_OUT_V_H
#define A_INC_GENERAL_OUT_V_H

#include "general/a_out.p.h"

#define A_OUT__STREAM_STDOUT stdout

#if A_CONFIG_SYSTEM_EMSCRIPTEN
    #define A_OUT__STREAM_STDERR stdout
#else
    #define A_OUT__STREAM_STDERR stderr
#endif

typedef enum {
    A_OUT__SOURCE_INVALID = -1,
    A_OUT__SOURCE_A2X,
    A_OUT__SOURCE_APP,
    A_OUT__SOURCE_NUM
} AOutSource;

typedef enum {
    A_OUT__TYPE_INVALID = -1,
    A_OUT__TYPE_INFO,
    A_OUT__TYPE_WARNING,
    A_OUT__TYPE_ERROR,
    A_OUT__TYPE_STATE,
    A_OUT__TYPE_FATAL,
    A_OUT__TYPE_NUM
} AOutType;

extern void a_out__info(const char* Format, ...);
extern void a_out__warning(const char* Format, ...);
extern void a_out__error(const char* Format, ...);
extern void a_out__errorv(const char* Format, va_list Args);
extern void a_out__state(const char* Format, ...);

#endif // A_INC_GENERAL_OUT_V_H
