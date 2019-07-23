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

#include "a_entity.v.h"
#include <a2x.v.h>

struct AEntity {
    char* id; // specified name for debugging
    const ATemplate* template; // template used to init this entity's components
    AEntity* parent; // manually associated parent entity
    AListNode* node; // list node in one of AEcsListId
    AListNode* collectionNode; // ACollection list nod
    AList* matchingSystemsActive; // list of ASystem
    AList* matchingSystemsRest; // list of ASystem
    AList* systemNodesActive; // list of nodes in active-only ASystem lists
    AList* systemNodesEither; // list of nodes in normal ASystem.entities lists
    ABitfield* componentBits; // each component's bit is set
    unsigned lastActive; // frame when a_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    AEntityFlags flags; // various properties
    AComponentInstance* componentsTable[A_CONFIG_ECS_COM_NUM]; // Comp, or NULL
};

static AComponentInstance* componentAdd(AEntity* Entity, int ComponentIndex, const AComponent* Component, const void* TemplateData)
{
    AComponentInstance* c = a_component__instanceNew(
                                Component, Entity, TemplateData);

    Entity->componentsTable[ComponentIndex] = c;
    a_bitfield_set(Entity->componentBits, (unsigned)ComponentIndex);

    return c;
}

AEntity* a_entity_new(const char* Template)
{
    AEntity* e = a_mem_zalloc(sizeof(AEntity));

    e->matchingSystemsActive = a_list_new();
    e->matchingSystemsRest = a_list_new();
    e->systemNodesActive = a_list_new();
    e->systemNodesEither = a_list_new();
    e->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);
    e->lastActive = a_fps_ticksGet() - 1;

    ACollection* collection = a_ecs_collectionGet();

    if(collection) {
        a_collection__add(collection, e);
    }

    if(Template != NULL) {
        const ATemplate* template = a_template__get(Template);
        const char* id = a_str__fmt512("%s#%u",
                                       Template,
                                       a_template__instanceGet(template));

        e->id = a_str_dup(id);
        e->template = template;

        for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
            if(a_template__componentHas(template, c)) {
                componentAdd(e,
                             c,
                             a_component__get(c),
                             a_template__dataGet(template, c));
            }
        }
    }

    a_ecs__entityAddToList(e, A_ECS__NEW);

    return e;
}

void a_entity__free(AEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity__free(%s)", a_entity_idGet(Entity));
        }
    #endif

    if(Entity->collectionNode) {
        a_list_removeNode(Entity->collectionNode);
    }

    a_list_free(Entity->matchingSystemsActive);
    a_list_free(Entity->matchingSystemsRest);
    a_list_freeEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
    a_list_freeEx(Entity->systemNodesEither, (AFree*)a_list_removeNode);

    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        a_component__instanceFree(Entity->componentsTable[c]);
    }

    if(Entity->parent) {
        a_entity_refDec(Entity->parent);
    }

    a_bitfield_free(Entity->componentBits);

    free(Entity->id);
    free(Entity);
}

void a_entity_debugSet(AEntity* Entity, bool DebugOn)
{
    if(DebugOn) {
        A_FLAG_SET(Entity->flags, A_ENTITY__DEBUG);
    } else {
        A_FLAG_CLEAR(Entity->flags, A_ENTITY__DEBUG);
    }
}

const char* a_entity_idGet(const AEntity* Entity)
{
    return Entity->id ? Entity->id : "AEntity";
}

AEntity* a_entity_parentGet(const AEntity* Entity)
{
    return Entity->parent;
}

void a_entity_parentSet(AEntity* Entity, AEntity* Parent)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_parentSet(%s, %s)",
                        a_entity_idGet(Entity),
                        Parent ? a_entity_idGet(Parent) : "NULL");
        }
    #endif

    if(Entity->parent) {
        a_entity_refDec(Entity->parent);
    }

    Entity->parent = Parent;

    if(Parent) {
        a_entity_refInc(Parent);
    }
}

bool a_entity_parentHas(const AEntity* Child, const AEntity* PotentialParent)
{
    for(AEntity* p = Child->parent; p != NULL; p = p->parent) {
        if(p == PotentialParent) {
            return true;
        }
    }

    return false;
}

void a_entity_refInc(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
            A__FATAL("a_entity_refInc(%s): Entity is removed",
                     a_entity_idGet(Entity));
        }

        if(Entity->references == INT_MAX) {
            A__FATAL("a_entity_refInc(%s): Count too high",
                     a_entity_idGet(Entity));
        }

        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_refInc(%s) %d->%d",
                        a_entity_idGet(Entity),
                        Entity->references,
                        Entity->references + 1);
        }
    #endif

    Entity->references++;
}

void a_entity_refDec(AEntity* Entity)
{
    if(a_ecs__isDeleting()) {
        // The entity could have already been freed despite any outstanding
        // references. This is the only AEntity API that may be called by
        // components' AComponentFree callbacks.
        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->references == 0) {
            A__FATAL(
                "a_entity_refDec(%s): Count too low", a_entity_idGet(Entity));
        }

        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_refDec(%s) %d->%d",
                        a_entity_idGet(Entity),
                        Entity->references,
                        Entity->references - 1);
        }
    #endif

    if(--Entity->references == 0
        && A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {

        a_ecs__entityMoveToList(Entity, A_ECS__FLUSH);
    }
}

bool a_entity_removeGet(const AEntity* Entity)
{
    return A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED);
}

void a_entity_removeSet(AEntity* Entity)
{
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__warning("a_entity_removeSet(%s): Entity is removed",
                           a_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_removeSet(%s)", a_entity_idGet(Entity));
        }
    #endif

    A_FLAG_SET(Entity->flags, A_ENTITY__REMOVED);
    a_ecs__entityMoveToList(Entity, A_ECS__FLUSH);

    if(Entity->collectionNode) {
        a_list_removeNode(Entity->collectionNode);
        Entity->collectionNode = NULL;
    }
}

bool a_entity_activeGet(const AEntity* Entity)
{
    return A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_PERMANENT)
        || Entity->lastActive == a_fps_ticksGet();
}

void a_entity_activeSet(AEntity* Entity)
{
    if(Entity->muteCount > 0
        || A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_activeSet(%s)", a_entity_idGet(Entity));
        }
    #endif

    Entity->lastActive = a_fps_ticksGet();

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_REMOVED)) {
        A_FLAG_CLEAR(Entity->flags, A_ENTITY__ACTIVE_REMOVED);

        // Add entity back to active-only systems
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            a_list_addLast(
                Entity->systemNodesActive, a_system__entityAdd(system, Entity));
        }
    }
}

void a_entity_activeSetPermanent(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info(
                "a_entity_activeSetPermanent(%s)", a_entity_idGet(Entity));
        }
    #endif

    A_FLAG_SET(Entity->flags, A_ENTITY__ACTIVE_PERMANENT);
}

void* a_entity_componentAdd(AEntity* Entity, int ComponentIndex)
{
    const AComponent* component = a_component__get(ComponentIndex);

    #if A_CONFIG_BUILD_DEBUG
        if(!a_ecs__entityIsInList(Entity, A_ECS__NEW)) {
            A__FATAL("a_entity_componentAdd(%s, %s): Too late",
                     a_entity_idGet(Entity),
                     a_component__stringGet(component));
        }

        if(Entity->componentsTable[ComponentIndex] != NULL) {
            A__FATAL("a_entity_componentAdd(%s, %s): Already added",
                     a_entity_idGet(Entity),
                     a_component__stringGet(component));
        }

        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_componentAdd(%s, %s)",
                        a_entity_idGet(Entity),
                        a_component__stringGet(component));
        }
    #endif

    return a_component__instanceGetBuffer(
            componentAdd(Entity, ComponentIndex, component, NULL));
}

bool a_entity_componentHas(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        a_component__get(ComponentIndex);
    #endif

    return Entity->componentsTable[ComponentIndex] != NULL;
}

void* a_entity_componentGet(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        a_component__get(ComponentIndex);
    #endif

    AComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    return instance ? a_component__instanceGetBuffer(instance) : NULL;
}

void* a_entity_componentReq(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        a_component__get(ComponentIndex);
    #endif

    AComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    #if A_CONFIG_BUILD_DEBUG
        if(instance == NULL) {
            const AComponent* component = a_component__get(ComponentIndex);

            A__FATAL("a_entity_componentReq(%s, %s): Missing component",
                     a_entity_idGet(Entity),
                     a_component__stringGet(component));
        }
    #endif

    return a_component__instanceGetBuffer(instance);
}

bool a_entity_muteGet(const AEntity* Entity)
{
    return Entity->muteCount > 0;
}

void a_entity_muteInc(AEntity* Entity)
{
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__warning("a_entity_muteInc(%s): Entity is removed",
                           a_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == INT_MAX) {
            A__FATAL(
                "a_entity_muteInc(%s): Count too high", a_entity_idGet(Entity));
        }

        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_muteInc(%s) %d->%d",
                        a_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount + 1);
        }
    #endif

    if(Entity->muteCount++ == 0) {
        a_ecs__entityMoveToList(Entity, A_ECS__FLUSH);
    }
}

void a_entity_muteDec(AEntity* Entity)
{
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            a_out__warning("a_entity_muteDec(%s): Entity is removed",
                           a_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == 0) {
            A__FATAL(
                "a_entity_muteDec(%s): Count too low", a_entity_idGet(Entity));
        }

        if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            a_out__info("a_entity_muteDec(%s) %d->%d",
                        a_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount - 1);
        }
    #endif

    if(--Entity->muteCount == 0) {
        if(!a_list_isEmpty(Entity->matchingSystemsActive)
            || !a_list_isEmpty(Entity->matchingSystemsRest)) {

            if(a_ecs__entityIsInList(Entity, A_ECS__FLUSH)) {
                // Entity was muted and unmuted before it left systems
                a_ecs__entityMoveToList(Entity, A_ECS__DEFAULT);
            } else {
                // To be added back to matched systems
                a_ecs__entityMoveToList(Entity, A_ECS__RESTORE);
            }
        } else {
            // Entity has not been matched to systems yet, treat it as new
            a_ecs__entityMoveToList(Entity, A_ECS__NEW);
        }
    }
}

const ATemplate* a_entity__templateGet(const AEntity* Entity)
{
    return Entity->template;
}

bool a_entity__ecsCanDelete(const AEntity* Entity)
{
    return Entity->references == 0
            && A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED);
}

const AList* a_entity__ecsListGet(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->node);
}

void a_entity__ecsListAdd(AEntity* Entity, AList* List)
{
    Entity->node = a_list_addLast(List, Entity);
}

void a_entity__ecsListMove(AEntity* Entity, AList* List)
{
    a_list_removeNode(Entity->node);

    Entity->node = a_list_addLast(List, Entity);
}

void a_entity__collectionListAdd(AEntity* Entity, AList* List)
{
    Entity->collectionNode = a_list_addLast(List, Entity);
}

void a_entity__systemsMatch(AEntity* Entity, ASystem* System)
{
    if(a_bitfield_testMask(
        Entity->componentBits, a_system__componentBitsGet(System))) {

        if(a_system__isActiveOnly(System)) {
            a_list_addLast(Entity->matchingSystemsActive, System);
        } else {
            a_list_addLast(Entity->matchingSystemsRest, System);
        }
    }
}

void a_entity__systemsAddTo(AEntity* Entity)
{
    if(!A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_REMOVED)) {
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            a_list_addLast(
                Entity->systemNodesActive, a_system__entityAdd(system, Entity));
        }
    }

    A_LIST_ITERATE(Entity->matchingSystemsRest, ASystem*, system) {
        a_list_addLast(
            Entity->systemNodesEither, a_system__entityAdd(system, Entity));
    }
}

void a_entity__systemsRemoveFromAll(AEntity* Entity)
{
    a_list_clearEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
    a_list_clearEx(Entity->systemNodesEither, (AFree*)a_list_removeNode);
}

void a_entity__systemsRemoveFromActive(AEntity* Entity)
{
    A_FLAG_SET(Entity->flags, A_ENTITY__ACTIVE_REMOVED);
    a_list_clearEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
}
