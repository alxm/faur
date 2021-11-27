/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_ECS_SYSTEM_P_H
#define F_INC_ECS_SYSTEM_P_H

#include "../general/f_system_includes.h"

typedef struct FSystem FSystem;

#include "../data/f_bitfield.v.h"
#include "../data/f_list.v.h"
#include "../ecs/f_component.p.h"
#include "../ecs/f_entity.p.h"

typedef void FCallSystemHandler(FEntity* Entity);
typedef int FCallSystemSort(const FEntity* A, const FEntity* B);

struct FSystem {
    const char* stringId; // unique string ID
    FList* entities; // entities currently picked up by this system
    const FComponent** components; // [componentsNum]
    FBitfield* componentBits; // IDs of components that this system works on
    FCallSystemHandler* handler; // invoked on each entity in list
    FCallSystemSort* compare; // for sorting the entities list before running
    unsigned componentsNum; // length of components array
    bool onlyActiveEntities; // kick out entities that are not marked active
};

#define F_SYSTEM(Name, Handler, SortCompare, OnlyActiveEntities, ...) \
    FSystem Name = {                                                  \
        .handler = Handler,                                           \
        .compare = SortCompare,                                       \
        .onlyActiveEntities = OnlyActiveEntities,                     \
        .components = (const FComponent*[]){__VA_ARGS__},             \
        .componentsNum = sizeof((const FComponent*[]){__VA_ARGS__})   \
                            / sizeof(const FComponent*),              \
        .stringId = F_STRINGIFY(Name),                                \
    }

extern void f_system_run(const FSystem* System);

#endif // F_INC_ECS_SYSTEM_P_H
