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

#ifndef F_INC_ECS_COMPONENT_V_H
#define F_INC_ECS_COMPONENT_V_H

#include "f_component.p.h"

typedef struct FComponent FComponent;

#include "../memory/f_mem.v.h"

typedef struct {
    const FComponent* component; // shared data for all components of same type
    FEntity* entity; // entity this component belongs to
    FMaxMemAlignType buffer[1];
} FComponentInstance;

extern void f_component__init(void);
extern void f_component__uninit(void);

extern int f_component__stringToIndex(const char* StringId);
extern const FComponent* f_component__get(int Index);

extern const char* f_component__stringGet(const FComponent* Component);

extern void* f_component__templateInit(const FComponent* Component, const FBlock* Block);
extern void f_component__templateFree(const FComponent* Component, void* Buffer);

extern FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity, const void* TemplateData);
extern void f_component__instanceFree(FComponentInstance* Instance);

#endif // F_INC_ECS_COMPONENT_V_H
