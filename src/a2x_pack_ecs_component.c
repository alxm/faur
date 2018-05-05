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

#include "a2x_system_includes.h"
#include "a2x_pack_ecs_component.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

AStrHash* a__ecsComponents; // table of declared AComponent

static inline AComponentHeader* getHeader(const void* Component)
{
    return (AComponentHeader*)Component - 1;
}

void a_ecs_component__init(void)
{
    a__ecsComponents = a_strhash_new();
}

void a_ecs_component__uninit(void)
{
    a_strhash_freeEx(a__ecsComponents, free);
}

void a_component_declare(const char* Name, size_t Size, AFree* Free)
{
    if(a_strhash_contains(a__ecsComponents, Name)) {
        a_out__fatal("Component '%s' already declared", Name);
    }

    AComponent* c = a_mem_malloc(sizeof(AComponent));

    c->size = sizeof(AComponentHeader) + Size;
    c->free = Free;
    c->bit = a_strhash_getSize(a__ecsComponents);

    a_strhash_add(a__ecsComponents, Name, c);
}

AEntity* a_component_getEntity(const void* Component)
{
    return getHeader(Component)->entity;
}
