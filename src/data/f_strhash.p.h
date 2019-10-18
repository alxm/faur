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

#ifndef A_INC_DATA_STRHASH_P_H
#define A_INC_DATA_STRHASH_P_H

#include "general/f_system_includes.h"

typedef struct AStrHash AStrHash;
typedef struct AStrHashEntry AStrHashEntry;

#include "data/f_listit.p.h"

extern AStrHash* f_strhash_new(void);
extern void f_strhash_free(AStrHash* Hash);
extern void f_strhash_freeEx(AStrHash* Hash, AFree* Free);

extern void f_strhash_add(AStrHash* Hash, const char* Key, void* Content);
extern void* f_strhash_update(AStrHash* Hash, const char* Key, void* NewContent);
extern void* f_strhash_get(const AStrHash* Hash, const char* Key);
extern bool f_strhash_contains(const AStrHash* Hash, const char* Key);
extern unsigned f_strhash_sizeGet(const AStrHash* Hash);

extern void** f_strhash_toArray(const AStrHash* Hash);

extern const AList* f__strhash_entries(const AStrHash* Hash);
extern void* f__strhash_entryValue(const AStrHashEntry* Entry);
extern const char* f__strhash_entryKey(const AStrHashEntry* Entry);

#define A_STRHASH_ITERATE(StrHash, PtrType, Name)                           \
    A_LIST_ITERATE(f__strhash_entries(StrHash), const AStrHashEntry*, f__e) \
        for(PtrType Name = f__strhash_entryValue(f__e);                     \
            f__e != NULL; f__e = NULL)

#define A_STRHASH_KEYS(StrHash, Name)                                       \
    A_LIST_ITERATE(f__strhash_entries(StrHash), const AStrHashEntry*, f__e) \
        for(const char* Name = f__strhash_entryKey(f__e);                   \
            f__e != NULL; f__e = NULL)

#define A_STRHASH_KEY() f__strhash_entryKey(f__e)

extern void f__strhash_printStats(const AStrHash* Hash, const char* Message);

#endif // A_INC_DATA_STRHASH_P_H
