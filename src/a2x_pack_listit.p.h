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

#pragma once

#include "a2x_system_includes.h"

typedef struct AListIt AListIt;

#include "a2x_pack_list.p.h"

struct AListIt {
    AList* list;
    AListNode* currentNode;
    void* currentItem;
};

extern AListIt a_listit__new(AList* List);

extern bool a_listit__getNext(AListIt* Iterator);
extern void a_listit__remove(AListIt* Iterator);

#define A_LIST_ITERATE(List, Item)                                       \
    for(AListIt a__it = a_listit__new(List);                             \
        a_listit__getNext(&a__it) && (Item = a__it.currentItem, true); )

#define A_LIST_FILTER(List, Item, Filter) \
    A_LIST_ITERATE(List, Item)            \
        if(!(Filter)) continue;           \
        else

#define A_LIST_REMOVE_CURRENT() a_listit__remove(&a__it)
