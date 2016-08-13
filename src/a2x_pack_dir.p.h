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

typedef struct ADir ADir;

extern ADir* a_dir_open(const char* Path);
extern ADir* a_dir_openFilter(const char* Path, int (*filter)(const struct dirent* Entry));
extern void a_dir_close(ADir* Dir);

extern void a_dir_reverse(ADir* Dir);

#define A_DIR_ITERATE(Dir, NameVar, FullPathVar)                               \
    for(const char *NameVar = NULL + 1, *FullPathVar; NameVar; NameVar = NULL) \
        while(a_dir__getNext(Dir, &NameVar, &FullPathVar))

extern bool a_dir__getNext(ADir* Dir, const char** Name, const char** FullPath);
extern void a_dir_reset(ADir* Dir);

extern const char* a_dir_path(const ADir* Dir);
extern const char* a_dir_name(const ADir* Dir);
extern int a_dir_num(const ADir* Dir);

extern bool a_dir_exists(const char* Path);
extern void a_dir_make(const char* Path);
