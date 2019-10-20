/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "general/f_system_includes.h"

#include "data/f_block.p.h"
#include "ecs/f_entity.p.h"

typedef void FComponentInstanceInit(void* Self);
typedef void FComponentInstanceInitEx(void* Self, const void* Data);
typedef void FComponentInstanceFree(void* Self);

typedef void FComponentTemplateInit(void* Data, const FBlock* Config);
typedef void FComponentTemplateFree(void* Data);

extern void f_component_new(int ComponentIndex, size_t InstanceSize, FComponentInstanceInit* InstanceInit, FComponentInstanceFree* InstanceFree);
extern void f_component_template(int ComponentIndex, const char* StringId, size_t TemplateSize, FComponentTemplateInit* TemplateInit, FComponentTemplateFree* TemplateFree, FComponentInstanceInitEx* InstanceInitEx);

extern const void* f_component_dataGet(const void* ComponentBuffer);
extern FEntity* f_component_entityGet(const void* ComponentBuffer);

#endif // F_INC_ECS_COMPONENT_P_H
