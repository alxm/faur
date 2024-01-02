/*
    Copyright 2011 Alex Margarit <alex@alxm.org>
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

#include "f_standard_dir.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_STDLIB_FILES
#include <dirent.h>
#include <sys/stat.h>

static int dirSort(const FPath* A, const FPath* B)
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

bool f_platform_api_standard__dirCreate(const char* Path)
{
    int ret;

    #if F_CONFIG_SYSTEM_MINGW64
        ret = mkdir(Path);
    #else
        ret = mkdir(Path, S_IRWXU);
    #endif

    return ret == 0;
}

FList* f_platform_api_standard__dirOpen(FPath* Path)
{
    const char* path = f_path_getFull(Path);
    DIR* dir = opendir(path);

    if(dir == NULL) {
        f_out__error("opendir(%s) failed", path);

        return NULL;
    }

    FList* files = f_list_new();

    for(struct dirent* ent = readdir(dir); ent; ent = readdir(dir)) {
        if(ent->d_name[0] != '.') {
            f_list_addLast(files, f_path_newf("%s/%s", path, ent->d_name));
        }
    }

    f_list_sort(files, (FCallListCompare*)dirSort);

    closedir(dir);

    return files;
}
#endif // F_CONFIG_LIB_STDLIB_FILES
