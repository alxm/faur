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

static FHash* g_templates; // FHash<char*, FTemplate*>

static FTemplate* templateNew(const char* Id, const FBlock* Block)
{
    FTemplate* t = f_mem_mallocz(
                    sizeof(FTemplate) + sizeof(void*) * (f_component__num - 1));

    if(f_sym_test(Id)) {
        t->init = (FCallEntityInit*)f_sym_address(Id);
    }

    t->componentsOwn = f_list_new();
    t->componentsAll = f_list_new();
    t->componentsBits = f_bitfield_new(f_component__num);

    char* parentId = f_str_prefixGetToLast(Id, '_');

    if(parentId) {
        FTemplate* parentTemplate = f_hash_get(g_templates, parentId);

        if(parentTemplate == NULL) {
            parentTemplate = templateNew(parentId, NULL);
        }

        t->parent = parentTemplate;

        F_LIST_ITERATE(parentTemplate->componentsAll, const FComponent*, c) {
            f_list_addLast(t->componentsAll, (void*)c);
            f_bitfield_set(t->componentsBits, c->bitId);

            t->data[c->bitId] = parentTemplate->data[c->bitId];
        }

        f_mem_free(parentId);
    }

    if(Block) {
        t->block = Block;

        F_LIST_ITERATE(f_block_blocksGet(Block), FBlock*, b) {
            const char* componentId = f_block_lineGetString(b, 0);
            const FComponent* c = f_hash_get(f_component__index, componentId);

            if(c == NULL) {
                f_out__error("f_template_load(%s): Unknown component '%s'",
                             Id,
                             componentId);

                continue;
            }

            if(f_bitfield_test(t->componentsBits, c->bitId)) {
                f_block__merge(
                    b, f_block_keyGetBlock(t->parent->block, c->stringId));
            } else {
                f_list_addLast(t->componentsAll, (void*)c);
                f_bitfield_set(t->componentsBits, c->bitId);
            }

            f_list_addLast(t->componentsOwn, (void*)c);

            t->data[c->bitId] = f_component__dataInit(c, b);
        }
    } else if(t->parent) {
        t->block = t->parent->block;
    }

    f_hash_add(g_templates, f_str_dup(Id), t);

    return t;
}

static void templateFree(FTemplate* Template)
{
    F_LIST_ITERATE(Template->componentsOwn, const FComponent*, c) {
        f_component__dataFree(c, Template->data[c->bitId]);
    }

    f_list_free(Template->componentsOwn);
    f_list_free(Template->componentsAll);
    f_bitfield_free(Template->componentsBits);

    f_mem_free(Template);
}

void f_template__init(void)
{
    g_templates = f_hash_newStr(256, true);
}

void f_template__uninit(void)
{
    f_hash_freeEx(g_templates, (FCallFree*)templateFree);
}

static void process_file(const char* FilePath, FList* Blocks)
{
    FBlock* root = f_block_new(FilePath);

    F_LIST_ITERATE(f_block_blocksGet(root), FBlock*, b) {
        f_list_addLast(Blocks, b);
        f_block__refInc(b);
    }

    f_block_free(root);
}

static void process_dir(const char* Path, FList* Blocks)
{
    FDir* dir = f_dir_new(Path);

    F_LIST_ITERATE(f_dir_entriesGet(dir), const FPath*, p) {
        const char* path = f_path_getFull(p);

        if(f_path_test(p, F_PATH_DIR)) {
            process_dir(path, Blocks);
        } else if(f_str_endsWith(f_path_getName(p), ".txt")) {
            process_file(path, Blocks);
        }
    }

    f_dir_free(dir);
}

static int cmp_blocks(const FBlock* A, const FBlock* B)
{
    return strcmp(f_block_lineGetString(A, 0), f_block_lineGetString(B, 0));
}

void f_template_load(const char* Dir)
{
    FList* blocks = f_list_new();

    process_dir(Dir, blocks);
    f_list_sort(blocks, (FCallListCompare*)cmp_blocks);

    F_LIST_ITERATE(blocks, const FBlock*, b) {
        templateNew(f_block_lineGetString(b, 0), b);
    }

    f_list_freeEx(blocks, (FCallFree*)f_block_free);
}

const FTemplate* f_template__get(const char* Id)
{
    FTemplate* t = f_hash_get(g_templates, Id);

    if(t == NULL) {
        F__FATAL("Unknown template '%s'", Id);
    }

    t->iNumber++;

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
