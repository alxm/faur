/*
    Copyright 2011, 2016-2018 Alex Margarit

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
    AListNode* sentinelNode;
    AListNode* currentNode;
    void* currentItem;
    bool reversed;
};

extern AListIt a__listit_new(AList* List, bool Reversed);

extern bool a__listit_getNext(AListIt* Iterator);
extern void a__listit_remove(AListIt* Iterator);
extern bool a__listit_isFirst(AListIt* Iterator);
extern bool a__listit_isLast(AListIt* Iterator);

#define A_LIST_ITERATE(List, PtrType, Name)                                    \
    for(unsigned a__it_i = 0; a__it_i != UINT_MAX; a__it_i = UINT_MAX)         \
        for(PtrType Name = (PtrType)1; Name; Name = NULL)                      \
            for(AListIt a__it = a__listit_new(List, false);                    \
                a__listit_getNext(&a__it) && (Name = a__it.currentItem, true); \
                a__it_i++)

#define A_LIST_ITERATE_BACKWARDS(List, PtrType, Name)                          \
    for(unsigned a__it_i = a_list_sizeGet(List) - 1;                           \
        a__it_i != UINT_MAX;                                                   \
        a__it_i = UINT_MAX)                                                    \
        for(PtrType Name = (PtrType)1; Name; Name = NULL)                      \
            for(AListIt a__it = a__listit_new(List, true);                     \
                a__listit_getNext(&a__it) && (Name = a__it.currentItem, true); \
                a__it_i--)

#define A_LIST_FILTER(List, PtrType, Name, Filter) \
    A_LIST_ITERATE(List, PtrType, Name)            \
        if(!(Filter)) continue;                    \
        else

#define A_LIST_FILTER_BACKWARDS(List, PtrType, Name, Filter) \
    A_LIST_ITERATE_BACKWARDS(List, PtrType, Name)            \
        if(!(Filter)) continue;                              \
        else

#define A_LIST_INDEX() a__it_i
#define A_LIST_REMOVE_CURRENT() a__listit_remove(&a__it)
#define A_LIST_IS_FIRST() a__listit_isFirst(&a__it)
#define A_LIST_IS_LAST() a__listit_isLast(&a__it)
