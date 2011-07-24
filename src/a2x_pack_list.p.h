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

#include "a2x_app_includes.h"

typedef struct List List;
typedef struct ListNode ListNode;

extern List* a_list_set(void);

#define a_list_free(list) a_list__free(list, 0)
#define a_list_freeContent(list) a_list__free(list, 1)
extern void a_list__free(List* const list, const int freeContent);

#define a_list_empty(list) a_list__empty(list, 0)
#define a_list_emptyContent(list) a_list__empty(list, 1)
extern void a_list__empty(List* const list, const int freeContent);

extern ListNode* a_list_addFirst(List* const list, void* const content);
extern ListNode* a_list_addLast(List* const list, void* const content);

extern void a_list_remove(List* const list, const void* const v);

#define a_list_removeFirst(list) a_list__removeFirst(list, 0)
#define a_list_removeFirstContent(list) a_list__removeFirst(list, 1)
extern void* a_list__removeFirst(List* const list, const int freeContent);

#define a_list_removeLast(list) a_list__removeLast(list, 0)
#define a_list_removeLastContent(list) a_list__removeLast(list, 1)
extern void* a_list__removeLast(List* const list, const int freeContent);

#define a_list_removeCurrent(list) a_list__removeCurrent(list, 0)
#define a_list_removeCurrentContent(list) a_list__removeCurrent(list, 1)
extern void a_list__removeCurrent(List* const list, const int freeContent);

#define a_list_removeNode(node) a_list__removeNode(node, 0)
#define a_list_removeNodeContent(node) a_list__removeNode(node, 1)
extern void a_list__removeNode(ListNode* const node, const int freeContent);

#define a_list_push(l, c) a_list_addFirst(l, c)
#define a_list_pop(l)     a_list_removeFirst(l)
#define a_list_peek(l)    a_list_first(l)

extern void a_list_reverse(List* const list);

extern bool a_list_iterate(List* const list);
extern void a_list_reset(List* const list);

#define a_list_iterateDone(list) \
({                               \
    a_list_reset(list);          \
    break;                       \
})

extern void** a_list_getArray(List* const list);

extern void* a_list_first(const List* const list);
extern void* a_list_last(const List* const list);
extern void* a_list_current(const List* const list);
extern ListNode* a_list_currentNode(const List* const list);

extern void* a_list_get(const List* const list, const int index);

extern int a_list_size(const List* const list);
extern bool a_list_isEmpty(const List* const list);

#endif // A2X_PACK_LIST_PH
