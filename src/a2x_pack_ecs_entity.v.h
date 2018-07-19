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
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_list.v.h"
#include "a2x_pack_strhash.v.h"

struct AEntity {
    char* id; // specified name for debugging
    void* context; // global context
    AEntity* parent; // manually associated parent entity
    AListNode* node; // list node in one of AEcsListId
    AList* matchingSystemsActive; // list of ASystem
    AList* matchingSystemsEither; // list of ASystem
    AList* systemNodesActive; // list of nodes in active-only ASystem lists
    AList* systemNodesEither; // list of nodes in normal ASystem.entities lists
    AStrHash* components; // table of AComponentHeader
    ABitfield* componentBits;
    AStrHash* handlers; // table of AMessageHandlerContainer
    unsigned lastActive; // frame when a_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    bool removedFromActive; // set when an active-only system kicks entity out
};

typedef struct {
    AEntity* to;
    AEntity* from;
    char* message;
} AMessage;

typedef struct {
    AMessageHandler* handler;
    bool handleImmediately;
} AMessageHandlerContainer;

extern void a_entity__free(AEntity* Entity);

extern void a_entity__removeFromAllSystems(AEntity* Entity);
extern void a_entity__removeFromActiveSystems(AEntity* Entity);

extern bool a_entity__isMatchedToSystems(const AEntity* Entity);

extern AMessage* a_message__new(AEntity* To, AEntity* From, const char* Message);
extern void a_message__free(AMessage* Message);
