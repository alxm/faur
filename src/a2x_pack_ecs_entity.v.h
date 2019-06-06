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

#pragma once

#include "a2x_pack_ecs_entity.p.h"

#include "a2x_pack_bitfield.v.h"
#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_ecs_template.v.h"
#include "a2x_pack_list.v.h"

typedef enum {
    A_ENTITY__ACTIVE_REMOVED = A_FLAG_BIT(0), // kicked out by active system
    A_ENTITY__ACTIVE_PERMANENT = A_FLAG_BIT(1), // entity always reports active
    A_ENTITY__DEBUG = A_FLAG_BIT(2), // print debug messages for this entity
    A_ENTITY__REMOVED = A_FLAG_BIT(3), // marked for removal, may have refs
} AEntityFlags;

struct AEntity {
    char* id; // specified name for debugging
    const ATemplate* template; // template used to init this entity's components
    AEntity* parent; // manually associated parent entity
    AListNode* node; // list node in one of AEcsListId
    AListNode* collectionNode; // ACollection list nod
    AList* matchingSystemsActive; // list of ASystem
    AList* matchingSystemsRest; // list of ASystem
    AList* systemNodesActive; // list of nodes in active-only ASystem lists
    AList* systemNodesEither; // list of nodes in normal ASystem.entities lists
    ABitfield* componentBits; // each component's bit is set
    unsigned lastActive; // frame when a_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    AEntityFlags flags; // various properties
    AComponentInstance* componentsTable[A_CONFIG_ECS_COM_NUM]; // Comp, or NULL
};

extern void a_entity__free(AEntity* Entity);

extern void a_entity__removeFromAllSystems(AEntity* Entity);
extern void a_entity__removeFromActiveSystems(AEntity* Entity);

extern bool a_entity__isMatchedToSystems(const AEntity* Entity);
