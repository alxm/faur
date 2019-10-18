/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_DATA_LIST_P_H
#define F_INC_DATA_LIST_P_H

#include "general/f_system_includes.h"

typedef struct AList AList;
typedef struct AListNode AListNode;
typedef int AListCompare(void* ItemA, void* ItemB);

extern AList* f_list_new(void);
extern void f_list_free(AList* List);
extern void f_list_freeEx(AList* List, AFree* Free);

extern AListNode* f_list_addFirst(AList* List, void* Content);
extern AListNode* f_list_addLast(AList* List, void* Content);

extern void f_list_appendMove(AList* Dst, AList* Src);
extern void f_list_appendCopy(AList* Dst, const AList* Src);

extern void* f_list_getByIndex(const AList* List, unsigned Index);
extern void* f_list_getFirst(const AList* List);
extern void* f_list_getLast(const AList* List);
extern void* f_list_getRandom(const AList* List);
extern void* f_list_getNodeContent(const AListNode* Node);

extern void* f_list_removeItem(AList* List, const void* Item);
extern void* f_list_removeFirst(AList* List);
extern void* f_list_removeLast(AList* List);
extern void* f_list_removeByIndex(AList* List, unsigned Index);
extern void* f_list_removeRandom(AList* List);
extern void f_list_removeNode(AListNode* Node);

extern void f_list_clear(AList* List);
extern void f_list_clearEx(AList* List, AFree* Free);

extern AList* f_list_dup(const AList* List);
extern void** f_list_toArray(const AList* List);

extern void f_list_reverse(AList* List);
extern void f_list_sort(AList* List, AListCompare* Compare);

extern unsigned f_list_sizeGet(const AList* List);
extern bool f_list_isEmpty(const AList* List);
extern bool f_list_contains(const AList* List, const void* Item);

static inline AListNode* f_list_push(AList* List, void* Content)
{
    return f_list_addFirst(List, Content);
}

static inline void* f_list_pop(AList* List)
{
    return f_list_removeFirst(List);
}

static inline void* f_list_peek(const AList* List)
{
    return f_list_getFirst(List);
}

#endif // F_INC_DATA_LIST_P_H
