/*
    Copyright 2010, 2016-2017, 2019 Alex Margarit <alex@alxm.org>
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

#include "a_mem.v.h"
#include <a2x.v.h>

#if A_CONFIG_BUILD_DEBUG_ALLOC
size_t a_mem__tally, a_mem__top;

static inline void tallyAdd(size_t Size)
{
    a_mem__tally += Size;
    a_mem__top = a_math_maxz(a_mem__top, a_mem__tally);
}
#endif

void* a_mem_malloc(size_t Size)
{
    #if A_CONFIG_BUILD_DEBUG_ALLOC
        size_t total = Size + sizeof(AMaxMemAlignType);
        AMaxMemAlignType* ptr = malloc(total);

        if(ptr == NULL) {
            A__FATAL("malloc(%u) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = malloc(Size);

        if(ptr == NULL) {
            A__FATAL("malloc(%u) failed", Size);
        }

        return ptr;
    #endif
}

void* a_mem_zalloc(size_t Size)
{
    #if A_CONFIG_BUILD_DEBUG_ALLOC
        size_t total = Size + sizeof(AMaxMemAlignType);
        AMaxMemAlignType* ptr = calloc(1, total);

        if(ptr == NULL) {
            A__FATAL("calloc(1, %u) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = calloc(1, Size);

        if(ptr == NULL) {
            A__FATAL("calloc(1, %u) failed", Size);
        }

        return ptr;
    #endif
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

    #if A_CONFIG_BUILD_DEBUG_ALLOC
        AMaxMemAlignType* header = (AMaxMemAlignType*)Buffer - 1;

        a_mem__tally -= header->u_size;

        free(header);
    #else
        free(Buffer);
    #endif
}
