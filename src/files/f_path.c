/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

static inline bool getPathInfo(const char* Path, FPathInfo* Info)
{
    return f_platform_api__fileStat(Path, Info) || f_embed__stat(Path, Info);
}

FPath* f_path_new(const char* Path)
{
    FPath* p = f_pool__alloc(F_POOL__PATH);

    if(!getPathInfo(Path, &p->info)) {
        p->info.flags = 0;
        p->info.size = 0;
    }

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
    f_pool_release(Path);
}

bool f_path_exists(const char* Path, FPathFlags Flags)
{
    FPathInfo info;

    return getPathInfo(Path, &info) && F_FLAGS_TEST_ALL(info.flags, Flags);
}

bool f_path_test(const FPath* Path, FPathFlags Flags)
{
    return F_FLAGS_TEST_ALL(Path->info.flags, Flags);
}

size_t f_path__sizeGet(const FPath* Path)
{
    return Path->info.size;
}

void f_path__flagsSet(FPath* Path, FPathFlags Flags)
{
    F_FLAGS_SET(Path->info.flags, Flags);
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
