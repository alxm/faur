/*
    Copyright 2016 Alex Margarit

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

typedef enum ASystemCollectionState {
    A_SYSTEM_STATE_DECLARE_COMPONENTS,
    A_SYSTEM_STATE_DECLARE_SYSTEMS,
    A_SYSTEM_STATE_CREATE_ENTITIES
} ASystemCollectionState;

typedef struct ASystemCollection {
    AList* entities;
    AStrHash* components;
    AStrHash* systems;
    AList* tickSystems;
    AList* drawSystems;
    void* context;
    size_t bitFieldSize;
    unsigned int nextBit;
    ASystemCollectionState state;
} ASystemCollection;

typedef struct ASystem {
    ASystemHandler* handler;
    AList* entities;
    uint8_t* bits;
} ASystem;

struct AEntity {
    AListNode* collectionNode;
    AList* systemNodes;
    AStrHash* components;
    uint8_t* bits;
};

typedef struct AComponent {
    size_t size;
    AComponentFree* free;
    AEntity* parent;
    unsigned int bit;
} AComponent;

#define GET_COMPONENT(Header) ((void*)(Header + 1))
#define GET_HEADER(Component) ((AComponent*)Component - 1)

static AList* g_stack;
static ASystemCollection* g_collection;

void a_entity__init(void)
{
    g_stack = a_list_new();
    g_collection = NULL;

    // In case application isn't using states
    a_system__pushCollection();
}

void a_entity__uninit(void)
{
    a_system__popCollection();
    a_list_free(g_stack);
}

void a_component_declare(const char* Name, size_t Size, AComponentFree* Free)
{
    if(g_collection->state != A_SYSTEM_STATE_DECLARE_COMPONENTS) {
        a_out__fatal("Cannot declare component '%s' after declaring systems "
                     "or creating entities",
                     Name);
    }

    if(a_strhash_contains(g_collection->components, Name)) {
        a_out__fatal("Component '%s' was already defined");
    }

    if(g_collection->nextBit % CHAR_BIT == 0) {
        g_collection->bitFieldSize++;
    }

    AComponent* h = a_mem_malloc(sizeof(AComponent));

    h->size = sizeof(AComponent) + Size;
    h->free = Free;
    h->parent = NULL;
    h->bit = g_collection->nextBit++;

    a_strhash_add(g_collection->components, Name, h);
}

AEntity* a_component_getEntity(void* Component)
{
    return GET_HEADER(Component)->parent;
}

AEntity* a_entity_new(void)
{
    g_collection->state = A_SYSTEM_STATE_CREATE_ENTITIES;

    AEntity* e = a_mem_malloc(sizeof(AEntity));

    e->collectionNode = a_list_addLast(g_collection->entities, e);
    e->systemNodes = a_list_new();
    e->components = a_strhash_new();
    e->bits = a_mem_malloc(g_collection->bitFieldSize);
    memset(e->bits, 0, g_collection->bitFieldSize);

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

    free(Entity->bits);
    free(Entity);
}

void a_entity_free(AEntity* Entity)
{
    a_list_removeNode(Entity->collectionNode);
    a_entity__free(Entity);
}

void* a_entity_addComponent(AEntity* Entity, const char* Component)
{
    const AComponent* c = a_strhash_get(g_collection->components, Component);

    if(c == NULL) {
        a_out__fatal("Undeclared component '%s'");
    }

    AComponent* header = a_mem_malloc(c->size);

    *header = *c;
    header->parent = Entity;

    a_strhash_add(Entity->components, Component, header);

    Entity->bits[header->bit >> 3] |= 1 << (header->bit & 7);

    // Check if the Entity now matches a system
    A_STRHASH_ITERATE(g_collection->systems, ASystem*, system) {
        bool matches = true;

        for(int i = 0; i < g_collection->bitFieldSize; i++) {
            if((Entity->bits[i] & system->bits[i]) != system->bits[i]) {
                matches = false;
                break;
            }
        }

        if(matches) {
            a_list_addLast(Entity->systemNodes,
                           a_list_addLast(system->entities, Entity));
        }
    }

    return GET_COMPONENT(header);
}

void* a_entity_getComponent(AEntity* Entity, const char* Component)
{
    AComponent* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        return NULL;
    }

    return GET_COMPONENT(header);
}

void a_system_declare(const char* Name, const char* Components, ASystemHandler* Handler)
{
    if(g_collection->state != A_SYSTEM_STATE_DECLARE_SYSTEMS) {
        if(g_collection->state == A_SYSTEM_STATE_DECLARE_COMPONENTS) {
            g_collection->state = A_SYSTEM_STATE_DECLARE_SYSTEMS;
        } else {
            a_out__fatal("Cannot declare component '%s' after declaring "
                         "systems or creating entities",
                         Name);
        }
    }

    ASystem* s = a_mem_malloc(sizeof(ASystem));

    s->handler = Handler;
    s->entities = a_list_new();
    s->bits = a_mem_malloc(g_collection->bitFieldSize);
    memset(s->bits, 0, g_collection->bitFieldSize);

    a_strhash_add(g_collection->systems, Name, s);

    AStrTok* tok = a_strtok_new(Components, " ");

    A_STRTOK_ITERATE(tok, name) {
        AComponent* c = a_strhash_get(g_collection->components, name);

        if(c == NULL) {
            a_out__fatal("Undeclared component '%s' for system '%s'",
                         name, Name);
        }

        s->bits[c->bit >> 3] |= 1 << (c->bit & 7);
    }

    a_strtok_free(tok);
}

void a_system_tick(const char* Systems)
{
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, systemName) {
        ASystem* system = a_strhash_get(g_collection->systems, systemName);

        if(system == NULL) {
            a_out__fatal("Undeclared tickSystems system '%s'", systemName);
        }

        a_list_addLast(g_collection->tickSystems, system);
    }

    a_strtok_free(tok);
}

void a_system_draw(const char* Systems)
{
    AStrTok* tok = a_strtok_new(Systems, " ");

    A_STRTOK_ITERATE(tok, systemName) {
        ASystem* system = a_strhash_get(g_collection->systems, systemName);

        if(system == NULL) {
            a_out__fatal("Undeclared drawSystems system '%s'", systemName);
        }

        a_list_addLast(g_collection->drawSystems, system);
    }

    a_strtok_free(tok);
}

void a_system_setContext(void* GlobalContext)
{
    g_collection->context = GlobalContext;
}

void a_system_run(void)
{
    A_LIST_ITERATE(g_collection->tickSystems, ASystem*, system) {
        A_LIST_ITERATE(system->entities, AEntity*, entity) {
            system->handler(entity, g_collection->context);
        }
    }

    if(a_fps_notSkipped()) {
        A_LIST_ITERATE(g_collection->drawSystems, ASystem*, system) {
            A_LIST_ITERATE(system->entities, AEntity*, entity) {
                system->handler(entity, g_collection->context);
            }
        }
    }
}

void a_system__pushCollection(void)
{
    ASystemCollection* c = a_mem_malloc(sizeof(ASystemCollection));

    c->entities = a_list_new();
    c->components = a_strhash_new();
    c->systems = a_strhash_new();
    c->tickSystems = a_list_new();
    c->drawSystems = a_list_new();
    c->context = NULL;
    c->bitFieldSize = 0;
    c->nextBit = 0;
    c->state = A_SYSTEM_STATE_DECLARE_COMPONENTS;

    if(g_collection != NULL) {
        a_list_push(g_stack, g_collection);
    }

    g_collection = c;
}

void a_system__popCollection(void)
{
    if(!a_list_empty(g_collection->entities)) {
        a_out__warning("Did not free %d entities",
                       a_list_size(g_collection->entities));

        A_LIST_ITERATE(g_collection->entities, AEntity*, entity) {
            a_entity__free(entity);
        }
    }

    a_list_free(g_collection->entities);

    A_STRHASH_ITERATE(g_collection->components, AComponent*, component) {
        free(component);
    }

    a_strhash_free(g_collection->components);

    A_STRHASH_ITERATE(g_collection->systems, ASystem*, system) {
        a_list_free(system->entities);
        free(system->bits);
        free(system);
    }

    a_strhash_free(g_collection->systems);
    a_list_free(g_collection->tickSystems);
    a_list_free(g_collection->drawSystems);

    free(g_collection);
    g_collection = a_list_pop(g_stack);
}
