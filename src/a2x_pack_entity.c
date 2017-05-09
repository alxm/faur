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
    AComponentFree* free;
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
} ASystem;

typedef struct ARunningCollection {
    AList* newEntities; // new entities are added to this list
    AList* runningEntities; // entities in this list are picked up by systems
    AList* removedEntities; // removed entities with outstanding references
    AList* tickSystems; // tick systems in the specified order
    AList* drawSystems; // draw systems in the specified order
    void* context; // global context
    bool deleting; // set when this collection is popped off the stack
} ARunningCollection;

struct AEntity {
    char* id; // specified name for debugging
    AListNode* collectionNode; // list node in one of new, running, or removed
    AList* systemNodes; // list of nodes in ASystem.entities lists
    AStrHash* components;
    ABitfield* componentBits;
    unsigned lastActive; // frame when a_entity_markActive was last called
    unsigned references; // if >0, then the entity lingers in the removed list
    bool muted; // systems don't run on this entity if this is set
};

#define ENTITY_NAME(Entity) (Entity->id ? Entity->id : "entity")

#define GET_COMPONENT(Header) ((void*)(Header + 1))
#define GET_HEADER(Component) ((AComponent*)Component - 1)

static ARunningCollection* g_collection;
static AList* g_stack; // list of ARunningCollection (one for each state)
static AStrHash* g_components; // table of declared AComponent
static AStrHash* g_systems; // table of declared ASystem

void a_entity__init(void)
{
    g_collection = NULL;
    g_stack = a_list_new();
    g_components = a_strhash_new();
    g_systems = a_strhash_new();
}

void a_entity__uninit(void)
{
    a_list_free(g_stack);

    A_STRHASH_ITERATE(g_systems, ASystem*, system) {
        a_list_free(system->entities);
        a_bitfield_free(system->componentBits);
        free(system);
    }

    A_STRHASH_ITERATE(g_components, AComponent*, component) {
        free(component);
    }

    a_strhash_free(g_systems);
    a_strhash_free(g_components);
}

void a_component_declare(const char* Name, size_t Size, AComponentFree* Free)
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
    return GET_HEADER(Component)->parent;
}

AEntity* a_entity_new(const char* Id)
{
    AEntity* e = a_mem_malloc(sizeof(AEntity));

    e->id = Id ? a_str_dup(Id) : NULL;
    e->collectionNode = a_list_addLast(g_collection->newEntities, e);
    e->systemNodes = a_list_new();
    e->components = a_strhash_new();
    e->componentBits = a_bitfield_new(a_strhash_getSize(g_components));
    e->lastActive = a_fps_getCounter() - 1;
    e->references = 0;
    e->muted = false;

    return e;
}

static void a_entity__free(AEntity* Entity)
{
    A_LIST_ITERATE(Entity->systemNodes, AListNode*, node) {
        a_list_removeNode(node);
    }

    a_list_free(Entity->systemNodes);

    A_STRHASH_ITERATE(Entity->components, AComponent*, header) {
        if(header->free) {
            header->free(GET_COMPONENT(header));
        }

        free(header);
    }

    a_strhash_free(Entity->components);
    a_bitfield_free(Entity->componentBits);
    free(Entity->id);
    free(Entity);
}

void a_entity_reference(AEntity* Entity)
{
    Entity->references++;
}

void a_entity_release(AEntity* Entity)
{
    if(g_collection->deleting) {
        // Entity could have already been freed. This is the only ECS function
        // that may be called from AComponentFree callbacks.
        return;
    }

    if(Entity->references-- == 0) {
        a_out__fatal("Release count exceeds reference count for '%s'",
                     ENTITY_NAME(Entity));
    }
}

void a_entity_remove(AEntity* Entity)
{
    if(a_list__nodeGetList(Entity->collectionNode) != g_collection->removedEntities) {
        a_list_removeNode(Entity->collectionNode);
        Entity->collectionNode = a_list_addLast(g_collection->removedEntities,
                                                Entity);
    }
}

bool a_entity_isRemoved(const AEntity* Entity)
{
    return a_list__nodeGetList(Entity->collectionNode) == g_collection->removedEntities;
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
    if(a_list__nodeGetList(Entity->collectionNode) != g_collection->newEntities) {
        a_out__fatal("Too late to add component '%s' to '%s'",
                     Component,
                     ENTITY_NAME(Entity));
    }

    const AComponent* c = a_strhash_get(g_components, Component);

    if(c == NULL) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     ENTITY_NAME(Entity));
    }

    if(a_bitfield_test(Entity->componentBits, c->bit)) {
        a_out__fatal("Component '%s' was already added to '%s'",
                     Component,
                     ENTITY_NAME(Entity));
    }

    AComponent* header = a_mem_zalloc(c->size);

    *header = *c;
    header->parent = Entity;

    a_strhash_add(Entity->components, Component, header);
    a_bitfield_set(Entity->componentBits, header->bit);

    return GET_COMPONENT(header);
}

bool a_entity_hasComponent(const AEntity* Entity, const char* Component)
{
    bool has = a_strhash_contains(Entity->components, Component);

    if(!has && !a_strhash_contains(g_components, Component)) {
        a_out__fatal("Unknown component '%s' for '%s'",
                     Component,
                     ENTITY_NAME(Entity));
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
                         ENTITY_NAME(Entity));
        }

        return NULL;
    }

    return GET_COMPONENT(header);
}

void* a_entity_requireComponent(const AEntity* Entity, const char* Component)
{
    AComponent* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        if(!a_strhash_contains(g_components, Component)) {
            a_out__fatal("Unknown component '%s' for '%s'",
                         Component,
                         ENTITY_NAME(Entity));
        }

        a_out__fatal("Missing required component '%s' in '%s'",
                     Component,
                     ENTITY_NAME(Entity));
    }

    return GET_COMPONENT(header);
}

void a_entity_mute(AEntity* Entity)
{
    Entity->muted = true;
}

void a_entity_unmute(AEntity* Entity)
{
    Entity->muted = false;
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

    a_strhash_add(g_systems, Name, s);

    AStrTok* tok = a_strtok_new(Components, " ");

    A_STRTOK_ITERATE(tok, name) {
        AComponent* c = a_strhash_get(g_components, name);

        if(c == NULL) {
            a_out__fatal("Unknown component '%s' for system '%s'",
                         name, Name);
        }

        a_bitfield_set(s->componentBits, c->bit);
    }

    a_strtok_free(tok);
}

void a_system_tick(const char* Systems)
{
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, systemName) {
        ASystem* system = a_strhash_get(g_systems, systemName);

        if(system == NULL) {
            a_out__fatal("Unknown tick system '%s'", systemName);
        }

        a_list_addLast(g_collection->tickSystems, system);
    }

    a_strtok_free(tok);
}

void a_system_draw(const char* Systems)
{
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, systemName) {
        ASystem* system = a_strhash_get(g_systems, systemName);

        if(system == NULL) {
            a_out__fatal("Unknown draw system '%s'", systemName);
        }

        a_list_addLast(g_collection->drawSystems, system);
    }

    a_strtok_free(tok);
}

void* a_system_getContext(void)
{
    return g_collection->context;
}

void a_system_setContext(void* GlobalContext)
{
    g_collection->context = GlobalContext;
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
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("a_system_execute: unknown system '%s'", name);
        }

        runSystem(system);
    }

    a_strtok_free(tok);
}

void a_system__run(void)
{
    a_system_flushNewEntities();

    A_LIST_ITERATE(g_collection->tickSystems, ASystem*, system) {
        runSystem(system);
    }

    if(a_fps__notSkipped()) {
        A_LIST_ITERATE(g_collection->drawSystems, ASystem*, system) {
            runSystem(system);
        }
    }

    A_LIST_ITERATE(g_collection->removedEntities, AEntity*, entity) {
        if(entity->references == 0) {
            a_entity__free(entity);
            A_LIST_REMOVE_CURRENT();
        }
    }
}

void a_system_flushNewEntities(void)
{
    A_LIST_ITERATE(g_collection->newEntities, AEntity*, e) {
        // Check if the entity matches any systems
        A_STRHASH_ITERATE(g_systems, ASystem*, s) {
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
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, name) {
        ASystem* system = a_strhash_get(g_systems, name);

        if(system == NULL) {
            a_out__fatal("%s: unknown system '%s'",
                         Muted ? "a_system_mute" : "a_system_unmute",
                         name);
        }

        system->muted = Muted;
    }

    a_strtok_free(tok);
}

void a_system_mute(const char* Systems)
{
    muteSystems(Systems, true);
}

void a_system_unmute(const char* Systems)
{
    muteSystems(Systems, false);
}

void a_system__pushCollection(void)
{
    ARunningCollection* c = a_mem_malloc(sizeof(ARunningCollection));

    c->newEntities = a_list_new();
    c->runningEntities = a_list_new();
    c->removedEntities = a_list_new();
    c->tickSystems = a_list_new();
    c->drawSystems = a_list_new();
    c->context = NULL;
    c->deleting = false;

    if(g_collection != NULL) {
        a_list_push(g_stack, g_collection);
    }

    g_collection = c;
}

void a_system__popCollection(void)
{
    g_collection->deleting = true;

    A_LIST_ITERATE(g_collection->newEntities, AEntity*, entity) {
        a_entity__free(entity);
    }

    A_LIST_ITERATE(g_collection->runningEntities, AEntity*, entity) {
        a_entity__free(entity);
    }

    A_LIST_ITERATE(g_collection->removedEntities, AEntity*, entity) {
        a_entity__free(entity);
    }

    A_LIST_ITERATE(g_collection->tickSystems, ASystem*, system) {
        system->muted = false;
    }

    A_LIST_ITERATE(g_collection->drawSystems, ASystem*, system) {
        system->muted = false;
    }

    a_list_free(g_collection->newEntities);
    a_list_free(g_collection->runningEntities);
    a_list_free(g_collection->removedEntities);

    a_list_free(g_collection->tickSystems);
    a_list_free(g_collection->drawSystems);

    free(g_collection);
    g_collection = a_list_pop(g_stack);
}
