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
#include "a2x_pack_ecs.v.h"

#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_ecs_message.v.h"
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_mem.v.h"

typedef struct {
    AList* lists[A_ECS__NUM]; // each entity is in at most one list
    AList* tickSystems; // tick systems in the specified order
    AList* drawSystems; // draw systems in the specified order
    AList* allSystems; // tick & draw systems
    AList* messageQueue; // queued messages
    bool deleting; // set when this collection is popped off the stack
} AEcs;

static AEcs* g_ecs;
static AList* g_stack; // list of AEcs (one for each state)

void a_ecs__init(void)
{
    g_ecs = NULL;
    g_stack = a_list_new();

    a_ecs_component__init();
    a_ecs_system__init();
}

void a_ecs__uninit(void)
{
    while(g_ecs != NULL) {
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

    if(g_ecs != NULL) {
        a_list_push(g_stack, g_ecs);
    }

    g_ecs = c;

    A_LIST_ITERATE(g_ecs->allSystems, ASystem*, system) {
        system->runsInCurrentState = true;
    }
}

void a_ecs__popCollection(void)
{
    g_ecs->deleting = true;

    A_LIST_ITERATE(g_ecs->allSystems, ASystem*, system) {
        system->muted = false;
        system->runsInCurrentState = false;
    }

    a_list_freeEx(g_ecs->messageQueue, (AFree*)a_ecs_message__free);

    for(int i = A_ECS__NUM; i--; ) {
        a_list_freeEx(g_ecs->lists[i], (AFree*)a_ecs_entity__free);
    }

    a_list_free(g_ecs->allSystems);

    free(g_ecs);
    g_ecs = a_list_pop(g_stack);

    if(g_ecs != NULL) {
        A_LIST_ITERATE(g_ecs->allSystems, ASystem*, system) {
            system->runsInCurrentState = true;
        }
    }
}

bool a_ecs__isDeleting(void)
{
    return g_ecs->deleting;
}

void a_ecs__tick(void)
{
    a_ecs_flushNewEntities();

    A_LIST_ITERATE(g_ecs->tickSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }

    A_LIST_ITERATE(g_ecs->messageQueue, AMessage*, m) {
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

    a_list_clear(g_ecs->messageQueue);

    A_LIST_ITERATE(g_ecs->lists[A_ECS__REMOVED], AEntity*, entity) {
        if(entity->references == 0) {
            a_ecs_entity__free(entity);
            A_LIST_REMOVE_CURRENT();
        } else if(!entity->cleared) {
            a_ecs_entity__removeFromSystems(entity);
            entity->cleared = true;
        }
    }

    A_LIST_ITERATE(g_ecs->lists[A_ECS__MUTED], AEntity*, entity) {
        a_ecs_entity__removeFromSystems(entity);

        A_LIST_REMOVE_CURRENT();
        a_ecs__addEntityToList(entity, A_ECS__DORMANT);
    }
}

void a_ecs__draw(void)
{
    A_LIST_ITERATE(g_ecs->drawSystems, ASystem*, system) {
        a_ecs_system__run(system);
    }
}

void a_ecs_flushNewEntities(void)
{
    A_LIST_ITERATE(g_ecs->lists[A_ECS__NEW], AEntity*, e) {
        // Check if the entity matches any systems
        A_LIST_ITERATE(g_ecs->allSystems, ASystem*, s) {
            if(a_bitfield_testMask(e->componentBits, s->componentBits)) {
                a_list_addLast(e->systemNodes, a_list_addLast(s->entities, e));
            }
        }

        A_LIST_REMOVE_CURRENT();
        a_ecs__addEntityToList(e, A_ECS__RUNNING);
    }
}

bool a_ecs__isEntityInList(const AEntity* Entity, AEcsListId List)
{
    if(Entity->node == NULL) {
        return false;
    }

    return a_list__nodeGetList(Entity->node) == g_ecs->lists[List];
}

void a_ecs__addEntityToList(AEntity* Entity, AEcsListId List)
{
    Entity->node = a_list_addLast(g_ecs->lists[List], Entity);
}

void a_ecs__moveEntityToList(AEntity* Entity, AEcsListId List)
{
    if(Entity->node != NULL) {
        if(a_list__nodeGetList(Entity->node) == g_ecs->lists[List]) {
            return;
        }

        a_list_removeNode(Entity->node);
    }

    Entity->node = a_list_addLast(g_ecs->lists[List], Entity);
}

void a_ecs__queueMessage(AEntity* To, AEntity* From, const char* Message)
{
    a_list_addLast(g_ecs->messageQueue, a_ecs_message__new(To, From, Message));
}
