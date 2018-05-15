/*
    Copyright 2016, 2018 Alex Margarit

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

#include "a2x_pack_ecs_entity.p.h"

#include "a2x_pack_bitfield.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_strhash.v.h"

struct AEntity {
    char* id; // specified name for debugging
    void* context; // global context
    AEntity* parent; // manually associated parent entity
    AListNode* node; // list node in one of AEcsListId
    AList* matchingSystemsActive; // list of ASystem
    AList* matchingSystemsEither; // list of ASystem
    AList* systemNodes; // list of nodes in ASystem.entities lists
    AList* sleepingInSystems; // list of ASystem that entity is asleep in
    AStrHash* components; // table of AComponentHeader
    ABitfield* componentBits;
    AStrHash* handlers; // table of AMessageHandlerContainer
    unsigned lastActive; // frame when a_entity_markActive was last called
    unsigned references; // if >0, then the entity lingers in the removed list
};

extern void a_entity__free(AEntity* Entity);
extern void a_entity__removeFromSystems(AEntity* Entity);
