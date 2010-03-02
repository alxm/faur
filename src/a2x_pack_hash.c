/*
    Copyright 2010 Alex Margarit

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

#include "a2x_pack_hash.h"

typedef struct HashEntry {
    char* key;
    void* content;
    struct HashEntry* next;
} HashEntry;

struct HashTable {
    int size;
    HashEntry** entries;
};

#define getSlot(h, k)               \
({                                  \
    int a__s = 0;                   \
    for(int i = strlen(k); i--; ) { \
        a__s += (int)k[i];          \
    }                               \
    a__s %= h->size;                \
})

HashTable* a_hash_set(const int size)
{
    HashTable* const h = malloc(sizeof(HashTable));

    h->size = size;
    h->entries = malloc(size * sizeof(HashEntry*));

    for(int i = size; i--; ) {
        h->entries[i] = NULL;
    }

    return h;
}

void a_hash_free(HashTable* const h)
{
    for(int i = h->size; i--; ) {
        HashEntry* e = h->entries[i];

        while(e) {
            HashEntry* const save = e->next;

            free(e->key);
            free(e);

            e = save;
        }
    }

    free(h->entries);
    free(h);
}

void a_hash_freeContent(HashTable* const h)
{
    for(int i = h->size; i--; ) {
        HashEntry* e = h->entries[i];

        while(e) {
            HashEntry* const save = e->next;

            free(e->key);
            free(e->content);
            free(e);

            e = save;
        }
    }

    free(h->entries);
    free(h);
}

void a_hash_add(HashTable* const h, const char* const key, void* const content)
{
    HashEntry* const e = malloc(sizeof(HashEntry));

    e->key = a_str_dup(key);
    e->content = content;
    e->next = NULL;

    const int slot = getSlot(h, key);
    HashEntry* entry = h->entries[slot];

    if(entry == NULL) {
        h->entries[slot] = e;
    } else {
        while(entry->next) {
            entry = entry->next;
        }

        entry->next = e;
    }
}

void* a_hash_get(HashTable* const h, const char* const key)
{
    HashEntry* e = h->entries[getSlot(h, key)];

    while(e && !a_str_same(key, e->key)) {
        e = e->next;
    }

    if(e) {
        return e->content;
    } else {
        return NULL;
    }
}
