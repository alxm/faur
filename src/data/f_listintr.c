/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

#include "f_listintr.v.h"
#include <faur.v.h>

static inline FListIntrNode* itemToNode(const FListIntr* List, const void* Item)
{
    return (FListIntrNode*)(void*)((uint8_t*)Item + List->itemNodeOffset);
}

static inline void* nodeToItem(const FListIntr* List, const FListIntrNode* Node)
{
    return (uint8_t*)Node - List->itemNodeOffset;
}

void f_listintr_init(FListIntr* List, size_t NodeOffset)
{
    List->root.prev = &List->root;
    List->root.next = &List->root;

    List->itemNodeOffset = NodeOffset;
}

void f_listintr_clear(FListIntr* List)
{
    List->root.prev = &List->root;
    List->root.next = &List->root;
}

void f_listintr_addFirst(FListIntr* List, void* Item)
{
    FListIntrNode* rootNode = &List->root;
    FListIntrNode* itemNode = itemToNode(List, Item);

    itemNode->prev = rootNode;
    itemNode->next = rootNode->next;

    itemNode->prev->next = itemNode;
    itemNode->next->prev = itemNode;
}

void f_listintr_addLast(FListIntr* List, void* Item)
{
    FListIntrNode* rootNode = &List->root;
    FListIntrNode* itemNode = itemToNode(List, Item);

    itemNode->prev = rootNode->prev;
    itemNode->next = rootNode;

    itemNode->prev->next = itemNode;
    itemNode->next->prev = itemNode;
}

void* f_listintr_getFirst(const FListIntr* List)
{
    FListIntrNode* first = List->root.next;

    if(first == &List->root) {
        return NULL;
    }

    return nodeToItem(List, first);
}

void* f_listintr_removeFirst(FListIntr* List)
{
    FListIntrNode* first = List->root.next;

    if(first == &List->root) {
        return NULL;
    }

    f_listintr_removeNode(first);

    return nodeToItem(List, first);
}

void f_listintr_removeItem(FListIntr* List, void* Item)
{
    f_listintr_removeNode(itemToNode(List, Item));
}

void f_listintr_removeNode(FListIntrNode* Node)
{
    if(Node->next == Node) {
        return;
    }

    Node->prev->next = Node->next;
    Node->next->prev = Node->prev;

    Node->prev = Node;
    Node->next = Node;
}

bool f_listintr_sizeIsEmpty(const FListIntr* List)
{
    return List->root.next == &List->root;
}

void f_listintr_apply(const FListIntr* List, FCallFree* Apply)
{
    for(FListIntrNode *n1 = List->root.next, *n2 = n1->next;
        n1 != &List->root;
        n1 = n2, n2 = n2->next) {

        Apply(nodeToItem(List, n1));
    }
}

void f_listintr_nodeInit(FListIntrNode* Node)
{
    Node->prev = Node;
    Node->next = Node;
}

bool f_listintr_nodeIsLinked(const FListIntrNode* Node)
{
    return Node->next != Node;
}

F__ListIntrIt f__listintrit_new(const FListIntr* List)
{
    return (F__ListIntrIt){List, List->root.next, UINT_MAX};
}

bool f__listintrit_getNext(F__ListIntrIt* Iterator, void* UserPtrAddress)
{
    if(Iterator->next == &Iterator->list->root) {
        return false;
    }

    *(void**)UserPtrAddress = nodeToItem(Iterator->list, Iterator->next);

    Iterator->next = Iterator->next->next;
    Iterator->index++;

    return true;
}
