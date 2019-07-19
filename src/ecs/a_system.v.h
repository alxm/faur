/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_ECS_SYSTEM_V_H
#define A_INC_ECS_SYSTEM_V_H

#include "ecs/a_system.p.h"

typedef struct ASystem ASystem;

#include "data/a_bitfield.v.h"
#include "data/a_list.v.h"

extern void a_system__uninit(void);

extern ASystem* a_system__get(int SystemIndex);

extern AListNode* a_system__entityAdd(const ASystem* System, AEntity* Entity);
extern const ABitfield* a_system__componentBitsGet(const ASystem* System);
extern bool a_system__isActiveOnly(const ASystem* System);

#endif // A_INC_ECS_SYSTEM_V_H
