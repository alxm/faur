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
    const AListNode* sentinelNode;
    const AListNode* nextNode;
    unsigned index;
    bool reversed;
};

extern AListIt a__listit_new(const AList* List, bool Reversed);

extern bool a__listit_getNext(AListIt* Iterator, void* UserPtrAddress);
extern void a__listit_remove(const AListIt* Iterator);
extern bool a__listit_isFirst(const AListIt* Iterator);
extern bool a__listit_isLast(const AListIt* Iterator);

#define A_LIST_ITERATE(List, PtrType, Name)                          \
    for(AListIt a__it = a__listit_new(List, false);                  \
        a__it.sentinelNode != NULL;                                  \
        a__it.sentinelNode = NULL)                                   \
        for(PtrType Name; a__listit_getNext(&a__it, (void*)&Name); )

#define A_LIST_ITERATE_BACKWARDS(List, PtrType, Name)                \
    for(AListIt a__it = a__listit_new(List, true);                   \
        a__it.sentinelNode != NULL;                                  \
        a__it.sentinelNode = NULL)                                   \
        for(PtrType Name; a__listit_getNext(&a__it, (void*)&Name); )

#define A_LIST_FILTER(List, PtrType, Name, Filter) \
    A_LIST_ITERATE(List, PtrType, Name)            \
        if(!(Filter)) continue;                    \
        else

#define A_LIST_FILTER_BACKWARDS(List, PtrType, Name, Filter) \
    A_LIST_ITERATE_BACKWARDS(List, PtrType, Name)            \
        if(!(Filter)) continue;                              \
        else

#define A_LIST_INDEX() a__it.index
#define A_LIST_REMOVE_CURRENT() a__listit_remove(&a__it)
#define A_LIST_IS_FIRST() a__listit_isFirst(&a__it)
#define A_LIST_IS_LAST() a__listit_isLast(&a__it)
