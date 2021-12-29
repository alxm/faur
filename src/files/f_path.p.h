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

#ifndef F_INC_FILES_PATH_P_H
#define F_INC_FILES_PATH_P_H

#include "../general/f_system_includes.h"

typedef struct FPath FPath;

#define F_PATH_FILE F_FLAGS_BIT(0)
#define F_PATH_DIR F_FLAGS_BIT(1)
#define F_PATH_OTHER F_FLAGS_BIT(2)
#define F_PATH_EMBEDDED F_FLAGS_BIT(3)
#define F_PATH_REAL F_FLAGS_BIT(4)

extern FPath* f_path_new(const char* Path);
extern FPath* f_path_newf(const char* Format, ...) F__ATTRIBUTE_FORMAT(1);
extern void f_path_free(FPath* Path);

extern bool f_path_exists(const char* Path, unsigned Flags);
extern bool f_path_test(const FPath* Path, unsigned Flags);

extern const char* f_path_getFull(const FPath* Path);
extern const char* f_path_getDirs(const FPath* Path);
extern const char* f_path_getName(const FPath* Path);

#endif // F_INC_FILES_PATH_P_H
