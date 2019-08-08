/*
    Copyright 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_ECS_ENTITY_V_H
#define A_INC_ECS_ENTITY_V_H

#include "ecs/a_entity.p.h"

#include "data/a_list.v.h"
#include "ecs/a_system.v.h"
#include "ecs/a_template.v.h"

typedef enum {
    A_ENTITY__ACTIVE_REMOVED = A_FLAGS_BIT(0), // kicked out by active system
    A_ENTITY__ACTIVE_PERMANENT = A_FLAGS_BIT(1), // entity always reports active
    A_ENTITY__DEBUG = A_FLAGS_BIT(2), // print debug messages for this entity
    A_ENTITY__REMOVED = A_FLAGS_BIT(3), // marked for removal, may have refs
} AEntityFlags;

extern void a_entity__free(AEntity* Entity);
extern void a_entity__freeEx(AEntity* Entity);

extern const ATemplate* a_entity__templateGet(const AEntity* Entity);

extern bool a_entity__ecsCanDelete(const AEntity* Entity);
extern void a_entity__ecsListAdd(AEntity* Entity, AList* List);

extern void a_entity__systemsMatch(AEntity* Entity, ASystem* System);
extern void a_entity__systemsAddTo(AEntity* Entity);
extern void a_entity__systemsRemoveFromAll(AEntity* Entity);
extern void a_entity__systemsRemoveFromActive(AEntity* Entity);

#endif // A_INC_ECS_ENTITY_V_H
