/*
    Copyright 2010, 2016, 2017 Alex Margarit

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
    AList* list = a_mem_malloc(sizeof(AList));
    AListNode* first = a_mem_malloc(sizeof(AListNode));
    AListNode* last = a_mem_malloc(sizeof(AListNode));

    first->content = NULL;
    first->list = list;
    first->next = last;
    first->prev = NULL;

    last->content = NULL;
    last->list = list;
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
        AListNode* t = n;
        n = n->next;

        free(t);
    }

    free(List->first);
    free(List->last);
    free(List);
}

void a_list_clear(AList* List)
{
    AListNode* n = List->first->next;

    while(n != List->last) {
        AListNode* saved = n;
        n = n->next;
        free(saved);
    }

    List->first->next = List->last;
    List->last->prev = List->first;

    List->items = 0;
}

AListNode* a_list_addFirst(AList* List, void* Content)
{
    AListNode* n = a_mem_malloc(sizeof(AListNode));

    n->content = Content;
    n->list = List;
    n->next = List->first->next;
    n->prev = List->first;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

AListNode* a_list_addLast(AList* List, void* Content)
{
    AListNode* n = a_mem_malloc(sizeof(AListNode));

    n->content = Content;
    n->list = List;
    n->next = List->last;
    n->prev = List->last->prev;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

void a_list_append(AList* Base, AList* NewEntries)
{
    if(NewEntries->items == 0) {
        return;
    }

    if(Base->items == 0) {
        AList save = *Base;
        *Base = *NewEntries;
        *NewEntries = save;
        return;
    }

    Base->last->prev->next = NewEntries->first->next;
    NewEntries->first->next->prev = Base->last->prev;
    Base->last->prev = NewEntries->last->prev;
    NewEntries->last->prev->next = Base->last;

    Base->items += NewEntries->items;

    NewEntries->first->next = NewEntries->last;
    NewEntries->last->prev = NewEntries->first;

    NewEntries->items = 0;
}

void* a_list_getFirst(const AList* List)
{
    return List->first->next->content;
}

void* a_list_getLast(const AList* List)
{
    return List->last->prev->content;
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
    AListNode* n = List->first->next;

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
    AListNode* n = List->last->prev;

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

    Node->list->items--;

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

void* a_list_get(const AList* List, unsigned Index)
{
    if(Index < List->items) {
        unsigned counter = 0;

        for(AListNode* n = List->first->next; n != List->last; n = n->next) {
            if(counter++ == Index) {
                return n->content;
            }
        }
    }

    return NULL;
}

unsigned a_list_size(const AList* List)
{
    return List->items;
}

bool a_list_empty(const AList* List)
{
    return List->first->next == List->last;
}

static inline AListNode* getNode(AListNode* Start, unsigned Index)
{
    while(Index--) {
        Start = Start->next;
    }

    return Start;
}

static inline void addHeadToMerged(AList* List, AListNode** MergedTail, AListNode** SortedHead)
{
    AListNode* nextSortedHead = (*SortedHead)->next;
    nextSortedHead->prev = List->first;

    (*MergedTail)->next = *SortedHead;
    (*SortedHead)->prev = (*MergedTail);
    (*SortedHead)->next = List->last;

    *MergedTail = *SortedHead;
    *SortedHead = nextSortedHead;
}

static AListNode* sort(AList* List, AListNode* Start, unsigned Length, AListCompare* Compare)
{
    if(Length == 1) {
        return Start;
    }

    unsigned halfPoint = Length / 2;

    AListNode* firstHalfHead = getNode(Start, 0);
    AListNode* secondHalfHead = getNode(Start, halfPoint);

    secondHalfHead->prev->next = List->last;
    secondHalfHead->prev = List->first;

    // sort [0, halfPoint) and [halfPoint, Length)
    AListNode* a = sort(List, firstHalfHead, halfPoint, Compare);
    AListNode* b = sort(List, secondHalfHead, Length - halfPoint, Compare);

    AListNode* merged = List->first;

    // Merge a and b
    while(true) {
        if(a == List->last) {
            merged->next = b;
            b->prev = merged;
            break;
        }

        if(b == List->last) {
            merged->next = a;
            a->prev = merged;
            break;
        }

        int result = Compare(a->content, b->content);

        if(result <= 0) {
            addHeadToMerged(List, &merged, &a);
        } else if(result > 0) {
            addHeadToMerged(List, &merged, &b);
        }
    }

    return List->first->next;
}

void a_list_sort(AList* List, AListCompare* Compare)
{
    if(List->items < 2) {
        return;
    }

    sort(List, List->first->next, List->items, Compare);
}
