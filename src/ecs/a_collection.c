/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "a_collection.v.h"
#include <a2x.v.h>

static ACollection* g_current; // New entities are added to this collection

ACollection* a_collection__get(void)
{
    return g_current;
}

void a_collection_set(ACollection* Collection)
{
    g_current = Collection;
}

ACollection* a_collection_new(void)
{
    return a_list_new();
}

void a_collection_free(ACollection* Collection)
{
    a_ecs__refDecIgnoreSet(true);
    a_list_freeEx(Collection, (AFree*)a_entity__freeEx);
    a_ecs__refDecIgnoreSet(false);
}

void a_collection_clear(ACollection* Collection)
{
    a_list_clearEx(Collection, (AFree*)a_entity_removedSet);
}

void a_collection_muteInc(ACollection* Collection)
{
    A_LIST_ITERATE(Collection, AEntity*, e) {
        a_entity_muteInc(e);
    }
}

void a_collection_muteDec(ACollection* Collection)
{
    A_LIST_ITERATE(Collection, AEntity*, e) {
        a_entity_muteDec(e);
    }
}
