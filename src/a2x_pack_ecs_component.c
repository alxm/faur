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

#include "a2x_system_includes.h"
#include "a2x_pack_ecs_component.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

static AStrHash* g_components; // table of AComponent

static inline AComponentHeader* getHeader(const void* Component)
{
    return (AComponentHeader*)Component - 1;
}

void a_component__init(void)
{
    g_components = a_strhash_new();
}

void a_component__uninit(void)
{
    a_strhash_freeEx(g_components, free);
}

AComponent* a_component__get(const char* Component)
{
    return a_strhash_get(g_components, Component);
}

unsigned a_component__num(void)
{
    return a_strhash_sizeGet(g_components);
}

void a_component_new(const char* Name, size_t Size, AInit* Init, AFree* Free)
{
    if(a_strhash_contains(g_components, Name)) {
        a_out__fatal("Component '%s' already declared", Name);
    }

    AComponent* c = a_mem_malloc(sizeof(AComponent));

    c->size = sizeof(AComponentHeader) + Size;
    c->init = Init;
    c->free = Free;
    c->bit = a_strhash_sizeGet(g_components);

    a_strhash_add(g_components, Name, c);
}

AEntity* a_component_entityGet(const void* Component)
{
    return getHeader(Component)->entity;
}
