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

#include "a2x_pack_ecs_component.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_strhash.v.h"

unsigned a_component__tableLen;
static AComponent* g_componentsTable;

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
    a_strhash_free(g_components);
    free(g_componentsTable);
}

void a_component__tableInit(unsigned NumComponents)
{
    a_component__tableLen = NumComponents;
    g_componentsTable = a_mem_malloc(NumComponents * sizeof(AComponent));

    while(NumComponents--) {
        g_componentsTable[NumComponents].name = "???";
        g_componentsTable[NumComponents].bit = UINT_MAX;
    }
}

const AComponent* a_component__tableGet(int Component, const char* CallerFunction)
{
    if(g_componentsTable == NULL) {
        a_out__fatal("%s: Call a_ecs_init first", CallerFunction);
    }

    if(Component < 0 || Component >= (int)a_component__tableLen) {
        a_out__fatal("%s: Unknown component %d", CallerFunction, Component);
    }

    if(g_componentsTable[Component].bit == UINT_MAX) {
        a_out__fatal(
            "%s: Uninitialized component %d", CallerFunction, Component);
    }

    return &g_componentsTable[Component];
}

void a_component_new(int Index, const char* Name, size_t Size, AInit* Init, AFree* Free)
{
    if(g_componentsTable == NULL) {
        a_out__fatal("a_component_new: Call a_ecs_init first");
    }

    if(g_componentsTable[Index].bit != UINT_MAX
        || a_strhash_contains(g_components, Name)) {

        a_out__fatal(
            "a_component_new: '%s' (%d) already declared", Name, Index);
    }

    AComponent* c = &g_componentsTable[Index];

    c->size = sizeof(AComponentHeader) + Size;
    c->init = Init;
    c->free = Free;
    c->name = Name;
    c->bit = (unsigned)Index;

    a_strhash_add(g_components, Name, c);
}

AEntity* a_component_entityGet(const void* Component)
{
    return getHeader(Component)->entity;
}

int a_component_stringToIndex(const char* StringId)
{
    const AComponent* c = a_strhash_get(g_components, StringId);

    if(c == NULL) {
        a_out__fatal("a_component_stringToIndex: Unknown id '%s'", StringId);
    }

    return (int)c->bit;
}
