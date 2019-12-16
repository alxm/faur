/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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
    FStrHash* t = f_strhash_new();

    FBlock* root = f_block_new(Path);
    FPath* rootPath = f_path_new(Path);
    const char* rootDirs = f_path_getDirs(rootPath);

    F_LIST_ITERATE(f_block_blocksGet(root), const FBlock*, b) {
        const char* name = f_block_lineGetString(b, 0);
        const char* path = f_str__fmt512("%s/%s", rootDirs, name);
        FSprite* sheet = f_sprite_newFromPng(path, 0, 0, -1, -1);

        F_LIST_ITERATE(f_block_blocksGet(b), const FBlock*, b) {
            const char* id = f_block_lineGetString(b, 0);
            FVectorInt coords = f_block_lineGetCoords(b, 1);
            FVectorInt dim = f_block_lineGetCoords(b, 2);

            FSprite* sprite = f_sprite_newFromSprite(
                                sheet, coords.x, coords.y, dim.x, dim.y);

            f_strhash_add(t, id, sprite);
        }

        f_sprite_free(sheet);
    }

    f_block_free(root);
    f_path_free(rootPath);

    return t;
}

void f_spritesheet_free(FSpriteSheet* Sheet)
{
    f_strhash_freeEx(Sheet, (FFree*)f_sprite_free);
}

const FSprite* f_spritesheet_get(const FSpriteSheet* Sheet, const char* Id)
{
    return f_strhash_get(Sheet, Id);
}
