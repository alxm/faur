/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

void f_component__init(FComponent* const* Components, size_t ComponentsNum)
{
    f_component__array = Components;
    f_component__num = (unsigned)ComponentsNum;
    f_component__index = f_hash_newStr(256, false);

    for(unsigned c = f_component__num; c--; ) {
        FComponent* com = f_component__array[c];

        com->size += sizeof(FComponentInstance) - sizeof(FMaxMemAlignType);
        com->bitId = c;

        f_hash_add(f_component__index, com->stringId, com);
    }
}

void f_component__uninit(void)
{
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
    const FTemplate* template = f_entity__templateGet(instance->entity);
    const void* data = NULL;

    f_template__dataGet(template, instance->component, &data);

    return data;
}

FEntity* f_component_entityGet(const void* ComponentBuffer)
{
    return bufferGetInstance(ComponentBuffer)->entity;
}

void* f_component__dataInit(const FComponent* Component, const FBlock* Block)
{
    if(Component->dataSize > 0) {
        void* buffer = f_mem_zalloc(Component->dataSize);

        if(Component->dataInit) {
            Component->dataInit(buffer, Block);
        }

        return buffer;
    }

    return NULL;
}

void f_component__dataFree(const FComponent* Component, void* Buffer)
{
    if(Component->dataFree) {
        Component->dataFree(Buffer);
    }

    f_mem_free(Buffer);
}

FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity, const void* Data)
{
    FComponentInstance* c = f_mem_zalloc(Component->size);

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

    f_mem_free(Instance);
}
