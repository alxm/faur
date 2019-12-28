/*
    Copyright 2010, 2016-2017, 2019 Alex Margarit <alex@alxm.org>
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

#include "f_mem.v.h"
#include <faur.v.h>

#if F_CONFIG_BUILD_DEBUG_ALLOC
size_t f_mem__tally, f_mem__top;

static inline void tallyAdd(size_t Size)
{
    f_mem__tally += Size;
    f_mem__top = f_math_maxz(f_mem__top, f_mem__tally);
}
#endif

#if F_CONFIG_SYSTEM_ODROID_GO
    extern void *ps_malloc(size_t size);
    extern void *ps_calloc(size_t n, size_t size);

    #define malloc(Size) ps_malloc(Size)
    #define calloc(N, Size) ps_calloc(N, Size)
#endif

void* f_mem_malloc(size_t Size)
{
    #if F_CONFIG_BUILD_DEBUG_ALLOC
        size_t total = Size + sizeof(FMaxMemAlignType);
        FMaxMemAlignType* ptr = malloc(total);

        if(ptr == NULL) {
            F__FATAL("malloc(%zu) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = malloc(Size);

        if(ptr == NULL) {
            F__FATAL("malloc(%zu) failed", Size);
        }

        return ptr;
    #endif
}

void* f_mem_zalloc(size_t Size)
{
    #if F_CONFIG_BUILD_DEBUG_ALLOC
        size_t total = Size + sizeof(FMaxMemAlignType);
        FMaxMemAlignType* ptr = calloc(1, total);

        if(ptr == NULL) {
            F__FATAL("calloc(1, %zu) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = calloc(1, Size);

        if(ptr == NULL) {
            F__FATAL("calloc(1, %zu) failed", Size);
        }

        return ptr;
    #endif
}

void* f_mem_dup(const void* Buffer, size_t Size)
{
    void* copy = f_mem_malloc(Size);

    memcpy(copy, Buffer, Size);

    return copy;
}

void f_mem_free(void* Buffer)
{
    if(Buffer == NULL) {
        return;
    }

    #if F_CONFIG_BUILD_DEBUG_ALLOC
        FMaxMemAlignType* header = (FMaxMemAlignType*)Buffer - 1;

        f_mem__tally -= header->u_size;

        free(header);
    #else
        free(Buffer);
    #endif
}
