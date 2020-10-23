/*
    Copyright 2016, 2018-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_ENTITY_V_H
#define F_INC_ECS_ENTITY_V_H

#include "f_entity.p.h"

#include "../data/f_list.v.h"
#include "../ecs/f_component.v.h"
#include "../ecs/f_system.v.h"
#include "../ecs/f_template.v.h"

#define F_ENTITY__ACTIVE_REMOVED F_FLAGS_BIT(0) // kicked by active-only system
#define F_ENTITY__ACTIVE_PERMANENT F_FLAGS_BIT(1) // always report as active
#define F_ENTITY__DEBUG F_FLAGS_BIT(2) // print debug messages for this entity
#define F_ENTITY__REMOVED F_FLAGS_BIT(3) // marked for removal, may have refs
#define F_ENTITY__REMOVE_INACTIVE F_FLAGS_BIT(4) // mark for removal if kicked
#define F_ENTITY__ALLOC_STRING_ID F_FLAGS_BIT(5) // free string ID if set

struct FEntity {
    char* id; // specified name for debugging
    const FTemplate* templ; // template used to init this entity's components
    FEntity* parent; // manually associated parent entity
    FListNode* node; // list node in one of FEntityList
    FListNode* collectionNode; // FCollection list nod
    FList* matchingSystemsActive; // FList<FSystem*>
    FList* matchingSystemsRest; // FList<FSystem*>
    FList* systemNodesActive; // FList<FListNode*> in active-only FSystem lists
    FList* systemNodesEither; // FList<FListNode*> in rest FSystem lists
    FBitfield* componentBits; // each component's bit is set
    unsigned lastActive; // frame when f_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    unsigned flags; // various properties
    FComponentInstance* componentsTable[1]; // [f_component__num] Buffer/NULL
};

extern bool f_entity__ignoreRefDec;

extern void f_entity__init(void);
extern void f_entity__uninit(void);

extern void f_entity__tick(void);
extern void f_entity__flushFromSystems(void);
extern unsigned f_entity__numGet(void);
extern unsigned f_entity__numGetActive(void);

extern void f_entity__free(FEntity* Entity);
extern void f_entity__freeEx(FEntity* Entity);

extern void f_entity__flushFromSystemsActive(FEntity* Entity);

#endif // F_INC_ECS_ENTITY_V_H
