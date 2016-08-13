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

void a_list_free(AList* List)
{
    AListNode* n = List->first->next;

    while(n != List->last) {
        AListNode* const t = n;
        n = n->next;

        free(t);
    }

    free(List->first);
    free(List->last);
    free(List);
}

void a_list_empty(AList* List)
{
    AListNode* n = List->first->next;

    while(n != List->last) {
        AListNode* const t = n;
        n = n->next;

        free(t);
    }

    List->first->next = List->last;
    List->last->prev = List->first;

    List->items = 0;
}

AListNode* a_list_addFirst(AList* List, void* Content)
{
    AListNode* const n = a_mem_malloc(sizeof(AListNode));

    n->content = Content;
    n->next = List->first->next;
    n->prev = List->first;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

AListNode* a_list_addLast(AList* List, void* Content)
{
    AListNode* const n = a_mem_malloc(sizeof(AListNode));

    n->content = Content;
    n->next = List->last;
    n->prev = List->last->prev;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

void* a_list_getFirst(const AList* List)
{
    return a_list__first(List);
}

void* a_list_getLast(const AList* List)
{
    return a_list__last(List);
}

void a_list_remove(AList* List, const void* Item)
{
    A_LIST_ITERATE(List, void*, item) {
        if(item == Item) {
            A_LIST_REMOVE_CURRENT();
            break;
        }
    }
}

void* a_list_removeFirst(AList* List)
{
    AListNode* const n = List->first->next;

    if(n == List->last) {
        return NULL;
    }

    void* v = n->content;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    List->items--;

    return v;
}

void* a_list_removeLast(AList* List)
{
    AListNode* const n = List->last->prev;

    if(n == List->first) {
        return NULL;
    }

    void* v = n->content;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    List->items--;

    return v;
}

void a_list_removeNode(AListNode* Node)
{
    Node->prev->next = Node->next;
    Node->next->prev = Node->prev;

    free(Node);
}

AListNode* a_list_push(AList* List, void* Content)
{
    return a_list_addFirst(List, Content);
}

void* a_list_pop(AList* List)
{
    return a_list_removeFirst(List);
}

void* a_list_peek(AList* List)
{
    return a_list_getFirst(List);
}

AList* a_list_clone(const AList* List)
{
    AList* l = a_list_new();

    for(AListNode* n = List->first->next; n != List->last; n = n->next) {
        a_list_addLast(l, n->content);
    }

    return l;
}

void a_list_reverse(AList* List)
{
    AListNode* save;

    for(AListNode* n = List->last; n; n = n->next) {
        save = n->next;
        n->next = n->prev;
        n->prev = save;
    }

    save = List->first;
    List->first = List->last;
    List->last = save;
}

void** a_list_array(AList* List)
{
    int i = 0;
    void** array = a_mem_malloc(List->items * sizeof(void*));

    A_LIST_ITERATE(List, void*, item) {
        array[i++] = item;
    }

    return array;
}

void* a_list_get(const AList* List, int Index)
{
    int counter = -1;

    for(AListNode* n = List->first; n->next != NULL; n = n->next, counter++) {
        if(counter == Index) {
            return n->content;
        }
    }

    return NULL;
}

int a_list_size(const AList* List)
{
    return a_list__size(List);
}

bool a_list_isEmpty(const AList* List)
{
    return List->first->next == List->last;
}
