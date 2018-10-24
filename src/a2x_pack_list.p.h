/*
    Copyright 2010, 2016-2018 Alex Margarit

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

typedef struct AList AList;
typedef struct AListNode AListNode;
typedef int AListCompare(void* ItemA, void* ItemB);

extern AList* a_list_new(void);
extern void a_list_free(AList* List);
extern void a_list_freeEx(AList* List, AFree* Free);

extern AListNode* a_list_addFirst(AList* List, void* Content);
extern AListNode* a_list_addLast(AList* List, void* Content);

extern void a_list_appendMove(AList* Dst, AList* Src);
extern void a_list_appendCopy(AList* Dst, const AList* Src);

extern void* a_list_getByIndex(const AList* List, unsigned Index);
extern void* a_list_getFirst(const AList* List);
extern void* a_list_getLast(const AList* List);
extern void* a_list_getRandom(const AList* List);
extern void* a_list_getNodeContent(const AListNode* Node);

extern void a_list_removeItem(AList* List, const void* Item);
extern void* a_list_removeFirst(AList* List);
extern void* a_list_removeLast(AList* List);
extern void a_list_removeNode(AListNode* Node);

extern void a_list_clear(AList* List);
extern void a_list_clearEx(AList* List, AFree* Free);

extern AList* a_list_dup(const AList* List);
extern void** a_list_toArray(const AList* List);

extern void a_list_reverse(AList* List);
extern void a_list_sort(AList* List, AListCompare* Compare);

extern unsigned a_list_sizeGet(const AList* List);
extern bool a_list_isEmpty(const AList* List);

static inline AListNode* a_list_push(AList* List, void* Content)
{
    return a_list_addFirst(List, Content);
}

static inline void* a_list_pop(AList* List)
{
    return a_list_removeFirst(List);
}

static inline void* a_list_peek(const AList* List)
{
    return a_list_getFirst(List);
}
