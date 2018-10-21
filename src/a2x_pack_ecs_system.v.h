/*
    Copyright 2016-2018 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_pack_ecs_system.p.h"

typedef struct ASystem ASystem;

#include "a2x_pack_bitfield.v.h"
#include "a2x_pack_list.v.h"

struct ASystem {
    const char* name;
    ASystemHandler* handler;
    ASystemSort* compare;
    ABitfield* componentBits; // IDs of components that this system works on
    AList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
    bool muted; // a_system__run skips muted systems
};

extern unsigned a_system__tableLen;

extern void a_system__init(void);
extern void a_system__uninit(void);

extern void a_system__tableInit(unsigned NumSystems);
extern ASystem* a_system__tableGet(int System, const char* CallerFunction);

extern void a_system__run(ASystem* System);
