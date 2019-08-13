/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_template.v.h"
#include <a2x.v.h>

struct ATemplate {
    unsigned instanceNumber; // Incremented by each new entity
    AEntityInit* init; // Optional callback, runs after the components init
    ABitfield* componentBits; // Set if template has corresponding component
    void* data[A_CONFIG_ECS_COM_NUM]; // Parsed component config data, or NULL
    const ATemplate* parent; // template chain
};

static AStrHash* g_templates; // table of ATemplate
static AEntityInit* g_init; // Optional callback, runs before template-specific

static ATemplate* templateNew(const ABlock* Block)
{
    ATemplate* t = a_mem_zalloc(sizeof(ATemplate));
    const char* templateId = a_block_lineGetString(Block, 0);

    t->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);

    A_LIST_ITERATE(a_block_blocksGet(Block), const ABlock*, b) {
        const char* id = a_block_lineGetString(b, 0);
        int index = a_component__stringToIndex(id);

        if(index < 0) {
            a_out__error(
                "a_template_new(%s): Unknown component '%s'", templateId, id);

            continue;
        }

        const AComponent* component = a_component__get(index);

        t->data[index] = a_component__templateInit(component, b);
        a_bitfield_set(t->componentBits, (unsigned)index);

        for(char* base = a_str_prefixGetToLast(templateId, '.'); base; ) {
            const ATemplate* parentTemplate = a_strhash_get(g_templates, base);

            if(parentTemplate) {
                t->parent = parentTemplate;
                a_mem_free(base);

                break;
            }

            char* nextBase = a_str_prefixGetToLast(base, '.');

            a_mem_free(base);
            base = nextBase;
        }
    }

    return t;
}

static void templateFree(ATemplate* Template)
{
    a_bitfield_free(Template->componentBits);

    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        if(Template->data[c]) {
            a_component__templateFree(a_component__get(c), Template->data[c]);
        }
    }

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
        const char* id = a_block_lineGetString(b, 0);

        #if A_CONFIG_BUILD_DEBUG
            if(a_strhash_contains(g_templates, id)) {
                A__FATAL(
                    "a_template_new(%s): '%s' already declared", FilePath, id);
            }
        #endif

        a_strhash_add(g_templates, id, templateNew(b));
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

AEntityInit* a_template__initGet(const ATemplate* Template)
{
    return Template == NULL ? g_init : Template->init;
}

void a_template_initSet(const char* Id, AEntityInit* Init)
{
    if(Id == NULL) {
        g_init = Init;

        return;
    }

    ATemplate* t = a_strhash_get(g_templates, Id);

    #if A_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            A__FATAL("a_template_initSet(%s): Unknown template", Id);
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
    for(const ATemplate* t = Template; t; t = t->parent) {
        if(a_bitfield_test(t->componentBits, (unsigned)ComponentIndex)) {
            return true;
        }
    }

    return false;
}

const void* a_template__dataGet(const ATemplate* Template, int ComponentIndex)
{
    for(const ATemplate* t = Template; t; t = t->parent) {
        if(a_bitfield_test(t->componentBits, (unsigned)ComponentIndex)) {
            return t->data[ComponentIndex];
        }
    }

    return NULL;
}
