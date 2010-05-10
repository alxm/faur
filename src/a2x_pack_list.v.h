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

#ifndef A2X_PACK_LIST_VH
#define A2X_PACK_LIST_VH

struct ListNode {
    void* content;
    struct ListNode* next;
    struct ListNode* prev;
};

struct List {
    ListNode* first;
    ListNode* last;
    ListNode* current;
    int items;
};

#define a_list__first(l)         ((l)->first->next->content)
#define a_list__last(l)          ((l)->last->prev->content)
#define a_list__current(l)       ((l)->current->content)
#define a_list__currentNode(l)   ((l)->current)
#define a_list__size(l)          ((l)->items)
#define a_list__isEmpty(l)       ((l)->first->next == (l)->last)
#define a_list__next(n)          ((n)->next)
#define a_list__prev(n)          ((n)->prev)
#define a_list__getContent(n)    ((n)->content)
#define a_list__setContent(n, c) ((n)->content = (c))

extern void a_list__iteratorRewind(ListIterator* const it);

#endif // A2X_PACK_LIST_VH
