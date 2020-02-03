/*
    Copyright 2010, 2016, 2018-2020 Alex Margarit <alex@alxm.org>
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

#include "f_hash.v.h"
#include <faur.v.h>

struct FHash {
    FHashFunction* function;
    FHashEqual* keyEqual;
    FFree* keyFree;
    unsigned numSlots;
    FList* entries;
    FHashEntry* slots[]; // [numSlots]
};

struct FHashEntry {
    FHashEntry* next;
    const void* key;
    void* content;
};

static bool keyEqual(const void* KeyA, const void* KeyB)
{
    return KeyA == KeyB;
}

static inline unsigned getSlot(const FHash* Hash, const void* Key)
{
    return Hash->function(Key) & (Hash->numSlots - 1);
}

static unsigned func_djb2(const char* Key)
{
    unsigned h = 5381;

    for(char c = *Key; c != '\0'; c = *++Key) {
        h += (h << 5) + (unsigned)c;
    }

    return h;
}

FHash* f_hash_new(FHashFunction* Function, FHashEqual* KeyEqual, FFree* KeyFree, unsigned NumSlots)
{
    #if F_CONFIG_BUILD_DEBUG
        if(NumSlots == 0) {
            F__FATAL("f_hash_new: 0 slots");
        }
    #endif

    if((NumSlots & (NumSlots - 1)) != 0) {
        unsigned slots = 1;

        while(slots < NumSlots) {
            slots <<= 1;
        }

        NumSlots = slots;
    }

    FHash* h = f_mem_zalloc(sizeof(FHash) + NumSlots * sizeof(FHashEntry*));

    h->function = Function;
    h->keyEqual = KeyEqual ? KeyEqual : keyEqual;
    h->keyFree = KeyFree;
    h->numSlots = NumSlots;
    h->entries = f_list_new();

    return h;
}

FHash* f_hash_newStr(unsigned NumSlots, bool FreeKeyString)
{
    return f_hash_new((FHashFunction*)func_djb2,
                      (FHashEqual*)f_str_equal,
                      FreeKeyString ? f_mem_free : NULL,
                      NumSlots);
}

void f_hash_free(FHash* Hash)
{
    f_hash_freeEx(Hash, NULL);
}

void f_hash_freeEx(FHash* Hash, FFree* Free)
{
    if(Hash == NULL) {
        return;
    }

    F_LIST_ITERATE(Hash->entries, FHashEntry*, e) {
        if(Free) {
            Free(e->content);
        }

        if(Hash->keyFree) {
            Hash->keyFree((void*)e->key);
        }

        f_mem_free(e);
    }

    f_list_free(Hash->entries);

    f_mem_free(Hash);
}

void f_hash_add(FHash* Hash, const void* Key, void* Content)
{
    unsigned slot = getSlot(Hash, Key);

    #if F_CONFIG_BUILD_DEBUG
        for(FHashEntry* e = Hash->slots[slot]; e; e = e->next) {
            if(Hash->keyEqual(Key, e->key)) {
                F__FATAL("f_hash_add: Key already in table");
            }
        }
    #endif

    FHashEntry* oldEntry = Hash->slots[slot];
    FHashEntry* newEntry = f_mem_malloc(sizeof(FHashEntry));

    newEntry->next = oldEntry;
    newEntry->key = Key;
    newEntry->content = Content;

    Hash->slots[slot] = newEntry;
    f_list_addLast(Hash->entries, newEntry);
}

void* f_hash_update(FHash* Hash, const void* Key, void* NewContent)
{
    unsigned slot = getSlot(Hash, Key);

    for(FHashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            void* oldContent = e->content;
            e->content = NewContent;

            return oldContent;
        }
    }

    return NULL;
}

static void removeEntry(FHash* Hash, unsigned Slot, FHashEntry* Current, FHashEntry* Previous)
{
    if(Previous == NULL) {
        Hash->slots[Slot] = Current->next;
    } else {
        Previous->next = Current->next;
    }

    f_list_removeItem(Hash->entries, Current);

    if(Hash->keyFree) {
        Hash->keyFree((void*)Current->key);
    }

    f_mem_free(Current);
}

void f_hash_removeKey(FHash* Hash, const void* Key)
{
    unsigned slot = getSlot(Hash, Key);
    FHashEntry* prev = NULL;

    for(FHashEntry* e = Hash->slots[slot]; e; prev = e, e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            removeEntry(Hash, slot, e, prev);

            return;
        }
    }
}

void f_hash_removeItem(FHash* Hash, const void* Content)
{
    for(unsigned slot = Hash->numSlots; slot--; ) {
        FHashEntry* prev = NULL;

        for(FHashEntry* e = Hash->slots[slot]; e; prev = e, e = e->next) {
            if(e->content == Content) {
                removeEntry(Hash, slot, e, prev);

                return;
            }
        }
    }
}

void* f_hash_get(const FHash* Hash, const void* Key)
{
    unsigned slot = getSlot(Hash, Key);

    for(FHashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool f_hash_contains(const FHash* Hash, const void* Key)
{
    unsigned slot = getSlot(Hash, Key);

    for(FHashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            return true;
        }
    }

    return false;
}

unsigned f_hash_sizeGet(const FHash* Hash)
{
    return f_list_sizeGet(Hash->entries);
}

void** f_hash_toArray(const FHash* Hash)
{
    void** array = f_mem_malloc(
                    f_list_sizeGet(Hash->entries) * sizeof(void*));

    F_LIST_ITERATE(Hash->entries, const FHashEntry*, e) {
        array[F_LIST_INDEX()] = e->content;
    }

    return array;
}

const FList* f__hash_entries(const FHash* Hash)
{
    return Hash->entries;
}

void* f__hash_entryValue(const FHashEntry* Entry)
{
    return Entry->content;
}

const void* f__hash_entryKey(const FHashEntry* Entry)
{
    return Entry->key;
}

void f__hash_printStats(const FHash* Hash, const char* Message)
{
    unsigned occupiedSlots = 0, slotsWithCollisions = 0, collisions = 0;
    unsigned minLength = UINT_MAX, maxLength = 0, lengthSum = 0;

    for(unsigned slot = 0; slot < Hash->numSlots; slot++) {
        unsigned entriesInSlot = 0;

        for(FHashEntry* e = Hash->slots[slot]; e; e = e->next) {
            entriesInSlot++;
        }

        if(entriesInSlot > 0) {
            occupiedSlots++;
            lengthSum += entriesInSlot;

            if(entriesInSlot > 1) {
                slotsWithCollisions++;
                collisions += entriesInSlot - 1;
            }

            if(entriesInSlot < minLength) {
                minLength = entriesInSlot;
            }

            if(entriesInSlot > maxLength) {
                maxLength = entriesInSlot;
            }
        }
    }

    printf("%s: ", Message);

    if(occupiedSlots == 0) {
        printf("empty\n");

        return;
    }

    unsigned numEntries = f_list_sizeGet(Hash->entries);

    printf("%d/%d (%d%%) slots used, %d/%d (%d%%) entries collide, "
           "chain min=%d avg=%.2f max=%d\n",
           occupiedSlots,
           Hash->numSlots,
           100 * occupiedSlots / Hash->numSlots,
           collisions,
           numEntries,
           100 * collisions / numEntries,
           minLength,
           (float)lengthSum / (float)occupiedSlots,
           maxLength);
}
