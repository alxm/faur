/*
    Copyright 2011, 2016, 2018 Alex Margarit

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

typedef struct ADir ADir;
typedef struct ADirEntry ADirEntry;

#include "a2x_pack_list.p.h"

extern ADir* a_dir_new(const char* Path);
extern void a_dir_free(ADir* Dir);

extern AList* a_dir_entriesListGet(const ADir* Dir);
extern unsigned a_dir_entriesNumGet(const ADir* Dir);

extern const char* a_dir_entryNameGet(const ADirEntry* Entry);
extern const char* a_dir_entryPathGet(const ADirEntry* Entry);
extern bool a_dir_entryIsDir(const ADirEntry* Entry);

extern const char* a_dir_pathGet(const ADir* Dir);
extern const char* a_dir_nameGet(const ADir* Dir);

extern bool a_dir_exists(const char* Path);
extern bool a_dir_make(const char* Path);
