/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_mem.v.h"
#include "a2x_pack_random.v.h"

#define A__ITERATE(List, N) \
    for(AListNode* N = List->sentinel.next; N != &List->sentinel; N = N->next)

#define A__ITERATE_SAFE(List, Current, Next)                             \
    for(AListNode *Current = List->sentinel.next, *Next = Current->next; \
        Current != &List->sentinel;                                      \
        Current = Next, Next = Next->next)

AList* a_list_new(void)
{
    AList* list = a_mem_malloc(sizeof(AList));

    list->sentinel.content = NULL;
    list->sentinel.list = list;
    list->sentinel.next = &list->sentinel;
    list->sentinel.prev = &list->sentinel;

    list->items = 0;

    return list;
}

void a_list_free(AList* List)
{
    a_list_freeEx(List, NULL);
}

void a_list_freeEx(AList* List, AFree* Free)
{
    if(List == NULL) {
        return;
    }

    a_list_clearEx(List, Free);

    free(List);
}

AListNode* a_list_addFirst(AList* List, void* Content)
{
    AListNode* n = a_mem_malloc(sizeof(AListNode));

    n->content = Content;
    n->list = List;
    n->next = List->sentinel.next;
    n->prev = &List->sentinel;

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
    n->next = &List->sentinel;
    n->prev = List->sentinel.prev;

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

    A__ITERATE(Src, n) {
        n->list = Dst;
    }

    Dst->sentinel.prev->next = Src->sentinel.next;
    Src->sentinel.next->prev = Dst->sentinel.prev;
    Dst->sentinel.prev = Src->sentinel.prev;
    Src->sentinel.prev->next = &Dst->sentinel;

    Dst->items += Src->items;

    Src->sentinel.next = &Src->sentinel;
    Src->sentinel.prev = &Src->sentinel;

    Src->items = 0;
}

void a_list_appendCopy(AList* Dst, const AList* Src)
{
    // Capture the address of Src's last node, in case Src == Dst
    AListNode* lastSrcNode = Src->sentinel.prev;

    for(const AListNode* n = &Src->sentinel; n != lastSrcNode; n = n->next) {
        a_list_addLast(Dst, n->next->content);
    }
}

void* a_list_getByIndex(const AList* List, unsigned Index)
{
    const AListNode* n;

    for(n = List->sentinel.next; n != &List->sentinel; n = n->next) {
        if(Index-- == 0) {
            break;
        }
    }

    return n->content;
}

void* a_list_getFirst(const AList* List)
{
    return List->sentinel.next->content;
}

void* a_list_getLast(const AList* List)
{
    return List->sentinel.prev->content;
}

void* a_list_getRandom(const AList* List)
{
    return a_list_getByIndex(List, a_random_intu(List->items));
}

void* a_list_getNodeContent(const AListNode* Node)
{
    return Node->content;
}

void a_list_removeItem(AList* List, const void* Item)
{
    A__ITERATE(List, n) {
        if(n->content == Item) {
            n->prev->next = n->next;
            n->next->prev = n->prev;

            List->items--;

            free(n);

            return;
        }
    }
}

void* a_list_removeFirst(AList* List)
{
    AListNode* n = List->sentinel.next;
    void* v = n->content;

    if(n != &List->sentinel) {
        n->prev->next = n->next;
        n->next->prev = n->prev;

        List->items--;

        free(n);
    }

    return v;
}

void* a_list_removeLast(AList* List)
{
    AListNode* n = List->sentinel.prev;
    void* v = n->content;

    if(n != &List->sentinel) {
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
    if(Free) {
        A__ITERATE_SAFE(List, current, next) {
            Free(current->content);

            // Check if the Free callback already self-removed from the list
            if(next->prev == current) {
                free(current);
            }
        }
    } else {
        A__ITERATE_SAFE(List, current, next) {
            free(current);
        }
    }

    List->sentinel.next = &List->sentinel;
    List->sentinel.prev = &List->sentinel;

    List->items = 0;
}

AList* a_list_dup(const AList* List)
{
    AList* l = a_list_new();

    A__ITERATE(List, n) {
        a_list_addLast(l, n->content);
    }

    return l;
}

void** a_list_toArray(const AList* List)
{
    int i = 0;
    void** array = a_mem_malloc(List->items * sizeof(void*));

    A__ITERATE(List, n) {
        array[i++] = n->content;
    }

    return array;
}

void a_list_reverse(AList* List)
{
    for(AListNode* n = List->sentinel.prev; n != &List->sentinel; n = n->next) {
        AListNode* save = n->next;

        n->next = n->prev;
        n->prev = save;
    }

    AListNode* save = List->sentinel.next;

    List->sentinel.next = List->sentinel.prev;
    List->sentinel.prev = save;
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

    List->sentinel.next->prev = NULL;
    List->sentinel.prev->next = NULL;

    AListNode* sorted = sort(List->sentinel.next, List->items, Compare);

    List->sentinel.next = sorted;
    sorted->prev = &List->sentinel;

    while(sorted->next != NULL) {
        sorted = sorted->next;
    }

    sorted->next = &List->sentinel;
    List->sentinel.prev = sorted;
}

unsigned a_list_sizeGet(const AList* List)
{
    return List->items;
}

bool a_list_isEmpty(const AList* List)
{
    return List->items == 0;
}
