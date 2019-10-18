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

#include "f_entity.v.h"
#include <faur.v.h>

#if A_CONFIG_ECS_ENABLED
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
    unsigned lastActive; // frame when f_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    AEntityFlags flags; // various properties
    AComponentInstance* componentsTable[A_CONFIG_ECS_COM_NUM]; // Comp, or NULL
};

static AComponentInstance* componentAdd(AEntity* Entity, int ComponentIndex, const AComponent* Component, const void* TemplateData)
{
    AComponentInstance* c = f_component__instanceNew(
                                Component, Entity, TemplateData);

    Entity->componentsTable[ComponentIndex] = c;
    f_bitfield_set(Entity->componentBits, (unsigned)ComponentIndex);

    return c;
}

static inline bool listIsIn(const AEntity* Entity, AEcsListId List)
{
    return f_list__nodeGetList(Entity->node) == f_ecs__listGet(List);
}

static inline void listAddTo(AEntity* Entity, AEcsListId List)
{
    Entity->node = f_list_addLast(f_ecs__listGet(List), Entity);
}

static inline void listMoveTo(AEntity* Entity, AEcsListId List)
{
    f_list_removeNode(Entity->node);

    listAddTo(Entity, List);
}

AEntity* f_entity_new(const char* Template, const void* Context)
{
    AEntity* e = f_mem_zalloc(sizeof(AEntity));

    listAddTo(e, A_ECS__NEW);

    e->matchingSystemsActive = f_list_new();
    e->matchingSystemsRest = f_list_new();
    e->systemNodesActive = f_list_new();
    e->systemNodesEither = f_list_new();
    e->componentBits = f_bitfield_new(A_CONFIG_ECS_COM_NUM);
    e->lastActive = f_fps_ticksGet() - 1;

    ACollection* collection = f_collection__get();

    if(collection) {
        e->collectionNode = f_list_addLast(collection, e);
    }

    if(Template) {
        const ATemplate* template = f_template__get(Template);
        const char* id = f_str__fmt512("%s#%u",
                                       Template,
                                       f_template__instanceGet(template));

        e->id = f_str_dup(id);
        e->template = template;

        for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
            if(f_template__componentHas(template, c)) {
                componentAdd(e,
                             c,
                             f_component__get(c),
                             f_template__dataGet(template, c));
            }
        }

        f_template__initRun(template, e, Context);
    }

    return e;
}

void f_entity__free(AEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity__free(%s)", f_entity_idGet(Entity));
        }
    #endif

    if(Entity->collectionNode) {
        f_list_removeNode(Entity->collectionNode);
    }

    f_list_free(Entity->matchingSystemsActive);
    f_list_free(Entity->matchingSystemsRest);
    f_list_freeEx(Entity->systemNodesActive, (AFree*)f_list_removeNode);
    f_list_freeEx(Entity->systemNodesEither, (AFree*)f_list_removeNode);

    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        f_component__instanceFree(Entity->componentsTable[c]);
    }

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    f_bitfield_free(Entity->componentBits);

    f_mem_free(Entity->id);
    f_mem_free(Entity);
}

void f_entity__freeEx(AEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    f_list_removeNode(Entity->node);

    f_entity__free(Entity);
}

void f_entity_debugSet(AEntity* Entity, bool DebugOn)
{
    if(DebugOn) {
        A_FLAGS_SET(Entity->flags, A_ENTITY__DEBUG);
    } else {
        A_FLAGS_CLEAR(Entity->flags, A_ENTITY__DEBUG);
    }
}

const char* f_entity_idGet(const AEntity* Entity)
{
    return Entity->id ? Entity->id : "AEntity";
}

AEntity* f_entity_parentGet(const AEntity* Entity)
{
    return Entity->parent;
}

void f_entity_parentSet(AEntity* Entity, AEntity* Parent)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_parentSet(%s, %s)",
                        f_entity_idGet(Entity),
                        Parent ? f_entity_idGet(Parent) : "NULL");
        }
    #endif

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Parent
            && ((Parent->collectionNode
                    && Entity->collectionNode
                    && f_list__nodeGetList(Parent->collectionNode)
                        != f_list__nodeGetList(Entity->collectionNode))
                || (!!Parent->collectionNode ^ !!Entity->collectionNode))) {

            A__FATAL("f_entity_parentSet(%s, %s): Different collections",
                     f_entity_idGet(Entity),
                     f_entity_idGet(Parent));
        }
    #endif

    Entity->parent = Parent;

    if(Parent) {
        f_entity_refInc(Parent);
    }
}

bool f_entity_parentHas(const AEntity* Child, const AEntity* PotentialParent)
{
    for(AEntity* p = Child->parent; p != NULL; p = p->parent) {
        if(p == PotentialParent) {
            return true;
        }
    }

    return false;
}

void f_entity_refInc(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
            A__FATAL("f_entity_refInc(%s): Entity is removed",
                     f_entity_idGet(Entity));
        }

        if(Entity->references == INT_MAX) {
            A__FATAL("f_entity_refInc(%s): Count too high",
                     f_entity_idGet(Entity));
        }

        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_refInc(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->references,
                        Entity->references + 1);
        }
    #endif

    Entity->references++;
}

void f_entity_refDec(AEntity* Entity)
{
    if(f_ecs__refDecIgnoreGet()) {
        // The entity could have already been freed despite any outstanding
        // references. This is the only AEntity API that may be called by
        // components' AComponentInstanceFree callbacks.
        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->references == 0) {
            A__FATAL(
                "f_entity_refDec(%s): Count too low", f_entity_idGet(Entity));
        }

        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_refDec(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->references,
                        Entity->references - 1);
        }
    #endif

    Entity->references--;

    if(f_entity__canDelete(Entity)) {
        listMoveTo(Entity, A_ECS__FLUSH);
    }
}

bool f_entity_removedGet(const AEntity* Entity)
{
    return A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED);
}

void f_entity_removedSet(AEntity* Entity)
{
    if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_removedSet(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_removedSet(%s)", f_entity_idGet(Entity));
        }
    #endif

    A_FLAGS_SET(Entity->flags, A_ENTITY__REMOVED);
    listMoveTo(Entity, A_ECS__FLUSH);
}

bool f_entity_activeGet(const AEntity* Entity)
{
    return A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_PERMANENT)
        || Entity->lastActive == f_fps_ticksGet();
}

void f_entity_activeSet(AEntity* Entity)
{
    if(Entity->muteCount > 0
        || A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_activeSet(%s)", f_entity_idGet(Entity));
        }
    #endif

    Entity->lastActive = f_fps_ticksGet();

    if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_REMOVED)) {
        A_FLAGS_CLEAR(Entity->flags, A_ENTITY__ACTIVE_REMOVED);

        // Add entity back to active-only systems
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            f_list_addLast(
                Entity->systemNodesActive, f_system__entityAdd(system, Entity));
        }
    }
}

void f_entity_activeSetRemove(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info(
                "f_entity_activeSetRemove(%s)", f_entity_idGet(Entity));
        }
    #endif

    A_FLAGS_SET(Entity->flags, A_ENTITY__REMOVE_INACTIVE);
}

void f_entity_activeSetPermanent(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info(
                "f_entity_activeSetPermanent(%s)", f_entity_idGet(Entity));
        }
    #endif

    A_FLAGS_SET(Entity->flags, A_ENTITY__ACTIVE_PERMANENT);
}

void* f_entity_componentAdd(AEntity* Entity, int ComponentIndex)
{
    const AComponent* component = f_component__get(ComponentIndex);

    #if A_CONFIG_BUILD_DEBUG
        if(!listIsIn(Entity, A_ECS__NEW)) {
            A__FATAL("f_entity_componentAdd(%s, %s): Too late",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }

        if(Entity->componentsTable[ComponentIndex] != NULL) {
            A__FATAL("f_entity_componentAdd(%s, %s): Already added",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }

        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_componentAdd(%s, %s)",
                        f_entity_idGet(Entity),
                        f_component__stringGet(component));
        }
    #endif

    return componentAdd(Entity, ComponentIndex, component, NULL)->buffer;
}

bool f_entity_componentHas(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    return Entity->componentsTable[ComponentIndex] != NULL;
}

void* f_entity_componentGet(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    AComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    return instance ? instance->buffer : NULL;
}

void* f_entity_componentReq(const AEntity* Entity, int ComponentIndex)
{
    #if A_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    AComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    #if A_CONFIG_BUILD_DEBUG
        if(instance == NULL) {
            const AComponent* component = f_component__get(ComponentIndex);

            A__FATAL("f_entity_componentReq(%s, %s): Missing component",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }
    #endif

    return instance->buffer;
}

bool f_entity_muteGet(const AEntity* Entity)
{
    return Entity->muteCount > 0;
}

void f_entity_muteInc(AEntity* Entity)
{
    if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_muteInc(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == INT_MAX) {
            A__FATAL(
                "f_entity_muteInc(%s): Count too high", f_entity_idGet(Entity));
        }

        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_muteInc(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount + 1);
        }
    #endif

    if(Entity->muteCount++ == 0) {
        listMoveTo(Entity, A_ECS__FLUSH);
    }
}

void f_entity_muteDec(AEntity* Entity)
{
    if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED)) {
        #if A_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_muteDec(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if A_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == 0) {
            A__FATAL(
                "f_entity_muteDec(%s): Count too low", f_entity_idGet(Entity));
        }

        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info("f_entity_muteDec(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount - 1);
        }
    #endif

    if(--Entity->muteCount == 0) {
        if(!f_list_isEmpty(Entity->matchingSystemsActive)
            || !f_list_isEmpty(Entity->matchingSystemsRest)) {

            if(listIsIn(Entity, A_ECS__FLUSH)) {
                // Entity was muted and unmuted before it left systems
                listMoveTo(Entity, A_ECS__DEFAULT);
            } else {
                // To be added back to matched systems
                listMoveTo(Entity, A_ECS__RESTORE);
            }
        } else {
            // Entity has not been matched to systems yet, treat it as new
            listMoveTo(Entity, A_ECS__NEW);
        }
    }
}

const ATemplate* f_entity__templateGet(const AEntity* Entity)
{
    return Entity->template;
}

bool f_entity__canDelete(const AEntity* Entity)
{
    return Entity->references == 0
            && A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVED);
}

void f_entity__ecsListAdd(AEntity* Entity, AList* List)
{
    Entity->node = f_list_addLast(List, Entity);
}

void f_entity__systemsMatch(AEntity* Entity, ASystem* System)
{
    if(f_bitfield_testMask(
        Entity->componentBits, f_system__componentBitsGet(System))) {

        if(f_system__isActiveOnly(System)) {
            f_list_addLast(Entity->matchingSystemsActive, System);
        } else {
            f_list_addLast(Entity->matchingSystemsRest, System);
        }
    }
}

void f_entity__systemsAddTo(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(f_list_isEmpty(Entity->matchingSystemsActive)
            && f_list_isEmpty(Entity->matchingSystemsRest)) {

            f_out__warning("Entity %s was not matched to any systems",
                           f_entity_idGet(Entity));
        }
    #endif

    if(!A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_REMOVED)) {
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            f_list_addLast(
                Entity->systemNodesActive, f_system__entityAdd(system, Entity));
        }
    }

    A_LIST_ITERATE(Entity->matchingSystemsRest, ASystem*, system) {
        f_list_addLast(
            Entity->systemNodesEither, f_system__entityAdd(system, Entity));
    }

    listAddTo(Entity, A_ECS__DEFAULT);
}

void f_entity__systemsRemoveFromAll(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info(
                "%s removed from all systems", f_entity_idGet(Entity));
        }
    #endif

    f_list_clearEx(Entity->systemNodesActive, (AFree*)f_list_removeNode);
    f_list_clearEx(Entity->systemNodesEither, (AFree*)f_list_removeNode);
}

void f_entity__systemsRemoveFromActive(AEntity* Entity)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
            f_out__info(
                "%s removed from active-only systems", f_entity_idGet(Entity));
        }
    #endif

    A_FLAGS_SET(Entity->flags, A_ENTITY__ACTIVE_REMOVED);
    f_list_clearEx(Entity->systemNodesActive, (AFree*)f_list_removeNode);

    if(A_FLAGS_TEST_ANY(Entity->flags, A_ENTITY__REMOVE_INACTIVE)) {
        f_entity_removedSet(Entity);
    }
}
#endif // A_CONFIG_ECS_ENABLED
