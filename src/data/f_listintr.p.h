/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_LISTINTR_P_H
#define F_INC_DATA_LISTINTR_P_H

#include "../general/f_system_includes.h"

typedef struct FListIntrNode {
    struct FListIntrNode* prev;
    struct FListIntrNode* next;
} FListIntrNode;

typedef struct {
    FListIntrNode root;
    size_t itemNodeOffset;
} FListIntr;

typedef struct {
    const FListIntr* list;
    const FListIntrNode* next;
    unsigned index;
} F__ListIntrIt;

#define F_LISTINTR_NEW(Self, Type, NodeField)        \
    {                                                \
        .root = {&Self.root, &Self.root},            \
        .itemNodeOffset = offsetof(Type, NodeField), \
    }

extern void f_listintr_init(FListIntr* List, size_t NodeOffset);
extern void f_listintr_initNode(FListIntrNode* Node);

extern void f_listintr_clear(FListIntr* List);

extern void f_listintr_addFirst(FListIntr* List, void* Item);
extern void f_listintr_addLast(FListIntr* List, void* Item);

extern void* f_listintr_getFirst(const FListIntr* List);

extern void* f_listintr_removeFirst(FListIntr* List);
extern void f_listintr_removeItem(FListIntr* List, void* Item);
extern void f_listintr_removeNode(FListIntrNode* Node);

extern bool f_listintr_sizeIsEmpty(const FListIntr* List);
extern bool f_listintr_nodeIsLinked(const FListIntrNode* Node);

static inline void f_listintr_push(FListIntr* List, void* Content)
{
    f_listintr_addFirst(List, Content);
}

static inline void* f_listintr_pop(FListIntr* List)
{
    return f_listintr_removeFirst(List);
}

static inline void* f_listintr_peek(const FListIntr* List)
{
    return f_listintr_getFirst(List);
}

extern F__ListIntrIt f__listintrit_new(const FListIntr* List);
extern bool f__listintrit_getNext(F__ListIntrIt* Iterator, void* UserPtrAddress);

#define F_LISTINTR_ITERATE(List, PtrType, Name)                          \
    for(F__ListIntrIt f__it = f__listintrit_new(List);                   \
        f__it.list != NULL;                                              \
        f__it.list = NULL)                                               \
        for(PtrType Name; f__listintrit_getNext(&f__it, (void*)&Name); )

#define F_LISTINTR_INDEX() f__it.index
#define F_LISTINTR_REMOVE_CURRENT() f_listintr_removeNode(f__it.current)

#endif // F_INC_DATA_LISTINTR_P_H
