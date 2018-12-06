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

#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_strhash.v.h"

unsigned a_component__tableLen;
static AComponent* g_componentsTable;

static AStrHash* g_components; // table of AComponent

static inline AComponentInstance* getHeader(const void* Component)
{
    return (AComponentInstance*)Component - 1;
}

void a_component__init(unsigned NumComponents)
{
    g_components = a_strhash_new();

    a_component__tableLen = NumComponents;
    g_componentsTable = a_mem_zalloc(NumComponents * sizeof(AComponent));

    while(NumComponents--) {
        g_componentsTable[NumComponents].stringId = "???";
        g_componentsTable[NumComponents].bit = UINT_MAX;
    }
}

void a_component__uninit(void)
{
    a_strhash_free(g_components);
    free(g_componentsTable);
}

int a_component__stringToIndex(const char* StringId)
{
    const AComponent* c = a_strhash_get(g_components, StringId);

    return c ? (int)c->bit : -1;
}

const AComponent* a_component__get(int Component, const char* CallerFunction)
{
    #if A_BUILD_DEBUG
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
    #else
        A_UNUSED(CallerFunction);
    #endif

    return &g_componentsTable[Component];
}

void a_component_new(int Index, const char* StringId, size_t Size, AInit* Init, AFree* Free)
{
    if(g_componentsTable == NULL) {
        a_out__fatal(
            "a_component_new(%d, %s): Call a_ecs_init first", Index, StringId);
    }

    AComponent* c = &g_componentsTable[Index];

    if(c->bit != UINT_MAX || a_strhash_contains(g_components, StringId)) {
        a_out__fatal(
            "a_component_new(%d, %s): Already declared", Index, StringId);
    }

    c->size = sizeof(AComponentInstance) + Size;
    c->init = Init;
    c->free = Free;
    c->stringId = StringId;
    c->bit = (unsigned)Index;

    a_strhash_add(g_components, StringId, c);
}

void a_component_newEx(int Index, const char* StringId, size_t DataSize, AComponentDataInit* DataInit, AFree* DataFree, size_t ComponentSize, AInitWithData* ComponentInitWithData, AFree* ComponentFree)
{
    a_component_new(Index, StringId, ComponentSize, NULL, ComponentFree);

    AComponent* c = &g_componentsTable[Index];

    c->dataSize = DataSize;
    c->dataInit = DataInit;
    c->dataFree = DataFree;
    c->initWithData = ComponentInitWithData;
}

const void* a_component_dataGet(const void* Component)
{
    AComponentInstance* h = getHeader(Component);

    return a_template__dataGet(
            h->entity->template, (int)(h->component - g_componentsTable));
}

AEntity* a_component_entityGet(const void* Component)
{
    return getHeader(Component)->entity;
}
