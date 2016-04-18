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

#include "a2x_pack_strhash.v.h"

#define A_STRHASH_NUM 256

typedef struct Entry Entry;

struct StrHash {
    Entry* entries[A_STRHASH_NUM];
};

struct Entry {
    char* key;
    void* content;
    Entry* next;
};

#define getSlot(k)                  \
({                                  \
    uint8_t s = 0;                  \
    for(int i = strlen(k); i--; ) { \
        s <<= 1;                    \
        s ^= k[i];                  \
    }                               \
    s;                              \
})

StrHash* a_strhash_new(void)
{
    StrHash* const h = a_mem_malloc(sizeof(StrHash));

    for(int i = A_STRHASH_NUM; i--; ) {
        h->entries[i] = NULL;
    }

    return h;
}

void a_strhash_free(StrHash* h)
{
    for(int i = A_STRHASH_NUM; i--; ) {
        Entry* e = h->entries[i];

        while(e) {
            Entry* const save = e->next;

            free(e->key);
            free(e);

            e = save;
        }
    }

    free(h);
}

void a_strhash_add(StrHash* h, const char* key, void* content)
{
    const uint8_t slot = getSlot(key);
    Entry* const entry = h->entries[slot];

    Entry* const e = a_mem_malloc(sizeof(Entry));

    e->key = a_str_dup(key);
    e->content = content;
    e->next = entry;

    h->entries[slot] = e;
}

void* a_strhash_get(const StrHash* h, const char* key)
{
    for(Entry* e = h->entries[getSlot(key)]; e; e = e->next) {
        if(a_str_same(key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool a_strhash_contains(const StrHash* h, const char* key)
{
    for(Entry* e = h->entries[getSlot(key)]; e; e = e->next) {
        if(a_str_same(key, e->key)) {
            return true;
        }
    }

    return false;
}
