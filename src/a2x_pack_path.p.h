/*
    Copyright 2018 Alex Margarit

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

typedef struct APath APath;

typedef enum {
    A_PATH_TYPE_INVALID = -1,
    A_PATH_TYPE_ANY,
    A_PATH_TYPE_FILE,
    A_PATH_TYPE_DIR,
    A_PATH_TYPE_NUM
} APathType;

extern APath* a_path_new(const char* Path);
extern APath* a_path_newf(const char* Format, ...);
extern void a_path_free(APath* Path);

extern const char* a_path_getFull(const APath* Path);
extern const char* a_path_getName(const APath* Path);

extern bool a_path_exists(const APath* Path, APathType Type);
