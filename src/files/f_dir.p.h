/*
    Copyright 2011, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_FILES_DIR_P_H
#define F_INC_FILES_DIR_P_H

#include "../general/f_system_includes.h"

typedef struct FDir FDir;

#include "../data/f_list.p.h"
#include "../files/f_path.p.h"

extern FDir* f_dir_new(const char* Path);
extern void f_dir_free(FDir* Dir);

extern const FPath* f_dir_pathGet(const FDir* Dir);
extern const FList* f_dir_entriesGet(const FDir* Dir);

#endif // F_INC_FILES_DIR_P_H
