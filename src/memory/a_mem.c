/*
    Copyright 2010, 2016-2017, 2019 Alex Margarit <alex@alxm.org>
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

#include "a_mem.v.h"
#include <a2x.v.h>

typedef union {
    intmax_t u_intmax;
    long double u_longdouble;
    size_t u_size;
    void* u_voidp;
    void (*u_funcp)(void);
} AMaxMemAlignType;

size_t g_tally;

void* a_mem_malloc(size_t Size)
{
    size_t total = Size + sizeof(AMaxMemAlignType);
    AMaxMemAlignType* ptr = malloc(total);

    if(ptr == NULL) {
        A__FATAL("malloc(%u) failed", total);
    }

    ptr->u_size = total;
    g_tally += total;

    return ptr + 1;
}

void* a_mem_zalloc(size_t Size)
{
    size_t total = Size + sizeof(AMaxMemAlignType);
    AMaxMemAlignType* ptr = calloc(1, total);

    if(ptr == NULL) {
        A__FATAL("calloc(1, %u) failed", total);
    }

    ptr->u_size = total;
    g_tally += total;

    return ptr + 1;
}

void* a_mem_dup(const void* Buffer, size_t Size)
{
    void* copy = a_mem_malloc(Size);

    memcpy(copy, Buffer, Size);

    return copy;
}

void a_mem_free(void* Buffer)
{
    if(Buffer == NULL) {
        return;
    }

    AMaxMemAlignType* header = (AMaxMemAlignType*)Buffer - 1;

    g_tally -= header->u_size;

    free(header);
}

size_t a_mem__bytesGetUsed(void)
{
    return g_tally;
}
