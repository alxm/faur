/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "../general/f_system_includes.h"

typedef struct FList FList;
typedef struct FListNode FListNode;
typedef int FListCompare(const void* A, const void* B);

extern FList* f_list_new(void);
extern void f_list_free(FList* List);
extern void f_list_freeEx(FList* List, FFree* Free);

extern FListNode* f_list_addFirst(FList* List, void* Content);
extern FListNode* f_list_addLast(FList* List, void* Content);

extern void f_list_appendMove(FList* Dst, FList* Src);
extern void f_list_appendCopy(FList* Dst, const FList* Src);

extern void* f_list_getByIndex(const FList* List, unsigned Index);
extern void* f_list_getByNode(const FListNode* Node);
extern void* f_list_getFirst(const FList* List);
extern void* f_list_getLast(const FList* List);
extern void* f_list_getRandom(const FList* List);

extern void* f_list_removeItem(FList* List, const void* Item);
extern void* f_list_removeFirst(FList* List);
extern void* f_list_removeLast(FList* List);
extern void* f_list_removeByIndex(FList* List, unsigned Index);
extern void* f_list_removeRandom(FList* List);
extern void f_list_removeNode(FListNode* Node);

extern void f_list_clear(FList* List);
extern void f_list_clearEx(FList* List, FFree* Free);

extern FList* f_list_dup(const FList* List);
extern void** f_list_toArray(const FList* List);

extern void f_list_reverse(FList* List);
extern void f_list_sort(FList* List, FListCompare* Compare);

extern unsigned f_list_sizeGet(const FList* List);
extern bool f_list_sizeIsEmpty(const FList* List);

extern bool f_list_contains(const FList* List, const void* Item);

static inline FListNode* f_list_push(FList* List, void* Content)
{
    return f_list_addFirst(List, Content);
}

static inline void* f_list_pop(FList* List)
{
    return f_list_removeFirst(List);
}

static inline void* f_list_peek(const FList* List)
{
    return f_list_getFirst(List);
}

#endif // F_INC_DATA_LIST_P_H
