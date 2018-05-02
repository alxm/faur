/*
    Copyright 2016-2018 Alex Margarit

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

#include "a2x_pack_out.p.h"

typedef enum {
    A_OUT__SOURCE_A2X,
    A_OUT__SOURCE_APP,
    A_OUT__SOURCE_NUM
} AOutSource;

typedef enum {
    A_OUT__TYPE_MESSAGE,
    A_OUT__TYPE_WARNING,
    A_OUT__TYPE_ERROR,
    A_OUT__TYPE_STATE,
    A_OUT__TYPE_FATAL,
    A_OUT__TYPE_NUM
} AOutType;

extern void a_out__message(const char* Format, ...);
extern void a_out__warning(const char* Format, ...);
extern void a_out__warningv(const char* Format, ...);
extern void a_out__error(const char* Format, ...);
extern void a_out__errorv(const char* Format, ...);
extern void a_out__fatal(const char* Format, ...) __attribute__((noreturn));
extern void a_out__state(const char* Format, ...);
extern void a_out__statev(const char* Format, ...);
extern void a_out__overwrite(AOutType Type, const char* Format, ...);
