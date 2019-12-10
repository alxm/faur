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

#include "f_system.v.h"
#include <faur.v.h>

struct FSystem {
    FSystemHandler* handler;
    FSystemSort* compare;
    FBitfield* componentBits; // IDs of components that this system works on
    FList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
};

static FSystem* g_systems; // [f_init__ecs_sys]

void f_system__init(void)
{
    g_systems = f_mem_zalloc(sizeof(FSystem) * f_init__ecs_sys);
}

void f_system__uninit(void)
{
    for(unsigned s = f_init__ecs_sys; s--; ) {
        f_list_free(g_systems[s].entities);
        f_bitfield_free(g_systems[s].componentBits);
    }

    f_mem_free(g_systems);
}

FSystem* f_system__get(unsigned SystemIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        if(SystemIndex >= f_init__ecs_sys) {
            F__FATAL("Unknown system %u", SystemIndex);
        }

        if(g_systems[SystemIndex].entities == NULL) {
            F__FATAL("Uninitialized system %u", SystemIndex);
        }
    #endif

    return &g_systems[SystemIndex];
}

void f_system_new(unsigned SystemIndex, FSystemHandler* Handler, FSystemSort* Compare, bool OnlyActiveEntities)
{
    #if F_CONFIG_BUILD_DEBUG
        if(g_systems[SystemIndex].entities != NULL) {
            F__FATAL("f_system_new(%u): Already declared", SystemIndex);
        }
    #endif

    FSystem* system = &g_systems[SystemIndex];

    system->handler = Handler;
    system->compare = Compare;
    system->entities = f_list_new();
    system->componentBits = f_bitfield_new(f_init__ecs_com);
    system->onlyActiveEntities = OnlyActiveEntities;
}

void f_system_add(unsigned SystemIndex, unsigned ComponentIndex)
{
    FSystem* system = f_system__get(SystemIndex);

    f_bitfield_set(system->componentBits, ComponentIndex);
}

void f_system_run(unsigned SystemIndex)
{
    FSystem* system = f_system__get(SystemIndex);

    if(system->compare) {
        f_list_sort(system->entities, (FListCompare*)system->compare);
    }

    if(system->onlyActiveEntities) {
        F_LIST_ITERATE(system->entities, FEntity*, entity) {
            if(f_entity_activeGet(entity)) {
                system->handler(entity);
            } else {
                f_entity__systemsRemoveFromActive(entity);
            }
        }
    } else {
        F_LIST_ITERATE(system->entities, FEntity*, entity) {
            system->handler(entity);
        }
    }

    f_ecs__flushEntitiesFromSystems();
}

FListNode* f_system__entityAdd(const FSystem* System, FEntity* Entity)
{
    return f_list_addLast(System->entities, Entity);
}

const FBitfield* f_system__componentBitsGet(const FSystem* System)
{
    return System->componentBits;
}

bool f_system__isActiveOnly(const FSystem* System)
{
    return System->onlyActiveEntities;
}
