/*
    Copyright 2010, 2017-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_pack_list.p.h"

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

static inline AList* a_list__nodeGetList(const AListNode* Node)
{
    return Node->list;
}
