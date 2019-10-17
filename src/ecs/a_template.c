/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_template.v.h"
#include <a2x.v.h>

#if A_CONFIG_ECS_ENABLED
struct ATemplate {
    unsigned instanceNumber; // Incremented by each new entity
    AEntityInit* init; // Optional, runs after components init and parent init
    ABitfield* componentBits; // Set if template or a parent has component
    ABitfield* componentBitsOwn; // Set if template actually has the component
    void* data[A_CONFIG_ECS_COM_NUM]; // Parsed component config data, or NULL
    const ATemplate* parent; // template chain
};

static AStrHash* g_templates; // table of ATemplate

static ATemplate* templateNew(const char* Id)
{
    ATemplate* t = a_mem_zalloc(sizeof(ATemplate));

    t->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);
    t->componentBitsOwn = a_bitfield_new(A_CONFIG_ECS_COM_NUM);

    char* parentId = a_str_prefixGetToLast(Id, '.');

    if(parentId) {
        ATemplate* parentTemplate = a_strhash_get(g_templates, parentId);

        if(parentTemplate == NULL) {
            parentTemplate = templateNew(parentId);
        }

        t->parent = parentTemplate;

        for(unsigned c = A_CONFIG_ECS_COM_NUM; c--; ) {
            if(a_bitfield_test(parentTemplate->componentBits, c)) {
                a_bitfield_set(t->componentBits, c);
                t->data[c] = parentTemplate->data[c];
            }
        }

        a_mem_free(parentId);
    }

    a_strhash_add(g_templates, Id, t);

    return t;
}

static void templateFree(ATemplate* Template)
{
    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        if(a_bitfield_test(Template->componentBitsOwn, (unsigned)c)
            && Template->data[c]) {

            a_component__templateFree(a_component__get(c), Template->data[c]);
        }
    }

    a_bitfield_free(Template->componentBits);
    a_bitfield_free(Template->componentBitsOwn);

    a_mem_free(Template);
}

void a_template__init(void)
{
    g_templates = a_strhash_new();
}

void a_template__uninit(void)
{
    a_strhash_freeEx(g_templates, (AFree*)templateFree);
}

void a_template_new(const char* FilePath)
{
    ABlock* root = a_block_new(FilePath);

    A_LIST_ITERATE(a_block_blocksGet(root), const ABlock*, b) {
        const char* templateId = a_block_lineGetString(b, 0);

        #if A_CONFIG_BUILD_DEBUG
            if(a_strhash_contains(g_templates, templateId)) {
                A__FATAL("a_template_new(%s): '%s' already declared",
                         FilePath,
                         templateId);
            }
        #endif

        ATemplate* t = templateNew(templateId);

        A_LIST_ITERATE(a_block_blocksGet(b), const ABlock*, b) {
            const char* componentId = a_block_lineGetString(b, 0);
            int componentIndex = a_component__stringToIndex(componentId);

            if(componentIndex < 0) {
                a_out__error("a_template_new(%s): Unknown component '%s'",
                             templateId,
                             componentId);

                continue;
            }

            a_bitfield_set(t->componentBits, (unsigned)componentIndex);
            a_bitfield_set(t->componentBitsOwn, (unsigned)componentIndex);
            t->data[componentIndex] =
                a_component__templateInit(a_component__get(componentIndex), b);
        }
    }

    a_block_free(root);
}

const ATemplate* a_template__get(const char* TemplateId)
{
    ATemplate* t = a_strhash_get(g_templates, TemplateId);

    #if A_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            A__FATAL("Unknown template '%s'", TemplateId);
        }
    #endif

    t->instanceNumber++;

    return t;
}

void a_template__initRun(const ATemplate* Template, AEntity* Entity, const void* Context)
{
    if(Template->parent) {
        a_template__initRun(Template->parent, Entity, Context);
    }

    if(Template->init) {
        Template->init(Entity, Context);
    }
}

void a_template_init(const char* Id, AEntityInit* Init)
{
    ATemplate* t = a_strhash_get(g_templates, Id);

    #if A_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            A__FATAL("a_template_init(%s): Unknown template", Id);
        }
    #endif

    t->init = Init;
}

unsigned a_template__instanceGet(const ATemplate* Template)
{
    return Template->instanceNumber;
}

bool a_template__componentHas(const ATemplate* Template, int ComponentIndex)
{
    return a_bitfield_test(Template->componentBits, (unsigned)ComponentIndex);
}

const void* a_template__dataGet(const ATemplate* Template, int ComponentIndex)
{
    return Template->data[ComponentIndex];
}
#endif // A_CONFIG_ECS_ENABLED
