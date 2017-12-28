/*
    Copyright 2016, 2017 Alex Margarit

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

#include "a2x_pack_ecs.v.h"

AEcsCollection* a__ecsCollection;
static AList* g_collectionStack; // list of AEcsCollection (one for each state)

void a_ecs__init(void)
{
    a__ecsCollection = NULL;
    g_collectionStack = a_list_new();

    a_ecs_component__init();
    a_ecs_system__init();
}

void a_ecs__uninit(void)
{
    while(a__ecsCollection != NULL) {
        a_ecs__popCollection();
    }

    a_list_free(g_collectionStack);

    a_ecs_system__uninit();
    a_ecs_component__uninit();
}

void a_ecs__pushCollection(AList* TickSystems, AList* DrawSystems)
{
    AEcsCollection* c = a_mem_malloc(sizeof(AEcsCollection));

    c->newEntities = a_list_new();
    c->runningEntities = a_list_new();
    c->removedEntities = a_list_new();
    c->tickSystems = TickSystems;
    c->drawSystems = DrawSystems;
    c->allSystems = a_list_new();
    c->messageQueue = a_list_new();
    c->deleting = false;

    a_list_appendCopy(c->allSystems, TickSystems);
    a_list_appendCopy(c->allSystems, DrawSystems);

    if(a__ecsCollection != NULL) {
        a_list_push(g_collectionStack, a__ecsCollection);
    }

    a__ecsCollection = c;

    A_LIST_ITERATE(a__ecsCollection->allSystems, ASystem*, system) {
        system->runsInCurrentState = true;
    }
}

void a_ecs__popCollection(void)
{
    a__ecsCollection->deleting = true;

    A_LIST_ITERATE(a__ecsCollection->allSystems, ASystem*, system) {
        system->muted = false;
        system->runsInCurrentState = false;
    }

    a_list_freeEx(a__ecsCollection->messageQueue, (AFree*)a_ecs_message__free);
    a_list_freeEx(a__ecsCollection->newEntities, (AFree*)a_entity__free);
    a_list_freeEx(a__ecsCollection->runningEntities, (AFree*)a_entity__free);
    a_list_freeEx(a__ecsCollection->removedEntities, (AFree*)a_entity__free);
    a_list_free(a__ecsCollection->allSystems);

    free(a__ecsCollection);
    a__ecsCollection = a_list_pop(g_collectionStack);

    if(a__ecsCollection != NULL) {
        A_LIST_ITERATE(a__ecsCollection->allSystems, ASystem*, system) {
            system->runsInCurrentState = true;
        }
    }
}

void a_ecs__tick(void)
{
    a_ecs_flushNewEntities();

    A_LIST_ITERATE(a__ecsCollection->tickSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }

    A_LIST_ITERATE(a__ecsCollection->messageQueue, AMessage*, m) {
        AMessageHandlerContainer* h = a_strhash_get(m->to->handlers,
                                                    m->message);

        if(!a_entity_isRemoved(m->to) && !a_entity_isRemoved(m->from)) {
            if(m->to->muted) {
                // Keep message in queue
                continue;
            } else {
                h->handler(m->to, m->from);
            }
        }

        a_ecs_message__free(m);
    }

    a_list_clear(a__ecsCollection->messageQueue);

    A_LIST_ITERATE(a__ecsCollection->removedEntities, AEntity*, entity) {
        if(entity->references == 0) {
            a_entity__free(entity);
            A_LIST_REMOVE_CURRENT();
        } else if(!entity->cleared) {
            // Remove entity from any systems it's in
            a_list_clearEx(entity->systemNodes, (AFree*)a_list_removeNode);
            a_list_clear(entity->sleepingInSystems);
            entity->cleared = true;
        }
    }
}

void a_ecs__draw(void)
{
    A_LIST_ITERATE(a__ecsCollection->drawSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }
}

void a_ecs_flushNewEntities(void)
{
    A_LIST_ITERATE(a__ecsCollection->newEntities, AEntity*, e) {
        // Check if the entity matches any systems
        A_LIST_ITERATE(a__ecsCollection->allSystems, ASystem*, s) {
            if(a_bitfield_testMask(e->componentBits, s->componentBits)) {
                a_list_addLast(e->systemNodes, a_list_addLast(s->entities, e));
            }
        }

        e->node = a_list_addLast(a__ecsCollection->runningEntities, e);
        A_LIST_REMOVE_CURRENT();
    }
}
