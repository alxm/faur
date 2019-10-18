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

#if !F_CONFIG_SYSTEM_GAMEBUINO
#include <dirent.h>
#include <sys/stat.h>

struct ADir {
    APath* path;
    AList* files; // list of APath
};

static int dirSort(const APath* A, const APath* B)
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

static AList* dirReal(APath* Path)
{
    const char* path = f_path_getFull(Path);
    DIR* dir = opendir(path);

    if(dir == NULL) {
        #if F_CONFIG_SYSTEM_LINUX
            int result = mkdir(path, S_IRWXU);
        #else
            int result = mkdir(path);
        #endif

        if(result == -1) {
            f_out__error("f_dir_new: mkdir(%s) failed", path);

            return NULL;
        }

        dir = opendir(path);

        if(dir != NULL) {
            f_path__flagsSet(Path, F_PATH_DIR | F_PATH_REAL);
        }
    }

    if(dir == NULL) {
        f_out__error("f_dir_new: opendir(%s) failed", path);

        return NULL;
    }

    AList* files = f_list_new();

    for(struct dirent* ent = readdir(dir); ent; ent = readdir(dir)) {
        if(ent->d_name[0] != '.') {
            f_list_addLast(files, f_path_newf("%s/%s", path, ent->d_name));
        }
    }

    f_list_sort(files, (AListCompare*)dirSort);

    closedir(dir);

    return files;
}

static AList* dirEmbedded(APath* Path)
{
    const char* path = f_path_getFull(Path);
    const AEmbeddedDir* data = f_embed__dirGet(path);

    AList* files = f_list_new();

    for(size_t e = data->size; e--; ) {
        f_list_addLast(files, f_path_newf("%s/%s", path, data->entries[e]));
    }

    return files;
}

ADir* f_dir_new(const char* Path)
{
    AList* files = NULL;
    APath* path = f_path_new(Path);

    if(f_path_test(path, F_PATH_DIR | F_PATH_REAL)) {
        files = dirReal(path);
    } else if(f_path_test(path, F_PATH_DIR | F_PATH_EMBEDDED)) {
        files = dirEmbedded(path);
    } else {
        files = dirReal(path);
    }

    if(files == NULL) {
        F__FATAL("f_dir_new(%s): Cannot open dir", Path);
    }

    ADir* d = f_mem_malloc(sizeof(ADir));

    d->path = path;
    d->files = files;

    return d;
}

void f_dir_free(ADir* Dir)
{
    if(Dir == NULL) {
        return;
    }

    f_list_freeEx(Dir->files, (AFree*)f_path_free);
    f_path_free(Dir->path);

    f_mem_free(Dir);
}

const APath* f_dir_pathGet(const ADir* Dir)
{
    return Dir->path;
}

const AList* f_dir_entriesGet(const ADir* Dir)
{
    return Dir->files;
}
#endif // !F_CONFIG_SYSTEM_GAMEBUINO
