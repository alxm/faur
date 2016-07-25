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

#pragma once

#include "a2x_system_includes.h"

typedef struct AStrHash AStrHash;

extern AStrHash* a_strhash_new(void);
extern void a_strhash_free(AStrHash* Hash);

extern void a_strhash_add(AStrHash* Hash, const char* Key, void* Content);
extern void* a_strhash_get(const AStrHash* Hash, const char* Key);
extern bool a_strhash_contains(const AStrHash* Hash, const char* Key);
