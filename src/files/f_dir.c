/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_FILES_STANDARD
#include <dirent.h>
#include <sys/stat.h>

struct FDir {
    FPath* path;
    FList* files; // list of FPath
};

static int dirSort(const FPath* A, const FPath* B)
{
    const char* nameA = f_path_getName(A);
    const char* nameB = f_path_getName(B);
    int a = *nameA;
    int b = *nameB;
    int lowerCaseUpperCaseCmp = 0;

    while(a != '\0' && b != '\0') {
        if(a != b) {
            int lowerA = tolower(a);
            int lowerB = tolower(b);

            if(lowerA == lowerB) {
                if(lowerCaseUpperCaseCmp == 0) {
                    lowerCaseUpperCaseCmp = b - a;
                }
            } else {
                return lowerA - lowerB;
            }
        }

        a = *++nameA;
        b = *++nameB;
    }

    if(a == b) {
        return lowerCaseUpperCaseCmp;
    }

    return a - b;
}

static void dirRealCreate(const char* Path)
{
    #if F_CONFIG_SYSTEM_LINUX
        int ret = mkdir(Path, S_IRWXU);
    #else
        int ret = mkdir(Path);
    #endif

    if(ret != 0) {
        F__FATAL("mkdir(%s) failed", Path);
    }
}

static FList* dirRealOpen(FPath* Path)
{
    const char* path = f_path_getFull(Path);
    DIR* dir = opendir(path);

    if(dir == NULL) {
        F__FATAL("opendir(%s) failed", path);
    }

    FList* files = f_list_new();

    for(struct dirent* ent = readdir(dir); ent; ent = readdir(dir)) {
        if(ent->d_name[0] != '.') {
            f_list_addLast(files, f_path_newf("%s/%s", path, ent->d_name));
        }
    }

    f_list_sort(files, (FListCompare*)dirSort);

    closedir(dir);

    return files;
}

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
    if(!f_path_exists(Path, F_PATH_DIR)) {
        dirRealCreate(Path);
    }

    FList* files;
    FPath* path = f_path_new(Path);

    if(f_path_test(path, F_PATH_DIR | F_PATH_REAL)) {
        files = dirRealOpen(path);
    } else if(f_path_test(path, F_PATH_DIR | F_PATH_EMBEDDED)) {
        files = dirEmbeddedOpen(path);
    } else {
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

    f_list_freeEx(Dir->files, (FFree*)f_path_free);
    f_path_free(Dir->path);

    f_mem_free(Dir);
}

const FPath* f_dir_pathGet(const FDir* Dir)
{
    return Dir->path;
}

const FList* f_dir_entriesGet(const FDir* Dir)
{
    return Dir->files;
}
#endif // F_CONFIG_FILES_STANDARD
