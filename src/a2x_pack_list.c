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

#include "a2x_system_includes.h"
#include "a2x_pack_list.v.h"

#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_random.v.h"

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
    a_list_freeEx(List, NULL);
}

void a_list_freeEx(AList* List, AFree* Free)
{
    AListNode* n = List->first->next;

    while(n != List->last) {
        AListNode* t = n;
        n = n->next;

        if(Free) {
            Free(t->content);
        }

        free(t);
    }

    free(List->first);
    free(List->last);
    free(List);
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

void a_list_appendMove(AList* Dst, AList* Src)
{
    if(Dst == Src || Src->items == 0) {
        return;
    }

    if(Dst->items == 0) {
        AList save = *Dst;
        *Dst = *Src;
        *Src = save;
        return;
    }

    for(AListNode* n = Src->first->next; n != Src->last; n = n->next) {
        n->list = Dst;
    }

    Dst->last->prev->next = Src->first->next;
    Src->first->next->prev = Dst->last->prev;
    Dst->last->prev = Src->last->prev;
    Src->last->prev->next = Dst->last;

    Dst->items += Src->items;

    Src->first->next = Src->last;
    Src->last->prev = Src->first;

    Src->items = 0;
}

void a_list_appendCopy(AList* Dst, AList* Src)
{
    AListNode* lastSrcEntry = Src->last->prev;

    for(AListNode* n = Src->first; n != lastSrcEntry; n = n->next) {
        a_list_addLast(Dst, n->next->content);
    }
}

void* a_list_getIndex(const AList* List, unsigned Index)
{
    AListNode* n;

    for(n = List->first->next; n != List->last; n = n->next) {
        if(Index-- == 0) {
            break;
        }
    }

    return n->content;
}

void* a_list_getFirst(const AList* List)
{
    return List->first->next->content;
}

void* a_list_getLast(const AList* List)
{
    return List->last->prev->content;
}

void* a_list_getRandom(const AList* List)
{
    return a_list_getIndex(List, a_random_intu(List->items));
}

void* a_list_getNodeContent(const AListNode* Node)
{
    return Node->content;
}

void a_list_removeItem(AList* List, const void* Item)
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
    void* v = n->content;

    if(n != List->last) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
        List->items--;

        free(n);
    }

    return v;
}

void* a_list_removeLast(AList* List)
{
    AListNode* n = List->last->prev;
    void* v = n->content;

    if(n != List->first) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
        List->items--;

        free(n);
    }

    return v;
}

void a_list_removeNode(AListNode* Node)
{
    Node->prev->next = Node->next;
    Node->next->prev = Node->prev;
    Node->list->items--;

    free(Node);
}

void a_list_clear(AList* List)
{
    a_list_clearEx(List, NULL);
}

void a_list_clearEx(AList* List, AFree* Free)
{
    AListNode* n = List->first->next;

    while(n != List->last) {
        AListNode* saved = n;
        n = n->next;

        if(Free) {
            Free(saved->content);
        }

        free(saved);
    }

    List->first->next = List->last;
    List->last->prev = List->first;

    List->items = 0;
}

AList* a_list_dup(const AList* List)
{
    AList* l = a_list_new();

    for(AListNode* n = List->first->next; n != List->last; n = n->next) {
        a_list_addLast(l, n->content);
    }

    return l;
}

void** a_list_toArray(AList* List)
{
    void** array = a_mem_malloc(List->items * sizeof(void*));

    A_LIST_ITERATE(List, void*, item) {
        array[A_LIST_INDEX()] = item;
    }

    return array;
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

static inline AListNode* getNode(AListNode* Start, unsigned Index)
{
    while(Index--) {
        Start = Start->next;
    }

    return Start;
}

static inline void addHeadToMerged(AListNode** MergedTail, AListNode** SortedHead)
{
    AListNode* nextSortedHead = (*SortedHead)->next;

    if(nextSortedHead != NULL) {
        nextSortedHead->prev = NULL;
    }

    (*MergedTail)->next = *SortedHead;
    (*SortedHead)->prev = (*MergedTail);
    (*SortedHead)->next = NULL;

    *MergedTail = *SortedHead;
    *SortedHead = nextSortedHead;
}

static AListNode* sort(AListNode* Start, unsigned Length, AListCompare* Compare)
{
    if(Length == 1) {
        return Start;
    }

    unsigned halfPoint = Length / 2;

    AListNode* firstHalfHead = getNode(Start, 0);
    AListNode* secondHalfHead = getNode(Start, halfPoint);

    secondHalfHead->prev->next = NULL;
    secondHalfHead->prev = NULL;

    // sort [0, halfPoint) and [halfPoint, Length)
    AListNode* a = sort(firstHalfHead, halfPoint, Compare);
    AListNode* b = sort(secondHalfHead, Length - halfPoint, Compare);

    AListNode mergedHead = {NULL, NULL, NULL, NULL};
    AListNode* mergedTail = &mergedHead;

    // Merge a and b
    while(true) {
        if(a == NULL) {
            mergedTail->next = b;
            b->prev = mergedTail;
            break;
        }

        if(b == NULL) {
            mergedTail->next = a;
            a->prev = mergedTail;
            break;
        }

        int result = Compare(a->content, b->content);

        if(result <= 0) {
            addHeadToMerged(&mergedTail, &a);
        } else if(result > 0) {
            addHeadToMerged(&mergedTail, &b);
        }
    }

    return mergedHead.next;
}

void a_list_sort(AList* List, AListCompare* Compare)
{
    if(List->items < 2) {
        return;
    }

    List->first->next->prev = NULL;
    List->last->prev->next = NULL;

    AListNode* sorted = sort(List->first->next, List->items, Compare);

    List->first->next = sorted;
    sorted->prev = List->first;

    while(sorted->next != NULL) {
        sorted = sorted->next;
    }

    sorted->next = List->last;
    List->last->prev = sorted;
}

unsigned a_list_getSize(const AList* List)
{
    return List->items;
}

bool a_list_isEmpty(const AList* List)
{
    return List->first->next == List->last;
}
