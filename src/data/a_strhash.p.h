/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "general/a_system_includes.h"

typedef struct AStrHash AStrHash;
typedef struct AStrHashEntry AStrHashEntry;

#include "data/a_listit.p.h"

extern AStrHash* a_strhash_new(void);
extern void a_strhash_free(AStrHash* Hash);
extern void a_strhash_freeEx(AStrHash* Hash, AFree* Free);

extern void a_strhash_add(AStrHash* Hash, const char* Key, void* Content);
extern void* a_strhash_update(AStrHash* Hash, const char* Key, void* NewContent);
extern void* a_strhash_get(const AStrHash* Hash, const char* Key);
extern bool a_strhash_contains(const AStrHash* Hash, const char* Key);
extern unsigned a_strhash_sizeGet(const AStrHash* Hash);

extern void** a_strhash_toArray(const AStrHash* Hash);

extern const AList* a__strhash_entries(const AStrHash* Hash);
extern void* a__strhash_entryValue(const AStrHashEntry* Entry);
extern const char* a__strhash_entryKey(const AStrHashEntry* Entry);

#define A_STRHASH_ITERATE(StrHash, PtrType, Name)                           \
    A_LIST_ITERATE(a__strhash_entries(StrHash), const AStrHashEntry*, a__e) \
        for(PtrType Name = a__strhash_entryValue(a__e);                     \
            a__e != NULL; a__e = NULL)

#define A_STRHASH_KEYS(StrHash, Name)                                       \
    A_LIST_ITERATE(a__strhash_entries(StrHash), const AStrHashEntry*, a__e) \
        for(const char* Name = a__strhash_entryKey(a__e);                   \
            a__e != NULL; a__e = NULL)

#define A_STRHASH_KEY() a__strhash_entryKey(a__e)

extern void a__strhash_printStats(const AStrHash* Hash, const char* Message);

#endif // A_INC_DATA_STRHASH_P_H
