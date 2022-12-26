/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_BUILD_GEN_LUTS
static uint8_t g_crc8[256];

static void f_hash__init(void)
{
    printf("g_crc8:\n");

    for(unsigned byte = 0; byte < 256; byte++) {
        unsigned crc = byte;

        for(int b = 8; b--; ) {
            crc <<= 1;

            if(crc & 0x100) {
                crc ^= 0x37;
            }
        }

        g_crc8[byte] = (uint8_t)crc;

        printf("%u, ", g_crc8[byte]);
    }

    printf("\n");
}

const FPack f_pack__hash = {
    "Hash",
    f_hash__init,
    NULL,
};
#else // !F_CONFIG_BUILD_GEN_LUTS
static const uint8_t g_crc8[256] = {
    0, 55, 110, 89, 220, 235, 178, 133, 143, 184, 225, 214, 83, 100, 61, 10, 41, 30, 71, 112, 245, 194, 155, 172, 166, 145, 200, 255, 122, 77, 20, 35, 82, 101, 60, 11, 142, 185, 224, 215, 221, 234, 179, 132, 1, 54, 111, 88, 123, 76, 21, 34, 167, 144, 201, 254, 244, 195, 154, 173, 40, 31, 70, 113, 164, 147, 202, 253, 120, 79, 22, 33, 43, 28, 69, 114, 247, 192, 153, 174, 141, 186, 227, 212, 81, 102, 63, 8, 2, 53, 108, 91, 222, 233, 176, 135, 246, 193, 152, 175, 42, 29, 68, 115, 121, 78, 23, 32, 165, 146, 203, 252, 223, 232, 177, 134, 3, 52, 109, 90, 80, 103, 62, 9, 140, 187, 226, 213, 127, 72, 17, 38, 163, 148, 205, 250, 240, 199, 158, 169, 44, 27, 66, 117, 86, 97, 56, 15, 138, 189, 228, 211, 217, 238, 183, 128, 5, 50, 107, 92, 45, 26, 67, 116, 241, 198, 159, 168, 162, 149, 204, 251, 126, 73, 16, 39, 4, 51, 106, 93, 216, 239, 182, 129, 139, 188, 229, 210, 87, 96, 57, 14, 219, 236, 181, 130, 7, 48, 105, 94, 84, 99, 58, 13, 136, 191, 230, 209, 242, 197, 156, 171, 46, 25, 64, 119, 125, 74, 19, 36, 161, 150, 207, 248, 137, 190, 231, 208, 85, 98, 59, 12, 6, 49, 104, 95, 218, 237, 180, 131, 160, 151, 206, 249, 124, 75, 18, 37, 47, 24, 65, 118, 243, 196, 157, 170
};
#endif // !F_CONFIG_BUILD_GEN_LUTS

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

FHash* f_hash_new(FCallHashFunction* Function, FCallHashEqual* KeyEqual, FCallFree* KeyFree, unsigned NumSlots)
{
    F__CHECK(Function != NULL);
    F__CHECK(NumSlots > 0);

    if(NumSlots == 0) {
        F__FATAL("f_hash_new: 0 slots");
    }

    if((NumSlots & (NumSlots - 1)) != 0) {
        unsigned slots = 1;

        while(slots < NumSlots) {
            slots <<= 1;
        }

        NumSlots = slots;
    }

    FHash* h = f_mem_mallocz(
                sizeof(FHash) + (NumSlots - 1) * sizeof(F__HashEntry*));

    h->function = Function;
    h->keyEqual = KeyEqual ? KeyEqual : keyEqual;
    h->keyFree = KeyFree;
    h->numSlots = NumSlots;

    f_listintr_init(&h->entries, F__HashEntry, listNode);

    return h;
}

FHash* f_hash_newStr(unsigned NumSlots, bool FreeKeyString)
{
    F__CHECK(NumSlots > 0);

    return f_hash_new((FCallHashFunction*)func_djb2,
                      (FCallHashEqual*)f_str_equal,
                      FreeKeyString ? f_mem_free : NULL,
                      NumSlots);
}

void f_hash_free(FHash* Hash)
{
    if(Hash == NULL) {
        return;
    }

    f_hash_freeEx(Hash, NULL);
}

void f_hash_freeEx(FHash* Hash, FCallFree* Free)
{
    if(Hash == NULL) {
        return;
    }

    F_LISTINTR_ITERATE(&Hash->entries, F__HashEntry*, e) {
        if(Free) {
            Free(e->content);
        }

        if(Hash->keyFree) {
            Hash->keyFree((void*)e->key);
        }

        f_pool_release(e);
    }

    f_mem_free(Hash);
}

void f_hash_add(FHash* Hash, const void* Key, void* Content)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Key != NULL);

    unsigned slot = getSlot(Hash, Key);

    #if F_CONFIG_DEBUG
        for(F__HashEntry* e = Hash->slots[slot]; e; e = e->next) {
            if(Hash->keyEqual(Key, e->key)) {
                f_out__error("f_hash_add: Key already in table");
            }
        }
    #endif

    F__HashEntry* oldEntry = Hash->slots[slot];
    F__HashEntry* newEntry = f_pool__alloc(F_POOL__HASHENTRY);

    newEntry->next = oldEntry;
    newEntry->key = Key;
    newEntry->content = Content;

    Hash->slots[slot] = newEntry;
    f_listintr_addLast(&Hash->entries, newEntry);
    Hash->numEntries++;
}

void* f_hash_update(FHash* Hash, const void* Key, void* NewContent)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Key != NULL);
    F__CHECK(NewContent != NULL);

    unsigned slot = getSlot(Hash, Key);

    for(F__HashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            void* oldContent = e->content;
            e->content = NewContent;

            return oldContent;
        }
    }

    return NULL;
}

static void removeEntry(FHash* Hash, unsigned Slot, F__HashEntry* Current, F__HashEntry* Previous)
{
    if(Previous == NULL) {
        Hash->slots[Slot] = Current->next;
    } else {
        Previous->next = Current->next;
    }

    f_listintr_removeNode(&Current->listNode);
    Hash->numEntries--;

    if(Hash->keyFree) {
        Hash->keyFree((void*)Current->key);
    }

    f_pool_release(Current);
}

void f_hash_removeKey(FHash* Hash, const void* Key)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Key != NULL);

    unsigned slot = getSlot(Hash, Key);
    F__HashEntry* prev = NULL;

    for(F__HashEntry* e = Hash->slots[slot]; e; prev = e, e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            removeEntry(Hash, slot, e, prev);

            return;
        }
    }
}

void f_hash_removeItem(FHash* Hash, const void* Content)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Content != NULL);

    for(unsigned slot = Hash->numSlots; slot--; ) {
        F__HashEntry* prev = NULL;

        for(F__HashEntry* e = Hash->slots[slot]; e; prev = e, e = e->next) {
            if(e->content == Content) {
                removeEntry(Hash, slot, e, prev);

                return;
            }
        }
    }
}

void* f_hash_get(const FHash* Hash, const void* Key)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Key != NULL);

    unsigned slot = getSlot(Hash, Key);

    for(F__HashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            return e->content;
        }
    }

    return NULL;
}

bool f_hash_contains(const FHash* Hash, const void* Key)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Key != NULL);

    unsigned slot = getSlot(Hash, Key);

    for(F__HashEntry* e = Hash->slots[slot]; e; e = e->next) {
        if(Hash->keyEqual(Key, e->key)) {
            return true;
        }
    }

    return false;
}

unsigned f_hash_sizeGet(const FHash* Hash)
{
    F__CHECK(Hash != NULL);

    return Hash->numEntries;
}

void** f_hash_toArray(const FHash* Hash)
{
    F__CHECK(Hash != NULL);
    F__CHECK(Hash->numEntries > 0);

    void** array = f_mem_malloc(Hash->numEntries * sizeof(void*));

    F_LISTINTR_ITERATE(&Hash->entries, const F__HashEntry*, e) {
        array[F_LISTINTR_INDEX()] = e->content;
    }

    return array;
}

const FListIntr* f__hash_entries(const FHash* Hash)
{
    return &Hash->entries;
}

void* f__hash_entryValue(const F__HashEntry* Entry)
{
    return Entry->content;
}

const void* f__hash_entryKey(const F__HashEntry* Entry)
{
    return Entry->key;
}

void f__hash_printStats(const FHash* Hash, const char* Message)
{
    unsigned occupiedSlots = 0, slotsWithCollisions = 0, collisions = 0;
    unsigned minLength = UINT_MAX, maxLength = 0, lengthSum = 0;

    for(unsigned slot = 0; slot < Hash->numSlots; slot++) {
        unsigned entriesInSlot = 0;

        for(F__HashEntry* e = Hash->slots[slot]; e; e = e->next) {
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

    printf(
        "%d/%d (%d%%) slots used, "
        "%d/%d (%d%%) slots have collisions, "
        "%d/%d (%d%%) entries collide, "
        "chain min=%d avg=%.2f max=%d\n",

        occupiedSlots,
        Hash->numSlots,
        100 * occupiedSlots / Hash->numSlots,

        slotsWithCollisions,
        Hash->numSlots,
        100 * slotsWithCollisions / Hash->numSlots,

        collisions,
        Hash->numEntries,
        100 * collisions / Hash->numEntries,

        minLength,
        (float)lengthSum / (float)occupiedSlots,
        maxLength);
}

uint8_t f_hash_crc8(const void* Buffer, size_t Size)
{
    F__CHECK(Buffer != NULL);
    F__CHECK(Size > 0);

    unsigned crc = 0;
    const uint8_t* key = Buffer;

    while(Size--) {
        crc = g_crc8[crc ^ *key++];
    }

    return (uint8_t)crc;
}
