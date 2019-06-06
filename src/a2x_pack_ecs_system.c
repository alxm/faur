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

#include "a2x_pack_ecs_system.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"

struct ASystem {
    ASystemHandler* handler;
    ASystemSort* compare;
    ABitfield* componentBits; // IDs of components that this system works on
    AList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
};

static ASystem g_systemsTable[A_CONFIG_ECS_SYS_NUM];

void a_system__uninit(void)
{
    for(unsigned s = A_CONFIG_ECS_SYS_NUM; s--; ) {
        a_list_free(g_systemsTable[s].entities);
        a_bitfield_free(g_systemsTable[s].componentBits);
    }
}

ASystem* a_system__get(int SystemIndex, const char* CallerFunction)
{
    #if A_CONFIG_BUILD_DEBUG
        if(SystemIndex < 0 || SystemIndex >= A_CONFIG_ECS_SYS_NUM) {
            A__FATAL("%s: Unknown system %d", CallerFunction, SystemIndex);
        }

        if(g_systemsTable[SystemIndex].entities == NULL) {
            A__FATAL(
                "%s: Uninitialized system %d", CallerFunction, SystemIndex);
        }
    #else
        A_UNUSED(CallerFunction);
    #endif

    return &g_systemsTable[SystemIndex];
}

void a_system_new(int SystemIndex, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    #if A_CONFIG_BUILD_DEBUG
        if(g_systemsTable[SystemIndex].entities != NULL) {
            A__FATAL("a_system_new(%d): Already declared", SystemIndex);
        }
    #endif

    ASystem* s = &g_systemsTable[SystemIndex];

    s->handler = Handler;
    s->compare = Compare;
    s->entities = a_list_new();
    s->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);
    s->onlyActiveEntities = OnlyActiveEntities;
}

void a_system_add(int SystemIndex, int ComponentIndex)
{
    ASystem* s = a_system__get(SystemIndex, __func__);

    a_bitfield_set(s->componentBits, (unsigned)ComponentIndex);
}

void a_system_run(int SystemIndex)
{
    ASystem* system = a_system__get(SystemIndex, __func__);

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
