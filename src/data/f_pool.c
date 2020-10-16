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

#define F__ENTRIES_NUM 8

typedef union FPoolEntryHeader FPoolEntryHeader;
typedef struct FPoolSlab FPoolSlab;

union FPoolEntryHeader {
    FPoolEntryHeader* nextFreeEntry; // Next free entry in a slab
    FPool* parentPool; // Pool this active entry belongs to
    FMaxMemAlignType alignment; // Used to prompt max alignment padding
};

struct FPoolSlab {
    FPoolSlab* nextSlab; // Next slab in a pool's slab list
    FPoolEntryHeader buffer[]; // Memory space for objects
};

struct FPool {
    size_t objSize; // Size of a user object within a pool entry
    size_t entrySize; // Size of each pool entry in a slab
    FPoolSlab* slabList; // Keeps track of all allocated slabs
    FPoolEntryHeader* freeEntryList; // Head of the free pool entries list
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

static void slab_new(FPool* Pool)
{
    FPoolSlab* s = f_mem_malloc(
                    sizeof(FPoolSlab) + Pool->entrySize * F__ENTRIES_NUM);

    s->nextSlab = Pool->slabList;

    Pool->slabList = s;
    Pool->freeEntryList = s->buffer;

    FPoolEntryHeader* entry;
    FPoolEntryHeader* lastEntry = s->buffer;

    for(unsigned e = 1; e < F__ENTRIES_NUM; e++) {
        entry = (FPoolEntryHeader*)((uintptr_t)s->buffer + e * Pool->entrySize);

        lastEntry->nextFreeEntry = entry;
        lastEntry = entry;
    }

    lastEntry->nextFreeEntry = NULL;
}

FPool* f_pool_new(size_t Size)
{
    FPool* p = f_mem_malloc(sizeof(FPool));

    p->objSize = Size;
    p->entrySize = sizeof(FPoolEntryHeader)
                    + ((Size + sizeof(FMaxMemAlignType) - 1)
                            & ~(sizeof(FMaxMemAlignType) - 1));
    p->slabList = NULL;
    p->freeEntryList = NULL;

    return p;
}

void f_pool_free(FPool* Pool)
{
    for(FPoolSlab* slab = Pool->slabList; slab != NULL; ) {
        FPoolSlab* nextSlab = slab->nextSlab;

        f_mem_free(slab);

        slab = nextSlab;
    }

    f_mem_free(Pool);
}

void* f_pool_alloc(FPool* Pool)
{
    if(Pool->freeEntryList == NULL) {
        slab_new(Pool);
    }

    FPoolEntryHeader* entry = Pool->freeEntryList;

    Pool->freeEntryList = entry->nextFreeEntry;
    entry->parentPool = Pool;

    void* userBuffer = entry + 1;

    memset(userBuffer, 0, Pool->objSize);

    return userBuffer;
}

void f_pool_release(void* Pointer)
{
    FPoolEntryHeader* entry = (FPoolEntryHeader*)Pointer - 1;
    FPool* pool = entry->parentPool;

    entry->nextFreeEntry = pool->freeEntryList;
    pool->freeEntryList = entry;
}
