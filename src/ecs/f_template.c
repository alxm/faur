/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

struct FTemplate {
    unsigned instanceNumber; // Incremented by each new entity
    FEntityInit* init; // Optional, runs after components init and parent init
    FBitfield* componentBits; // Set if template or a parent has component
    FBitfield* componentBitsOwn; // Set if template actually has the component
    const FTemplate* parent; // template chain
    void* data[]; // [f_component__num] Loaded config data, or NULL
                  // Component bit might be set even if data is NULL
};

static FHash* g_templates; // FHash<char*, FTemplate*>

static FTemplate* templateNew(const char* Id)
{
    FTemplate* t = f_mem_zalloc(
                    sizeof(FTemplate) + sizeof(void*) * f_component__num);

    t->componentBits = f_bitfield_new(f_component__num);
    t->componentBitsOwn = f_bitfield_new(f_component__num);

    char* parentId = f_str_prefixGetToLast(Id, '.');

    if(parentId) {
        FTemplate* parentTemplate = f_hash_get(g_templates, parentId);

        if(parentTemplate == NULL) {
            parentTemplate = templateNew(parentId);
        }

        t->parent = parentTemplate;

        for(unsigned c = f_component__num; c--; ) {
            if(f_bitfield_test(parentTemplate->componentBits, c)) {
                f_bitfield_set(t->componentBits, c);
                t->data[c] = parentTemplate->data[c];
            }
        }

        f_mem_free(parentId);
    }

    f_hash_add(g_templates, f_str_dup(Id), t);

    return t;
}

static void templateFree(FTemplate* Template)
{
    for(unsigned c = f_component__num; c--; ) {
        if(f_bitfield_test(Template->componentBitsOwn, c)
            && Template->data[c]) {

            f_component__dataFree(
                f_component__getByIndex(c), Template->data[c]);
        }
    }

    f_bitfield_free(Template->componentBits);
    f_bitfield_free(Template->componentBitsOwn);

    f_mem_free(Template);
}

void f_template__init(void)
{
    g_templates = f_hash_newStr(256, true);
}

void f_template__uninit(void)
{
    f_hash_freeEx(g_templates, (FFree*)templateFree);
}

static void process_file(const char* FilePath)
{
    FBlock* root = f_block_new(FilePath);

    F_LIST_ITERATE(f_block_blocksGet(root), const FBlock*, b) {
        const char* templateId = f_block_lineGetString(b, 0);

        #if F_CONFIG_BUILD_DEBUG
            if(f_hash_contains(g_templates, templateId)) {
                F__FATAL("f_template_load(%s): '%s' already declared",
                         FilePath,
                         templateId);
            }
        #endif

        FTemplate* t = templateNew(templateId);

        F_LIST_ITERATE(f_block_blocksGet(b), const FBlock*, b) {
            const char* componentId = f_block_lineGetString(b, 0);
            const FComponent* component = f_component__getByString(componentId);

            if(component == NULL) {
                f_out__error("f_template_load(%s): Unknown component '%s'",
                             templateId,
                             componentId);

                continue;
            }

            f_bitfield_set(t->componentBits, component->bitId);
            f_bitfield_set(t->componentBitsOwn, component->bitId);
            t->data[component->bitId] = f_component__dataInit(component, b);
        }
    }

    f_block_free(root);
}

static void process_dir(const char* Path)
{
    FDir* dir = f_dir_new(Path);

    F_LIST_ITERATE(f_dir_entriesGet(dir), const FPath*, p) {
        const char* path = f_path_getFull(p);

        if(f_path_test(p, F_PATH_DIR)) {
            process_dir(path);
        } else if(f_str_endsWith(f_path_getName(p), ".txt")) {
            process_file(path);
        }
    }

    f_dir_free(dir);
}

void f_template_load(const char* Dir)
{
    process_dir(Dir);
}

const FTemplate* f_template__get(const char* TemplateId)
{
    FTemplate* t = f_hash_get(g_templates, TemplateId);

    #if F_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            F__FATAL("Unknown template '%s'", TemplateId);
        }
    #endif

    t->instanceNumber++;

    return t;
}

void f_template__initRun(const FTemplate* Template, FEntity* Entity, const void* Context)
{
    if(Template->parent) {
        f_template__initRun(Template->parent, Entity, Context);
    }

    if(Template->init) {
        Template->init(Entity, Context);
    }
}

void f_template_init(const char* Id, FEntityInit* Init)
{
    FTemplate* t = f_hash_get(g_templates, Id);

    #if F_CONFIG_BUILD_DEBUG
        if(t == NULL) {
            F__FATAL("f_template_init(%s): Unknown template", Id);
        }
    #endif

    t->init = Init;
}

unsigned f_template__instanceGet(const FTemplate* Template)
{
    return Template->instanceNumber;
}

bool f_template__dataGet(const FTemplate* Template, const FComponent* Component, const void** Data)
{
    if(f_bitfield_test(Template->componentBits, Component->bitId)) {
        *Data = Template->data[Component->bitId];

        return true;
    }

    return false;
}
