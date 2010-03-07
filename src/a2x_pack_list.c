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

#include "a2x_pack_list.p.h"
#include "a2x_pack_list.v.h"

List* a_list_set(void)
{
    List* const list = malloc(sizeof(List));
    Node* const first = malloc(sizeof(Node));
    Node* const last = malloc(sizeof(Node));

    first->content = NULL;
    first->next = last;
    first->prev = NULL;

    last->content = NULL;
    last->next = NULL;
    last->prev = first;

    list->first = first;
    list->last = last;
    list->current = first;
    list->items = 0;

    return list;
}

void a_list_free(List* const list)
{
    Node* n = list->first->next;

    while(n != list->last) {
        Node* const t = n;
        n = n->next;

        free(t);
    }

    free(list->first);
    free(list->last);
    free(list);
}

void a_list_freeContent(List* const list)
{
    Node* n = list->first->next;

    while(n != list->last) {
        Node* const t = n;
        n = n->next;

        free(t->content);
        free(t);
    }

    free(list->first);
    free(list->last);
    free(list);
}

void a_list_empty(List* const list)
{
    Node* n = list->first->next;

    while(n != list->last) {
        Node* const t = n;
        n = n->next;

        free(t);
    }

    list->first->next = list->last;
    list->last->prev = list->first;

    list->items = 0;
}

void a_list_emptyContent(List* const list)
{
    Node* n = list->first->next;

    while(n != list->last) {
        Node* const t = n;
        n = n->next;

        free(t->content);
        free(t);
    }

    list->first->next = list->last;
    list->last->prev = list->first;

    list->items = 0;
}

Node* a_list_addFirst(List* const list, void* const content)
{
    Node* const n = malloc(sizeof(Node));

    n->content = content;
    n->next = list->first->next;
    n->prev = list->first;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

Node* a_list_addLast(List* const list, void* const content)
{
    Node* const n = malloc(sizeof(Node));

    n->content = content;
    n->next = list->last;
    n->prev = list->last->prev;

    n->prev->next = n;
    n->next->prev = n;

    list->items++;

    return n;
}

void a_list_reset(List* const list)
{
    list->current = list->first;
}

int a_list_iterate(List* const list)
{
    Node* const n = list->current->next;

    if(n == list->last) {
        list->current = list->first;
        return 0;
    }

    list->current = n;
    return 1;
}

void* a_list_pop(List* const list)
{
    Node* const n = list->first->next;

    if(n == list->last) return NULL;

    void* const v = n->content;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->current = n->prev;

    list->items--;

    free(n);
    return v;
}

void* a_list_peek(List* const list)
{
    Node* const n = list->first->next;

    if(n == list->last) return NULL;
    else return n->content;
}

void a_list_removeFirst(List* const list)
{
    Node* const n = list->first->next;

    if(n == list->last) return;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->items--;

    free(n);
}

void a_list_removeFirstContent(List* const list)
{
    Node* const n = list->first->next;

    if(n == list->last) return;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->items--;

    free(n->content);
    free(n);
}

void a_list_removeLast(List* const list)
{
    Node* const n = list->last->prev;

    if(n == list->first) return;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->items--;

    free(n);
}

void a_list_removeLastContent(List* const list)
{
    Node* const n = list->last->prev;

    if(n == list->first) return;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->items--;

    free(n->content);
    free(n);
}

void a_list_removeCurrent(List* const list)
{
    Node* const n = list->current;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->current = n->prev;

    list->items--;

    free(n);
}

void a_list_removeCurrentContent(List* const list)
{
    Node* const n = list->current;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->current = n->prev;

    list->items--;

    free(n->content);
    free(n);
}

void a_list_removeNode(Node* const node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
}

void a_list_removeNodeContent(Node* const node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node->content);
    free(node);
}

void* a_list_getArray(List* const list, const int size)
{
    void* data = malloc(a_list_items(list) * size);

    for(int i = 0; a_list_iterate(list); i++) {
        memcpy(data + i * size, a_list_current(list), size);
    }

    return data;
}

ListIterator a_list_iterator(List* const list)
{
    ListIterator i;

    i.list = list;
    i.current = list->current;

    return i;
}
