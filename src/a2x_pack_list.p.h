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

#ifndef A2X_PACK_LIST_PH
#define A2X_PACK_LIST_PH

#include <stdlib.h>
#include <string.h>

typedef struct ListNode ListNode;
typedef struct List List;
typedef struct ListIterator ListIterator;

extern List* a_list_set(void);
extern void a_list_free(List* const list);
extern void a_list_freeContent(List* const list);

extern void a_list_empty(List* const list);
extern void a_list_emptyContent(List* const list);
extern ListNode* a_list_addFirst(List* const list, void* const content);
extern ListNode* a_list_addLast(List* const list, void* const content);
extern void a_list_reset(List* const list);
extern int a_list_iterate(List* const list);
extern void* a_list_peek(List* const list);
extern void* a_list_removeFirst(List* const list);
extern void a_list_removeFirstContent(List* const list);
extern void* a_list_removeLast(List* const list);
extern void a_list_removeLastContent(List* const list);
extern void a_list_removeCurrent(List* const list);
extern void a_list_removeCurrentContent(List* const list);
extern void a_list_removeNode(ListNode* const node);
extern void a_list_removeNodeContent(ListNode* const node);
extern void* a_list_getArray(List* const list, const int size);
extern void* a_list_first(const List* const list);
extern void* a_list_last(const List* const list);
extern void* a_list_current(const List* const list);
extern ListNode* a_list_currentNode(const List* const list);
extern int a_list_size(const List* const list);
extern int a_list_isEmpty(const List* const list);

#define a_list_push(l, c) a_list_addFirst(l, c)
#define a_list_pop(l)     a_list_removeFirst(l)

extern ListIterator* a_list_setIterator(List* const list);
extern void a_list_freeIterator(ListIterator* const it);
extern int a_list_iteratorNext(ListIterator* const it);
extern void* a_list_iteratorGet(const ListIterator* const it);

#endif // A2X_PACK_LIST_PH
