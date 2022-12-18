/*
    Copyright 2011 Alex Margarit <alex@alxm.org>
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

#include "f_dir.v.h"
#include <faur.v.h>

static FList* dirEmbeddedOpen(FPath* Path)
{
    const char* path = f_path_getFull(Path);
    const FEmbeddedDir* data = f_embed__dirGet(path);

    FList* files = f_list_new();

    for(size_t e = data->size; e--; ) {
        f_list_addLast(files, f_path_newf("%s/%s", path, data->entries[e]));
    }

    return files;
}

FDir* f_dir_new(const char* Path)
{
    F__CHECK(Path != NULL);

    if(!f_path_exists(Path, F_PATH_DIR) && !f_platform_api__dirCreate(Path)) {
        F__FATAL("f_dir_new(%s): Cannot create dir", Path);
    }

    FList* files = NULL;
    FPath* path = f_path_new(Path);

    if(f_path_test(path, F_PATH_DIR | F_PATH_REAL)) {
        files = f_platform_api__dirOpen(path);
    } else if(f_path_test(path, F_PATH_DIR | F_PATH_EMBEDDED)) {
        files = dirEmbeddedOpen(path);
    }

    if(files == NULL) {
        F__FATAL("f_dir_new(%s): Cannot open dir", Path);
    }

    FDir* d = f_mem_malloc(sizeof(FDir));

    d->path = path;
    d->files = files;

    return d;
}

void f_dir_free(FDir* Dir)
{
    if(Dir == NULL) {
        return;
    }

    f_list_freeEx(Dir->files, (FCallFree*)f_path_free);
    f_path_free(Dir->path);

    f_mem_free(Dir);
}

const FPath* f_dir_pathGet(const FDir* Dir)
{
    F__CHECK(Dir != NULL);

    return Dir->path;
}

const FList* f_dir_entriesGet(const FDir* Dir)
{
    F__CHECK(Dir != NULL);

    return Dir->files;
}

unsigned f_dir_entriesGetNum(const FDir* Dir)
{
    F__CHECK(Dir != NULL);

    return f_list_sizeGet(Dir->files);
}
