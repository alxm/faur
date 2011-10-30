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

struct ListIt {
    List* list;
    ListNode* current;
};

extern ListIt a_listit__new(List* list);

extern bool a_listit__next(const ListIt* it);
extern void* a_listit__get(ListIt* it);
extern void a_listit__remove(ListIt* it, bool freeContent);

#define A_LIST_ITERATE(list, type, var)                                                 \
    for(ListIt a__it = a_listit__new(list); a__it.current; a__it.current = NULL)        \
        for(type* var; a_listit__next(&a__it) && (var = a_listit__get(&a__it), true); )

#define A_LIST_REMOVE(freeContent) a_listit__remove(&a__it, freeContent)

#endif // A2X_PACK_LISTIT_PH
