/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_component.v.h"
#include <faur.v.h>

#if F_CONFIG_ECS_ENABLED
struct FComponent {
    size_t size; // total size of FComponentInstance + user data that follows
    FComponentInstanceInit* init; // sets component buffer default values
    FComponentInstanceInitEx* initEx; // init with template data
    FComponentInstanceFree* free; // does not free the actual component buffer
    size_t templateSize; // size of template data buffer
    FComponentTemplateInit* templateInit; // init template buffer with FBlock
    FComponentTemplateFree* templateFree; // does not free the template buffer
    const char* stringId; // string ID
};

static FComponent g_components[F_CONFIG_ECS_COM_NUM];
static FStrHash* g_componentsIndex; // table of FComponent
static const char* g_defaultId = "Unknown";

static inline const FComponentInstance* bufferGetInstance(const void* ComponentBuffer)
{
    return (void*)
            ((uint8_t*)ComponentBuffer - offsetof(FComponentInstance, buffer));
}

void f_component__init(void)
{
    g_componentsIndex = f_strhash_new();
}

void f_component__uninit(void)
{
    f_strhash_free(g_componentsIndex);
}

int f_component__stringToIndex(const char* StringId)
{
    const FComponent* component = f_strhash_get(g_componentsIndex, StringId);

    return component ? (int)(component - g_components) : -1;
}

const FComponent* f_component__get(int ComponentIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        if(ComponentIndex < 0 || ComponentIndex >= F_CONFIG_ECS_COM_NUM) {
            F__FATAL("Unknown component %d", ComponentIndex);
        }

        if(g_components[ComponentIndex].stringId == NULL) {
            F__FATAL("Uninitialized component %d", ComponentIndex);
        }
    #endif

    return &g_components[ComponentIndex];
}

void f_component_new(int ComponentIndex, size_t InstanceSize, FComponentInstanceInit* InstanceInit, FComponentInstanceFree* InstanceFree)
{
    FComponent* component = &g_components[ComponentIndex];

    #if F_CONFIG_BUILD_DEBUG
        if(component->stringId != NULL) {
            F__FATAL("f_component_new(%d): Already declared", ComponentIndex);
        }
    #endif

    component->size = sizeof(FComponentInstance)
                        - sizeof(FMaxMemAlignType) + InstanceSize;
    component->init = InstanceInit;
    component->free = InstanceFree;
    component->stringId = g_defaultId;
}

void f_component_template(int ComponentIndex, const char* StringId, size_t TemplateSize, FComponentTemplateInit* TemplateInit, FComponentTemplateFree* TemplateFree, FComponentInstanceInitEx* InstanceInitEx)
{
    FComponent* component = &g_components[ComponentIndex];

    #if F_CONFIG_BUILD_DEBUG
        if(ComponentIndex < 0 || ComponentIndex >= F_CONFIG_ECS_COM_NUM) {
            F__FATAL("f_component_template(%s): Unknown component", StringId);
        }

        if(g_components[ComponentIndex].stringId == NULL) {
            F__FATAL(
                "f_component_template(%s): Uninitialized component", StringId);
        }

        if(f_strhash_contains(g_componentsIndex, StringId)) {
            F__FATAL("f_component_template(%s): Already declared", StringId);
        }
    #endif

    component->initEx = InstanceInitEx;
    component->templateSize = TemplateSize;
    component->templateInit = TemplateInit;
    component->templateFree = TemplateFree;
    component->stringId = StringId;

    f_strhash_add(g_componentsIndex, StringId, component);
}

const void* f_component_dataGet(const void* ComponentBuffer)
{
    const FComponentInstance* instance = bufferGetInstance(ComponentBuffer);

    return f_template__dataGet(f_entity__templateGet(instance->entity),
                               (int)(instance->component - g_components));
}


FEntity* f_component_entityGet(const void* ComponentBuffer)
{
    return bufferGetInstance(ComponentBuffer)->entity;
}

const char* f_component__stringGet(const FComponent* Component)
{
    return Component->stringId;
}

void* f_component__templateInit(const FComponent* Component, const FBlock* Block)
{
    if(Component->templateSize > 0) {
        void* buffer = f_mem_zalloc(Component->templateSize);

        if(Component->templateInit) {
            Component->templateInit(buffer, Block);
        }

        return buffer;
    }

    return NULL;
}

void f_component__templateFree(const FComponent* Component, void* Buffer)
{
    if(Component->templateFree) {
        Component->templateFree(Buffer);
    }

    f_mem_free(Buffer);
}

FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity, const void* TemplateData)
{
    FComponentInstance* c = f_mem_zalloc(Component->size);

    c->component = Component;
    c->entity = Entity;

    if(Component->init) {
        Component->init(c->buffer);
    }

    if(Component->initEx && TemplateData) {
        Component->initEx(c->buffer, TemplateData);
    }

    return c;
}

void f_component__instanceFree(FComponentInstance* Instance)
{
    if(Instance == NULL) {
        return;
    }

    if(Instance->component->free) {
        Instance->component->free(Instance->buffer);
    }

    f_mem_free(Instance);
}
#endif // F_CONFIG_ECS_ENABLED