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

#ifndef F_INC_MEMORY_POOL_V_H
#define F_INC_MEMORY_POOL_V_H

#include "f_pool.p.h"

typedef enum {
    F_POOL__BLOCK,
    F_POOL__CONSOLE,
    F_POOL__ENTITY,
    F_POOL__HASHENTRY,
    F_POOL__LIST,
    F_POOL__LISTINTR,
    F_POOL__LISTNODE,
    F_POOL__PATH,
    F_POOL__SAMPLE,
    F_POOL__SPRITE,
    F_POOL__SPRITE_LAYER,
    F_POOL__STACK_ALIGN,
    F_POOL__STACK_COLOR,
    F_POOL__STACK_FONT,
    F_POOL__STACK_SCREEN,
    F_POOL__STACK_STATE,
    F_POOL__TIMER,
    F_POOL__NUM
} FPoolId;

typedef union FPoolEntryHeader FPoolEntryHeader;
typedef struct FPoolSlab FPoolSlab;

#include "../general/f_init.v.h"
#include "../memory/f_mem.v.h"

union FPoolEntryHeader {
    FPoolEntryHeader* nextFreeEntry; // Next free entry in a slab
    FPool* parentPool; // Pool this active entry belongs to
    FMaxMemAlignType alignment; // Used to prompt max alignment padding
};

struct FPoolSlab {
    FPoolSlab* nextSlab; // Next slab in a pool's slab list
    FPoolEntryHeader buffer[1]; // Memory space for objects
};

struct FPool {
    unsigned objSize; // Size of a user object within a pool entry
    unsigned entrySize; // Size of each pool entry in a slab
    unsigned numEntriesPerSlab; // Grows with usage
    FPoolSlab* slabList; // Keeps track of all allocated slabs
    FPoolEntryHeader* freeEntryList; // Head of the free pool entries list
};

extern const FPack f_pack__pool;

extern void* f_pool__alloc(FPoolId Pool);
extern void* f_pool__dup(FPoolId Pool, const void* Buffer);

#endif // F_INC_MEMORY_POOL_V_H
