/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

struct FEntity {
    char* id; // specified name for debugging
    const FTemplate* template; // template used to init this entity's components
    FEntity* parent; // manually associated parent entity
    FListNode* node; // list node in one of FEntityList
    FListNode* collectionNode; // FCollection list nod
    FList* matchingSystemsActive; // FList<FSystem*>
    FList* matchingSystemsRest; // FList<FSystem*>
    FList* systemNodesActive; // FList<FListNode*> in active-only FSystem lists
    FList* systemNodesEither; // FList<FListNode*> in rest FSystem lists
    FBitfield* componentBits; // each component's bit is set
    unsigned lastActive; // frame when f_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    FEntityFlags flags; // various properties
    FComponentInstance* componentsTable[]; // [f_component__num] Buffer/NULL
};

typedef enum {
    F_LIST__INVALID = -1,
    F_LIST__DEFAULT, // no pending changes
    F_LIST__NEW, // new entities that aren't in any systems yet
    F_LIST__RESTORE, // entities matched to systems, to be added to them
    F_LIST__FLUSH, // muted or removed entities, to be flushed from systems
    F_LIST__FREE, // entities to be freed at the end of current frame
    F_LIST__NUM
} FEntityList;

static FList* g_lists[F_LIST__NUM]; // Each entity is in exactly one of these
bool f_entity__ignoreRefDec; // Set to prevent using freed entities

static FComponentInstance* componentAdd(FEntity* Entity, const FComponent* Component, const void* Data)
{
    FComponentInstance* c = f_component__instanceNew(Component, Entity, Data);

    Entity->componentsTable[Component->bitId] = c;
    f_bitfield_set(Entity->componentBits, Component->bitId);

    return c;
}

static inline bool canDelete(const FEntity* Entity)
{
    return Entity->references == 0
            && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED);
}

static inline bool listIsIn(const FEntity* Entity, FEntityList List)
{
    return f_list__nodeGetList(Entity->node) == g_lists[List];
}

static inline void listAddTo(FEntity* Entity, FEntityList List)
{
    Entity->node = f_list_addLast(g_lists[List], Entity);
}

static inline void listMoveTo(FEntity* Entity, FEntityList List)
{
    f_list_removeNode(Entity->node);

    listAddTo(Entity, List);
}

void f_entity__init(void)
{
    for(int i = F_LIST__NUM; i--; ) {
        g_lists[i] = f_list_new();
    }
}

void f_entity__uninit(void)
{
    f_entity__ignoreRefDec = true;

    for(int i = F_LIST__NUM; i--; ) {
        f_list_freeEx(g_lists[i], (FFree*)f_entity__free);
    }
}

void f_entity__tick(void)
{
    if(g_lists[0] == NULL) {
        return;
    }

    f_entity__flushFromSystems();

    // Check what systems the new entities match
    F_LIST_ITERATE(g_lists[F_LIST__NEW], FEntity*, e) {
        for(unsigned s = f_system__num; s--; ) {
            FSystem* system = f_system__array[s];

            if(f_bitfield_testMask(e->componentBits, system->componentBits)) {
                if(system->onlyActiveEntities) {
                    f_list_addLast(e->matchingSystemsActive, system);
                } else {
                    f_list_addLast(e->matchingSystemsRest, system);
                }
            }
        }

        listAddTo(e, F_LIST__RESTORE);
    }

    // Add entities to the systems they match
    F_LIST_ITERATE(g_lists[F_LIST__RESTORE], FEntity*, e) {
        #if F_CONFIG_BUILD_DEBUG
            if(f_list_isEmpty(e->matchingSystemsActive)
                && f_list_isEmpty(e->matchingSystemsRest)) {

                f_out__warning("Entity %s was not matched to any systems",
                               f_entity_idGet(e));
            }
        #endif

        if(!F_FLAGS_TEST_ANY(e->flags, F_ENTITY__ACTIVE_REMOVED)) {
            F_LIST_ITERATE(e->matchingSystemsActive, FSystem*, system) {
                f_list_addLast(
                    e->systemNodesActive, f_system__entityAdd(system, e));
            }
        }

        F_LIST_ITERATE(e->matchingSystemsRest, FSystem*, system) {
            f_list_addLast(
                e->systemNodesEither, f_system__entityAdd(system, e));
        }

        listAddTo(e, F_LIST__DEFAULT);
    }

    f_list_clear(g_lists[F_LIST__NEW]);
    f_list_clear(g_lists[F_LIST__RESTORE]);
    f_list_clearEx(g_lists[F_LIST__FREE], (FFree*)f_entity__free);
}

void f_entity__flushFromSystems(void)
{
    F_LIST_ITERATE(g_lists[F_LIST__FLUSH], FEntity*, e) {
        #if F_CONFIG_BUILD_DEBUG
            if(F_FLAGS_TEST_ANY(e->flags, F_ENTITY__DEBUG)) {
                f_out__info("%s removed from all systems", f_entity_idGet(e));
            }
        #endif

        f_list_clearEx(e->systemNodesActive, (FFree*)f_list_removeNode);
        f_list_clearEx(e->systemNodesEither, (FFree*)f_list_removeNode);

        listAddTo(e, canDelete(e) ? F_LIST__FREE : F_LIST__DEFAULT);
    }

    f_list_clear(g_lists[F_LIST__FLUSH]);
}

void f_entity__flushFromSystemsActive(FEntity* Entity)
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

unsigned f_entity__numGet(void)
{
    if(g_lists[0] == NULL) {
        return 0;
    }

    unsigned sum = 0;

    for(int i = F_LIST__NUM; i--; ) {
        sum += f_list_sizeGet(g_lists[i]);
    }

    return sum;
}

FEntity* f_entity_new(const char* Template, const void* Context)
{
    FEntity* e = f_mem_zalloc(
                    sizeof(FEntity)
                        + sizeof(FComponentInstance*) * f_component__num);

    listAddTo(e, F_LIST__NEW);

    e->matchingSystemsActive = f_list_new();
    e->matchingSystemsRest = f_list_new();
    e->systemNodesActive = f_list_new();
    e->systemNodesEither = f_list_new();
    e->componentBits = f_bitfield_new(f_component__num);
    e->lastActive = f_fps_ticksGet() - 1;

    FCollection* collection = f_collection__get();

    if(collection) {
        e->collectionNode = f_list_addLast(collection, e);
    }

    if(Template) {
        const FTemplate* t = f_template__get(Template);
        const char* id = f_str__fmt512("%s#%u",
                                       Template,
                                       f_template__instanceGet(t));

        e->id = f_str_dup(id);
        e->template = t;

        F_LIST_ITERATE(f_template__componentsGet(t), const FComponent*, c) {
            componentAdd(e, c, f_template__dataGet(t, c));
        }

        f_template__initRun(t, e, Context);
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

    for(unsigned c = f_component__num; c--; ) {
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

        if(Parent
            && ((!!Parent->collectionNode != !!Entity->collectionNode)
                || (Parent->collectionNode
                    && f_list__nodeGetList(Parent->collectionNode)
                        != f_list__nodeGetList(Entity->collectionNode)))) {

            F__FATAL("f_entity_parentSet(%s, %s): Different collections",
                     f_entity_idGet(Entity),
                     f_entity_idGet(Parent));
        }
    #endif

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

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
    if(f_entity__ignoreRefDec) {
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

    if(canDelete(Entity)) {
        listMoveTo(Entity, F_LIST__FLUSH);
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
    listMoveTo(Entity, F_LIST__FLUSH);
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

void* f_entity_componentAdd(FEntity* Entity, const FComponent* Component)
{
    #if F_CONFIG_BUILD_DEBUG
        if(!listIsIn(Entity, F_LIST__NEW)) {
            F__FATAL("f_entity_componentAdd(%s, %s): Too late",
                     f_entity_idGet(Entity),
                     Component->stringId);
        }

        if(Entity->componentsTable[Component->bitId] != NULL) {
            F__FATAL("f_entity_componentAdd(%s, %s): Already added",
                     f_entity_idGet(Entity),
                     Component->stringId);
        }

        if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
            f_out__info("f_entity_componentAdd(%s, %s)",
                        f_entity_idGet(Entity),
                        Component->stringId);
        }
    #endif

    return componentAdd(Entity, Component, NULL)->buffer;
}

bool f_entity_componentHas(const FEntity* Entity, const FComponent* Component)
{
    return Entity->componentsTable[Component->bitId] != NULL;
}

void* f_entity_componentGet(const FEntity* Entity, const FComponent* Component)
{
    FComponentInstance* instance = Entity->componentsTable[Component->bitId];

    return instance ? instance->buffer : NULL;
}

void* f_entity_componentReq(const FEntity* Entity, const FComponent* Component)
{
    FComponentInstance* instance = Entity->componentsTable[Component->bitId];

    #if F_CONFIG_BUILD_DEBUG
        if(instance == NULL) {
            F__FATAL("f_entity_componentReq(%s, %s): Missing component",
                     f_entity_idGet(Entity),
                     Component->stringId);
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
        listMoveTo(Entity, F_LIST__FLUSH);
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

            if(listIsIn(Entity, F_LIST__FLUSH)) {
                // Entity was muted and unmuted before it left systems
                listMoveTo(Entity, F_LIST__DEFAULT);
            } else {
                // To be added back to matched systems
                listMoveTo(Entity, F_LIST__RESTORE);
            }
        } else {
            // Entity has not been matched to systems yet, treat it as new
            listMoveTo(Entity, F_LIST__NEW);
        }
    }
}

const FTemplate* f_entity__templateGet(const FEntity* Entity)
{
    return Entity->template;
}
