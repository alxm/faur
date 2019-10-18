/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "f_path.v.h"
#include <faur.v.h>
#include <sys/stat.h>

struct FPath {
    FPathFlags flags;
    char* full;
    char* dirsPart;
    char* namePart;
};

static FPathFlags getPathFlags(const char* Path)
{
    struct stat info;
    FPathFlags flags = 0;

    if(stat(Path, &info) == 0) {
        F_FLAGS_SET(flags, F_PATH_REAL);

        if(S_ISREG(info.st_mode)) {
            F_FLAGS_SET(flags, F_PATH_FILE);
        } else if(S_ISDIR(info.st_mode)) {
            F_FLAGS_SET(flags, F_PATH_DIR);
        } else {
            F_FLAGS_SET(flags, F_PATH_OTHER);
        }
    } else if(f_embed__fileGet(Path) != NULL) {
        F_FLAGS_SET(flags, F_PATH_EMBEDDED | F_PATH_FILE);
    } else if(f_embed__dirGet(Path) != NULL) {
        F_FLAGS_SET(flags, F_PATH_EMBEDDED | F_PATH_DIR);
    }

    return flags;
}

FPath* f_path_new(const char* Path)
{
    FPath* p = f_mem_malloc(sizeof(FPath));

    p->flags = getPathFlags(Path);
    p->full = f_str_dup(Path);
    p->dirsPart = f_str_prefixGetToLast(Path, '/');
    p->namePart = f_str_suffixGetFromLast(Path, '/');

    if(p->dirsPart == NULL) {
        p->dirsPart = f_str_dup(".");
    }

    if(p->namePart == NULL) {
        p->namePart = f_str_dup(Path);
    }

    return p;
}

FPath* f_path_newf(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    va_end(args);

    if(bytesNeeded <= 1) {
        F__FATAL("f_path_newf(%s): vsnprintf failed", Format);
    }

    char* buffer = f_mem_malloc((size_t)bytesNeeded);

    va_start(args, Format);
    vsnprintf(buffer, (size_t)bytesNeeded, Format, args);
    va_end(args);

    FPath* p = f_path_new(buffer);

    f_mem_free(buffer);

    return p;
}

void f_path_free(FPath* Path)
{
    f_mem_free(Path->full);
    f_mem_free(Path->dirsPart);
    f_mem_free(Path->namePart);
    f_mem_free(Path);
}

bool f_path_exists(const char* Path, FPathFlags Flags)
{
    return F_FLAGS_TEST_ALL(getPathFlags(Path), Flags);
}

bool f_path_test(const FPath* Path, FPathFlags Flags)
{
    return F_FLAGS_TEST_ALL(Path->flags, Flags);
}

void f_path__flagsSet(FPath* Path, FPathFlags Flags)
{
    F_FLAGS_SET(Path->flags, Flags);
}

const char* f_path_getFull(const FPath* Path)
{
    return Path->full;
}

const char* f_path_getDirs(const FPath* Path)
{
    return Path->dirsPart;
}

const char* f_path_getName(const FPath* Path)
{
    return Path->namePart;
}
