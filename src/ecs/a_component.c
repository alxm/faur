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

#include "ecs/a_component.v.h"

#include "data/a_strhash.v.h"
#include "ecs/a_entity.v.h"
#include "general/a_main.v.h"
#include "memory/a_mem.v.h"

struct AComponent {
    size_t size; // total size of AComponentInstance + user data that follows
    AComponentInit* init; // sets component buffer default values
    AComponentInitWithTemplate* initWithTemplate; // init with template data
    AFree* free; // does not free the actual component buffer
    size_t templateSize; // size of template data buffer
    AComponentTemplateInit* templateInit; // init template buffer with ABlock
    AFree* templateFree; // does not free the actual template buffer
    const char* stringId; // string ID
};

static AComponent g_components[A_CONFIG_ECS_COM_NUM];
static AStrHash* g_componentsIndex; // table of AComponent
static const char* g_defaultId = "Unknown";

static inline const AComponentInstance* bufferGetInstance(const void* ComponentBuffer)
{
    return (AComponentInstance*)ComponentBuffer - 1;
}

void a_component__init(void)
{
    g_componentsIndex = a_strhash_new();
}

void a_component__uninit(void)
{
    a_strhash_free(g_componentsIndex);
}

int a_component__stringToIndex(const char* StringId)
{
    const AComponent* component = a_strhash_get(g_componentsIndex, StringId);

    return component ? (int)(component - g_components) : -1;
}

const AComponent* a_component__get(int ComponentIndex, const char* CallerFunction)
{
    #if A_CONFIG_BUILD_DEBUG
        if(ComponentIndex < 0 || ComponentIndex >= A_CONFIG_ECS_COM_NUM) {
            A__FATAL(
                "%s: Unknown component %d", CallerFunction, ComponentIndex);
        }

        if(g_components[ComponentIndex].stringId == NULL) {
            A__FATAL("%s: Uninitialized component %d",
                     CallerFunction,
                     ComponentIndex);
        }
    #else
        A_UNUSED(CallerFunction);
    #endif

    return &g_components[ComponentIndex];
}

void a_component_new(int ComponentIndex, size_t Size, AComponentInit* Init, AFree* Free)
{
    AComponent* component = &g_components[ComponentIndex];

    #if A_CONFIG_BUILD_DEBUG
        if(component->stringId != NULL) {
            A__FATAL("a_component_new(%d): Already declared", ComponentIndex);
        }
    #endif

    component->size = sizeof(AComponentInstance) + Size;
    component->init = Init;
    component->free = Free;
    component->stringId = g_defaultId;
}

void a_component_template(int ComponentIndex, const char* StringId, size_t TemplateSize, AComponentTemplateInit* TemplateInit, AFree* TemplateFree, AComponentInitWithTemplate* InitWithTemplate)
{
    AComponent* component = &g_components[ComponentIndex];

    #if A_CONFIG_BUILD_DEBUG
        if(ComponentIndex < 0 || ComponentIndex >= A_CONFIG_ECS_COM_NUM) {
            A__FATAL("a_component_template(%d, %s): Unknown component",
                     ComponentIndex,
                     StringId);
        }

        if(g_components[ComponentIndex].stringId == NULL) {
            A__FATAL("a_component_template(%d, %s): Uninitialized component",
                     ComponentIndex,
                     StringId);
        }

        if(a_strhash_contains(g_componentsIndex, StringId)) {
            A__FATAL("a_component_template(%d, %s): Already declared",
                     ComponentIndex,
                     StringId);
        }
    #endif

    component->initWithTemplate = InitWithTemplate;
    component->templateSize = TemplateSize;
    component->templateInit = TemplateInit;
    component->templateFree = TemplateFree;
    component->stringId = StringId;

    a_strhash_add(g_componentsIndex, StringId, component);
}

const void* a_component_dataGet(const void* Component)
{
    const AComponentInstance* instance = bufferGetInstance(Component);

    return a_template__dataGet(a_entity__templateGet(instance->entity),
                               (int)(instance->component - g_components));
}

AEntity* a_component_entityGet(const void* Component)
{
    return bufferGetInstance(Component)->entity;
}

const char* a_component__stringGet(const AComponent* Component)
{
    return Component->stringId;
}

void* a_component__templateInit(const AComponent* Component, const ABlock* Block)
{
    if(Component->templateSize > 0) {
        void* buffer = a_mem_zalloc(Component->templateSize);

        if(Component->templateInit) {
            Component->templateInit(buffer, Block);
        }

        return buffer;
    }

    return NULL;
}

void a_component__templateFree(const AComponent* Component, void* Buffer)
{
    if(Component->templateFree) {
        Component->templateFree(Buffer);
    }

    free(Buffer);
}

AComponentInstance* a_component__instanceNew(const AComponent* Component, AEntity* Entity, const void* TemplateData)
{
    AComponentInstance* c = a_mem_zalloc(Component->size);

    c->component = Component;
    c->entity = Entity;

    void* self = a_component__instanceGetBuffer(c);

    if(Component->init) {
        Component->init(self);
    }

    if(Component->initWithTemplate && TemplateData) {
        Component->initWithTemplate(self, TemplateData);
    }

    return c;
}

void a_component__instanceFree(AComponentInstance* Instance)
{
    if(Instance == NULL) {
        return;
    }

    if(Instance->component->free) {
        Instance->component->free(a_component__instanceGetBuffer(Instance));
    }

    free(Instance);
}