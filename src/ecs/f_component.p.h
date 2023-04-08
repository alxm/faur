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

struct FComponent {
    unsigned size; // total size of FComponentInstance + user data that follows
    const char* stringId; // unique string ID
    FCallComponentInstanceInit* init; // sets component buffer default values
    FCallComponentInstanceFree* free; // does not free the actual comp buffer
    unsigned bitId; // unique number ID
};

#define F_COMPONENT(Name, InstanceSize, InstanceInit, InstanceFree) \
    FComponent Name = {                                             \
        .size = (unsigned)InstanceSize,                             \
        .init = (FCallComponentInstanceInit*)InstanceInit,          \
        .free = (FCallComponentInstanceFree*)InstanceFree,          \
        .stringId = F_STRINGIFY(Name),                              \
    }

extern FEntity* f_component_entityGet(const void* ComponentBuffer);

#endif // F_INC_ECS_COMPONENT_P_H
