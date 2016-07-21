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

AListIt a_listit__new(AList* list)
{
    AListIt it;

    it.list = list;
    it.current = list->first;

    return it;
}

bool a_listit__next(const AListIt* it)
{
    return it->current->next != it->list->last;
}

void* a_listit__get(AListIt* it)
{
    it->current = it->current->next;
    return it->current->content;
}

void* a_listit__peek(const AListIt* it)
{
    return it->current->next->content;
}

void a_listit__remove(AListIt* it)
{
    AList* const list = it->list;
    AListNode* const n = it->current;

    it->current = n->prev;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    free(n);

    list->items--;
}
