/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_ECS_COMPONENT_P_H
#define A_INC_ECS_COMPONENT_P_H

#include "general/a_system_includes.h"

#include "data/a_block.p.h"
#include "ecs/a_entity.p.h"

typedef void AComponentInit(void* Self);
typedef void AComponentInitWithTemplate(void* Self, const void* Data);
typedef void AComponentTemplateInit(void* Data, const ABlock* Block);
typedef void AComponentFree(void* Self);

extern void a_component_new(int ComponentIndex, size_t Size, AComponentInit* Init, AFree* AComponentFree);

extern const void* a_component_templateGet(const void* Component);
extern void a_component_templateSet(int ComponentIndex, const char* StringId, size_t TemplateSize, AComponentTemplateInit* TemplateInit, AComponentFree* TemplateFree, AComponentInitWithTemplate* InitWithTemplate);

extern AEntity* a_component_entityGet(const void* Component);

#endif // A_INC_ECS_COMPONENT_P_H
