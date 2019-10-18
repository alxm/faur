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

#if F_CONFIG_ECS_ENABLED
struct ASystem {
    ASystemHandler* handler;
    ASystemSort* compare;
    ABitfield* componentBits; // IDs of components that this system works on
    AList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
};

static ASystem g_systems[F_CONFIG_ECS_SYS_NUM];

void f_system__uninit(void)
{
    for(unsigned s = F_CONFIG_ECS_SYS_NUM; s--; ) {
        f_list_free(g_systems[s].entities);
        f_bitfield_free(g_systems[s].componentBits);
    }
}

ASystem* f_system__get(int SystemIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        if(SystemIndex < 0 || SystemIndex >= F_CONFIG_ECS_SYS_NUM) {
            F__FATAL("Unknown system %d", SystemIndex);
        }

        if(g_systems[SystemIndex].entities == NULL) {
            F__FATAL("Uninitialized system %d", SystemIndex);
        }
    #endif

    return &g_systems[SystemIndex];
}

void f_system_new(int SystemIndex, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    #if F_CONFIG_BUILD_DEBUG
        if(g_systems[SystemIndex].entities != NULL) {
            F__FATAL("f_system_new(%d): Already declared", SystemIndex);
        }
    #endif

    ASystem* system = &g_systems[SystemIndex];

    system->handler = Handler;
    system->compare = Compare;
    system->entities = f_list_new();
    system->componentBits = f_bitfield_new(F_CONFIG_ECS_COM_NUM);
    system->onlyActiveEntities = OnlyActiveEntities;
}

void f_system_add(int SystemIndex, int ComponentIndex)
{
    ASystem* system = f_system__get(SystemIndex);

    f_bitfield_set(system->componentBits, (unsigned)ComponentIndex);
}

void f_system_run(int SystemIndex)
{
    ASystem* system = f_system__get(SystemIndex);

    if(system->compare) {
        f_list_sort(system->entities, (AListCompare*)system->compare);
    }

    if(system->onlyActiveEntities) {
        F_LIST_ITERATE(system->entities, AEntity*, entity) {
            if(f_entity_activeGet(entity)) {
                system->handler(entity);
            } else {
                f_entity__systemsRemoveFromActive(entity);
            }
        }
    } else {
        F_LIST_ITERATE(system->entities, AEntity*, entity) {
            system->handler(entity);
        }
    }

    f_ecs__flushEntitiesFromSystems();
}

AListNode* f_system__entityAdd(const ASystem* System, AEntity* Entity)
{
    return f_list_addLast(System->entities, Entity);
}

const ABitfield* f_system__componentBitsGet(const ASystem* System)
{
    return System->componentBits;
}

bool f_system__isActiveOnly(const ASystem* System)
{
    return System->onlyActiveEntities;
}
#endif // F_CONFIG_ECS_ENABLED
