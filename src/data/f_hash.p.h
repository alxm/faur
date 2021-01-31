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

#ifndef F_INC_DATA_HASH_P_H
#define F_INC_DATA_HASH_P_H

#include "../general/f_system_includes.h"

typedef struct FHash FHash;
typedef struct F__HashEntry F__HashEntry;

typedef unsigned FCallHashFunction(const void* Key);
typedef bool FCallHashEqual(const void* KeyA, const void* KeyB);

#include "../data/f_listintr.p.h"

extern FHash* f_hash_new(FCallHashFunction* Function, FCallHashEqual* KeyEqual, FCallFree* KeyFree, unsigned NumSlots);
extern FHash* f_hash_newStr(unsigned NumSlots, bool FreeKeyString);
extern void f_hash_free(FHash* Hash);
extern void f_hash_freeEx(FHash* Hash, FCallFree* Free);

extern void f_hash_add(FHash* Hash, const void* Key, void* Content);
extern void* f_hash_update(FHash* Hash, const void* Key, void* NewContent);

extern void f_hash_removeKey(FHash* Hash, const void* Key);
extern void f_hash_removeItem(FHash* Hash, const void* Content);

extern void* f_hash_get(const FHash* Hash, const void* Key);
extern bool f_hash_contains(const FHash* Hash, const void* Key);

extern unsigned f_hash_sizeGet(const FHash* Hash);
extern void** f_hash_toArray(const FHash* Hash);

extern const FListIntr* f__hash_entries(const FHash* Hash);
extern void* f__hash_entryValue(const F__HashEntry* Entry);
extern const void* f__hash_entryKey(const F__HashEntry* Entry);

#define F_HASH_ITERATE(Hash, PtrType, Name)                              \
    F_LISTINTR_ITERATE(f__hash_entries(Hash), const F__HashEntry*, f__e) \
        for(PtrType Name = f__hash_entryValue(f__e);                     \
            f__e != NULL; f__e = NULL)

#define F_HASH_ITERATE_KEYS(Hash, PtrType, Name)                         \
    F_LISTINTR_ITERATE(f__hash_entries(Hash), const F__HashEntry*, f__e) \
        for(PtrType Name = f__hash_entryKey(f__e);                       \
            f__e != NULL; f__e = NULL)

#define F_HASH_KEY() f__hash_entryKey(f__e)

extern void f__hash_printStats(const FHash* Hash, const char* Message);

extern uint8_t f_hash_crc8(const void* Buffer, size_t Size);

#endif // F_INC_DATA_HASH_P_H
