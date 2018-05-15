/*
    Copyright 2016, 2017 Alex Margarit

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
    AFree* free; // does not free the actual pointer
    unsigned bit; // component's unique bit ID
} AComponent;

typedef struct {
    const AComponent* component;
    AEntity* entity; // entity this component belongs to
} AComponentHeader;

#include "a2x_pack_strhash.v.h"

extern AStrHash* a__ecsComponents;

extern void a_component__init(void);
extern void a_component__uninit(void);
