/*
    Copyright 2011, 2016, 2018 Alex Margarit <alex@alxm.org>
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

#include "f_listit.v.h"
#include <faur.v.h>

FListIt f__listit_new(const FList* List, bool Reversed)
{
    FListIt it;

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

bool f__listit_getNext(FListIt* Iterator, void* UserPtrAddress)
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

void f__listit_remove(const FListIt* Iterator)
{
    f_list_removeNode(Iterator->reversed
                        ? Iterator->nextNode->next : Iterator->nextNode->prev);
}

bool f__listit_isFirst(const FListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->nextNode->next->next == Iterator->sentinelNode;
    } else {
        return Iterator->nextNode->prev->prev == Iterator->sentinelNode;
    }
}

bool f__listit_isLast(const FListIt* Iterator)
{
    return Iterator->nextNode == Iterator->sentinelNode;
}
