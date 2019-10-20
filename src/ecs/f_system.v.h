/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_SYSTEM_V_H
#define F_INC_ECS_SYSTEM_V_H

#include "ecs/f_system.p.h"

typedef struct FSystem FSystem;

#include "data/f_bitfield.v.h"
#include "data/f_list.v.h"

extern void f_system__uninit(void);

extern FSystem* f_system__get(int SystemIndex);

extern FListNode* f_system__entityAdd(const FSystem* System, FEntity* Entity);
extern const FBitfield* f_system__componentBitsGet(const FSystem* System);
extern bool f_system__isActiveOnly(const FSystem* System);

#endif // F_INC_ECS_SYSTEM_V_H
