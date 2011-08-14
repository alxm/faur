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

struct ListIt {
    List* list;
    ListNode* current;
};

ListIt* a_listit_new(List* const list)
{
    ListIt* const it = malloc(sizeof(ListIt));

    it->list = list;
    it->current = list->first;

    return it;
}

void a_listit_free(ListIt* const it)
{
    free(it);
}

bool a_listit_next(const ListIt* const it)
{
    return it->current->next != it->list->last;
}

void* a_listit_get(ListIt* const it)
{
    it->current = it->current->next;
    return it->current->content;
}

void* a_listit_peek(const ListIt* const it)
{
    return it->current->next->content;
}

void a_listit__remove(ListIt* const it, const int freeContent)
{
    List* const list = it->list;
    ListNode* const n = it->current;

    // prevents errors when calling from inside an a_list_iterate() loop
    if(n == list->current) {
        list->current = n->prev;
    }

    n->prev->next = n->next;
    n->next->prev = n->prev;

    list->items--;

    if(freeContent) {
        free(n->content);
    }

    free(n);
}
