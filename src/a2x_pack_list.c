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

#include "a2x_pack_list.v.h"

AList* a_list_new(void)
{
    AList* const list = a_mem_malloc(sizeof(AList));
    AListNode* const first = a_mem_malloc(sizeof(AListNode));
    AListNode* const last = a_mem_malloc(sizeof(AListNode));

    first->content = NULL;
    first->next = last;
    first->prev = NULL;

    last->content = NULL;
    last->next = NULL;
    last->prev = first;

    list->first = first;
    list->last = last;
    list->items = 0;

    return list;
}

void a_list_free(AList* list)
{
    AListNode* n = list->first->next;

    while(n != list->last) {
        AListNode* const t = n;
        n = n->next;

        free(t);
    }

    free(list->first);
    free(list->last);
    free(list);
}

void a_list_empty(AList* list)
{
    AListNode* n = list->first->next;

    while(n != list->last) {
        AListNode* const t = n;
        n = n->next;

        free(t);
    }

    list->first->next = list->last;
    list->last->prev = list->first;

    list->items = 0;
}

AListNode* a_list_addFirst(AList* list, void* content)
{
    AListNode* const n = a_mem_malloc(sizeof(AListNode));

    n->content = content;
    n->next = list->first->next;
    n->prev = list->first;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

AListNode* a_list_addLast(AList* list, void* content)
{
    AListNode* const n = a_mem_malloc(sizeof(AListNode));

    n->content = content;
    n->next = list->last;
    n->prev = list->last->prev;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

void a_list_remove(AList* list, const void* v)
{
    A_LIST_ITERATE(list, void, var) {
        if(var == v) {
            A_LIST_REMOVE();
            break;
        }
    }
}

void* a_list_removeFirst(AList* list)
{
    AListNode* const n = list->first->next;

    if(n == list->last) {
        return NULL;
    }

    void* v = n->content;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    list->items--;

    return v;
}

void* a_list_removeLast(AList* list)
{
    AListNode* const n = list->last->prev;

    if(n == list->first) {
        return NULL;
    }

    void* v = n->content;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    list->items--;

    return v;
}

void a_list_removeNode(AListNode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
}

AList* a_list_clone(const AList* list)
{
    AList* l = a_list_new();

    for(AListNode* n = list->first->next; n != list->last; n = n->next) {
        a_list_addLast(l, n->content);
    }

    return l;
}

void a_list_reverse(AList* list)
{
    AListNode* save;

    for(AListNode* n = list->last; n; n = n->next) {
        save = n->next;
        n->next = n->prev;
        n->prev = save;
    }

    save = list->first;
    list->first = list->last;
    list->last = save;
}

void** a_list_array(AList* list)
{
    int i = 0;
    void** const array = a_mem_malloc(list->items * sizeof(void*));

    A_LIST_ITERATE(list, void, v) {
        array[i++] = v;
    }

    return array;
}

void* a_list_first(const AList* list)
{
    return a_list__first(list);
}

void* a_list_last(const AList* list)
{
    return a_list__last(list);
}

void* a_list_get(const AList* list, int index)
{
    int counter = -1;

    for(AListNode* n = list->first; n->next != NULL; n = n->next, counter++) {
        if(counter == index) {
            return n->content;
        }
    }

    return NULL;
}

int a_list_size(const AList* list)
{
    return a_list__size(list);
}

bool a_list_isEmpty(const AList* list)
{
    return list->first->next == list->last;
}
