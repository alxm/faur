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

#ifndef F_INC_FILES_PATH_V_H
#define F_INC_FILES_PATH_V_H

#include "f_path.p.h"

typedef struct {
    unsigned flags;
    size_t size;
} FPathInfo;

struct FPath {
    FPathInfo info;
    char* full;
    char* dirsPart;
    char* namePart;
};

extern size_t f_path__sizeGet(const FPath* Path);

extern void f_path__flagsSet(FPath* Path, unsigned Flags);

#endif // F_INC_FILES_PATH_V_H
