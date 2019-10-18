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

struct AStrHash {
    AList* entriesList;
    AStrHashEntry* slots[F_STRHASH__SLOTS];
};

struct AStrHashEntry {
    char* key;
    void* content;
    AStrHashEntry* next;
};

static inline unsigned getSlot(const char* Key)
{
    unsigned s = 0;

    for( ; *Key != '\0'; Key++) {
        s = (unsigned)*Key + (s << 5) - s;
    }

    return s & F_STRHASH__SLOTS_MASK;
}

AStrHash* f_strhash_new(void)
{
    AStrHash* h = f_mem_malloc(sizeof(AStrHash));

    h->entriesList = f_list_new();

    for(int i = F_STRHASH__SLOTS; i--; ) {
        h->slots[i] = NULL;
    }

    return h;
}

void f_strhash_free(AStrHash* Hash)
{
    f_strhash_freeEx(Hash, NULL);
}

void f_strhash_freeEx(AStrHash* Hash, AFree* Free)
{
    if(Hash == NULL) {
        return;
    }

    F_LIST_ITERATE(Hash->entriesList, AStrHashEntry*, e) {
        if(Free) {
            Free(e->content);
        }

        f_mem_free(e->key);
        f_mem_free(e);
    }

    f_list_free(Hash->entriesList);

    f_mem_free(Hash);
}

void f_strhash_add(AStrHash* Hash, const char* Key, void* Content)
{
    unsigned slot = getSlot(Key);
    AStrHashEntry* oldEntry = Hash->slots[slot];
    AStrHashEntry* newEntry = f_mem_malloc(sizeof(AStrHashEntry));

    newEntry->key = f_str_dup(Key);
    newEntry->content = Content;
    newEntry->next = oldEntry;

    Hash->slots[slot] = newEntry;
    f_list_addLast(Hash->entriesList, newEntry);
}

void* f_strhash_update(AStrHash* Hash, const char* Key, void* NewContent)
{
    for(AStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            void* oldContent = e->content;
            e->content = NewContent;
            return oldContent;
        }
    }

    return NULL;
}

void* f_strhash_get(const AStrHash* Hash, const char* Key)
{
    for(AStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool f_strhash_contains(const AStrHash* Hash, const char* Key)
{
    for(AStrHashEntry* e = Hash->slots[getSlot(Key)]; e; e = e->next) {
        if(f_str_equal(Key, e->key)) {
            return true;
        }
    }

    return false;
}

unsigned f_strhash_sizeGet(const AStrHash* Hash)
{
    return f_list_sizeGet(Hash->entriesList);
}

void** f_strhash_toArray(const AStrHash* Hash)
{
    void** array = f_mem_malloc(
                    f_list_sizeGet(Hash->entriesList) * sizeof(void*));

    F_LIST_ITERATE(Hash->entriesList, const AStrHashEntry*, e) {
        array[F_LIST_INDEX()] = e->content;
    }

    return array;
}

const AList* f__strhash_entries(const AStrHash* Hash)
{
    return Hash->entriesList;
}

void* f__strhash_entryValue(const AStrHashEntry* Entry)
{
    return Entry->content;
}

const char* f__strhash_entryKey(const AStrHashEntry* Entry)
{
    return Entry->key;
}

void f__strhash_printStats(const AStrHash* Hash, const char* Message)
{
    unsigned maxInSlot = 0, maxInSlotNum = 0;
    unsigned occupiedSlots = 0;
    unsigned slotsWithCollisions = 0;

    for(int i = 0; i < F_STRHASH__SLOTS; i++) {
        unsigned entriesInSlot = 0;

        for(AStrHashEntry* e = Hash->slots[i]; e; e = e->next) {
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
