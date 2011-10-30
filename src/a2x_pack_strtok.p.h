/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A2X_PACK_STRTOK_PH
#define A2X_PACK_STRTOK_PH

#include "a2x_app_includes.h"

typedef struct StringTok StringTok;

extern StringTok* a_strtok_new(const char* s, const char* d);
extern void a_strtok_free(StringTok* t);

extern bool a_strtok_next(StringTok* t);
extern char* a_strtok_get(StringTok* t);

#endif // A2X_PACK_STRTOK_PH
