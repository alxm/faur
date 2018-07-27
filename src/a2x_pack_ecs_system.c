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

#include "a2x_system_includes.h"
#include "a2x_pack_ecs_system.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strhash.v.h"

static AStrHash* g_systems; // table of declared ASystem
static AList* g_inactive; // list of AEntity detected as inactive

void a_system__init(void)
{
    g_systems = a_strhash_new();
    g_inactive = a_list_new();
}

void a_system__uninit(void)
{
    A_STRHASH_ITERATE(g_systems, ASystem*, system) {
        a_list_free(system->entities);
        a_bitfield_free(system->componentBits);
        free(system);
    }

    a_strhash_free(g_systems);
    a_list_free(g_inactive);
}

ASystem* a_system__get(const char* System)
{
    return a_strhash_get(g_systems, System);
}

void a_system_new(const char* System, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    if(a_strhash_contains(g_systems, System)) {
        a_out__fatal("a_system_new: System '%s' already declared", System);
    }

    ASystem* s = a_mem_malloc(sizeof(ASystem));

    s->handler = Handler;
    s->compare = Compare;
    s->entities = a_list_new();
    s->componentBits = a_bitfield_new(a_component__num());
    s->onlyActiveEntities = OnlyActiveEntities;
    s->muted = false;
    s->runsInCurrentState = false;

    a_strhash_add(g_systems, System, s);
}

void a_system_add(const char* System, const char* Component)
{
    ASystem* s = a_strhash_get(g_systems, System);

    if(s == NULL) {
        a_out__fatal("a_system_add: Unknown system '%s'", System);
    }

    AComponent* c = a_component__get(Component);

    if(c == NULL) {
        a_out__fatal("a_system_add: Unknown component '%s'", Component);
    }

    a_bitfield_set(s->componentBits, c->bit);
}

void a_system__run(ASystem* System)
{
    if(System->muted) {
        return;
    }

    if(System->compare) {
        a_list_sort(System->entities, (AListCompare*)System->compare);
    }

    if(System->onlyActiveEntities) {
        A_LIST_ITERATE(System->entities, AEntity*, entity) {
            if(a_entity_activeGet(entity)) {
                System->handler(entity);
            } else {
                a_list_addLast(g_inactive, entity);
            }
        }

        a_list_clearEx(g_inactive, (AFree*)a_entity__removeFromActiveSystems);
    } else {
        A_LIST_ITERATE(System->entities, AEntity*, entity) {
            System->handler(entity);
        }
    }

    a_ecs__flushEntitiesFromSystems();
}

void a_system_run(const char* System)
{
    ASystem* system = a_strhash_get(g_systems, System);

    if(system == NULL) {
        a_out__fatal("a_system_run: unknown system '%s'", System);
    }

    if(!system->runsInCurrentState) {
        a_out__fatal("a_system_run: '%s' is not set to run", System);
    }

    a_system__run(system);
}

void a_system_muteSet(const char* System, bool DoMute)
{
    ASystem* system = a_strhash_get(g_systems, System);

    if(system == NULL) {
        a_out__fatal("a_system_muteSet: unknown system '%s'", System);
    }

    if(!system->runsInCurrentState) {
        a_out__fatal("a_system_muteSet: '%s' is not set to run", System);
    }

    system->muted = DoMute;
}
