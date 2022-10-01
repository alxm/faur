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

#if F_CONFIG_FILES_STANDARD
#include <dirent.h>
#include <sys/stat.h>

bool f_platform_api_standard__dirCreate(const char* Path)
{
    int ret;

    #if F_CONFIG_SYSTEM_MINGW
        ret = mkdir(Path);
    #else
        ret = mkdir(Path, S_IRWXU);
    #endif

    return ret == 0;
}
#endif // F_CONFIG_FILES_STANDARD
