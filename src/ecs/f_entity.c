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

#if F_CONFIG_FEATURE_ECS
struct FEntity {
    char* id; // specified name for debugging
    const FTemplate* template; // template used to init this entity's components
    FEntity* parent; // manually associated parent entity
    FListNode* node; // list node in one of FEcsListId
    FListNode* collectionNode; // FCollection list nod
    FList* matchingSystemsActive; // list of FSystem
    FList* matchingSystemsRest; // list of FSystem
    FList* systemNodesActive; // list of nodes in active-only FSystem lists
    FList* systemNodesEither; // list of nodes in normal FSystem.entities lists
    FBitfield* componentBits; // each component's bit is set
    unsigned lastActive; // frame when f_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    FEntityFlags flags; // various properties
    FComponentInstance* componentsTable[F_CONFIG_ECS_COM_NUM]; // Comp, or NULL
};

static FComponentInstance* componentAdd(FEntity* Entity, int ComponentIndex, const FComponent* Component, const void* TemplateData)
{
    FComponentInstance* c = f_component__instanceNew(
                                Component, Entity, TemplateData);

    Entity->componentsTable[ComponentIndex] = c;
    f_bitfield_set(Entity->componentBits, (unsigned)ComponentIndex);

    return c;
}

static inline bool listIsIn(const FEntity* Entity, FEcsListId List)
{
    return f_list__nodeGetList(Entity->node) == f_ecs__listGet(List);
}

static inline void listAddTo(FEntity* Entity, FEcsListId List)
{
    Entity->node = f_list_addLast(f_ecs__listGet(List), Entity);
}

static inline void listMoveTo(FEntity* Entity, FEcsListId List)
{
    f_list_removeNode(Entity->node);

    listAddTo(Entity, List);
}

FEntity* f_entity_new(const char* Template, const void* Context)
{
    FEntity* e = f_mem_zalloc(sizeof(FEntity));

    listAddTo(e, F_ECS__NEW);

    e->matchingSystemsActive = f_list_new();
    e->matchingSystemsRest = f_list_new();
    e->systemNodesActive = f_list_new();
    e->systemNodesEither = f_list_new();
    e->componentBits = f_bitfield_new(F_CONFIG_ECS_COM_NUM);
    e->lastActive = f_fps_ticksGet() - 1;

    FCollection* collection = f_collection__get();

    if(collection) {
        e->collectionNode = f_list_addLast(collection, e);
    }

    if(Template) {
        const FTemplate* template = f_template__get(Template);
        const char* id = f_str__fmt512("%s#%u",
                                       Template,
                                       f_template__instanceGet(template));

        e->id = f_str_dup(id);
        e->template = template;

        for(int c = F_CONFIG_ECS_COM_NUM; c--; ) {
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

void f_entity__free(FEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity__free(%s)", f_entity_idGet(Entity));
        }
    #endif

    if(Entity->collectionNode) {
        f_list_removeNode(Entity->collectionNode);
    }

    f_list_free(Entity->matchingSystemsActive);
    f_list_free(Entity->matchingSystemsRest);
    f_list_freeEx(Entity->systemNodesActive, (FFree*)f_list_removeNode);
    f_list_freeEx(Entity->systemNodesEither, (FFree*)f_list_removeNode);

    for(int c = F_CONFIG_ECS_COM_NUM; c--; ) {
        f_component__instanceFree(Entity->componentsTable[c]);
    }

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    f_bitfield_free(Entity->componentBits);

    f_mem_free(Entity->id);
    f_mem_free(Entity);
}

void f_entity__freeEx(FEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    f_list_removeNode(Entity->node);

    f_entity__free(Entity);
}

void f_entity_debugSet(FEntity* Entity, bool DebugOn)
{
    if(DebugOn) {
        F_FLAGS_SET(Entity->flags, F_ENTITY__DEBUG);
    } else {
        F_FLAGS_CLEAR(Entity->flags, F_ENTITY__DEBUG);
    }
}

const char* f_entity_idGet(const FEntity* Entity)
{
    return Entity->id ? Entity->id : "FEntity";
}

FEntity* f_entity_parentGet(const FEntity* Entity)
{
    return Entity->parent;
}

void f_entity_parentSet(FEntity* Entity, FEntity* Parent)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_parentSet(%s, %s)",
                        f_entity_idGet(Entity),
                        Parent ? f_entity_idGet(Parent) : "NULL");
        }
    #endif

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    #if F_CONFIG_BUILD_DEBUG
        if(Parent
            && ((Parent->collectionNode
                    && Entity->collectionNode
                    && f_list__nodeGetList(Parent->collectionNode)
                        != f_list__nodeGetList(Entity->collectionNode))
                || (!!Parent->collectionNode ^ !!Entity->collectionNode))) {

            F__FATAL("f_entity_parentSet(%s, %s): Different collections",
                     f_entity_idGet(Entity),
                     f_entity_idGet(Parent));
        }
    #endif

    Entity->parent = Parent;

    if(Parent) {
        f_entity_refInc(Parent);
    }
}

bool f_entity_parentHas(const FEntity* Child, const FEntity* PotentialParent)
{
    for(FEntity* p = Child->parent; p != NULL; p = p->parent) {
        if(p == PotentialParent) {
            return true;
        }
    }

    return false;
}

void f_entity_refInc(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
            F__FATAL("f_entity_refInc(%s): Entity is removed",
                     f_entity_idGet(Entity));
        }

        if(Entity->references == INT_MAX) {
            F__FATAL("f_entity_refInc(%s): Count too high",
                     f_entity_idGet(Entity));
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_refInc(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->references,
                        Entity->references + 1);
        }
    #endif

    Entity->references++;
}

void f_entity_refDec(FEntity* Entity)
{
    if(f_ecs__refDecIgnoreGet()) {
        // The entity could have already been freed despite any outstanding
        // references. This is the only FEntity API that may be called by
        // components' FComponentInstanceFree callbacks.
        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(Entity->references == 0) {
            F__FATAL(
                "f_entity_refDec(%s): Count too low", f_entity_idGet(Entity));
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_refDec(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->references,
                        Entity->references - 1);
        }
    #endif

    Entity->references--;

    if(f_entity__canDelete(Entity)) {
        listMoveTo(Entity, F_ECS__FLUSH);
    }
}

bool f_entity_removedGet(const FEntity* Entity)
{
    return F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED);
}

void f_entity_removedSet(FEntity* Entity)
{
    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_removedSet(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_removedSet(%s)", f_entity_idGet(Entity));
        }
    #endif

    F_FLAGS_SET(Entity->flags, F_ENTITY__REMOVED);
    listMoveTo(Entity, F_ECS__FLUSH);
}

bool f_entity_activeGet(const FEntity* Entity)
{
    return F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_PERMANENT)
        || Entity->lastActive == f_fps_ticksGet();
}

void f_entity_activeSet(FEntity* Entity)
{
    if(Entity->muteCount > 0
        || F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_activeSet(%s)", f_entity_idGet(Entity));
        }
    #endif

    Entity->lastActive = f_fps_ticksGet();

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_REMOVED)) {
        F_FLAGS_CLEAR(Entity->flags, F_ENTITY__ACTIVE_REMOVED);

        // Add entity back to active-only systems
        F_LIST_ITERATE(Entity->matchingSystemsActive, FSystem*, system) {
            f_list_addLast(
                Entity->systemNodesActive, f_system__entityAdd(system, Entity));
        }
    }
}

void f_entity_activeSetRemove(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info(
                "f_entity_activeSetRemove(%s)", f_entity_idGet(Entity));
        }
    #endif

    F_FLAGS_SET(Entity->flags, F_ENTITY__REMOVE_INACTIVE);
}

void f_entity_activeSetPermanent(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info(
                "f_entity_activeSetPermanent(%s)", f_entity_idGet(Entity));
        }
    #endif

    F_FLAGS_SET(Entity->flags, F_ENTITY__ACTIVE_PERMANENT);
}

void* f_entity_componentAdd(FEntity* Entity, int ComponentIndex)
{
    const FComponent* component = f_component__get(ComponentIndex);

    #if F_CONFIG_BUILD_DEBUG
        if(!listIsIn(Entity, F_ECS__NEW)) {
            F__FATAL("f_entity_componentAdd(%s, %s): Too late",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }

        if(Entity->componentsTable[ComponentIndex] != NULL) {
            F__FATAL("f_entity_componentAdd(%s, %s): Already added",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_componentAdd(%s, %s)",
                        f_entity_idGet(Entity),
                        f_component__stringGet(component));
        }
    #endif

    return componentAdd(Entity, ComponentIndex, component, NULL)->buffer;
}

bool f_entity_componentHas(const FEntity* Entity, int ComponentIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    return Entity->componentsTable[ComponentIndex] != NULL;
}

void* f_entity_componentGet(const FEntity* Entity, int ComponentIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    FComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    return instance ? instance->buffer : NULL;
}

void* f_entity_componentReq(const FEntity* Entity, int ComponentIndex)
{
    #if F_CONFIG_BUILD_DEBUG
        f_component__get(ComponentIndex);
    #endif

    FComponentInstance* instance = Entity->componentsTable[ComponentIndex];

    #if F_CONFIG_BUILD_DEBUG
        if(instance == NULL) {
            const FComponent* component = f_component__get(ComponentIndex);

            F__FATAL("f_entity_componentReq(%s, %s): Missing component",
                     f_entity_idGet(Entity),
                     f_component__stringGet(component));
        }
    #endif

    return instance->buffer;
}

bool f_entity_muteGet(const FEntity* Entity)
{
    return Entity->muteCount > 0;
}

void f_entity_muteInc(FEntity* Entity)
{
    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_muteInc(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == INT_MAX) {
            F__FATAL(
                "f_entity_muteInc(%s): Count too high", f_entity_idGet(Entity));
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_muteInc(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount + 1);
        }
    #endif

    if(Entity->muteCount++ == 0) {
        listMoveTo(Entity, F_ECS__FLUSH);
    }
}

void f_entity_muteDec(FEntity* Entity)
{
    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__warning("f_entity_muteDec(%s): Entity is removed",
                           f_entity_idGet(Entity));
        #endif

        return;
    }

    #if F_CONFIG_BUILD_DEBUG
        if(Entity->muteCount == 0) {
            F__FATAL(
                "f_entity_muteDec(%s): Count too low", f_entity_idGet(Entity));
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_muteDec(%s) %d->%d",
                        f_entity_idGet(Entity),
                        Entity->muteCount,
                        Entity->muteCount - 1);
        }
    #endif

    if(--Entity->muteCount == 0) {
        if(!f_list_isEmpty(Entity->matchingSystemsActive)
            || !f_list_isEmpty(Entity->matchingSystemsRest)) {

            if(listIsIn(Entity, F_ECS__FLUSH)) {
                // Entity was muted and unmuted before it left systems
                listMoveTo(Entity, F_ECS__DEFAULT);
            } else {
                // To be added back to matched systems
                listMoveTo(Entity, F_ECS__RESTORE);
            }
        } else {
            // Entity has not been matched to systems yet, treat it as new
            listMoveTo(Entity, F_ECS__NEW);
        }
    }
}

const FTemplate* f_entity__templateGet(const FEntity* Entity)
{
    return Entity->template;
}

bool f_entity__canDelete(const FEntity* Entity)
{
    return Entity->references == 0
            && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED);
}

void f_entity__ecsListAdd(FEntity* Entity, FList* List)
{
    Entity->node = f_list_addLast(List, Entity);
}

void f_entity__systemsMatch(FEntity* Entity, FSystem* System)
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

void f_entity__systemsAddTo(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(f_list_isEmpty(Entity->matchingSystemsActive)
            && f_list_isEmpty(Entity->matchingSystemsRest)) {

            f_out__warning("Entity %s was not matched to any systems",
                           f_entity_idGet(Entity));
        }
    #endif

    if(!F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_REMOVED)) {
        F_LIST_ITERATE(Entity->matchingSystemsActive, FSystem*, system) {
            f_list_addLast(
                Entity->systemNodesActive, f_system__entityAdd(system, Entity));
        }
    }

    F_LIST_ITERATE(Entity->matchingSystemsRest, FSystem*, system) {
        f_list_addLast(
            Entity->systemNodesEither, f_system__entityAdd(system, Entity));
    }

    listAddTo(Entity, F_ECS__DEFAULT);
}

void f_entity__systemsRemoveFromAll(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info(
                "%s removed from all systems", f_entity_idGet(Entity));
        }
    #endif

    f_list_clearEx(Entity->systemNodesActive, (FFree*)f_list_removeNode);
    f_list_clearEx(Entity->systemNodesEither, (FFree*)f_list_removeNode);
}

void f_entity__systemsRemoveFromActive(FEntity* Entity)
{
    #if F_CONFIG_BUILD_DEBUG
        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info(
                "%s removed from active-only systems", f_entity_idGet(Entity));
        }
    #endif

    F_FLAGS_SET(Entity->flags, F_ENTITY__ACTIVE_REMOVED);
    f_list_clearEx(Entity->systemNodesActive, (FFree*)f_list_removeNode);

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVE_INACTIVE)) {
        f_entity_removedSet(Entity);
    }
}
#endif // F_CONFIG_FEATURE_ECS
