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

#pragma once

#include "a2x_system_includes.h"

typedef struct AList AList;
typedef struct AListNode AListNode;

extern AList* a_list_new(void);
extern void a_list_free(AList* List);
extern void a_list_empty(AList* List);

extern AListNode* a_list_addFirst(AList* List, void* Content);
extern AListNode* a_list_addLast(AList* List, void* Content);

extern void a_list_remove(AList* List, const void* Item);
extern void* a_list_removeFirst(AList* List);
extern void* a_list_removeLast(AList* List);
extern void a_list_removeNode(AListNode* Node);

#define a_list_push(List, Item) a_list_addFirst(List, Item)
#define a_list_pop(List) a_list_removeFirst(List)
#define a_list_peek(List) a_list_first(List)

extern AList* a_list_clone(const AList* List);
extern void a_list_reverse(AList* List);
extern void** a_list_array(AList* List);

extern void* a_list_first(const AList* List);
extern void* a_list_last(const AList* List);

extern void* a_list_get(const AList* List, int Index);

extern int a_list_size(const AList* List);
extern bool a_list_isEmpty(const AList* List);
