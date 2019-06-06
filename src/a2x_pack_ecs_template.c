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

#include "a2x_pack_ecs_template.v.h"

#include "a2x_pack_bitfield.v.h"
#include "a2x_pack_block.v.h"
#include "a2x_pack_ecs_component.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_strhash.v.h"

struct ATemplate {
    unsigned instanceNumber; // Incremented by each a_entity_newEx call
    ABitfield* componentBits; // Set if template has corresponding component
    void* data[A_CONFIG_ECS_COM_NUM]; // Parsed component config data, or NULL
};

static AStrHash* g_templates; // table of ATemplate

static ATemplate* templateNew(const char* TemplateId, const ABlock* Block)
{
    ATemplate* t = a_mem_zalloc(sizeof(ATemplate));

    t->componentBits = a_bitfield_new(A_CONFIG_ECS_COM_NUM);

    A_LIST_ITERATE(a_block_blocksGet(Block), const ABlock*, b) {
        const char* id = a_block_lineGetString(b, 0);
        int index = a_component__stringToIndex(id);

        if(index < 0) {
            a_out__error(
                "a_template_new(%s): Unknown component '%s'", TemplateId, id);

            continue;
        }

        const AComponent* component = a_component__get(index, __func__);

        t->data[index] = a_component__dataInit(component, b);

        a_bitfield_set(t->componentBits, a_component__bitGet(component));
    }

    return t;
}

static void templateFree(ATemplate* Template)
{
    a_bitfield_free(Template->componentBits);

    for(int c = A_CONFIG_ECS_COM_NUM; c--; ) {
        if(Template->data[c]) {
            a_component__dataFree(
                a_component__get(c, __func__), Template->data[c]);
        }
    }

    free(Template);
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

        if(a_strhash_contains(g_templates, id)) {
            A__FATAL("a_template_new(%s): '%s' already declared", FilePath, id);
        }

        a_strhash_add(g_templates, id, templateNew(id, b));
    }

    a_block_free(root);
}

const ATemplate* a_template__get(const char* TemplateId, const char* CallerFunction)
{
    ATemplate* t = a_strhash_get(g_templates, TemplateId);

    if(t == NULL) {
        A__FATAL("%s: Unknown template '%s'", CallerFunction, TemplateId);
    }

    t->instanceNumber++;

    return t;
}

unsigned a_template__instanceGet(const ATemplate* Template)
{
    return Template->instanceNumber;
}

bool a_template__componentHas(const ATemplate* Template, int Component)
{
    const AComponent* c = a_component__get(Component, __func__);

    return a_bitfield_test(Template->componentBits, a_component__bitGet(c));
}

const void* a_template__dataGet(const ATemplate* Template, int Component)
{
    return Template->data[Component];
}
