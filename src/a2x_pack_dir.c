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
    AListIt iterator;
    int num;
    const char* current[2];
};

typedef struct ADirEntry {
    char* name;
    char* full;
} ADirEntry;

static int defaultFilter(const struct dirent* f)
{
    return strlen(f->d_name) > 0 && f->d_name[0] != '.';
}

ADir* a_dir_open(const char* path)
{
    return a_dir_openFilter(path, defaultFilter);
}

ADir* a_dir_openFilter(const char* path, int (*filter)(const struct dirent* f))
{
    extern int scandir(
        const char *dirp, struct dirent ***namelist,
        int (*filter)(const struct dirent *),
        int (*compar)(const struct dirent **, const struct dirent **));

    extern int alphasort(const struct dirent **, const struct dirent **);

    struct dirent** dlist = NULL;
    const int numFiles = scandir(path, &dlist, filter, alphasort);

    ADir* const d = a_mem_malloc(sizeof(ADir));

    d->path = a_str_dup(path);
    d->name = a_str_getSuffixLastFind(path, '/');
    d->files = a_list_new();
    d->num = a_math_max(0, numFiles);

    for(int i = d->num; i--; ) {
        ADirEntry* const e = a_mem_malloc(sizeof(ADirEntry));

        e->name = a_str_dup(dlist[i]->d_name);
        e->full = a_str_merge(path, "/", e->name);

        a_list_addFirst(d->files, e);
        free(dlist[i]);
    }

    d->iterator = a_listit__new(d->files);

    free(dlist);

    return d;
}

void a_dir_close(ADir* d)
{
    free(d->path);
    free(d->name);

    A_LIST_ITERATE(d->files, ADirEntry, e) {
        free(e->name);
        free(e->full);
        free(e);
    }
    a_list_free(d->files);

    free(d);
}

void a_dir_reverse(ADir* d)
{
    a_list_reverse(d->files);
    d->iterator = a_listit__new(d->files);
}

const char** a_dir__next(ADir* d)
{
    if(a_listit__next(&d->iterator)) {
        ADirEntry* const e = a_listit__get(&d->iterator);

        d->current[A_DIR_NAME] = e->name;
        d->current[A_DIR_PATH] = e->full;

        return (const char**)d->current;
    }

    d->iterator = a_listit__new(d->files);
    return NULL;
}

void a_dir_reset(ADir* d)
{
    d->iterator = a_listit__new(d->files);
}

const char* a_dir_path(const ADir* d)
{
    return d->path;
}

const char* a_dir_name(const ADir* d)
{
    return d->name;
}

int a_dir_num(const ADir* d)
{
    return d->num;
}

bool a_dir_exists(const char* path)
{
    DIR* const d = opendir(path);

    if(d) {
        closedir(d);
        return true;
    }

    return false;
}

void a_dir_make(const char* path)
{
    mkdir(path, S_IRWXU);
}
