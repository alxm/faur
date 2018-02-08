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
#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_ecs_message.v.h"
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_mem.v.h"

AEcs* a__ecs;
static AList* g_stack; // list of AEcs (one for each state)

void a_ecs__init(void)
{
    a__ecs = NULL;
    g_stack = a_list_new();

    a_ecs_component__init();
    a_ecs_system__init();
}

void a_ecs__uninit(void)
{
    while(a__ecs != NULL) {
        a_ecs__popCollection();
    }

    a_list_free(g_stack);

    a_ecs_system__uninit();
    a_ecs_component__uninit();
}

void a_ecs__pushCollection(AList* TickSystems, AList* DrawSystems)
{
    AEcs* c = a_mem_malloc(sizeof(AEcs));

    for(int i = A_ECS__NUM; i--; ) {
        c->lists[i] = a_list_new();
    }

    c->tickSystems = TickSystems;
    c->drawSystems = DrawSystems;
    c->allSystems = a_list_new();
    c->messageQueue = a_list_new();
    c->deleting = false;

    a_list_appendCopy(c->allSystems, TickSystems);
    a_list_appendCopy(c->allSystems, DrawSystems);

    if(a__ecs != NULL) {
        a_list_push(g_stack, a__ecs);
    }

    a__ecs = c;

    A_LIST_ITERATE(a__ecs->allSystems, ASystem*, system) {
        system->runsInCurrentState = true;
    }
}

void a_ecs__popCollection(void)
{
    a__ecs->deleting = true;

    A_LIST_ITERATE(a__ecs->allSystems, ASystem*, system) {
        system->muted = false;
        system->runsInCurrentState = false;
    }

    a_list_freeEx(a__ecs->messageQueue, (AFree*)a_ecs_message__free);

    for(int i = A_ECS__NUM; i--; ) {
        a_list_freeEx(a__ecs->lists[i], (AFree*)a_ecs_entity__free);
    }

    a_list_free(a__ecs->allSystems);

    free(a__ecs);
    a__ecs = a_list_pop(g_stack);

    if(a__ecs != NULL) {
        A_LIST_ITERATE(a__ecs->allSystems, ASystem*, system) {
            system->runsInCurrentState = true;
        }
    }
}

void a_ecs__tick(void)
{
    a_ecs_flushNewEntities();

    A_LIST_ITERATE(a__ecs->tickSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }

    A_LIST_ITERATE(a__ecs->messageQueue, AMessage*, m) {
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

    a_list_clear(a__ecs->messageQueue);

    A_LIST_ITERATE(a__ecs->lists[A_ECS__REMOVED], AEntity*, entity) {
        if(entity->references == 0) {
            a_ecs_entity__free(entity);
            A_LIST_REMOVE_CURRENT();
        } else if(!entity->cleared) {
            a_ecs_entity__removeFromSystems(entity);
            entity->cleared = true;
        }
    }

    A_LIST_ITERATE(a__ecs->lists[A_ECS__MUTED], AEntity*, entity) {
        a_ecs_entity__removeFromSystems(entity);
        entity->node = NULL;
    }

    a_list_clear(a__ecs->lists[A_ECS__MUTED]);
}

void a_ecs__draw(void)
{
    A_LIST_ITERATE(a__ecs->drawSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }
}

void a_ecs_flushNewEntities(void)
{
    A_LIST_ITERATE(a__ecs->lists[A_ECS__NEW], AEntity*, e) {
        // Check if the entity matches any systems
        A_LIST_ITERATE(a__ecs->allSystems, ASystem*, s) {
            if(a_bitfield_testMask(e->componentBits, s->componentBits)) {
                a_list_addLast(e->systemNodes, a_list_addLast(s->entities, e));
            }
        }

        A_LIST_REMOVE_CURRENT();
        a_ecs__addEntityToList(e, A_ECS__RUNNING);
    }
}

bool a_ecs__isEntityInList(const AEntity* Entity, AEcsListType List)
{
    if(Entity->node == NULL) {
        return false;
    }

    return a_list__nodeGetList(Entity->node) == a__ecs->lists[List];
}

void a_ecs__addEntityToList(AEntity* Entity, AEcsListType List)
{
    Entity->node = a_list_addLast(a__ecs->lists[List], Entity);
}

void a_ecs__moveEntityToList(AEntity* Entity, AEcsListType List)
{
    if(Entity->node != NULL) {
        if(a_list__nodeGetList(Entity->node) == a__ecs->lists[List]) {
            return;
        }

        a_list_removeNode(Entity->node);
    }

    Entity->node = a_list_addLast(a__ecs->lists[List], Entity);
}
