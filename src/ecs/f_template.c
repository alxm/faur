/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "f_template.v.h"
#include <faur.v.h>

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
    ATemplate* t = f_mem_zalloc(sizeof(ATemplate));

    t->componentBits = f_bitfield_new(A_CONFIG_ECS_COM_NUM);
    t->componentBitsOwn = f_bitfield_new(A_CONFIG_ECS_COM_NUM);

    char* parentId = f_str_prefixGetToLast(Id, '.');

    if(parentId) {
        ATemplate* parentTemplate = f_strhash_get(g_templates, parentId);

        if(parentTemplate == NULL) {
            parentTemplate = templateNew(parentId);
        }

        t->parent = parentTemplate;

        for(unsigned c = A_CONFIG_ECS_COM_NUM; c--; ) {
            if(f_bitfield_test(parentTemplate->componentBits, c)) {
                f_bitfield_set(t->componentBits, c);
                t->data[c] = parentTemplate->data[c];
            }
        }

        f_mem_free(parentId);
    }

    f_strhash_add(g_templates, Id, t);

    return t;
}

static void templateFree(ATemplate* Template)
{
    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        if(f_bitfield_test(Template->componentBitsOwn, (unsigned)c)
            && Template->data[c]) {

            f_component__templateFree(f_component__get(c), Template->data[c]);
        }
    }

    f_bitfield_free(Template->componentBits);
    f_bitfield_free(Template->componentBitsOwn);

    f_mem_free(Template);
}

void f_template__init(void)
{
    g_templates = f_strhash_new();
}

void f_template__uninit(void)
{
    f_strhash_freeEx(g_templates, (AFree*)templateFree);
}

void f_template_new(const char* FilePath)
{
    ABlock* root = f_block_new(FilePath);

    A_LIST_ITERATE(f_block_blocksGet(root), const ABlock*, b) {
        const char* templateId = f_block_lineGetString(b, 0);

        #if A_CONFIG_BUILD_DEBUG
            if(f_strhash_contains(g_templates, templateId)) {
                A__FATAL("f_template_new(%s): '%s' already declared",
                         FilePath,
                         templateId);
            }
        #endif

        ATemplate* t = templateNew(templateId);

        A_LIST_ITERATE(f_block_blocksGet(b), const ABlock*, b) {
            const char* componentId = f_block_lineGetString(b, 0);
            int componentIndex = f_component__stringToIndex(componentId);

            if(componentIndex < 0) {
                f_out__error("f_template_new(%s): Unknown component '%s'",
                             templateId,
                             componentId);

                continue;
            }

            f_bitfield_set(t->componentBits, (unsigned)componentIndex);
            f_bitfield_set(t->componentBitsOwn, (unsigned)componentIndex);
            t->data[componentIndex] =
                f_component__templateInit(f_component__get(componentIndex), b);
        }
    }

    f_block_free(root);
}

const ATemplate* f_template__get(const char* TemplateId)
{
    ATemplate* t = f_strhash_get(g_templates, TemplateId);

    #if A_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            A__FATAL("Unknown template '%s'", TemplateId);
        }
    #endif

    t->instanceNumber++;

    return t;
}

void f_template__initRun(const ATemplate* Template, AEntity* Entity, const void* Context)
{
    if(Template->parent) {
        f_template__initRun(Template->parent, Entity, Context);
    }

    if(Template->init) {
        Template->init(Entity, Context);
    }
}

void f_template_init(const char* Id, AEntityInit* Init)
{
    ATemplate* t = f_strhash_get(g_templates, Id);

    #if A_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            A__FATAL("f_template_init(%s): Unknown template", Id);
        }
    #endif

    t->init = Init;
}

unsigned f_template__instanceGet(const ATemplate* Template)
{
    return Template->instanceNumber;
}

bool f_template__componentHas(const ATemplate* Template, int ComponentIndex)
{
    return f_bitfield_test(Template->componentBits, (unsigned)ComponentIndex);
}

const void* f_template__dataGet(const ATemplate* Template, int ComponentIndex)
{
    return Template->data[ComponentIndex];
}
#endif // A_CONFIG_ECS_ENABLED
