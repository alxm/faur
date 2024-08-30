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

#if F_CONFIG_ECS
static FPool** g_pools;

void f_component__init(void)
{
    g_pools = f_mem_malloc(f_component__num * sizeof(FPool*));

    for(unsigned c = f_component__num; c--; ) {
        const FComponent* component = f_component__array[c];

        component->runtime->bitId = c;

        g_pools[c] = f_pool_new(
                        component->size
                            + (unsigned)(sizeof(FComponentInstance)
                            - sizeof(FMaxMemAlignType)));
    }
}

void f_component__uninit(void)
{
    for(unsigned c = f_component__num; c--; ) {
        f_pool_free(g_pools[c]);
    }

    f_mem_free(g_pools);
}

static inline const FComponentInstance* bufferGetInstance(const void* ComponentBuffer)
{
    return (void*)
            ((uint8_t*)ComponentBuffer - offsetof(FComponentInstance, buffer));
}

FEntity* f_component_entityGet(const void* ComponentBuffer)
{
    F__CHECK(ComponentBuffer != NULL);

    return bufferGetInstance(ComponentBuffer)->entity;
}

FComponentInstance* f_component__instanceNew(const FComponent* Component, FEntity* Entity)
{
    FComponentInstance* c = f_pool_alloc(g_pools[Component->runtime->bitId]);

    c->component = Component;
    c->entity = Entity;

    if(Component->init) {
        Component->init(c->buffer);
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
#endif // F_CONFIG_ECS
