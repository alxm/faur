/*
    Copyright 2010, 2016 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_system_includes.h"
#include "a2x_pack_strhash.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

#define A_STRHASH_NUM 256

struct AStrHash {
    AList* entriesList;
    AStrHashEntry* entriesTable[A_STRHASH_NUM];
};

struct AStrHashEntry {
    char* key;
    void* content;
    AStrHashEntry* next;
};

static inline uint8_t getSlot(const char* Key)
{
    unsigned s = 0;

    for( ; *Key != '\0'; Key++) {
        s <<= 1;
        s ^= (unsigned)*Key;
    }

    return s & 0xff;
}

AStrHash* a_strhash_new(void)
{
    AStrHash* h = a_mem_malloc(sizeof(AStrHash));

    h->entriesList = a_list_new();

    for(int i = A_STRHASH_NUM; i--; ) {
        h->entriesTable[i] = NULL;
    }

    return h;
}

void a_strhash_free(AStrHash* Hash)
{
    a_strhash_freeEx(Hash, NULL);
}

void a_strhash_freeEx(AStrHash* Hash, AFree* Free)
{
    A_LIST_ITERATE(Hash->entriesList, AStrHashEntry*, e) {
        if(Free) {
            Free(e->content);
        }

        free(e->key);
        free(e);
    }

    a_list_free(Hash->entriesList);
    free(Hash);
}

void a_strhash_add(AStrHash* Hash, const char* Key, void* Content)
{
    const uint8_t slot = getSlot(Key);
    AStrHashEntry* oldEntry = Hash->entriesTable[slot];
    AStrHashEntry* newEntry = a_mem_malloc(sizeof(AStrHashEntry));

    newEntry->key = a_str_dup(Key);
    newEntry->content = Content;
    newEntry->next = oldEntry;

    Hash->entriesTable[slot] = newEntry;
    a_list_addLast(Hash->entriesList, newEntry);
}

void* a_strhash_update(AStrHash* Hash, const char* Key, void* NewContent)
{
    for(AStrHashEntry* e = Hash->entriesTable[getSlot(Key)]; e; e = e->next) {
        if(a_str_equal(Key, e->key)) {
            void* oldContent = e->content;
            e->content = NewContent;
            return oldContent;
        }
    }

    a_out__error("a_strhash_update: key '%s' not found", Key);

    return NULL;
}

void* a_strhash_get(const AStrHash* Hash, const char* Key)
{
    for(AStrHashEntry* e = Hash->entriesTable[getSlot(Key)]; e; e = e->next) {
        if(a_str_equal(Key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool a_strhash_contains(const AStrHash* Hash, const char* Key)
{
    for(AStrHashEntry* e = Hash->entriesTable[getSlot(Key)]; e; e = e->next) {
        if(a_str_equal(Key, e->key)) {
            return true;
        }
    }

    return false;
}

unsigned a_strhash_getSize(const AStrHash* Hash)
{
    return a_list_getSize(Hash->entriesList);
}

AList* a_strhash__entries(const AStrHash* Hash)
{
    return Hash->entriesList;
}

void* a_strhash__entryValue(const AStrHashEntry* Entry)
{
    return Entry->content;
}

const char* a_strhash__entryKey(const AStrHashEntry* Entry)
{
    return Entry->key;
}
