/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_INIT_V_H
#define F_INC_GENERAL_INIT_V_H

#include "f_init.p.h"

typedef void FCallPackInit(void);
typedef void FCallPackUninit(void);

typedef struct {
    const char* name;
    FCallPackInit* init;
    FCallPackUninit* uninit;
} FPack;

extern void f_init__init(void);
extern void f_init__uninit(void);

#endif // F_INC_GENERAL_INIT_V_H
