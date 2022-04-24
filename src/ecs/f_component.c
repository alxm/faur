/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

FComponent* const* f_component__array; // [f_component__num]
unsigned f_component__num;
FHash* f_component__index; // FHash<const char*, FComponent*>

static FPool** g_pools;

void f_component__init(FComponent* const* Components, size_t ComponentsNum)
{
    f_component__array = Components;
    f_component__num = (unsigned)ComponentsNum;
    f_component__index = f_hash_newStr(256, false);

    g_pools = f_mem_malloc(ComponentsNum * sizeof(FPool*));

    for(unsigned c = f_component__num; c--; ) {
        FComponent* com = f_component__array[c];

        com->size +=
            (unsigned)(sizeof(FComponentInstance) - sizeof(FMaxMemAlignType));
        com->bitId = c;

        f_hash_add(f_component__index, com->stringId, com);

        g_pools[c] = f_pool_new(com->size);
    }
}

void f_component__uninit(void)
{
    for(unsigned c = f_component__num; c--; ) {
        f_pool_free(g_pools[c]);
    }

    f_mem_free(g_pools);
    f_hash_free(f_component__index);
}

static inline const FComponentInstance* bufferGetInstance(const void* ComponentBuffer)
{
    return (void*)
            ((uint8_t*)ComponentBuffer - offsetof(FComponentInstance, buffer));
}

const void* f_component_dataGet(const void* ComponentBuffer)
{
    const FComponentInstance* instance = bufferGetInstance(ComponentBuffer);

    if(instance->entity->templ == NULL) {
        F__FATAL("f_component_dataGet(%s.%s): No template",
                 instance->entity->id,
                 instance->component->stringId);
    }

    return instance->entity->templ->data[instance->component->bitId];
}

FEntity* f_component_entityGet(const void* ComponentBuffer)
{
    return bufferGetInstance(ComponentBuffer)->entity;
}

void* f_component__dataInit(const FComponent* Component, const FBlock* Block)
{
    if(Component->dataSize > 0) {
        void* buffer = f_mem_mallocz(Component->dataSize);

        if(Component->dataInit) {
            Component->dataInit(buffer, Block);
        }

        return buffer;
    }

    return NULL;
}

void f_component__dataFree(const FComponent* Component, void* Buffer)
{
    if(Buffer == NULL) {
        return;
    }

    if(Component->dataFree) {
        Component->dataFree(Buffer);
    }

    f_mem_free(Buffer);
}

FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity, const void* Data)
{
    FComponentInstance* c = f_pool_alloc(g_pools[Component->bitId]);

    c->component = Component;
    c->entity = Entity;

    if(Component->init) {
        Component->init(c->buffer, Data);
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

    f_pool_release(Instance);
}
