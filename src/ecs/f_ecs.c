/*
    Copyright 2016-2021 Alex Margarit <alex@alxm.org>
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

#include "f_ecs.v.h"
#include <faur.v.h>

static void f_ecs__init(void)
{
    f_ecs__populate();
}

static void f_ecs__uninit(void)
{
    f_entity__uninit();
    f_template__uninit();
    f_system__uninit();
    f_component__uninit();
}

const FPack f_pack__ecs = {
    "ECS",
    f_ecs__init,
    f_ecs__uninit,
};

void f_ecs__set(FComponent* const* Components, size_t ComponentsNum, FSystem* const* Systems, size_t SystemsNum)
{
    f_component__init(Components, ComponentsNum);
    f_system__init(Systems, SystemsNum);
    f_template__init();
    f_entity__init();
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

void f_ecs_init(void)
{
    FList* blocks = f_list_new();

    process_dir(F_CONFIG_DIR_MEDIA_ECS, blocks);
    f_list_sort(blocks, (FCallListCompare*)cmp_blocks);

    F_LIST_ITERATE(blocks, const FBlock*, b) {
        f_template__new(f_block_lineGetString(b, 0), b);
    }

    f_list_freeEx(blocks, (FCallFree*)f_block_free);
}
