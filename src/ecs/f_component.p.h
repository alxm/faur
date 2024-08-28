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

#ifndef F_INC_ECS_COMPONENT_P_H
#define F_INC_ECS_COMPONENT_P_H

#include "../general/f_system_includes.h"

typedef struct FComponent FComponent;

#include "../data/f_block.p.h"
#include "../ecs/f_entity.p.h"

typedef void FCallComponentDataInit(void* Data, const FBlock* Config);
typedef void FCallComponentDataFree(void* Data);

typedef void FCallComponentInstanceInit(void* Self);
typedef void FCallComponentInstanceFree(void* Self);

typedef struct {
    unsigned bitId; // unique number ID
} F__ComponentRuntime;

struct FComponent {
    F__ComponentRuntime* runtime; // all the non-constant data
    const char* stringId; // unique string ID
    unsigned size; // size of user data that follows FComponentInstance
    FCallComponentInstanceInit* init; // sets component buffer default values
    FCallComponentInstanceFree* free; // does not free the actual comp buffer
};

#define F_COMPONENT(Name, Type, InstanceInit, InstanceFree)          \
    static F__ComponentRuntime F_GLUE2(f__component_runtime_, Name); \
                                                                     \
    const FComponent Name = {                                        \
        .runtime = &F_GLUE2(f__component_runtime_, Name),            \
        .stringId = F_STRINGIFY(Name),                               \
        .size = (unsigned)sizeof(Type),                              \
        .init = (FCallComponentInstanceInit*)InstanceInit,           \
        .free = (FCallComponentInstanceFree*)InstanceFree,           \
    }

extern FEntity* f_component_entityGet(const void* ComponentBuffer);

#endif // F_INC_ECS_COMPONENT_P_H
