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

#ifndef A2X_PACK_HASH_PH
#define A2X_PACK_HASH_PH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "a2x_pack_str.p.h"

typedef struct HashTable HashTable;

extern HashTable* a_hash_set(const int size);
extern void a_hash_free(HashTable* const h);
extern void a_hash_freeContent(HashTable* const h);
extern void a_hash_add(HashTable* const h, const char* const key, void* const content);
extern void* a_hash_get(HashTable* const h, const char* const key);

#endif // A2X_PACK_HASH_PH
