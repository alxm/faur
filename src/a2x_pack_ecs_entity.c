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

#include "a2x_pack_ecs_entity.v.h"

#include "a2x_pack_ecs.v.h"
#include "a2x_pack_ecs_system.v.h"
#include "a2x_pack_fps.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

AEntity* a_entity_new(const char* Id, void* Context)
{
    AEntity* e = a_mem_zalloc(
        sizeof(AEntity) + a_component__tableLen * sizeof(AComponentHeader*));

    e->id = a_str_dup(Id);
    e->context = Context;
    e->matchingSystemsActive = a_list_new();
    e->matchingSystemsEither = a_list_new();
    e->systemNodesActive = a_list_new();
    e->systemNodesEither = a_list_new();
    e->componentBits = a_bitfield_new(a_component__tableLen);
    e->handlers = a_strhash_new();
    e->lastActive = a_fps_ticksGet() - 1;

    a_ecs__entityAddToList(e, A_ECS__NEW);

    return e;
}

void a_entity__free(AEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    if(Entity->debug) {
        a_out__message("a_entity__free('%s')", a_entity_idGet(Entity));
    }

    a_list_free(Entity->matchingSystemsActive);
    a_list_free(Entity->matchingSystemsEither);
    a_list_freeEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
    a_list_freeEx(Entity->systemNodesEither, (AFree*)a_list_removeNode);

    for(unsigned c = 0; c < a_component__tableLen; c++) {
        AComponentHeader* header = Entity->componentsTable[c];

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

    a_strhash_freeEx(Entity->handlers, free);
    a_bitfield_free(Entity->componentBits);

    free(Entity->id);
    free(Entity);
}

void a_entity_debugSet(AEntity* Entity, bool DebugOn)
{
    Entity->debug = DebugOn;
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
    if(Entity->debug) {
        a_out__message("a_entity_parentSet('%s', '%s')",
                       a_entity_idGet(Entity),
                       a_entity_idGet(Parent));
    }

    if(Entity->parent) {
        a_entity_refDec(Entity->parent);
    }

    Entity->parent = Parent;

    if(Parent) {
        a_entity_refInc(Parent);
    }
}

void a_entity_refInc(AEntity* Entity)
{
    if(Entity->debug) {
        a_out__message("a_entity_refInc('%s')", a_entity_idGet(Entity));
    }

    if(a_entity_removeGet(Entity)) {
        a_out__fatal(
            "a_entity_refInc: '%s' is removed", a_entity_idGet(Entity));
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

    if(Entity->debug) {
        a_out__message("a_entity_refDec('%s')", a_entity_idGet(Entity));
    }

    Entity->references--;

    if(Entity->references < 0) {
        a_out__fatal("a_entity_refDec: Mismatched ref count for '%s'",
                     a_entity_idGet(Entity));
    } else if(Entity->references == 0
        && a_ecs__entityIsInList(Entity, A_ECS__REMOVED_LIMBO)) {

        a_ecs__entityMoveToList(Entity, A_ECS__REMOVED_QUEUE);
    }
}

bool a_entity_removeGet(const AEntity* Entity)
{
    return a_ecs__entityIsInList(Entity, A_ECS__REMOVED_QUEUE)
        || a_ecs__entityIsInList(Entity, A_ECS__REMOVED_LIMBO)
        || a_ecs__entityIsInList(Entity, A_ECS__REMOVED_FREE);
}

void a_entity_removeSet(AEntity* Entity)
{
    if(Entity->debug) {
        a_out__message("a_entity_removeSet('%s')", a_entity_idGet(Entity));
    }

    if(a_entity_removeGet(Entity)) {
        a_out__fatal("a_entity_removeSet: '%s' is already removed",
                     a_entity_idGet(Entity));
        return;
    }

    a_ecs__entityMoveToList(Entity, A_ECS__REMOVED_QUEUE);
}

bool a_entity_activeGet(const AEntity* Entity)
{
    return Entity->permanentActive || Entity->lastActive == a_fps_ticksGet();
}

void a_entity_activeSet(AEntity* Entity)
{
    if(Entity->debug) {
        a_out__message("a_entity_activeSet('%s')", a_entity_idGet(Entity));
    }

    if(a_entity_removeGet(Entity)) {
        // Ignore if entity is removed
        return;
    }

    Entity->lastActive = a_fps_ticksGet();

    if(Entity->removedFromActive) {
        Entity->removedFromActive = false;

        // Add entity back to active-only systems
        A_LIST_ITERATE(Entity->matchingSystemsActive, ASystem*, system) {
            a_list_addLast(Entity->systemNodesActive,
                           a_list_addLast(system->entities, Entity));
        }
    }
}

void a_entity_activeSetPermanent(AEntity* Entity)
{
    if(Entity->debug) {
        a_out__message(
            "a_entity_activeSetPermanent('%s')", a_entity_idGet(Entity));
    }

    Entity->permanentActive = true;
}

void* a_entity_componentAdd(AEntity* Entity, int Component)
{
    const AComponent* c = a_component__tableGet(Component, __func__);

    if(Entity->debug) {
        a_out__message("a_entity_componentAdd('%s', '%s')",
                       a_entity_idGet(Entity),
                       c->name);
    }

    if(!a_ecs__entityIsInList(Entity, A_ECS__NEW)) {
        a_out__fatal("a_entity_componentAdd: Too late to add '%s' to '%s'",
                     c->name,
                     a_entity_idGet(Entity));
    }

    if(Entity->componentsTable[Component] != NULL) {
        a_out__fatal("a_entity_componentAdd: '%s' was already added to '%s'",
                     c->name,
                     a_entity_idGet(Entity));
    }

    AComponentHeader* header = a_mem_zalloc(c->size);

    header->component = c;
    header->entity = Entity;

    Entity->componentsTable[Component] = header;
    a_bitfield_set(Entity->componentBits, c->bit);

    if(c->init) {
        c->init(a_component__headerGetData(header));
    }

    return a_component__headerGetData(header);
}

bool a_entity_componentHas(const AEntity* Entity, int Component)
{
    a_component__tableGet(Component, __func__);

    return Entity->componentsTable[Component] != NULL;
}

void* a_entity_componentGet(const AEntity* Entity, int Component)
{
    a_component__tableGet(Component, __func__);
    AComponentHeader* header = Entity->componentsTable[Component];

    return header ? a_component__headerGetData(header) : NULL;
}

void* a_entity_componentReq(const AEntity* Entity, int Component)
{
    const AComponent* c = a_component__tableGet(Component, __func__);
    AComponentHeader* header = Entity->componentsTable[Component];

    if(header == NULL) {
        a_out__fatal(
            "a_entity_componentReq: Missing required component '%s' in '%s'",
            c->name,
            a_entity_idGet(Entity));
    }

    return a_component__headerGetData(header);
}

bool a_entity_muteGet(const AEntity* Entity)
{
    return a_ecs__entityIsInList(Entity, A_ECS__MUTED_QUEUE)
        || a_ecs__entityIsInList(Entity, A_ECS__MUTED_LIMBO);
}

void a_entity_muteSet(AEntity* Entity, bool DoMute)
{
    if(Entity->debug) {
        a_out__message(
            "a_entity_muteSet('%s', %d)", a_entity_idGet(Entity), DoMute);
    }

    if(a_entity_removeGet(Entity)) {
        a_out__warningv("a_entity_muteSet: Entity '%s' is removed, cannot mute",
                        a_entity_idGet(Entity));
        return;
    } else if(a_entity_muteGet(Entity) == DoMute) {
        if(DoMute) {
            a_out__warningv("a_entity_muteSet: '%s' is already muted",
                            a_entity_idGet(Entity));
        } else {
            a_out__warningv("a_entity_muteSet: '%s' is not muted",
                            a_entity_idGet(Entity));
        }

        return;
    }

    if(DoMute) {
        a_ecs__entityMoveToList(Entity, A_ECS__MUTED_QUEUE);
    } else {
        if(a_entity__isMatchedToSystems(Entity)) {
            if(a_ecs__entityIsInList(Entity, A_ECS__MUTED_QUEUE)) {
                // Entity was muted and unmuted before it left systems
                a_ecs__entityMoveToList(Entity, A_ECS__RUNNING);
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
    Entity->removedFromActive = true;
    a_list_clearEx(Entity->systemNodesActive, (AFree*)a_list_removeNode);
}

bool a_entity__isMatchedToSystems(const AEntity* Entity)
{
    return !a_list_isEmpty(Entity->matchingSystemsActive)
        || !a_list_isEmpty(Entity->matchingSystemsEither);
}

void a_entity_messageHandlerSet(AEntity* Entity, const char* Message, AMessageHandler* Handler)
{
    if(a_strhash_contains(Entity->handlers, Message)) {
        a_out__fatal("a_entity_messageHandlerSet: '%s' already set for '%s'",
                     Message,
                     a_entity_idGet(Entity));
    }

    AMessageHandlerContainer* h = a_mem_malloc(sizeof(AMessageHandlerContainer));

    h->handler = Handler;

    a_strhash_add(Entity->handlers, Message, h);
}

void a_entity_messageSend(AEntity* To, AEntity* From, const char* Message)
{
    if(To->debug || From->debug) {
        a_out__message("a_entity_messageSend('%s', '%s', '%s')",
                       a_entity_idGet(To),
                       a_entity_idGet(From),
                       Message);
    }

    AMessageHandlerContainer* h = a_strhash_get(To->handlers, Message);

    if(h == NULL) {
        // Entity does not handle this Message
        return;
    }

    if(a_entity_removeGet(To) || a_entity_removeGet(From)
        || a_entity_muteGet(To)) {

        // Ignore message if one of the entities was already removed,
        // or if the destination entity is muted
        return;
    }

    h->handler(To, From);
}
