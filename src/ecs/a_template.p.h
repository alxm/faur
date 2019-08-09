/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_ECS_TEMPLATE_P_H
#define A_INC_ECS_TEMPLATE_P_H

#include "general/a_system_includes.h"

#include "ecs/a_entity.p.h"

typedef void AEntityInit(AEntity* Entity, const void* Context);

extern void a_template_new(const char* FilePath);
extern void a_template_initSet(const char* Id, AEntityInit* Init);

#endif // A_INC_ECS_TEMPLATE_P_H
