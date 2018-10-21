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

#include "a2x_pack_ecs.v.h"

#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_ecs_entity.v.h"
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"

typedef struct {
    AList* lists[A_ECS__NUM]; // each entity is in exactly one list
    bool deleting; // set when this collection is popped off the stack
} AEcs;

static AEcs* g_ecs;
static AList* g_stack; // list of AEcs (one for each state)

void a_ecs__init(void)
{
    g_ecs = NULL;
    g_stack = a_list_new();

    a_component__init();
    a_system__init();
}

void a_ecs__uninit(void)
{
    while(g_ecs != NULL) {
        a_ecs__collectionPop();
    }

    a_list_free(g_stack);

    a_system__uninit();
    a_component__uninit();
}

void a_ecs_init(unsigned NumComponents, unsigned NumSystems, unsigned NumMessages)
{
    a_component__tableInit(NumComponents);
    a_system__tableInit(NumSystems);

    a_entity__msgLen = NumMessages;
}

bool a_ecs__isDeleting(void)
{
    return g_ecs->deleting;
}

void a_ecs__collectionPush(void)
{
    AEcs* c = a_mem_malloc(sizeof(AEcs));

    for(int i = A_ECS__NUM; i--; ) {
        c->lists[i] = a_list_new();
    }

    c->deleting = false;

    if(g_ecs) {
        a_list_push(g_stack, g_ecs);
    }

    g_ecs = c;
}

void a_ecs__collectionPop(void)
{
    g_ecs->deleting = true;

    for(unsigned id = a_system__tableLen; id--; ) {
        ASystem* system = a_system__tableGet((int)id, __func__);

        system->muted = false;
    }

    for(int i = A_ECS__NUM; i--; ) {
        a_list_freeEx(g_ecs->lists[i], (AFree*)a_entity__free);
    }

    free(g_ecs);
    g_ecs = a_list_pop(g_stack);
}

void a_ecs__tick(void)
{
    a_ecs__flushEntitiesFromSystems();
    a_list_clearEx(g_ecs->lists[A_ECS__REMOVED_FREE], (AFree*)a_entity__free);

    // Check what systems the new entities match
    A_LIST_ITERATE(g_ecs->lists[A_ECS__NEW], AEntity*, e) {
        for(unsigned id = a_system__tableLen; id--; ) {
            ASystem* s = a_system__tableGet((int)id, __func__);

            if(a_bitfield_testMask(e->componentBits, s->componentBits)) {
                if(s->onlyActiveEntities) {
                    a_list_addLast(e->matchingSystemsActive, s);
                } else {
                    a_list_addLast(e->matchingSystemsEither, s);
                }
            }
        }

        a_ecs__entityAddToList(e, A_ECS__RESTORE);
    }

    a_list_clear(g_ecs->lists[A_ECS__NEW]);

    // Add entities to the systems they match
    A_LIST_ITERATE(g_ecs->lists[A_ECS__RESTORE], AEntity*, e) {
        if(!e->removedFromActive) {
            A_LIST_ITERATE(e->matchingSystemsActive, ASystem*, system) {
                a_list_addLast(
                    e->systemNodesActive, a_list_addLast(system->entities, e));
            }
        }

        A_LIST_ITERATE(e->matchingSystemsEither, ASystem*, system) {
            a_list_addLast(
                e->systemNodesEither, a_list_addLast(system->entities, e));
        }

        a_ecs__entityAddToList(e, A_ECS__RUNNING);
    }

    a_list_clear(g_ecs->lists[A_ECS__RESTORE]);
}

bool a_ecs__entityIsInList(const AEntity* Entity, AEcsListId List)
{
    return a_list__nodeGetList(Entity->node) == g_ecs->lists[List];
}

void a_ecs__entityAddToList(AEntity* Entity, AEcsListId List)
{
    Entity->node = a_list_addLast(g_ecs->lists[List], Entity);
}

void a_ecs__entityMoveToList(AEntity* Entity, AEcsListId List)
{
    if(a_list__nodeGetList(Entity->node) == g_ecs->lists[List]) {
        return;
    }

    a_list_removeNode(Entity->node);
    Entity->node = a_list_addLast(g_ecs->lists[List], Entity);
}

void a_ecs__flushEntitiesFromSystems(void)
{
    A_LIST_ITERATE(g_ecs->lists[A_ECS__MUTED_QUEUE], AEntity*, e) {
        a_entity__removeFromAllSystems(e);
        a_ecs__entityAddToList(e, A_ECS__MUTED_LIMBO);
    }

    A_LIST_ITERATE(g_ecs->lists[A_ECS__REMOVED_QUEUE], AEntity*, e) {
        a_entity__removeFromAllSystems(e);

        if(e->references == 0) {
            a_ecs__entityAddToList(e, A_ECS__REMOVED_FREE);
        } else {
            a_ecs__entityAddToList(e, A_ECS__REMOVED_LIMBO);
        }
    }

    a_list_clear(g_ecs->lists[A_ECS__MUTED_QUEUE]);
    a_list_clear(g_ecs->lists[A_ECS__REMOVED_QUEUE]);
}
