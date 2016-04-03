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

#include "a2x_app_includes.h"

typedef struct StrHash StrHash;

extern StrHash* a_strhash_new(void);
extern void a_strhash_free(StrHash* h);

extern void a_strhash_add(StrHash* h, const char* key, void* content);
extern void* a_strhash_get(const StrHash* h, const char* key);
extern bool a_strhash_contains(const StrHash* h, const char* key);
