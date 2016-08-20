/*
    Copyright 2010 Alex Margarit

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

#include "a2x_pack_dir.v.h"

struct ADir {
    char* path;
    char* name;
    AList* files;
    unsigned int num;
    AListNode* node;
};

static AList* g_openedDirs;
static void a_dir__close(ADir* Dir);

void a_dir__init(void)
{
    g_openedDirs = a_list_new();
}

void a_dir__uninit(void)
{
    A_LIST_ITERATE(g_openedDirs, ADir*, d) {
        a_out__warning("You should close %s with a_dir_close", d->path);
        a_dir__close(d);
    }

    a_list_free(g_openedDirs);
}

ADir* a_dir_open(const char* Path)
{
    struct dirent** dlist = NULL;
    const int numFiles = scandir(Path, &dlist, NULL, alphasort);

    if(numFiles == -1) {
        a_out__fatal("a_dir_open(%s): scandir failed", Path);
    }

    ADir* d = a_mem_malloc(sizeof(ADir));

    d->path = a_str_dup(Path);
    d->name = a_str_getSuffixLastFind(Path, '/');
    d->files = a_list_new();
    d->num = a_math_max(0, numFiles);

    if(d->name == NULL) {
        d->name = a_str_dup(Path);
    }

    for(unsigned int i = d->num; i--; ) {
        ADirEntry* e = a_mem_malloc(sizeof(ADirEntry));

        e->name = a_str_dup(dlist[i]->d_name);
        e->full = a_str_merge(Path, "/", e->name);

        a_list_addFirst(d->files, e);
        free(dlist[i]);
    }

    free(dlist);

    d->node = a_list_addLast(g_openedDirs, d);
    return d;
}

void a_dir_close(ADir* Dir)
{
    a_list_removeNode(Dir->node);
    a_dir__close(Dir);
}

void a_dir__close(ADir* Dir)
{
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

AList* a_dir__files(const ADir* Dir)
{
    return Dir->files;
}

const char* a_dir_path(const ADir* Dir)
{
    return Dir->path;
}

const char* a_dir_name(const ADir* Dir)
{
    return Dir->name;
}

unsigned int a_dir_numEntries(const ADir* Dir)
{
    return Dir->num;
}

bool a_dir_exists(const char* Path)
{
    DIR* const d = opendir(Path);

    if(d) {
        closedir(d);
        return true;
    }

    return false;
}

void a_dir_make(const char* Path)
{
    if(mkdir(Path, S_IRWXU) == -1) {
        a_out__error("mkdir(%s) failed", Path);
    }
}
