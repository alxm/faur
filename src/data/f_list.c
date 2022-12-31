/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_list.v.h"
#include <faur.v.h>

#define F__ITERATE(List, N) \
    for(FListNode* N = List->sentinel.next; N != &List->sentinel; N = N->next)

#define F__ITERATE_REV(List, N) \
    for(FListNode* N = List->sentinel.prev; N != &List->sentinel; N = N->prev)

#define F__ITERATE_SAFE(List, Current, Next)                             \
    for(FListNode *Current = List->sentinel.next, *Next = Current->next; \
        Current != &List->sentinel;                                      \
        Current = Next, Next = Next->next)

const FList f__list_empty = {
    {
        NULL,
        (FList*)&f__list_empty,
        (FListNode*)&f__list_empty.sentinel,
        (FListNode*)&f__list_empty.sentinel,
    },
    0,
};

FList* f_list_new(void)
{
    FList* list = f_pool__alloc(F_POOL__LIST);

    list->sentinel.content = NULL;
    list->sentinel.list = list;
    list->sentinel.next = &list->sentinel;
    list->sentinel.prev = &list->sentinel;

    list->items = 0;

    return list;
}

void f_list_free(FList* List)
{
    f_list_freeEx(List, NULL);
}

void f_list_freeEx(FList* List, FCallFree* Free)
{
    if(List == NULL) {
        return;
    }

    f_list_clearEx(List, Free);

    f_pool_release(List);
}

FListNode* f_list_addFirst(FList* List, void* Content)
{
    F__CHECK(List != NULL);
    F__CHECK(Content != NULL);

    FListNode* n = f_pool__alloc(F_POOL__LISTNODE);

    n->content = Content;
    n->list = List;
    n->next = List->sentinel.next;
    n->prev = &List->sentinel;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

FListNode* f_list_addLast(FList* List, void* Content)
{
    F__CHECK(List != NULL);
    F__CHECK(Content != NULL);

    FListNode* n = f_pool__alloc(F_POOL__LISTNODE);

    n->content = Content;
    n->list = List;
    n->next = &List->sentinel;
    n->prev = List->sentinel.prev;

    n->prev->next = n;
    n->next->prev = n;

    List->items++;

    return n;
}

void f_list_appendMove(FList* Dst, FList* Src)
{
    F__CHECK(Dst != NULL);
    F__CHECK(Src != NULL);

    if(Dst == Src || Src->items == 0) {
        return;
    }

    F__ITERATE(Src, n) {
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

void f_list_appendCopy(FList* Dst, const FList* Src)
{
    F__CHECK(Dst != NULL);
    F__CHECK(Src != NULL);

    // Capture the address of Src's last node, in case Src == Dst
    FListNode* lastSrcNode = Src->sentinel.prev;

    for(const FListNode* n = &Src->sentinel; n != lastSrcNode; n = n->next) {
        f_list_addLast(Dst, n->next->content);
    }
}

void* f_list_getByIndex(const FList* List, unsigned Index)
{
    F__CHECK(List != NULL);
    F__CHECK(Index < List->items);

    if(Index < List->items >> 1) {
        F__ITERATE(List, n) {
            if(Index-- == 0) {
                return n->content;
            }
        }
    } else {
        Index = List->items - 1 - Index;

        F__ITERATE_REV(List, n) {
            if(Index-- == 0) {
                return n->content;
            }
        }
    }

    return NULL;
}

void* f_list_getByNode(const FListNode* Node)
{
    F__CHECK(Node != NULL);

    return Node->content;
}

void* f_list_getFirst(const FList* List)
{
    F__CHECK(List != NULL);

    return List->sentinel.next->content;
}

void* f_list_getLast(const FList* List)
{
    F__CHECK(List != NULL);

    return List->sentinel.prev->content;
}

void* f_list_getRandom(const FList* List)
{
    F__CHECK(List != NULL);

    if(List->items == 0) {
        return NULL;
    }

    return f_list_getByIndex(List, f_random_intu(List->items));
}

static inline void* removeNode(FListNode* Node)
{
    void* v = Node->content;

    Node->prev->next = Node->next;
    Node->next->prev = Node->prev;

    Node->list->items--;

    f_pool_release(Node);

    return v;
}

void* f_list_removeItem(FList* List, const void* Item)
{
    F__CHECK(List != NULL);
    F__CHECK(Item != NULL);

    F__ITERATE(List, n) {
        if(n->content == Item) {
            return removeNode(n);
        }
    }

    return NULL;
}

void* f_list_removeFirst(FList* List)
{
    F__CHECK(List != NULL);

    FListNode* n = List->sentinel.next;

    if(n != &List->sentinel) {
        return removeNode(n);
    }

    return NULL;
}

void* f_list_removeLast(FList* List)
{
    F__CHECK(List != NULL);

    FListNode* n = List->sentinel.prev;

    if(n != &List->sentinel) {
        return removeNode(n);
    }

    return NULL;
}

void* f_list_removeByIndex(FList* List, unsigned Index)
{
    F__CHECK(List != NULL);
    F__CHECK(Index < List->items);

    if(Index < List->items >> 1) {
        F__ITERATE(List, n) {
            if(Index-- == 0) {
                return removeNode(n);
            }
        }
    } else {
        Index = List->items - 1 - Index;

        F__ITERATE_REV(List, n) {
            if(Index-- == 0) {
                return removeNode(n);
            }
        }
    }

    return NULL;
}

void* f_list_removeRandom(FList* List)
{
    F__CHECK(List != NULL);

    if(List->items > 0) {
        return f_list_removeByIndex(List, f_random_intu(List->items));
    }

    return NULL;
}

void f_list_removeNode(FListNode* Node)
{
    F__CHECK(Node != NULL);

    removeNode(Node);
}

void f_list_clear(FList* List)
{
    F__CHECK(List != NULL);

    f_list_clearEx(List, NULL);
}

void f_list_clearEx(FList* List, FCallFree* Free)
{
    F__CHECK(List != NULL);

    if(Free) {
        F__ITERATE_SAFE(List, current, next) {
            Free(current->content);

            // Check if the Free callback already self-removed from the list
            if(next->prev == current) {
                f_pool_release(current);
            }
        }
    } else {
        F__ITERATE_SAFE(List, current, next) {
            f_pool_release(current);
        }
    }

    List->sentinel.next = &List->sentinel;
    List->sentinel.prev = &List->sentinel;

    List->items = 0;
}

FList* f_list_dup(const FList* List)
{
    F__CHECK(List != NULL);

    FList* l = f_list_new();

    F__ITERATE(List, n) {
        f_list_addLast(l, n->content);
    }

    return l;
}

void** f_list_toArray(const FList* List)
{
    F__CHECK(List != NULL);
    F__CHECK(List->items > 0);

    int i = 0;
    void** array = f_mem_malloc(List->items * sizeof(void*));

    F__ITERATE(List, n) {
        array[i++] = n->content;
    }

    return array;
}

void f_list_reverse(FList* List)
{
    F__CHECK(List != NULL);

    for(FListNode* n = List->sentinel.prev; n != &List->sentinel; n = n->next) {
        FListNode* save = n->next;

        n->next = n->prev;
        n->prev = save;
    }

    FListNode* save = List->sentinel.next;

    List->sentinel.next = List->sentinel.prev;
    List->sentinel.prev = save;
}

static inline FListNode* getNode(FListNode* Start, unsigned Index)
{
    while(Index--) {
        Start = Start->next;
    }

    return Start;
}

static inline void addHeadToMerged(FListNode** MergedTail, FListNode** SortedHead)
{
    FListNode* nextSortedHead = (*SortedHead)->next;

    if(nextSortedHead != NULL) {
        nextSortedHead->prev = NULL;
    }

    (*MergedTail)->next = *SortedHead;
    (*SortedHead)->prev = (*MergedTail);
    (*SortedHead)->next = NULL;

    *MergedTail = *SortedHead;
    *SortedHead = nextSortedHead;
}

static FListNode* sort(FListNode* Start, unsigned Length, FCallListCompare* Compare)
{
    if(Length == 1) {
        return Start;
    }

    unsigned halfPoint = Length / 2;

    FListNode* firstHalfHead = getNode(Start, 0);
    FListNode* secondHalfHead = getNode(Start, halfPoint);

    secondHalfHead->prev->next = NULL;
    secondHalfHead->prev = NULL;

    // sort [0, halfPoint) and [halfPoint, Length)
    FListNode* a = sort(firstHalfHead, halfPoint, Compare);
    FListNode* b = sort(secondHalfHead, Length - halfPoint, Compare);

    FListNode mergedHead = {NULL, NULL, NULL, NULL};
    FListNode* mergedTail = &mergedHead;

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

void f_list_sort(FList* List, FCallListCompare* Compare)
{
    F__CHECK(List != NULL);
    F__CHECK(Compare != NULL);

    if(List->items < 2) {
        return;
    }

    List->sentinel.next->prev = NULL;
    List->sentinel.prev->next = NULL;

    FListNode* sorted = sort(List->sentinel.next, List->items, Compare);

    List->sentinel.next = sorted;
    sorted->prev = &List->sentinel;

    while(sorted->next != NULL) {
        sorted = sorted->next;
    }

    sorted->next = &List->sentinel;
    List->sentinel.prev = sorted;
}

unsigned f_list_sizeGet(const FList* List)
{
    F__CHECK(List != NULL);

    return List->items;
}

bool f_list_sizeIsEmpty(const FList* List)
{
    F__CHECK(List != NULL);

    return List->items == 0;
}

bool f_list_contains(const FList* List, const void* Item)
{
    F__CHECK(List != NULL);
    F__CHECK(Item != NULL);

    F__ITERATE(List, n) {
        if(n->content == Item) {
            return true;
        }
    }

    return false;
}

void f_list_apply(const FList* List, FCallFree* Apply)
{
    F__CHECK(List != NULL);
    F__CHECK(Apply != NULL);

    F__ITERATE_SAFE(List, current, next) {
        Apply(current->content);
    }
}

F__ListIt f__listit_new(const FList* List, bool Reversed)
{
    F__CHECK(List != NULL);

    F__ListIt it;

    it.sentinelNode = &List->sentinel;

    if(Reversed) {
        it.nextNode = List->sentinel.prev;
        it.index = f_list_sizeGet(List);
    } else {
        it.nextNode = List->sentinel.next;
        it.index = UINT_MAX;
    }

    it.reversed = Reversed;

    return it;
}

bool f__listit_getNext(F__ListIt* Iterator, void* UserPtrAddress)
{
    if(Iterator->nextNode == Iterator->sentinelNode) {
        return false;
    }

    *(void**)UserPtrAddress = Iterator->nextNode->content;

    if(Iterator->reversed) {
        Iterator->nextNode = Iterator->nextNode->prev;
        Iterator->index--;
    } else {
        Iterator->nextNode = Iterator->nextNode->next;
        Iterator->index++;
    }

    return true;
}

void f__listit_remove(const F__ListIt* Iterator)
{
    f_list_removeNode(Iterator->reversed
                        ? Iterator->nextNode->next : Iterator->nextNode->prev);
}

bool f__listit_isFirst(const F__ListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->nextNode->next->next == Iterator->sentinelNode;
    } else {
        return Iterator->nextNode->prev->prev == Iterator->sentinelNode;
    }
}

bool f__listit_isLast(const F__ListIt* Iterator)
{
    return Iterator->nextNode == Iterator->sentinelNode;
}
