/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_ECS_ECS_V_H
#define A_INC_ECS_ECS_V_H

#include "ecs/a_ecs.p.h"

typedef enum {
    A_ECS__INVALID = -1,
    A_ECS__DEFAULT, // no pending changes
    A_ECS__NEW, // new entities that aren't in any systems yet
    A_ECS__RESTORE, // entities that will be added to systems
    A_ECS__REMOVED_QUEUE, // entities marked for removal, still in systems
    A_ECS__REMOVED_FREE, // entities to be freed at the end of current frame
    A_ECS__MUTED_QUEUE, // just-muted entities, still in systems
    A_ECS__NUM
} AEcsListId;

#include "data/a_list.v.h"
#include "ecs/a_entity.v.h"
#include "general/a_main.v.h"

extern const APack a_pack__ecs;

extern bool a_ecs__isDeleting(void);

extern void a_ecs__tick(void);

extern bool a_ecs__entityIsInList(const AEntity* Entity, AEcsListId List);
extern void a_ecs__entityAddToList(AEntity* Entity, AEcsListId List);
extern void a_ecs__entityMoveToList(AEntity* Entity, AEcsListId List);

extern void a_ecs__flushEntitiesFromSystems(void);

#endif // A_INC_ECS_ECS_V_H
