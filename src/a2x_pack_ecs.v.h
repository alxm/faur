/*
    Copyright 2016, 2017 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_pack_ecs.p.h"

typedef struct AEcs AEcs;

typedef enum {
    A_ECS__INVALID = -1,
    A_ECS__NEW, // new entities are added to this list
    A_ECS__RUNNING, // entities in this list are picked up by systems
    A_ECS__REMOVED, // removed entities with outstanding references
    A_ECS__MUTED, // recently muted entities
    A_ECS__NUM
} AEcsListType;

#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_list.v.h"

struct AEcs {
    AList* lists[A_ECS__NUM]; // each entity is in at most one list
    AList* tickSystems; // tick systems in the specified order
    AList* drawSystems; // draw systems in the specified order
    AList* allSystems; // tick & draw systems
    AList* messageQueue; // queued messages
    bool deleting; // set when this collection is popped off the stack
};

extern AEcs* a__ecs;

extern void a_ecs__init(void);
extern void a_ecs__uninit(void);

extern void a_ecs__tick(void);
extern void a_ecs__draw(void);

extern void a_ecs__pushCollection(AList* TickSystems, AList* DrawSystems);
extern void a_ecs__popCollection(void);

extern bool a_ecs__isEntityInList(const AEntity* Entity, AEcsListType List);
extern void a_ecs__addEntityToList(AEntity* Entity, AEcsListType List);
extern void a_ecs__moveEntityToList(AEntity* Entity, AEcsListType List);
