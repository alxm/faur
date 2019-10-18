/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

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

#include "f_collection.v.h"
#include <faur.v.h>

#if A_CONFIG_ECS_ENABLED
static ACollection* g_current; // New entities are added to this collection

ACollection* f_collection__get(void)
{
    return g_current;
}

void f_collection_set(ACollection* Collection)
{
    g_current = Collection;
}

ACollection* f_collection_new(void)
{
    return f_list_new();
}

void f_collection_free(ACollection* Collection)
{
    f_ecs__refDecIgnoreSet(true);
    f_list_freeEx(Collection, (AFree*)f_entity__freeEx);
    f_ecs__refDecIgnoreSet(false);
}

void f_collection_clear(ACollection* Collection)
{
    f_list_clearEx(Collection, (AFree*)f_entity_removedSet);
}

void f_collection_muteInc(ACollection* Collection)
{
    A_LIST_ITERATE(Collection, AEntity*, e) {
        f_entity_muteInc(e);
    }
}

void f_collection_muteDec(ACollection* Collection)
{
    A_LIST_ITERATE(Collection, AEntity*, e) {
        f_entity_muteDec(e);
    }
}
#endif // A_CONFIG_ECS_ENABLED
