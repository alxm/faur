/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_LISTIT_P_H
#define F_INC_DATA_LISTIT_P_H

#include "general/f_system_includes.h"

typedef struct AListIt AListIt;

#include "data/f_list.p.h"

struct AListIt {
    const AListNode* sentinelNode;
    const AListNode* nextNode;
    unsigned index;
    bool reversed;
};

extern AListIt f__listit_new(const AList* List, bool Reversed);

extern bool f__listit_getNext(AListIt* Iterator, void* UserPtrAddress);
extern void f__listit_remove(const AListIt* Iterator);
extern bool f__listit_isFirst(const AListIt* Iterator);
extern bool f__listit_isLast(const AListIt* Iterator);

#define F_LIST_ITERATE(List, PtrType, Name)                          \
    for(AListIt f__it = f__listit_new(List, false);                  \
        f__it.sentinelNode != NULL;                                  \
        f__it.sentinelNode = NULL)                                   \
        for(PtrType Name; f__listit_getNext(&f__it, (void*)&Name); )

#define F_LIST_ITERATE_REV(List, PtrType, Name)                      \
    for(AListIt f__it = f__listit_new(List, true);                   \
        f__it.sentinelNode != NULL;                                  \
        f__it.sentinelNode = NULL)                                   \
        for(PtrType Name; f__listit_getNext(&f__it, (void*)&Name); )

#define F_LIST_FILTER(List, PtrType, Name, Filter) \
    F_LIST_ITERATE(List, PtrType, Name)            \
        if(!(Filter)) continue;                    \
        else

#define F_LIST_FILTER_REV(List, PtrType, Name, Filter) \
    F_LIST_ITERATE_REV(List, PtrType, Name)            \
        if(!(Filter)) continue;                        \
        else

#define F_LIST_INDEX() f__it.index
#define F_LIST_REMOVE_CURRENT() f__listit_remove(&f__it)
#define F_LIST_IS_FIRST() f__listit_isFirst(&f__it)
#define F_LIST_IS_LAST() f__listit_isLast(&f__it)

#endif // F_INC_DATA_LISTIT_P_H
