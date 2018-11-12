/*
    Copyright 2018 Alex Margarit

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

#include "a2x_pack_path.v.h"

#include <sys/stat.h>

#include "a2x_pack_embed.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_str.v.h"

struct APath {
    APathFlags flags;
    char* full;
    char* dirsPart;
    char* namePart;
};

static APathFlags getPathFlags(const char* Path)
{
    struct stat info;
    APathFlags flags = 0;

    if(stat(Path, &info) == 0) {
        flags |= A_PATH_REAL;

        if(S_ISREG(info.st_mode)) {
            flags |= A_PATH_FILE;
        } else if(S_ISDIR(info.st_mode)) {
            flags |= A_PATH_DIR;
        } else {
            flags |= A_PATH_OTHER;
        }
    } else if(a_embed__getFile(Path, NULL, NULL)) {
        flags |= A_PATH_EMBEDDED;
        flags |= A_PATH_FILE;
    }

    return flags;
}

APath* a_path_new(const char* Path)
{
    APath* p = a_mem_malloc(sizeof(APath));

    p->flags = getPathFlags(Path);
    p->full = a_str_dup(Path);
    p->dirsPart = a_str_prefixGetToLast(Path, '/');
    p->namePart = a_str_suffixGetFromLast(Path, '/');

    if(p->dirsPart == NULL) {
        p->dirsPart = a_str_dup(".");
    }

    if(p->namePart == NULL) {
        p->namePart = a_str_dup(Path);
    }

    return p;
}

APath* a_path_newf(const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    APath* p = NULL;
    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    va_end(args);

    if(bytesNeeded > 0) {
        char* buffer = a_mem_malloc((size_t)bytesNeeded);

        va_start(args, Format);

        if(vsnprintf(buffer, (size_t)bytesNeeded, Format, args) > 0) {
            p = a_path_new(buffer);
        }

        va_end(args);

        free(buffer);
    }

    return p;
}

void a_path_free(APath* Path)
{
    free(Path->full);
    free(Path->dirsPart);
    free(Path->namePart);
    free(Path);
}

bool a_path_exists(const char* Path, APathFlags Flags)
{
    return (getPathFlags(Path) & Flags) == Flags;
}

bool a_path_test(const APath* Path, APathFlags Flags)
{
    return (Path->flags & Flags) == Flags;
}

const char* a_path_getFull(const APath* Path)
{
    return Path->full;
}

const char* a_path_getDirs(const APath* Path)
{
    return Path->dirsPart;
}

const char* a_path_getName(const APath* Path)
{
    return Path->namePart;
}
