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

    it.list = List;
    it.currentNode = &List->sentinel;
    it.currentItem = NULL;
    it.reversed = Reversed;

    return it;
}

bool a__listit_getNext(AListIt* Iterator)
{
    if(Iterator->reversed) {
        if(Iterator->currentNode->prev == &Iterator->list->sentinel) {
            return false;
        }

        Iterator->currentNode = Iterator->currentNode->prev;
    } else {
        if(Iterator->currentNode->next == &Iterator->list->sentinel) {
            return false;
        }

        Iterator->currentNode = Iterator->currentNode->next;
    }

    Iterator->currentItem = Iterator->currentNode->content;

    return true;
}

void a__listit_remove(AListIt* Iterator)
{
    AListNode* n = Iterator->currentNode;

    if(Iterator->reversed) {
        Iterator->currentNode = n->next;
    } else {
        Iterator->currentNode = n->prev;
    }

    a_list_removeNode(n);
}

bool a__listit_isFirst(AListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->currentNode->next == &Iterator->list->sentinel;
    } else {
        return Iterator->currentNode->prev == &Iterator->list->sentinel;
    }
}

bool a__listit_isLast(AListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->currentNode->prev == &Iterator->list->sentinel;
    } else {
        return Iterator->currentNode->next == &Iterator->list->sentinel;
    }
}
