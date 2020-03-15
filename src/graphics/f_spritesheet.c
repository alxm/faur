/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#include "f_spritesheet.v.h"
#include <faur.v.h>

FSpriteSheet* f_spritesheet_new(const char* Path)
{
    FBlock* root = f_block_new(Path);
    FPath* rootPath = f_path_new(Path);
    const char* rootDirs = f_path_getDirs(rootPath);
    const FList* blocks = f_block_blocksGet(root);
    unsigned num = 0;

    F_LIST_ITERATE(blocks, const FBlock*, b) {
        num += f_list_sizeGet(f_block_blocksGet(b));
    }

    FHash* t = f_hash_newStr(f_math_minu(num, 256), true);

    F_LIST_ITERATE(blocks, const FBlock*, b) {
        char path[256];
        const char* name = f_block_lineGetString(b, 0);

        if(!f_str_fmt(path, sizeof(path), false, "%s/%s", rootDirs, name)) {
            F__FATAL("f_spritesheet_new(%s): f_str_fmt failed", Path);
        }

        FSprite* sheet = f_sprite_newFromPng(path, 0, 0, -1, -1);
        FVecInt fixedDim = {0, 0};

        F_LIST_ITERATE(f_block_blocksGet(b), const FBlock*, b) {
            const char* id = f_block_lineGetString(b, 0);

            if(id[0] == '*') {
                fixedDim = f_block_lineGetCoords(b, 1);

                continue;
            }

            FVecInt coords = f_block_lineGetCoords(b, 1);
            FVecInt dim = fixedDim.x > 0
                            ? fixedDim : f_block_lineGetCoords(b, 2);

            FSprite* sprite = f_sprite_newFromSprite(
                                sheet, coords.x, coords.y, dim.x, dim.y);

            f_hash_add(t, f_str_dup(id), sprite);
        }

        f_sprite_free(sheet);
    }

    f_block_free(root);
    f_path_free(rootPath);

    return t;
}

void f_spritesheet_free(FSpriteSheet* Sheet)
{
    f_hash_freeEx(Sheet, (FFree*)f_sprite_free);
}

const FSprite* f_spritesheet_get(const FSpriteSheet* Sheet, const char* Id)
{
    return f_hash_get(Sheet, Id);
}
