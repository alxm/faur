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

#ifndef F_INC_PLATFORM_MEMORY_ODROIDGO_MALLOC_V_H
#define F_INC_PLATFORM_MEMORY_ODROIDGO_MALLOC_V_H

#include "f_odroidgo_malloc.p.h"

#include "../f_platform.v.h"

extern FCallApi_Malloc f_platform_api_odroidgo__malloc;
extern FCallApi_Mallocz f_platform_api_odroidgo__mallocz;

#endif // F_INC_PLATFORM_MEMORY_ODROIDGO_MALLOC_V_H
