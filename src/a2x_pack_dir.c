/*
    Copyright 2011, 2016-2018 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_system_includes.h"
#include "a2x_pack_dir.v.h"

#include <dirent.h>
#include <sys/stat.h>

#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

struct ADir {
    char* path;
    char* name;
    AList* files; // list of ADirEntry
};

struct ADirEntry {
    char* name;
    char* full;
    bool isDir;
};

static int a_dir__sort(const ADirEntry* A, const ADirEntry* B)
{
    const char* nameA = A->name;
    const char* nameB = B->name;
    int a = *nameA;
    int b = *nameB;
    int lowerCaseComp = 0;

    while(a != '\0' && b != '\0') {
        if(a != b) {
            int lower_a = tolower(a);
            int lower_b = tolower(b);

            if(lower_a == lower_b) {
                if(lowerCaseComp == 0) {
                    lowerCaseComp = b - a;
                }
            } else {
                a = lower_a;
                b = lower_b;
                break;
            }
        }

        a = *++nameA;
        b = *++nameB;
    }

    if(a == '\0' && b == '\0' && lowerCaseComp != 0) {
        return lowerCaseComp;
    } else {
        return a - b;
    }
}

ADir* a_dir_new(const char* Path)
{
    DIR* dir = opendir(Path);

    if(dir == NULL) {
        a_out__error("opendir(%s) failed", Path);
        return NULL;
    }

    struct dirent* ent;
    AList* files = a_list_new();

    while((ent = readdir(dir)) != NULL) {
        ADirEntry* e = a_mem_malloc(sizeof(ADirEntry));

        e->name = a_str_dup(ent->d_name);
        e->full = a_str_merge(Path, "/", e->name, NULL);
        e->isDir = a_dir_exists(e->full);

        a_list_addLast(files, e);
    }

    closedir(dir);

    ADir* d = a_mem_malloc(sizeof(ADir));

    d->path = a_str_dup(Path);
    d->name = a_str_suffixGetFromLast(Path, '/');
    d->files = files;

    if(d->name == NULL) {
        d->name = a_str_dup(Path);
    }

    a_list_sort(files, (AListCompare*)a_dir__sort);

    return d;
}

void a_dir_free(ADir* Dir)
{
    if(Dir == NULL) {
        return;
    }

    free(Dir->path);
    free(Dir->name);

    A_LIST_ITERATE(Dir->files, ADirEntry*, e) {
        free(e->name);
        free(e->full);
        free(e);
    }

    a_list_free(Dir->files);
    free(Dir);
}

AList* a_dir_entriesListGet(const ADir* Dir)
{
    return Dir->files;
}

unsigned a_dir_entriesNumGet(const ADir* Dir)
{
    return a_list_sizeGet(Dir->files);
}

const char* a_dir_entryNameGet(const ADirEntry* Entry)
{
    return Entry->name;
}

const char* a_dir_entryPathGet(const ADirEntry* Entry)
{
    return Entry->full;
}

bool a_dir_entryIsDir(const ADirEntry* Entry)
{
    return Entry->isDir;
}

const char* a_dir_pathGet(const ADir* Dir)
{
    return Dir->path;
}

const char* a_dir_nameGet(const ADir* Dir)
{
    return Dir->name;
}

bool a_dir_exists(const char* Path)
{
    struct stat info;

    if(stat(Path, &info) != 0) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

bool a_dir_make(const char* Path)
{
    #if A_PLATFORM_SYSTEM_LINUX
        int result = mkdir(Path, S_IRWXU);
    #else
        int result = mkdir(Path);
    #endif

    if(result == -1) {
        a_out__error("mkdir(%s) failed", Path);
        return false;
    }

    return true;
}
