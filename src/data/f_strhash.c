/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "f_strhash.v.h"
#include <faur.v.h>

#define F_STRHASH__SLOTS 128
#define F_STRHASH__SLOTS_MASK (F_STRHASH__SLOTS - 1)

struct FStrHash {
    FList* entriesList;
    FStrHashEntry* slots[F_STRHASH__SLOTS];
};

struct FStrHashEntry {
    char* key;
    void* content;
    FStrHashEntry* next;
};

static inline unsigned getSlot(const char* Key)
{
    unsigned s = 0;

    for( ; *Key != '\0'; Key++) {
        s = (unsigned)*Key + (s << 5) - s;
    }

    return s & F_STRHASH__SLOTS_MASK;
}

FStrHash* f_strhash_new(void)
{
    FStrHash* h = f_mem_malloc(sizeof(FStrHash));

    h->entriesList = f_list_new();

    for(int i = F_STRHASH__SLOTS; i--; ) {
        h->slots[i] = NULL;
    }

    return h;
}

void f_strhash_free(FStrHash* Hash)
{
    f_strhash_freeEx(Hash, NULL);
}

void f_strhash_freeEx(FStrHash* Hash, FFree* Free)
{
    if(Hash == NULL) {
        return;
    }

    F_LIST_ITERATE(Hash->entriesList, FStrHashEntry*, e) {
        if(Free) {
            Free(e->content);
        }

        f_mem_free(e->key);
        f_mem_free(e);
    }

    f_list_free(Hash->entriesList);

    f_mem_free(Hash);
}

void f_strhash_add(FStrHash* Hash, const char* Key, void* Content)
{
    unsigned slot = getSlot(Key);
    FStrHashEntry* oldEntry = Hash->slots[slot];
    FStrHashEntry* newEntry = f_mem_malloc(sizeof(FStrHashEntry));

    newEntry->key = f_str_dup(Key);
    newEntry->content = Content;
    newEntry->next = oldEntry;

    Hash->slots[slot] = newEntry;
    f_list_addLast(Hash->entriesList, newEntry);
}

void* f_strhash_update(FStrHash* Hash, const char* Key, void* NewContent)
{
    for(FStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            void* oldContent = e->content;
            e->content = NewContent;
            return oldContent;
        }
    }

    return NULL;
}

void* f_strhash_get(const FStrHash* Hash, const char* Key)
{
    for(FStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool f_strhash_contains(const FStrHash* Hash, const char* Key)
{
    for(FStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            return true;
        }
    }

    return false;
}

unsigned f_strhash_sizeGet(const FStrHash* Hash)
{
    return f_list_sizeGet(Hash->entriesList);
}

void** f_strhash_toArray(const FStrHash* Hash)
{
    void** array = f_mem_malloc(
                    f_list_sizeGet(Hash->entriesList) * sizeof(void*));

    F_LIST_ITERATE(Hash->entriesList, const FStrHashEntry*, e) {
        array[F_LIST_INDEX()] = e->content;
    }

    return array;
}

const FList* f__strhash_entries(const FStrHash* Hash)
{
    return Hash->entriesList;
}

void* f__strhash_entryValue(const FStrHashEntry* Entry)
{
    return Entry->content;
}

const char* f__strhash_entryKey(const FStrHashEntry* Entry)
{
    return Entry->key;
}

void f__strhash_printStats(const FStrHash* Hash, const char* Message)
{
    unsigned maxInSlot = 0, maxInSlotNum = 0;
    unsigned occupiedSlots = 0;
    unsigned slotsWithCollisions = 0;

    for(int i = 0; i < F_STRHASH__SLOTS; i++) {
        unsigned entriesInSlot = 0;

        for(FStrHashEntry* e = Hash->slots[i]; e; e = e->next) {
            entriesInSlot++;
        }

        occupiedSlots += entriesInSlot > 0;

        if(entriesInSlot >= 2) {
            slotsWithCollisions++;

            if(entriesInSlot > maxInSlot) {
                maxInSlot = entriesInSlot;
                maxInSlotNum = 1;
            } else if(entriesInSlot == maxInSlot) {
                maxInSlotNum++;
            }
        }
    }

    printf("%s: ", Message);

    if(occupiedSlots == 0) {
        printf("empty\n");
        return;
    }

    printf("%d entries, %d%% slots used, %d%% have collisions - ",
           f_list_sizeGet(Hash->entriesList),
           100 * occupiedSlots / F_STRHASH__SLOTS,
           100 * slotsWithCollisions / occupiedSlots);

    if(maxInSlot < 2) {
        printf("no collisions\n");
    } else {
        printf("longest chain is %d (%d slots, %d%%)\n",
               maxInSlot,
               maxInSlotNum,
               100 * maxInSlotNum / occupiedSlots);
    }
}
