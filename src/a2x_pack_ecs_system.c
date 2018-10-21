/*
    Copyright 2016-2018 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_ecs_system.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

unsigned a_system__tableLen;
static ASystem* g_systemsTable;

static AList* g_inactive; // list of AEntity detected as inactive

void a_system__init(void)
{
    g_inactive = a_list_new();
}

void a_system__uninit(void)
{
    for(unsigned s = a_system__tableLen; s--; ) {
        a_list_free(g_systemsTable[s].entities);
        a_bitfield_free(g_systemsTable[s].componentBits);
    }

    free(g_systemsTable);

    a_list_free(g_inactive);
}

void a_system__tableInit(unsigned NumSystems)
{
    a_system__tableLen = NumSystems;
    g_systemsTable = a_mem_malloc(NumSystems * sizeof(ASystem));

    while(NumSystems--) {
        g_systemsTable[NumSystems].name = "???";
        g_systemsTable[NumSystems].entities = NULL;
    }
}

ASystem* a_system__tableGet(int System, const char* CallerFunction)
{
    if(g_systemsTable == NULL) {
        a_out__fatal("%s: Call a_ecs_init first", CallerFunction);
    }

    if(System < 0 || System >= (int)a_system__tableLen) {
        a_out__fatal("%s: Unknown system %d", CallerFunction, System);
    }

    if(g_systemsTable[System].entities == NULL) {
        a_out__fatal("%s: Uninitialized system %d", CallerFunction, System);
    }

    return &g_systemsTable[System];
}

void a_system_new(int Index, const char* Name, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    if(g_systemsTable == NULL) {
        a_out__fatal("a_system_new: Call a_ecs_init first");
    }

    if(g_systemsTable[Index].entities != NULL) {
        a_out__fatal("a_system_new: '%s' (%d) already declared", Name, Index);
    }

    ASystem* s = &g_systemsTable[Index];

    s->name = Name;
    s->handler = Handler;
    s->compare = Compare;
    s->entities = a_list_new();
    s->componentBits = a_bitfield_new(a_component__tableLen);
    s->onlyActiveEntities = OnlyActiveEntities;
    s->muted = false;
}

void a_system_add(int System, int Component)
{
    ASystem* s = a_system__tableGet(System, __func__);
    const AComponent* c = a_component__tableGet(Component, __func__);

    a_bitfield_set(s->componentBits, c->bit);
}

void a_system_run(int System)
{
    ASystem* system = a_system__tableGet(System, __func__);

    if(system->muted) {
        return;
    }

    if(system->compare) {
        a_list_sort(system->entities, (AListCompare*)system->compare);
    }

    if(system->onlyActiveEntities) {
        A_LIST_ITERATE(system->entities, AEntity*, entity) {
            if(a_entity_activeGet(entity)) {
                system->handler(entity);
            } else {
                a_list_addLast(g_inactive, entity);
            }
        }

        a_list_clearEx(g_inactive, (AFree*)a_entity__removeFromActiveSystems);
    } else {
        A_LIST_ITERATE(system->entities, AEntity*, entity) {
            system->handler(entity);
        }
    }

    a_ecs__flushEntitiesFromSystems();
}

void a_system_muteSet(int System, bool DoMute)
{
    ASystem* system = a_system__tableGet(System, __func__);

    system->muted = DoMute;
}
