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

List* a_list_new(void)
{
    List* const list = malloc(sizeof(List));
    ListNode* const first = malloc(sizeof(ListNode));
    ListNode* const last = malloc(sizeof(ListNode));

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

void a_list_free(List* list)
{
    ListNode* n = list->first->next;

    while(n != list->last) {
        ListNode* const t = n;
        n = n->next;

        free(t);
    }

    free(list->first);
    free(list->last);
    free(list);
}

void a_list_empty(List* list)
{
    ListNode* n = list->first->next;

    while(n != list->last) {
        ListNode* const t = n;
        n = n->next;

        free(t);
    }

    list->first->next = list->last;
    list->last->prev = list->first;

    list->items = 0;
}

ListNode* a_list_addFirst(List* list, void* content)
{
    ListNode* const n = malloc(sizeof(ListNode));

    n->content = content;
    n->next = list->first->next;
    n->prev = list->first;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

ListNode* a_list_addLast(List* list, void* content)
{
    ListNode* const n = malloc(sizeof(ListNode));

    n->content = content;
    n->next = list->last;
    n->prev = list->last->prev;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

void a_list_remove(List* list, const void* v)
{
    A_LIST_ITERATE(list, void, var) {
        if(var == v) {
            A_LIST_REMOVE();
            break;
        }
    }
}

void* a_list_removeFirst(List* list)
{
    ListNode* const n = list->first->next;

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

void* a_list_removeLast(List* list)
{
    ListNode* const n = list->last->prev;

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

void a_list_removeNode(ListNode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
}

List* a_list_clone(const List* list)
{
    List* l = a_list_new();

    for(ListNode* n = list->first->next; n != list->last; n = n->next) {
        a_list_addLast(l, n->content);
    }

    return l;
}

void a_list_reverse(List* list)
{
    ListNode* save;

    for(ListNode* n = list->last; n; n = n->next) {
        save = n->next;
        n->next = n->prev;
        n->prev = save;
    }

    save = list->first;
    list->first = list->last;
    list->last = save;
}

void** a_list_array(List* list)
{
    int i = 0;
    void** const array = malloc(list->items * sizeof(void*));

    A_LIST_ITERATE(list, void, v) {
        array[i++] = v;
    }

    return array;
}

void* a_list_first(const List* list)
{
    return a_list__first(list);
}

void* a_list_last(const List* list)
{
    return a_list__last(list);
}

void* a_list_get(const List* list, int index)
{
    int counter = -1;

    for(ListNode* n = list->first; n->next != NULL; n = n->next, counter++) {
        if(counter == index) {
            return n->content;
        }
    }

    return NULL;
}

int a_list_size(const List* list)
{
    return a_list__size(list);
}

bool a_list_isEmpty(const List* list)
{
    return list->first->next == list->last;
}
