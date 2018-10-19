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

#include "a2x_pack_ecs_component.p.h"

typedef struct {
    size_t size; // total size of AComponentHeader + user data that follows
    AInit* init; // sets default values
    AFree* free; // does not free the actual pointer
    const char* name; // string ID
    unsigned bit; // component's unique bit ID
} AComponent;

typedef struct {
    const AComponent* component; // shared data for all components of same type
    AEntity* entity; // entity this component belongs to
} AComponentHeader;

#include "a2x_pack_strhash.v.h"

extern unsigned a_component__tableLen;

extern void a_component__init(void);
extern void a_component__uninit(void);

extern void a_component__tableInit(unsigned NumComponents);
extern const AComponent* a_component__tableGet(int Component, const char* CallerFunction);

static inline void* a_component__headerGetData(const AComponentHeader* Header)
{
    return (void*)(Header + 1);
}
