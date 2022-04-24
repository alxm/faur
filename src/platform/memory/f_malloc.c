/*
    Copyright 2022 Alex Margarit <alex@alxm.org>
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

#include "f_malloc.v.h"
#include <faur.v.h>

#if F_CONFIG_SYSTEM_ODROID_GO
    extern void *ps_malloc(size_t size);
    extern void *ps_calloc(size_t n, size_t size);
#endif

void* f_platform_api__malloc(size_t Size)
{
    #if F_CONFIG_SYSTEM_ODROID_GO
        return ps_malloc(Size);
    #else
        return malloc(Size);
    #endif
}

void* f_platform_api__mallocz(size_t Size)
{
    #if F_CONFIG_SYSTEM_ODROID_GO
        return ps_calloc(1, Size);
    #else
        return calloc(1, Size);
    #endif
}
