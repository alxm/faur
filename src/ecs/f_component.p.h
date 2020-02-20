/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

typedef void FComponentTemplateInit(void* Data, const FBlock* Config);
typedef void FComponentTemplateFree(void* Data);

typedef void FComponentInstanceInit(void* Self, const void* Data);
typedef void FComponentInstanceFree(void* Self);

struct FComponent {
    size_t size; // total size of FComponentInstance + user data that follows
    size_t templateSize; // size of template data buffer
    const char* stringId; // unique string ID
    FComponentInstanceInit* init; // sets component buffer default values
    FComponentInstanceFree* free; // does not free the actual component buffer
    FComponentTemplateInit* templateInit; // init template buffer with FBlock
    FComponentTemplateFree* templateFree; // does not free the template buffer
    unsigned bitId; // unique number ID
};

#define F_COMPONENT(Name, InstanceSize, InstanceInit, InstanceFree, TemplateSize, TemplateInit, TemplateFree) \
    FComponent Name = {                                                                                       \
        .size = InstanceSize,                                                                                 \
        .init = InstanceInit,                                                                                 \
        .free = InstanceFree,                                                                                 \
        .templateSize = TemplateSize,                                                                         \
        .templateInit = TemplateInit,                                                                         \
        .templateFree = TemplateFree,                                                                         \
        .stringId = F_STRINGIFY(Name),                                                                        \
    }

extern const void* f_component_dataGet(const void* ComponentBuffer);
extern FEntity* f_component_entityGet(const void* ComponentBuffer);

#endif // F_INC_ECS_COMPONENT_P_H
