/*
    Copyright 2011, 2016 Alex Margarit

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

AListIt a_listit__new(AList* List, bool Reversed)
{
    AListIt it;

    it.list = List;
    it.currentNode = Reversed ? List->last : List->first;
    it.currentItem = NULL;
    it.reversed = Reversed;

    return it;
}

bool a_listit__getNext(AListIt* Iterator)
{
    if(Iterator->reversed) {
        if(Iterator->currentNode->prev == Iterator->list->first) {
            return false;
        }

        Iterator->currentNode = Iterator->currentNode->prev;
    } else {
        if(Iterator->currentNode->next == Iterator->list->last) {
            return false;
        }

        Iterator->currentNode = Iterator->currentNode->next;
    }

    Iterator->currentItem = Iterator->currentNode->content;
    return true;
}

void a_listit__remove(AListIt* Iterator)
{
    AList* list = Iterator->list;
    AListNode* n = Iterator->currentNode;

    if(Iterator->reversed) {
        Iterator->currentNode = n->next;
    } else {
        Iterator->currentNode = n->prev;
    }

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);
    list->items--;
}

bool a_listit__isFirst(AListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->currentNode->next == Iterator->list->last;
    } else {
        return Iterator->currentNode->prev == Iterator->list->first;
    }
}

bool a_listit__isLast(AListIt* Iterator)
{
    if(Iterator->reversed) {
        return Iterator->currentNode->prev == Iterator->list->first;
    } else {
        return Iterator->currentNode->next == Iterator->list->last;
    }
}
