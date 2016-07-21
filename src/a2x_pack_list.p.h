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

#include "a2x_app_includes.h"

typedef struct AList AList;
typedef struct AListNode AListNode;

extern AList* a_list_new(void);
extern void a_list_free(AList* list);
extern void a_list_empty(AList* list);

extern AListNode* a_list_addFirst(AList* list, void* content);
extern AListNode* a_list_addLast(AList* list, void* content);

extern void a_list_remove(AList* list, const void* v);
extern void* a_list_removeFirst(AList* list);
extern void* a_list_removeLast(AList* list);
extern void a_list_removeNode(AListNode* node);

#define a_list_push(l, c) a_list_addFirst(l, c)
#define a_list_pop(l)     a_list_removeFirst(l)
#define a_list_peek(l)    a_list_first(l)

extern AList* a_list_clone(const AList* list);
extern void a_list_reverse(AList* list);
extern void** a_list_array(AList* list);

extern void* a_list_first(const AList* list);
extern void* a_list_last(const AList* list);

extern void* a_list_get(const AList* list, int index);

extern int a_list_size(const AList* list);
extern bool a_list_isEmpty(const AList* list);
