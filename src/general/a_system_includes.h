/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_GENERAL_SYSTEM_INCLUDES_H
#define A_INC_GENERAL_SYSTEM_INCLUDES_H

#define A_UNUSED(X) (X = X)
#define A_ARRAY_LEN(A) (sizeof(A) / sizeof(A[0]))

#define A_FLAGS_BIT(N) (1u << (unsigned)(N))
#define A_FLAGS_SET(Value, Mask) ((Value) |= (unsigned)(Mask))
#define A_FLAGS_CLEAR(Value, Mask) ((Value) &= ~(unsigned)(Mask))
#define A_FLAGS_TEST_ANY(Value, Mask) !!((unsigned)(Value) & (unsigned)(Mask))
#define A_FLAGS_TEST_ALL(Value, Mask) (((unsigned)(Value) & (unsigned)(Mask)) == (unsigned)(Mask))

#define A__STRINGIFY_DO_NOT_CALL(X) #X
#define A_STRINGIFY(X) A__STRINGIFY_DO_NOT_CALL(X)

typedef void AFree(void* Self);

typedef volatile int AEvent;

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if CHAR_BIT != 8
    #error Code needs CHAR_BIT == 8
#endif

#endif // A_INC_GENERAL_SYSTEM_INCLUDES_H
