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

#ifndef A2X_PACK_MEM_PH
#define A2X_PACK_MEM_PH

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "a2x_pack_list.p.h"

extern void* a_mem_track(void* const v);

#define a_mem_mallocz(size)            \
({                                     \
    void* const a__m = malloc((size)); \
    memset(a__m, 0, (size));           \
    a__m;                              \
})

#define a_mem_make(c)                              \
({                                                 \
    typeof(c)* const a__m = malloc(sizeof(*a__m)); \
    *a__m = c;                                     \
    a__m;                                          \
})

extern void* a_mem_encodeRLE(const void* const data, const int length, const int size, int* const encLength);
extern void* a_mem_decodeRLE(const void* const data, const int length, const int size, int* const decLength);

#endif // A2X_PACK_MEM_PH
