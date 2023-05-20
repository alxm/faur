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

#include "../data/f_bitfield.p.h"
#include "../data/f_list.p.h"
#include "../ecs/f_component.p.h"
#include "../ecs/f_entity.p.h"

typedef void FCallSystemHandler(FEntity* Entity);
typedef int FCallSystemSort(const FEntity* A, const FEntity* B);

typedef struct {
    FList* entities; // entities currently picked up by this system
    FBitfield* componentBits; // IDs of components that this system works on
} F__SystemRuntime;

struct FSystem {
    F__SystemRuntime* runtime; // all the non-constant data
    const char* stringId; // unique string ID
    FCallSystemHandler* handler; // invoked on each entity in list
    FCallSystemSort* compare; // for sorting the entities list before running
    bool onlyActiveEntities; // kick out entities that are not marked active
    const FComponent** components; // [componentsNum]
    unsigned componentsNum; // length of components array
};

#define F_SYSTEM(Name, Handler, SortCompare, OnlyActiveEntities, ...) \
    static F__SystemRuntime F_GLUE2(f__system_runtime_, Name);        \
                                                                      \
    const FSystem Name = {                                            \
        .runtime = &F_GLUE2(f__system_runtime_, Name),                \
        .stringId = F_STRINGIFY(Name),                                \
        .handler = Handler,                                           \
        .compare = SortCompare,                                       \
        .onlyActiveEntities = OnlyActiveEntities,                     \
        .components = (const FComponent*[]){__VA_ARGS__},             \
        .componentsNum = sizeof((const FComponent*[]){__VA_ARGS__})   \
                            / sizeof(const FComponent*),              \
    }

extern void f_system_run(const FSystem* System);

#endif // F_INC_ECS_SYSTEM_P_H
