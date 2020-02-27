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

#ifndef F_INC_ECS_COMPONENT_V_H
#define F_INC_ECS_COMPONENT_V_H

#include "f_component.p.h"

#include "../data/f_hash.v.h"
#include "../memory/f_mem.v.h"

typedef struct {
    const FComponent* component; // shared data for all components of same type
    FEntity* entity; // entity this component belongs to
    FMaxMemAlignType buffer[1];
} FComponentInstance;

extern FComponent* const* f_component__array;
extern unsigned f_component__num;
extern FHash* f_component__index;

extern void f_component__init(FComponent* const* Components, size_t ComponentsNum);
extern void f_component__uninit(void);

extern void* f_component__dataInit(const FComponent* Component, const FBlock* Block);
extern void f_component__dataFree(const FComponent* Component, void* Buffer);

extern FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity, const void* Data);
extern void f_component__instanceFree(FComponentInstance* Instance);

#endif // F_INC_ECS_COMPONENT_V_H
