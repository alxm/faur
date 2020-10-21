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

typedef struct {
    size_t typeSize;
} FPoolInit;

static const FPoolInit g_poolsInit[F_POOL__NUM] = {
    [F_POOL__BLOCK] = {sizeof(FBlock)},
    [F_POOL__CONSOLE] = {sizeof(FConsoleLine)},
    [F_POOL__LIST] = {sizeof(FList)},
    [F_POOL__LISTNODE] = {sizeof(FListNode)},
    [F_POOL__PATH] = {sizeof(FPath)},
    [F_POOL__SAMPLE] = {sizeof(FSample)},
    [F_POOL__SPRITE] = {sizeof(FSprite)},
    [F_POOL__STACK_ALIGN] = {sizeof(FAlign)},
    [F_POOL__STACK_COLOR] = {sizeof(FColorState)},
    [F_POOL__STACK_FONT] = {sizeof(FFontState)},
    [F_POOL__STACK_SCREEN] = {sizeof(FScreen)},
    [F_POOL__TIMER] = {sizeof(FTimer)},
};

static FPool* g_pools[F_POOL__NUM];

static void f_pool__init(void)
{
    for(int p = F_POOL__NUM; p--; ) {
        const FPoolInit* init = &g_poolsInit[p];

        g_pools[p] = f_pool_new(init->typeSize);
    }
}

static void f_pool__uninit(void)
{
    for(int p = F_POOL__NUM; p--; ) {
        f_pool_free(g_pools[p]);
    }
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

void* f_pool__alloc(FPoolId Pool)
{
    return f_pool_alloc(g_pools[Pool]);
}

void* f_pool__dup(FPoolId Pool, const void* Buffer)
{
    void* copy = f_pool__alloc(Pool);

    memcpy(copy, Buffer, g_poolsInit[Pool].typeSize);

    return copy;
}
