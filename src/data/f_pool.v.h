/*
    Copyright 2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_POOL_V_H
#define F_INC_DATA_POOL_V_H

#include "f_pool.p.h"

typedef enum {
    F_POOL__INVALID = -1,
    F_POOL__CONSOLE,
    F_POOL__LIST,
    F_POOL__LISTNODE,
    F_POOL__NUM
} FPoolId;

#include "../general/f_init.v.h"

extern const FPack f_pack__pool;

extern void* f_pool__alloc(FPoolId Pool);

#endif // F_INC_DATA_POOL_V_H
