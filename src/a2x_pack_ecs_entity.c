/*
    Copyright 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#include "a2x_pack_ecs_entity.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_collection.v.h"
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

static unsigned g_numMessages;

void a_entity__init(unsigned NumMessages)
{
    g_numMessages = NumMessages;
}

static void* componentAdd(AEntity* Entity, int Index, const AComponent* Component)
{
    AComponentInstance* header = a_mem_zalloc(Component->size);

    header->component = Component;
    header->entity = Entity;

    Entity->componentsTable[Index] = header;
    a_bitfield_set(Entity->componentBits, Component->bit);

    void* self = a_component__headerGetData(header);

    if(Component->init) {
        Component->init(self);
    }

    return a_component__headerGetData(header);
}

AEntity* a_entity_new(const char* Id, void* Context)
{
    AEntity* e = a_mem_zalloc(
        sizeof(AEntity) + a_component__tableLen * sizeof(AComponentInstance*));

    e->id = a_str_dup(Id);
    e->context = Context;
    e->matchingSystemsActive = a_list_new();
    e->matchingSystemsRest = a_list_new();
    e->systemNodesActive = a_list_new();
    e->systemNodesEither = a_list_new();
    e->componentBits = a_bitfield_new(a_component__tableLen);
    e->lastActive = a_fps_ticksGet() - 1;

    a_ecs__entityAddToList(e, A_ECS__NEW);

    ACollection* collection = a_ecs_collectionGet();

    if(collection) {
        a_collection__add(collection, e);
    }

    return e;
}

AEntity* a_entity_newEx(const char* Template, const void* ComponentInitContext, void* Context)
{
    const ATemplate* t = a_template__get(Template, __func__);
    const char* id = a_str__fmt512(
                        "%s#%u", Template, a_template__instanceGet(t));
    AEntity* e = a_entity_new(id, Context);

    e->template = t;

    for(int c = (int)a_component__tableLen; c--; ) {
        if(a_template__componentHas(t, c)) {
            const AComponent* component = a_component__get(c, __func__);
            void* self = componentAdd(e, c, component);

            if(component->initWithData) {
                component->initWithData(
                    self, a_template__dataGet(t, c), ComponentInitContext);
            }
        }
    }

    return e;
}

void a_entity__free(AEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity__free(%s)", a_entity_idGet(Entity));
    }

    if(Entity->collectionNode) {
        a_list_removeNode(Entity->collectionNode);
    }

    a_list_free(Entity->matchingSystemsActive);
    a_list_free(Entity->matchingSystemsRest);
    a_list_freeEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
    a_list_freeEx(Entity->systemNodesEither, (AFree*)a_list_removeNode);

    for(unsigned c = 0; c < a_component__tableLen; c++) {
        AComponentInstance* header = Entity->componentsTable[c];

        if(header == NULL) {
            continue;
        }

        if(header->component->free) {
            header->component->free(a_component__headerGetData(header));
        }

        free(header);
    }

    if(Entity->parent) {
        a_entity_refDec(Entity->parent);
    }

    a_bitfield_free(Entity->componentBits);

    free(Entity->messageHandlers);
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

void* a_entity_contextGet(const AEntity* Entity)
{
    return Entity->context;
}

AEntity* a_entity_parentGet(const AEntity* Entity)
{
    return Entity->parent;
}

void a_entity_parentSet(AEntity* Entity, AEntity* Parent)
{
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_parentSet(%s, %s)",
                       a_entity_idGet(Entity),
                       Parent ? a_entity_idGet(Parent) : "NULL");
    }

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
    if(a_entity_removeGet(Entity)) {
        A__FATAL(
            "a_entity_refInc(%s): Entity is removed", a_entity_idGet(Entity));
    }

    if(Entity->references == INT_MAX) {
        A__FATAL("a_entity_refInc(%s): Count too high", a_entity_idGet(Entity));
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_refInc(%s) %d->%d",
                       a_entity_idGet(Entity),
                       Entity->references,
                       Entity->references + 1);
    }

    Entity->references++;
}

void a_entity_refDec(AEntity* Entity)
{
    if(a_ecs__isDeleting()) {
        // Entity could have already been freed. This is the only ECS function
        // that may be called from AFree callbacks.
        return;
    }

    if(Entity->references == 0) {
        A__FATAL("a_entity_refDec(%s): Count too low", a_entity_idGet(Entity));
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_refDec(%s) %d->%d",
                       a_entity_idGet(Entity),
                       Entity->references,
                       Entity->references - 1);
    }

    Entity->references--;

    if(Entity->references == 0
        && a_ecs__entityIsInList(Entity, A_ECS__REMOVED_LIMBO)) {

        a_ecs__entityMoveToList(Entity, A_ECS__REMOVED_QUEUE);
    }
}

bool a_entity_removeGet(const AEntity* Entity)
{
    return A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__REMOVED);
}

void a_entity_removeSet(AEntity* Entity)
{
    if(a_entity_removeGet(Entity)) {
        A__FATAL(
            "a_entity_removeSet(%s): Already removed", a_entity_idGet(Entity));

        return;
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_removeSet(%s)", a_entity_idGet(Entity));
    }

    A_FLAG_SET(Entity->flags, A_ENTITY__REMOVED);
    a_ecs__entityMoveToList(Entity, A_ECS__REMOVED_QUEUE);

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
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_activeSet(%s)", a_entity_idGet(Entity));
    }

    if(a_entity_muteGet(Entity) || a_entity_removeGet(Entity)) {
        return;
    }

    Entity->lastActive = a_fps_ticksGet();

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__ACTIVE_REMOVED)) {
        A_FLAG_CLEAR(Entity->flags, A_ENTITY__ACTIVE_REMOVED);

        // Add entity back to active-only systems
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            a_list_addLast(Entity->systemNodesActive,
                           a_list_addLast(system->entities, Entity));
        }
    }
}

void a_entity_activeSetPermanent(AEntity* Entity)
{
    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message(
            "a_entity_activeSetPermanent(%s)", a_entity_idGet(Entity));
    }

    A_FLAG_SET(Entity->flags, A_ENTITY__ACTIVE_PERMANENT);
}

void* a_entity_componentAdd(AEntity* Entity, int Component)
{
    const AComponent* c = a_component__get(Component, __func__);

    if(!a_ecs__entityIsInList(Entity, A_ECS__NEW)) {
        A__FATAL("a_entity_componentAdd(%s, %s): Too late",
                 a_entity_idGet(Entity),
                 c->stringId);
    }

    if(Entity->componentsTable[Component] != NULL) {
        A__FATAL("a_entity_componentAdd(%s, %s): Already added",
                 a_entity_idGet(Entity),
                 c->stringId);
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_componentAdd(%s, %s)",
                       a_entity_idGet(Entity),
                       c->stringId);
    }

    return componentAdd(Entity, Component, c);
}

bool a_entity_componentHas(const AEntity* Entity, int Component)
{
    a_component__get(Component, __func__);

    return Entity->componentsTable[Component] != NULL;
}

void* a_entity_componentGet(const AEntity* Entity, int Component)
{
    a_component__get(Component, __func__);
    AComponentInstance* header = Entity->componentsTable[Component];

    return header ? a_component__headerGetData(header) : NULL;
}

void* a_entity_componentReq(const AEntity* Entity, int Component)
{
    const AComponent* c = a_component__get(Component, __func__);
    AComponentInstance* header = Entity->componentsTable[Component];

    if(header == NULL) {
        A__FATAL("a_entity_componentReq(%s, %s): Missing component",
                 a_entity_idGet(Entity),
                 c->stringId);
    }

    return a_component__headerGetData(header);
}

bool a_entity_muteGet(const AEntity* Entity)
{
    return Entity->muteCount > 0;
}

void a_entity_muteInc(AEntity* Entity)
{
    if(a_entity_removeGet(Entity)) {
        a_out__warningv(
            "a_entity_muteInc(%s): Entity is removed", a_entity_idGet(Entity));

        return;
    }

    if(Entity->muteCount == INT_MAX) {
        A__FATAL(
            "a_entity_muteInc(%s): Count too high", a_entity_idGet(Entity));
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_muteInc(%s) %d->%d",
                       a_entity_idGet(Entity),
                       Entity->muteCount,
                       Entity->muteCount + 1);
    }

    if(Entity->muteCount++ == 0) {
        a_ecs__entityMoveToList(Entity, A_ECS__MUTED_QUEUE);
    }
}

void a_entity_muteDec(AEntity* Entity)
{
    if(a_entity_removeGet(Entity)) {
        a_out__warningv(
            "a_entity_muteDec(%s): Entity is removed", a_entity_idGet(Entity));

        return;
    }

    if(Entity->muteCount == 0) {
        A__FATAL("a_entity_muteDec(%s): Count too low", a_entity_idGet(Entity));
    }

    if(A_FLAG_TEST_ANY(Entity->flags, A_ENTITY__DEBUG)) {
        a_out__message("a_entity_muteDec(%s) %d->%d",
                       a_entity_idGet(Entity),
                       Entity->muteCount,
                       Entity->muteCount - 1);
    }

    if(--Entity->muteCount == 0) {
        if(a_entity__isMatchedToSystems(Entity)) {
            if(a_ecs__entityIsInList(Entity, A_ECS__MUTED_QUEUE)) {
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

void a_entity__removeFromAllSystems(AEntity* Entity)
{
    a_list_clearEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
    a_list_clearEx(Entity->systemNodesEither, (AFree*)a_list_removeNode);
}

void a_entity__removeFromActiveSystems(AEntity* Entity)
{
    A_FLAG_SET(Entity->flags, A_ENTITY__ACTIVE_REMOVED);
    a_list_clearEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
}

bool a_entity__isMatchedToSystems(const AEntity* Entity)
{
    return !a_list_isEmpty(Entity->matchingSystemsActive)
        || !a_list_isEmpty(Entity->matchingSystemsRest);
}

void a_entity_messageSet(AEntity* Entity, int Message, AMessageHandler* Handler)
{
    #if A_BUILD_DEBUG
        if(Message < 0 || Message >= (int)g_numMessages) {
            A__FATAL("a_entity_messageSet(%s, %d): Unknown message",
                     a_entity_idGet(Entity),
                     Message);
        }
    #endif

    if(Entity->messageHandlers == NULL) {
        Entity->messageHandlers = a_mem_zalloc(
                                    g_numMessages * sizeof(AMessageHandler*));
    } else if(Entity->messageHandlers[Message] != NULL) {
        A__FATAL("a_entity_messageSet(%s, %d): Already set",
                 a_entity_idGet(Entity),
                 Message);
    }

    Entity->messageHandlers[Message] = Handler;
}

void a_entity_messageSend(AEntity* To, AEntity* From, int Message)
{
    #if A_BUILD_DEBUG
        if(Message < 0 || Message >= (int)g_numMessages) {
            A__FATAL("a_entity_messageSend(%s, %s, %d): Unknown message",
                     a_entity_idGet(To),
                     a_entity_idGet(From),
                     Message);
        }
    #endif

    if(A_FLAG_TEST_ANY(To->flags, A_ENTITY__DEBUG)
        || A_FLAG_TEST_ANY(From->flags, A_ENTITY__DEBUG)) {

        a_out__message("a_entity_messageSend(%s, %s, %d)",
                       a_entity_idGet(To),
                       a_entity_idGet(From),
                       Message);
    }

    if(To->messageHandlers == NULL || To->messageHandlers[Message] == NULL) {
        // Entity does not handle this Message
        return;
    }

    if(a_entity_removeGet(To) || a_entity_removeGet(From)
        || a_entity_muteGet(To)) {

        // Ignore message if one of the entities was already removed,
        // or if the destination entity is muted
        return;
    }

    To->messageHandlers[Message](To, From);
}
