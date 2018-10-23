/*
    Copyright 2011, 2016, 2018 Alex Margarit

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

#include "a2x_pack_listit.v.h"

#include "a2x_pack_list.v.h"

AListIt a__listit_new(AList* List, bool Reversed)
{
    AListIt it;

    it.sentinelNode = &List->sentinel;
    it.nextNode = Reversed ? List->sentinel.prev : List->sentinel.next;
    it.currentItem = NULL;
    it.reversed = Reversed;

    return it;
}

bool a__listit_getNext(AListIt* Iterator)
{
    if(Iterator->nextNode == Iterator->sentinelNode) {
        return false;
    }

    Iterator->currentItem = Iterator->nextNode->content;

    if(Iterator->reversed) {
        Iterator->nextNode = Iterator->nextNode->prev;
    } else {
        Iterator->nextNode = Iterator->nextNode->next;
    }

    return true;
}

void a__listit_remove(AListIt* Iterator)
{
    a_list_removeNode(Iterator->reversed
                        ? Iterator->nextNode->next
                        : Iterator->nextNode->prev);
}

bool a__listit_isFirst(AListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->nextNode->next->next == Iterator->sentinelNode;
    } else {
        return Iterator->nextNode->prev->prev == Iterator->sentinelNode;
    }
}

bool a__listit_isLast(AListIt* Iterator)
{
    return Iterator->nextNode == Iterator->sentinelNode;
}
