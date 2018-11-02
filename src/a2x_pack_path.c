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
    char* full;
    char* dirsPart;
    char* namePart;
};

APath* a_path_new(const char* Path)
{
    APath* p = a_mem_malloc(sizeof(APath));

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

    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    if(bytesNeeded <= 0) {
        return NULL;
    }

    va_end(args);
    va_start(args, Format);

    char* buffer = a_mem_malloc((size_t)bytesNeeded);
    bytesNeeded = vsnprintf(buffer, (size_t)bytesNeeded, Format, args);

    va_end(args);

    if(bytesNeeded <= 0) {
        return NULL;
    }

    return a_path_new(buffer);
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
    struct stat info;

    switch(Type) {
        case A_PATH_TYPE_ANY:
            return access(Path, F_OK) == 0;

        case A_PATH_TYPE_FILE:
            return stat(Path, &info) == 0 && S_ISREG(info.st_mode);

        case A_PATH_TYPE_DIR:
            return stat(Path, &info) == 0 && S_ISDIR(info.st_mode);

        default:
            return false;
    }
}

bool a_path_test(const APath* Path, APathType Type)
{
    return a_path_exists(Path->full, Type);
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
