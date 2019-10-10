/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_system.v.h"
#include <a2x.v.h>

struct ASystem {
    ASystemHandler* handler;
    ASystemSort* compare;
    ABitfield* componentBits; // IDs of components that this system works on
    AList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
};

static ASystem g_systems[A_CONFIG_ECS_SYS_NUM];

void a_system__uninit(void)
{
    for(unsigned s = A_CONFIG_ECS_SYS_NUM; s--; ) {
        a_list_free(g_systems[s].entities);
        a_bitfield_free(g_systems[s].componentBits);
    }
}

ASystem* a_system__get(int SystemIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        if(SystemIndex < 0 || SystemIndex >= A_CONFIG_ECS_SYS_NUM) {
            A__FATAL("Unknown system %d", SystemIndex);
        }

        if(g_systems[SystemIndex].entities == NULL) {
            A__FATAL("Uninitialized system %d", SystemIndex);
        }
    #endif

    return &g_systems[SystemIndex];
}

void a_system_new(int SystemIndex, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    #if A_CONFIG_BUILD_DEBUG
        if(g_systems[SystemIndex].entities != NULL) {
            A__FATAL("a_system_new(%d): Already declared", SystemIndex);
        }
    #endif

    ASystem* system = &g_systems[SystemIndex];

    system->handler = Handler;
    system->compare = Compare;
    system->entities = a_list_new();
    system->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);
    system->onlyActiveEntities = OnlyActiveEntities;
}

void a_system_add(int SystemIndex, int ComponentIndex)
{
    ASystem* system = a_system__get(SystemIndex);

    a_bitfield_set(system->componentBits, (unsigned)ComponentIndex);
}

void a_system_run(int SystemIndex)
{
    ASystem* system = a_system__get(SystemIndex);

    if(system->compare) {
        a_list_sort(system->entities, (AListCompare*)system->compare);
    }

    if(system->onlyActiveEntities) {
        A_LIST_ITERATE(system->entities, AEntity*, entity) {
            if(a_entity_activeGet(entity)) {
                system->handler(entity);
            } else {
                a_entity__systemsRemoveFromActive(entity);
            }
        }
    } else {
        A_LIST_ITERATE(system->entities, AEntity*, entity) {
            system->handler(entity);
        }
    }

    a_ecs__flushEntitiesFromSystems();
}

AListNode* a_system__entityAdd(const ASystem* System, AEntity* Entity)
{
    return a_list_addLast(System->entities, Entity);
}

const ABitfield* a_system__componentBitsGet(const ASystem* System)
{
    return System->componentBits;
}

bool a_system__isActiveOnly(const ASystem* System)
{
    return System->onlyActiveEntities;
}
