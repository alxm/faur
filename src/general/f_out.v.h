/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_OUT_V_H
#define F_INC_GENERAL_OUT_V_H

#include "general/f_out.p.h"

typedef enum {
    F_OUT__SOURCE_INVALID = -1,
    F_OUT__SOURCE_FAUR,
    F_OUT__SOURCE_APP,
    F_OUT__SOURCE_NUM
} FOutSource;

typedef enum {
    F_OUT__TYPE_INVALID = -1,
    F_OUT__TYPE_INFO,
    F_OUT__TYPE_WARNING,
    F_OUT__TYPE_ERROR,
    F_OUT__TYPE_STATE,
    F_OUT__TYPE_FATAL,
    F_OUT__TYPE_NUM
} FOutType;

#if F_CONFIG_FEATURE_OUTPUT
    extern void f_out__info(const char* Format, ...);
    extern void f_out__warning(const char* Format, ...);
    extern void f_out__error(const char* Format, ...);
    extern void f_out__errorv(const char* Format, va_list Args);
    extern void f_out__state(const char* Format, ...);
#else
    #define f_out__info(...)
    #define f_out__warning(...)
    #define f_out__error(...)
    #define f_out__errorv(Format, Args)
    #define f_out__state(...)
#endif

#endif // F_INC_GENERAL_OUT_V_H
