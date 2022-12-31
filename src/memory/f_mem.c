/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_DEBUG_MEM_MALLOC
size_t f_mem__tally, f_mem__top;

static inline void tallyAdd(size_t Size)
{
    f_mem__tally += Size;
    f_mem__top = f_math_maxz(f_mem__top, f_mem__tally);
}
#endif

void* f_mem_malloc(size_t Size)
{
    F__CHECK(Size > 0);

    #if F_CONFIG_DEBUG_MEM_MALLOC
        size_t total = Size + sizeof(FMaxMemAlignType);
        FMaxMemAlignType* ptr = f_platform_api__malloc(total);

        if(ptr == NULL) {
            F__FATAL("malloc(%zu) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = f_platform_api__malloc(Size);

        if(ptr == NULL) {
            F__FATAL("malloc(%zu) failed", Size);
        }

        return ptr;
    #endif
}

void* f_mem_mallocz(size_t Size)
{
    F__CHECK(Size > 0);

    #if F_CONFIG_DEBUG_MEM_MALLOC
        size_t total = Size + sizeof(FMaxMemAlignType);
        FMaxMemAlignType* ptr = f_platform_api__mallocz(total);

        if(ptr == NULL) {
            F__FATAL("calloc(1, %zu) failed", total);
        }

        ptr->u_size = total;

        tallyAdd(total);

        return ptr + 1;
    #else
        void* ptr = f_platform_api__mallocz(Size);

        if(ptr == NULL) {
            F__FATAL("calloc(1, %zu) failed", Size);
        }

        return ptr;
    #endif
}

void* f_mem_malloca(size_t Size, unsigned AlignExp)
{
    F__CHECK(Size > 0);
    F__CHECK((1u << AlignExp) >= sizeof(void*));

    uintptr_t mask = (uintptr_t)((1u << AlignExp) - 1);
    void* ptr = f_mem_malloc(sizeof(void*) + Size + mask);
    uintptr_t adj = ((uintptr_t)ptr + sizeof(void*) + mask) & ~mask;

    *((void**)adj - 1) = ptr;

    return (void*)adj;
}

void* f_mem_dup(const void* Buffer, size_t Size)
{
    F__CHECK(Buffer != NULL);
    F__CHECK(Size > 0);

    void* copy = f_mem_malloc(Size);

    memcpy(copy, Buffer, Size);

    return copy;
}

void f_mem_free(void* Buffer)
{
    if(Buffer == NULL) {
        return;
    }

    #if F_CONFIG_DEBUG_MEM_MALLOC
        FMaxMemAlignType* header = (FMaxMemAlignType*)Buffer - 1;

        f_mem__tally -= header->u_size;

        free(header);
    #else
        free(Buffer);
    #endif
}

void f_mem_freea(void* Buffer)
{
    if(Buffer == NULL) {
        return;
    }

    f_mem_free(*((void**)Buffer - 1));
}
