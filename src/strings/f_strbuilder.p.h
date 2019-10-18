/*
    Copyright 2016-2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_STRINGS_STRBUILDER_P_H
#define A_INC_STRINGS_STRBUILDER_P_H

#include "general/a_system_includes.h"

typedef struct AStrBuilder AStrBuilder;

extern AStrBuilder* a_strbuilder_new(size_t Bytes);
extern void a_strbuilder_free(AStrBuilder* Builder);

extern const char* a_strbuilder_get(AStrBuilder* Builder);

extern bool a_strbuilder_add(AStrBuilder* Builder, const char* String);
extern bool a_strbuilder_addf(AStrBuilder* Builder, const char* Format, ...);

#endif // A_INC_STRINGS_STRBUILDER_P_H
