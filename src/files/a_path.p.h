/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_FILES_PATH_P_H
#define A_INC_FILES_PATH_P_H

#include "general/a_system_includes.h"

typedef struct APath APath;

typedef enum {
    A_PATH_FILE = A_FLAGS_BIT(0),
    A_PATH_DIR = A_FLAGS_BIT(1),
    A_PATH_OTHER = A_FLAGS_BIT(2),
    A_PATH_EMBEDDED = A_FLAGS_BIT(3),
    A_PATH_REAL = A_FLAGS_BIT(4),
} APathFlags;

extern APath* a_path_new(const char* Path);
extern APath* a_path_newf(const char* Format, ...);
extern void a_path_free(APath* Path);

extern bool a_path_exists(const char* Path, APathFlags Flags);
extern bool a_path_test(const APath* Path, APathFlags Flags);

extern const char* a_path_getFull(const APath* Path);
extern const char* a_path_getDirs(const APath* Path);
extern const char* a_path_getName(const APath* Path);

#endif // A_INC_FILES_PATH_P_H
