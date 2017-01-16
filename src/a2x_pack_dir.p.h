/*
    Copyright 2011, 2016 Alex Margarit

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

typedef struct ADir ADir;
typedef struct ADirEntry ADirEntry;

#include "a2x_pack_list.p.h"

struct ADirEntry {
    char* name;
    char* full;
};

extern ADir* a_dir_open(const char* Path);
extern void a_dir_close(ADir* Dir);

extern AList* a_dir__files(const ADir* Dir);

#define A_DIR_ITERATE(Dir, NameVar, FullPathVar)                \
    for(const char *NameVar = (char*)1, *FullPathVar;           \
        NameVar;                                                \
        NameVar = NULL)                                         \
        A_LIST_FILTER(                                          \
            a_dir__files(Dir), ADirEntry*, entry,               \
            (NameVar = entry->name, FullPathVar = entry->full))

#define A_DIR_ITERATE_BACKWARDS(Dir, NameVar, FullPathVar)      \
    for(const char *NameVar = (char*)1, *FullPathVar;           \
        NameVar;                                                \
        NameVar = NULL)                                         \
        A_LIST_FILTER_BACKWARDS(                                \
            a_dir__files(Dir), ADirEntry*, entry,               \
            (NameVar = entry->name, FullPathVar = entry->full))

#define A_DIR_FILTER(Dir, NameVar, FullPathVar, Filter)                       \
    for(const char *NameVar = (char*)1, *FullPathVar;                         \
        NameVar;                                                              \
        NameVar = NULL)                                                       \
        A_LIST_FILTER(                                                        \
            a_dir__files(Dir), ADirEntry*, entry,                             \
            ((NameVar = entry->name, FullPathVar = entry->full) && (Filter)))

#define A_DIR_FILTER_BACKWARDS(Dir, NameVar, FullPathVar, Filter)             \
    for(const char *NameVar = (char*)1, *FullPathVar;                         \
        NameVar;                                                              \
        NameVar = NULL)                                                       \
        A_LIST_FILTER_BACKWARDS(                                              \
            a_dir__files(Dir), ADirEntry*, entry,                             \
            ((NameVar = entry->name, FullPathVar = entry->full) && (Filter)))

extern const char* a_dir_path(const ADir* Dir);
extern const char* a_dir_name(const ADir* Dir);
extern unsigned a_dir_numEntries(const ADir* Dir);

extern bool a_dir_exists(const char* Path);
extern bool a_dir_make(const char* Path);
