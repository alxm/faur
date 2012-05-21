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

extern List* a_list_new(void);
extern void a_list_free(List* list);
extern void a_list_empty(List* list);

extern ListNode* a_list_addFirst(List* list, void* content);
extern ListNode* a_list_addLast(List* list, void* content);

extern void a_list_remove(List* list, const void* v);
extern void* a_list_removeFirst(List* list);
extern void* a_list_removeLast(List* list);
extern void a_list_removeNode(ListNode* node);

#define a_list_push(l, c) a_list_addFirst(l, c)
#define a_list_pop(l)     a_list_removeFirst(l)
#define a_list_peek(l)    a_list_first(l)

extern List* a_list_clone(const List* list);
extern void a_list_reverse(List* list);
extern void** a_list_array(List* list);

extern void* a_list_first(const List* list);
extern void* a_list_last(const List* list);

extern void* a_list_get(const List* list, int index);

extern int a_list_size(const List* list);
extern bool a_list_isEmpty(const List* list);

#endif // A2X_PACK_LIST_PH
