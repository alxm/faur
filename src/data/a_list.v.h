/*
    Copyright 2010, 2017-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_DATA_LIST_V_H
#define A_INC_DATA_LIST_V_H

#include "data/a_list.p.h"

struct AListNode {
    void* content;
    AList* list;
    AListNode* next;
    AListNode* prev;
};

struct AList {
    AListNode sentinel;
    unsigned items;
};

extern const AList a__list_empty;

static inline AList* a_list__nodeGetList(const AListNode* Node)
{
    return Node->list;
}

#endif // A_INC_DATA_LIST_V_H
