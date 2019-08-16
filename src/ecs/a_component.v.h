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

#ifndef A_INC_ECS_COMPONENT_V_H
#define A_INC_ECS_COMPONENT_V_H

#include "ecs/a_component.p.h"

typedef struct AComponent AComponent;

#include "memory/a_mem.v.h"

typedef struct {
    const AComponent* component; // shared data for all components of same type
    AEntity* entity; // entity this component belongs to
    AMaxMemAlignType buffer[];
} AComponentInstance;

extern void a_component__init(void);
extern void a_component__uninit(void);

extern int a_component__stringToIndex(const char* StringId);
extern const AComponent* a_component__get(int Index);

extern const char* a_component__stringGet(const AComponent* Component);

extern void* a_component__templateInit(const AComponent* Component, const ABlock* Block);
extern void a_component__templateFree(const AComponent* Component, void* Buffer);

extern AComponentInstance* a_component__instanceNew(const AComponent* Component, AEntity* Entity, const void* TemplateData);
extern void a_component__instanceFree(AComponentInstance* Instance);

#endif // A_INC_ECS_COMPONENT_V_H
