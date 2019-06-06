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

#include "a2x_pack_ecs_component.v.h"

#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_strhash.v.h"

struct AComponent {
    size_t size; // total size of AComponentInstance + user data that follows
    AInit* init; // sets component buffer default values
    AInitWithData* initWithData; // init component buffer with template data
    AFree* free; // does not free the actual component buffer
    size_t dataSize; // size of template data buffer
    AComponentDataInit* dataInit; // init template buffer with info from ABlock
    AFree* dataFree; // does not free the actual template buffer
    const char* stringId; // string ID
    unsigned bit; // component's unique bit ID
};

static AComponent g_componentsTable[A_CONFIG_ECS_COM_NUM];
static AStrHash* g_components; // table of AComponent

static inline AComponentInstance* getHeader(const void* Component)
{
    return (AComponentInstance*)Component - 1;
}

void a_component__init(void)
{
    g_components = a_strhash_new();

    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        g_componentsTable[c].stringId = "???";
        g_componentsTable[c].bit = UINT_MAX;
    }
}

void a_component__uninit(void)
{
    a_strhash_free(g_components);
}

int a_component__stringToIndex(const char* StringId)
{
    const AComponent* c = a_strhash_get(g_components, StringId);

    return c ? (int)c->bit : -1;
}

const AComponent* a_component__get(int Component, const char* CallerFunction)
{
    #if A_CONFIG_BUILD_DEBUG
        if(Component < 0 || Component >= A_CONFIG_ECS_COM_NUM) {
            A__FATAL("%s: Unknown component %d", CallerFunction, Component);
        }

        if(g_componentsTable[Component].bit == UINT_MAX) {
            A__FATAL(
                "%s: Uninitialized component %d", CallerFunction, Component);
        }
    #else
        A_UNUSED(CallerFunction);
    #endif

    return &g_componentsTable[Component];
}

void a_component_new(int Index, const char* StringId, size_t Size, AInit* Init, AFree* Free)
{
    AComponent* c = &g_componentsTable[Index];

    if(c->bit != UINT_MAX || a_strhash_contains(g_components, StringId)) {
        A__FATAL("a_component_new(%d, %s): Already declared", Index, StringId);
    }

    c->size = sizeof(AComponentInstance) + Size;
    c->init = Init;
    c->free = Free;
    c->stringId = StringId;
    c->bit = (unsigned)Index;

    a_strhash_add(g_components, StringId, c);
}

void a_component_newEx(int Index, const char* StringId, size_t Size, AInitWithData* InitWithData, AFree* Free, size_t DataSize, AComponentDataInit* DataInit, AFree* DataFree)
{
    a_component_new(Index, StringId, Size, NULL, Free);

    AComponent* c = &g_componentsTable[Index];

    c->initWithData = InitWithData;
    c->dataSize = DataSize;
    c->dataInit = DataInit;
    c->dataFree = DataFree;
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

const char* a_component__stringGet(const AComponent* Component)
{
    return Component->stringId;
}

unsigned a_component__bitGet(const AComponent* Component)
{
    return Component->bit;
}

void* a_component__dataInit(const AComponent* Component, const ABlock* Block)
{
    if(Component->dataSize > 0) {
        void* buffer = a_mem_zalloc(Component->dataSize);

        if(Component->dataInit) {
            Component->dataInit(buffer, Block);
        }

        return buffer;
    }

    return NULL;
}

void a_component__dataFree(const AComponent* Component, void* Buffer)
{
    if(Component->dataFree) {
        Component->dataFree(Buffer);
    }

    free(Buffer);
}

AComponentInstance* a_component__instanceNew(const AComponent* Component, AEntity* Entity, const void* TemplateData)
{
    AComponentInstance* c = a_mem_zalloc(Component->size);

    c->component = Component;
    c->entity = Entity;

    void* self = a_component__headerGetData(c);

    if(Component->init) {
        Component->init(self);
    }

    if(Component->initWithData && TemplateData) {
        Component->initWithData(self, TemplateData);
    }

    return c;
}

void a_component__instanceFree(AComponentInstance* Instance)
{
    if(Instance == NULL) {
        return;
    }

    if(Instance->component->free) {
        Instance->component->free(a_component__headerGetData(Instance));
    }

    free(Instance);
}
