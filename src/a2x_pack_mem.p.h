/*
    Copyright 2010 Alex Margarit

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

#include "a2x_app_includes.h"

#define a_mem_mallocz(size)            \
({                                     \
    void* const a__m = malloc((size)); \
    memset(a__m, 0, (size));           \
    a__m;                              \
})

#define a_mem_make(c)                                \
({                                                   \
    __typeof(c)* const a__m = malloc(sizeof(*a__m)); \
    *a__m = c;                                       \
    a__m;                                            \
})

extern void* a_mem_encodeRLE(const void* data, int length, int size, int* encLength);
extern void* a_mem_decodeRLE(const void* data, int length, int size, int* decLength);
