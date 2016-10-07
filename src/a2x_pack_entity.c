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

struct AEntity {
    AStrHash* components;
};

struct AComponentHeader {
    size_t size;
    AComponentFree free;
    AComponentTick tick;
    AComponentDraw draw;
    AEntity* parent;
    AListNode* collectionNode;
};

static AStrHash* g_prototypes;
static AList* g_stack;

void a_entity__init(void)
{
    g_prototypes = a_strhash_new();
    g_stack = a_list_new();
}

void a_entity__uninit(void)
{
    A_STRHASH_ITERATE(g_prototypes, AComponentHeader*, prototype) {
        free(prototype);
    }

    a_strhash_free(g_prototypes);
    a_list_free(g_stack);
}

void a_component_declare(const char* Name, size_t Size)
{
    if(a_strhash_contains(g_prototypes, Name)) {
        a_out__fatal("Component '%s' was already defined");
    }

    AComponentHeader* h = a_mem_malloc(sizeof(AComponentHeader));

    h->size = sizeof(AComponentHeader) + Size;
    h->free = NULL;
    h->tick = NULL;
    h->draw = NULL;
    h->parent = NULL;
    h->collectionNode = NULL;

    a_strhash_add(g_prototypes, Name, h);
}

void a_component_setFree(const char* Name, AComponentFree Free)
{
    AComponentHeader* h = a_strhash_get(g_prototypes, Name);

    if(h == NULL) {
        a_out__fatal("Undeclared component '%s'", Name);
    }

    h->free = Free;
}

void a_component_setTick(const char* Name, AComponentTick Tick)
{
    AComponentHeader* h = a_strhash_get(g_prototypes, Name);

    if(h == NULL) {
        a_out__fatal("Undeclared component '%s'", Name);
    }

    h->tick = Tick;
}

void a_component_setDraw(const char* Name, AComponentDraw Draw)
{
    AComponentHeader* h = a_strhash_get(g_prototypes, Name);

    if(h == NULL) {
        a_out__fatal("Undeclared component '%s'", Name);
    }

    h->draw = Draw;
}

AEntity* a_component_getEntity(void* Component)
{
    AComponentHeader* header = (AComponentHeader*)
                               ((uint8_t*)Component - sizeof(AComponentHeader));
    return header->parent;
}

AEntity* a_entity_new(void)
{
    AEntity* e = a_mem_malloc(sizeof(AEntity));

    e->components = a_strhash_new();

    return e;
}

void a_entity_free(AEntity* Entity)
{
    A_STRHASH_ITERATE(Entity->components, AComponentHeader*, component) {
        if(component->collectionNode) {
            a_list_removeNode(component->collectionNode);
        }

        if(component->free) {
            component->free((uint8_t*)component + sizeof(AComponentHeader));
        }

        free(component);
    }

    a_strhash_free(Entity->components);
    free(Entity);
}

void* a_entity_addComponent(AEntity* Entity, const char* Component)
{
    const AComponentHeader* proto = a_strhash_get(g_prototypes, Component);

    if(proto == NULL) {
        a_out__fatal("Undeclared component '%s'");
    }

    AComponentHeader* c = a_mem_malloc(proto->size);

    *c = *proto;
    c->parent = Entity;

    a_strhash_add(Entity->components, Component, c);

    AStrHash* collection = a_list_peek(g_stack);

    if(collection != NULL) {
        AList* components = a_strhash_get(collection, Component);

        if(components == NULL) {
            components = a_list_new();
            a_strhash_add(collection, Component, components);
        }

        c->collectionNode = a_list_addLast(components, c);
    }

    return (uint8_t*)c + sizeof(AComponentHeader);
}

void* a_entity_getComponent(AEntity* Entity, const char* Component)
{
    AComponentHeader* header = a_strhash_get(Entity->components, Component);

    if(header == NULL) {
        return NULL;
    }

    return (uint8_t*)header + sizeof(AComponentHeader);
}

void a_entity__pushCollection(void)
{
    a_list_push(g_stack, a_strhash_new());
}

void a_entity__popCollection(void)
{
    AStrHash* collection = a_list_pop(g_stack);

    A_STRHASH_ITERATE(collection, AList*, components) {
        a_list_free(components);
    }

    a_strhash_free(collection);
}

void a_entity__handleComponents(void)
{
    AStrHash* collection = a_list_peek(g_stack);

    A_STRHASH_ITERATE(collection, AList*, components) {
        A_LIST_ITERATE(components, AComponentHeader*, c) {
            if(c->tick) {
                c->tick((uint8_t*)c + sizeof(AComponentHeader));
            }
        }
    }

    if(a_fps_notSkipped()) {
        A_STRHASH_ITERATE(collection, AList*, components) {
            A_LIST_ITERATE(components, AComponentHeader*, c) {
                if(c->draw) {
                    c->draw((uint8_t*)c + sizeof(AComponentHeader));
                }
            }
        }
    }
}
