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

#include "a2x_pack_mem.v.h"
#include "a2x_pack_str.v.h"

struct APath {
    APathType type;
    char* full;
    char* dirsPart;
    char* namePart;
};

static APathType getPathType(const char* Path)
{
    struct stat info;

    if(stat(Path, &info) == 0) {
        if(S_ISREG(info.st_mode)) {
            return A_PATH_TYPE_FILE;
        } else if(S_ISDIR(info.st_mode)) {
            return A_PATH_TYPE_DIR;
        } else {
            return A_PATH_TYPE_OTHER;
        }
    }

    return A_PATH_TYPE_INVALID;
}

APath* a_path_new(const char* Path)
{
    APath* p = a_mem_malloc(sizeof(APath));

    p->type = getPathType(Path);
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

bool a_path_exists(const char* Path, APathType Type)
{
    return getPathType(Path) == Type;
}

bool a_path_test(const APath* Path, APathType Type)
{
    return Path->type == Type;
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
