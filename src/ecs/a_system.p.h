/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_ECS_SYSTEM_P_H
#define A_INC_ECS_SYSTEM_P_H

#include "general/a_system_includes.h"

#include "ecs/a_entity.p.h"

typedef void ASystemHandler(AEntity* Entity);
typedef int ASystemSort(AEntity* A, AEntity* B);

extern void a_system_new(int SystemIndex, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities);
extern void a_system_add(int SystemIndex, int ComponentIndex);

extern void a_system_run(int SystemIndex);

#endif // A_INC_ECS_SYSTEM_P_H
