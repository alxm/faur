/*
    Copyright 2020 Alex Margarit <alex@alxm.org>
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

#include "f_pool.v.h"
#include <faur.v.h>

#define F__SLAB_OBJ_NUM 8

typedef union FPoolHeader {
    union FPoolHeader* nextFree; // Next free object memory in a pool slab
    FPool* pool; // Pool this object belongs to
} FPoolHeader;

typedef struct FPoolSlab {
    struct FPoolSlab* nextSlab; // Next slab in a pool's slab list
    FMaxMemAlignType buffer[]; // FPool->objSize * F__SLAB_OBJ_NUM bytes
} FPoolSlab;

struct FPool {
    unsigned objSize; // Size of each individual object in a slab
    struct FPoolSlab* slabList; // All slabs used by this pool
    union FPoolHeader* freeList; // Pointer to a free obj memory in a slab
};

static void f_pool__init(void)
{
    //
}

static void f_pool__uninit(void)
{
    //
}

const FPack f_pack__pool = {
    "Pool",
    f_pool__init,
    f_pool__uninit
};

FPool* f_pool_new(unsigned Size)
{
    F_UNUSED(Size);

    return NULL;
}

void f_pool_free(FPool* Pool)
{
    F_UNUSED(Pool);
}

void* f_pool_alloc(FPool* Pool)
{
    F_UNUSED(Pool);

    return NULL;
}

void f_pool_release(void* Pointer)
{
    F_UNUSED(Pointer);
}
