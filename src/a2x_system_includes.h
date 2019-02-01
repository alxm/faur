/*
    Copyright 2011, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#define A_UNUSED(X) (X = X)
#define A_ARRAY_LEN(A) (sizeof(A) / sizeof(A[0]))

#define A_FLAG_BIT(N) (1 << (N))
#define A_FLAG_SET(Value, Mask) ((Value) |= (Mask))
#define A_FLAG_CLEAR(Value, Mask) ((Value) &= ~(unsigned)(Mask))
#define A_FLAG_TEST_ANY(Value, Mask) !!((Value) & (Mask))
#define A_FLAG_TEST_ALL(Value, Mask) (((Value) & (Mask)) == (Mask))

#define A__STRINGIFY_DO_NOT_CALL(X) #X
#define A_STRINGIFY(X) A__STRINGIFY_DO_NOT_CALL(X)

typedef void AFree(void* Self);
typedef void AInit(void* Self);

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
