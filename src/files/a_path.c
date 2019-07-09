/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <a2x.v.h>
#include <sys/stat.h>

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
        A_FLAG_SET(flags, A_PATH_REAL);

        if(S_ISREG(info.st_mode)) {
            A_FLAG_SET(flags, A_PATH_FILE);
        } else if(S_ISDIR(info.st_mode)) {
            A_FLAG_SET(flags, A_PATH_DIR);
        } else {
            A_FLAG_SET(flags, A_PATH_OTHER);
        }
    } else if(a_embed__fileGet(Path) != NULL) {
        A_FLAG_SET(flags, A_PATH_EMBEDDED | A_PATH_FILE);
    } else if(a_embed__dirGet(Path) != NULL) {
        A_FLAG_SET(flags, A_PATH_EMBEDDED | A_PATH_DIR);
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

    int bytesNeeded = vsnprintf(NULL, 0, Format, args) + 1;

    va_end(args);

    if(bytesNeeded <= 1) {
        A__FATAL("a_path_newf(%s): vsnprintf failed", Format);
    }

    char* buffer = a_mem_malloc((size_t)bytesNeeded);

    va_start(args, Format);
    vsnprintf(buffer, (size_t)bytesNeeded, Format, args);
    va_end(args);

    APath* p = a_path_new(buffer);

    free(buffer);

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
    return A_FLAG_TEST_ALL(getPathFlags(Path), Flags);
}

bool a_path_test(const APath* Path, APathFlags Flags)
{
    return A_FLAG_TEST_ALL(Path->flags, Flags);
}

void a_path__flagsSet(APath* Path, APathFlags Flags)
{
    A_FLAG_SET(Path->flags, Flags);
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
