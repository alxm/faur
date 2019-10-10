/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_ECS_COMPONENT_P_H
#define A_INC_ECS_COMPONENT_P_H

#include "general/a_system_includes.h"

#include "data/a_block.p.h"
#include "ecs/a_entity.p.h"

typedef void AComponentInstanceInit(void* Self);
typedef void AComponentInstanceInitEx(void* Self, const void* Data);
typedef void AComponentInstanceFree(void* Self);

typedef void AComponentTemplateInit(void* Data, const ABlock* Config);
typedef void AComponentTemplateFree(void* Data);

extern void a_component_new(int ComponentIndex, size_t InstanceSize, AComponentInstanceInit* InstanceInit, AComponentInstanceFree* InstanceFree);
extern void a_component_template(int ComponentIndex, const char* StringId, size_t TemplateSize, AComponentTemplateInit* TemplateInit, AComponentTemplateFree* TemplateFree, AComponentInstanceInitEx* InstanceInitEx);

extern const void* a_component_dataGet(const void* ComponentBuffer);
extern AEntity* a_component_entityGet(const void* ComponentBuffer);

#endif // A_INC_ECS_COMPONENT_P_H
