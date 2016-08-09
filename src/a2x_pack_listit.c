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

#include "a2x_pack_listit.v.h"

AListIt a_listit__new(AList* List)
{
    AListIt it;

    it.list = List;
    it.currentNode = List->first;
    it.currentItem = NULL;

    return it;
}

bool a_listit__getNext(AListIt* Iterator)
{
    if(Iterator->currentNode->next == Iterator->list->last) {
        return false;
    }

    Iterator->currentNode = Iterator->currentNode->next;
    Iterator->currentItem = Iterator->currentNode->content;

    return true;
}

void a_listit__remove(AListIt* Iterator)
{
    AList* list = Iterator->list;
    AListNode* n = Iterator->currentNode;

    Iterator->currentNode = n->prev;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    list->items--;
}
