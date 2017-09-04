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

#include "a2x_pack_entity.v.h"

typedef struct AComponent {
    size_t size; // total size of AComponent + user data that follows
    AFree* free; // does not free the actual pointer
    AEntity* parent; // only valid for AComponent instances, not prototype
    unsigned bit; // component's unique bit ID
} AComponent;

typedef struct ASystem {
    ASystemHandler* handler;
    ASystemSort* compare;
    ABitfield* componentBits; // IDs of components that this system works on
    AList* entities; // entities currently picked up by this system
    bool onlyActiveEntities; // skip entities that are not active
    bool muted; // runSystem skips muted systems
    bool runsInCurrentState; // whether this system runs in the current state
} ASystem;

typedef struct ARunningCollection {
    AList* newEntities; // new entities are added to this list
    AList* runningEntities; // entities in this list are picked up by systems
    AList* removedEntities; // removed entities with outstanding references
    AList* tickSystems; // tick systems in the specified order
    AList* drawSystems; // draw systems in the specified order
    AList* allSystems; // tick & draw systems
    AList* messageQueue; // queued messages
    bool deleting; // set when this collection is popped off the stack
} ARunningCollection;

struct AEntity {
    char* id; // specified name for debugging
    void* context; // global context
    AEntity* parent; // manually associated parent entity
    AListNode* collectionNode; // list node in one of new, running, or removed
    AList* systemNodes; // list of nodes in ASystem.entities lists
    AStrHash* components;
    ABitfield* componentBits;
    AStrHash* handlers; // table of AMessageHandlerContainer
    unsigned lastActive; // frame when a_entity_markActive was last called
    unsigned references; // if >0, then the entity lingers in the removed list
    bool muted; // systems don't run on this entity if this is set
};

typedef struct AMessageHandlerContainer {
    AMessageHandler* handler;
    bool handleImmediately;
} AMessageHandlerContainer;

typedef struct AMessage {
    AEntity* to;
    AEntity* from;
    char* message;
} AMessage;

static ARunningCollection* g_collection;
static AList* g_stack; // list of ARunningCollection (one for each state)
static AStrHash* g_components; // table of declared AComponent
static AStrHash* g_systems; // table of declared ASystem

static inline bool entityIsNew(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->collectionNode)
        == g_collection->newEntities;
}

static inline bool entityIsRemoved(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->collectionNode)
        == g_collection->removedEntities;
}

static inline void* getComponent(const AComponent* Header)
{
    return (void*)(Header + 1);
}

static inline AComponent* getHeader(const void* Component)
{
    return (AComponent*)Component - 1;
}

static AMessage* message_new(AEntity* To, AEntity* From, const char* Message)
{
    AMessage* m = a_mem_malloc(sizeof(AMessage));

    m->to = To;
    m->from = From;
    m->message = a_str_dup(Message);

    a_entity_reference(To);
    a_entity_reference(From);

    return m;
}

static void message_free(AMessage* Message)
{
    a_entity_release(Message->to);
    a_entity_release(Message->from);

    free(Message->message);
    free(Message);
}

void a_entity__init(void)
{
    g_collection = NULL;
    g_stack = a_list_new();
    g_components = a_strhash_new();
    g_systems = a_strhash_new();
}

void a_entity__uninit(void)
{
    while(g_collection != NULL) {
        a_system__popCollection();
    }

    a_list_free(g_stack);

    A_STRHASH_ITERATE(g_systems, ASystem*, system) {
        a_list_free(system->entities);
        a_bitfield_free(system->componentBits);
        free(system);
    }

    a_strhash_free(g_systems);
    a_strhash_freeEx(g_components, free);
}

void a_component_declare(const char* Name, size_t Size, AFree* Free)
{
    if(a_strhash_contains(g_components, Name)) {
        a_out__fatal("Component '%s' already declared", Name);
    }

    AComponent* h = a_mem_malloc(sizeof(AComponent));

    h->size = sizeof(AComponent) + Size;
    h->free = Free;
    h->parent = NULL;
    h->bit = a_strhash_getSize(g_components);

    a_strhash_add(g_components, Name, h);
}

AEntity* a_component_getEntity(const void* Component)
{
    return getHeader(Component)->parent;
}

AEntity* a_entity_new(const char* Id, void* Context)
{
    AEntity* e = a_mem_malloc(sizeof(AEntity));

    e->id = Id ? a_str_dup(Id) : NULL;
    e->context = Context;
    e->parent = NULL;
    e->collectionNode = a_list_addLast(g_collection->newEntities, e);
    e->systemNodes = a_list_new();
    e->components = a_strhash_new();
    e->componentBits = a_bitfield_new(a_strhash_getSize(g_components));
    e->handlers = a_strhash_new();
    e->lastActive = a_fps_getCounter() - 1;
    e->references = 0;
    e->muted = false;

    return e;
}

static void a_entity__free(AEntity* Entity)
{
    a_list_freeEx(Entity->systemNodes, (AFree*)a_list_removeNode);

    A_STRHASH_ITERATE(Entity->components, AComponent*, header) {
        if(header->free) {
            header->free(getComponent(header));
        }

        free(header);
    }

    a_strhash_free(Entity->components);
    a_strhash_freeEx(Entity->handlers, free);
    a_bitfield_free(Entity->componentBits);
    free(Entity->id);
    free(Entity);
}

const char* a_entity_getId(const AEntity* Entity)
{
    return Entity->id ? Entity->id : "AEntity";
}

void* a_entity_getContext(const AEntity* Entity)
{
    return Entity->context;
}

AEntity* a_entity_getParent(AEntity* Entity)
{
    return Entity->parent;
}

void a_entity_setParent(AEntity* Entity, AEntity* Parent)
{
    Entity->parent = Parent;
}

void a_entity_reference(AEntity* Entity)
{
    Entity->references++;
}

void a_entity_release(AEntity* Entity)
{
    if(g_collection->deleting) {
        // Entity could have already been freed. This is the only ECS function
        // that may be called from AFree callbacks.
        return;
    }

    if(Entity->references-- == 0) {
        a_out__fatal("Release count exceeds reference count for '%s'",
                     a_entity_getId(Entity));
    }
}

void a_entity_remove(AEntity* Entity)
{
    if(!entityIsRemoved(Entity)) {
        a_list_removeNode(Entity->collectionNode);
        Entity->collectionNode = a_list_addLast(g_collection->removedEntities,
                                                Entity);
    }
}

bool a_entity_isRemoved(const AEntity* Entity)
{
    return entityIsRemoved(Entity);
}

void a_entity_markActive(AEntity* Entity)
{
    Entity->lastActive = a_fps_getCounter();
}

bool a_entity_isActive(const AEntity* Entity)
{
    return Entity->lastActive == a_fps_getCounter();
}

void* a_entity_addComponent(AEntity* Entity, const char* Component)
{
    if(!entityIsNew(Entity)) {
        a_out__fatal("Too late to add component '%s' to '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    const AComponent* c = a_strhash_get(g_components, Component);

    if(c == NULL) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    if(a_bitfield_test(Entity->componentBits, c->bit)) {
        a_out__fatal("Component '%s' was already added to '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    AComponent* header = a_mem_zalloc(c->size);

    *header = *c;
    header->parent = Entity;

    a_strhash_add(Entity->components, Component, header);
    a_bitfield_set(Entity->componentBits, header->bit);

    return getComponent(header);
}

bool a_entity_hasComponent(const AEntity* Entity, const char* Component)
{
    bool has = a_strhash_contains(Entity->components, Component);

    if(!has && !a_strhash_contains(g_components, Component)) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    return has;
}

void* a_entity_getComponent(const AEntity* Entity, const char* Component)
{
    AComponent* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        if(!a_strhash_contains(g_components, Component)) {
            a_out__fatal("Unknown component '%s' for '%s'",
                         Component,
                         a_entity_getId(Entity));
        }

        return NULL;
    }

    return getComponent(header);
}

void* a_entity_requireComponent(const AEntity* Entity, const char* Component)
{
    AComponent* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        if(!a_strhash_contains(g_components, Component)) {
            a_out__fatal("Unknown component '%s' for '%s'",
                         Component,
                         a_entity_getId(Entity));
        }

        a_out__fatal("Missing required component '%s' in '%s'",
                     Component,
                     a_entity_getId(Entity));
    }

    return getComponent(header);
}

void a_entity_mute(AEntity* Entity)
{
    Entity->muted = true;
}

void a_entity_unmute(AEntity* Entity)
{
    Entity->muted = false;
}

bool a_entity_isMuted(const AEntity* Entity)
{
    return Entity->muted;
}

void a_entity_setMessageHandler(AEntity* Entity, const char* Message, AMessageHandler* Handler, bool HandleImmediately)
{
    if(a_strhash_contains(Entity->handlers, Message)) {
        a_out__fatal("'%s' handler already set for '%s'",
                     Message,
                     a_entity_getId(Entity));
    }

    AMessageHandlerContainer* h = a_mem_malloc(sizeof(AMessageHandlerContainer));

    h->handler = Handler;
    h->handleImmediately = HandleImmediately;

    a_strhash_add(Entity->handlers, Message, h);
}

void a_entity_sendMessage(AEntity* To, AEntity* From, const char* Message)
{
    AMessageHandlerContainer* h = a_strhash_get(To->handlers, Message);

    if(h == NULL) {
        a_out__warningv("'%s' does not handle '%s'",
                        a_entity_getId(To),
                        Message);
        return;
    }

    if(h->handleImmediately) {
        if(!entityIsRemoved(To) && !entityIsRemoved(From) && !To->muted) {
            h->handler(To, From);
        }
    } else {
        AMessage* message = message_new(To, From, Message);
        a_list_addLast(g_collection->messageQueue, message);
    }
}

void a_system_declare(const char* Name, const char* Components, ASystemHandler* Handler, ASystemSort* Compare, bool OnlyActiveEntities)
{
    if(a_strhash_contains(g_systems, Name)) {
        a_out__fatal("System '%s' already declared", Name);
    }

    ASystem* s = a_mem_malloc(sizeof(ASystem));

    s->handler = Handler;
    s->compare = Compare;
    s->entities = a_list_new();
    s->componentBits = a_bitfield_new(a_strhash_getSize(g_components));
    s->onlyActiveEntities = OnlyActiveEntities;
    s->muted = false;
    s->runsInCurrentState = false;

    a_strhash_add(g_systems, Name, s);

    AList* tok = a_str_split(Components, " ");

    A_LIST_ITERATE(tok, char*, name) {
        AComponent* c = a_strhash_get(g_components, name);

        if(c == NULL) {
            a_out__fatal("Unknown component '%s' for system '%s'",
                         name, Name);
        }

        a_bitfield_set(s->componentBits, c->bit);
    }

    a_list_freeEx(tok, free);
}

static void runSystem(const ASystem* System)
{
    if(System->muted) {
        return;
    }

    if(System->compare) {
        a_list_sort(System->entities, (AListCompare*)System->compare);
    }

    if(System->onlyActiveEntities) {
        A_LIST_FILTER(System->entities, AEntity*, entity, !entity->muted) {
            if(a_entity_isActive(entity)) {
                System->handler(entity);
            }
        }
    } else {
        A_LIST_FILTER(System->entities, AEntity*, entity, !entity->muted) {
            System->handler(entity);
        }
    }
}

void a_system_execute(const char* Systems)
{
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("a_system_execute: unknown system '%s'", name);
        }

        if(!system->runsInCurrentState) {
            a_out__fatal("a_system_execute: '%s' does not run in state", name);
        }

        runSystem(system);
    }

    a_list_freeEx(tok, free);
}

void a_system__tick(void)
{
    a_system_flushNewEntities();

    A_LIST_ITERATE(g_collection->tickSystems, ASystem*, system) {
        runSystem(system);
    }

    A_LIST_ITERATE(g_collection->messageQueue, AMessage*, m) {
        AMessageHandlerContainer* h = a_strhash_get(m->to->handlers,
                                                    m->message);

        if(!entityIsRemoved(m->to) && !entityIsRemoved(m->from)) {
            if(m->to->muted) {
                // Keep message in queue
                continue;
            } else {
                h->handler(m->to, m->from);
            }
        }

        message_free(m);
    }

    a_list_clear(g_collection->messageQueue);

    A_LIST_ITERATE(g_collection->removedEntities, AEntity*, entity) {
        if(entity->references == 0) {
            a_entity__free(entity);
            A_LIST_REMOVE_CURRENT();
        } else if(!a_list_isEmpty(entity->systemNodes)) {
            // Remove entity from any systems it's in
            a_list_clearEx(entity->systemNodes, (AFree*)a_list_removeNode);
        }
    }
}

void a_system__draw(void)
{
    A_LIST_ITERATE(g_collection->drawSystems, ASystem*, system) {
        runSystem(system);
    }
}

void a_system_flushNewEntities(void)
{
    A_LIST_ITERATE(g_collection->newEntities, AEntity*, e) {
        // Check if the entity matches any systems
        A_LIST_ITERATE(g_collection->allSystems, ASystem*, s) {
            if(a_bitfield_testMask(e->componentBits, s->componentBits)) {
                a_list_addLast(e->systemNodes, a_list_addLast(s->entities, e));
            }
        }

        e->collectionNode = a_list_addLast(g_collection->runningEntities, e);
        A_LIST_REMOVE_CURRENT();
    }
}

static void muteSystems(const char* Systems, bool Muted)
{
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("%s: unknown system '%s'",
                         Muted ? "a_system_mute" : "a_system_unmute",
                         name);
        }

        if(!system->runsInCurrentState) {
            a_out__fatal("%s: '%s' does not run in state",
                         Muted ? "a_system_mute" : "a_system_unmute",
                         name);
        }

        system->muted = Muted;
    }

    a_list_freeEx(tok, free);
}

void a_system_mute(const char* Systems)
{
    muteSystems(Systems, true);
}

void a_system_unmute(const char* Systems)
{
    muteSystems(Systems, false);
}

AList* a_system__parse(const char* Systems)
{
    AList* systems = a_list_new();
    AList* tok = a_str_split(Systems, " ");

    A_LIST_ITERATE(tok, char*, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("Unknown system '%s'", name);
        }

        a_list_addLast(systems, system);
    }

    a_list_freeEx(tok, free);

    return systems;
}

void a_system__pushCollection(AList* TickSystems, AList* DrawSystems)
{
    ARunningCollection* c = a_mem_malloc(sizeof(ARunningCollection));

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

    if(g_collection != NULL) {
        a_list_push(g_stack, g_collection);
    }

    g_collection = c;

    A_LIST_ITERATE(g_collection->allSystems, ASystem*, system) {
        system->runsInCurrentState = true;
    }
}

void a_system__popCollection(void)
{
    g_collection->deleting = true;

    A_LIST_ITERATE(g_collection->allSystems, ASystem*, system) {
        system->muted = false;
        system->runsInCurrentState = false;
    }

    a_list_freeEx(g_collection->messageQueue, (AFree*)message_free);
    a_list_freeEx(g_collection->newEntities, (AFree*)a_entity__free);
    a_list_freeEx(g_collection->runningEntities, (AFree*)a_entity__free);
    a_list_freeEx(g_collection->removedEntities, (AFree*)a_entity__free);
    a_list_free(g_collection->allSystems);

    free(g_collection);
    g_collection = a_list_pop(g_stack);

    if(g_collection != NULL) {
        A_LIST_ITERATE(g_collection->allSystems, ASystem*, system) {
            system->runsInCurrentState = true;
        }
    }
}
