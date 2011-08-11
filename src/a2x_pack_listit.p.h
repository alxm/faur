/*
    Copyright 2010 Alex Margarit

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

#ifndef A2X_PACK_LISTIT_PH
#define A2X_PACK_LISTIT_PH

#include "a2x_app_includes.h"

typedef struct ListIt ListIt;

#include "a2x_pack_list.p.h"

extern ListIt* a_listit_new(List* const list);
extern void a_listit_free(ListIt* const it);
extern bool a_listit_next(const ListIt* const it);
extern void* a_listit_get(ListIt* const it);

#define a_listit_remove(it) a_listit__remove(it, 0)
#define a_listit_removeContent(it) a_listit__remove(it, 1)
extern void a_listit__remove(ListIt* const it, const int freeContent);

#define ListIterate(list, type, var)                                                   \
    for(ListIt* a__it = a_listit_new(list); a__it; a_listit_free(a__it), a__it = NULL) \
        for(type* var; a_listit_next(a__it) && (var = a_listit_get(a__it), true); )

#endif // A2X_PACK_LISTIT_PH
