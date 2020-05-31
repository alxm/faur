/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

FCollection* f__collection; // New entities are added to this collection

void f_collection_set(FCollection* Collection)
{
    f__collection = Collection;
}

FCollection* f_collection_new(void)
{
    return f_list_new();
}

void f_collection_free(FCollection* Collection)
{
    f_entity__ignoreRefDec = true;

    f_list_freeEx(Collection, (FFree*)f_entity__freeEx);

    f_entity__ignoreRefDec = false;
}

void f_collection_clear(FCollection* Collection)
{
    f_list_clearEx(Collection, (FFree*)f_entity_removedSet);
}

void f_collection_muteInc(FCollection* Collection)
{
    F_LIST_ITERATE(Collection, FEntity*, e) {
        f_entity_muteInc(e);
    }
}

void f_collection_muteDec(FCollection* Collection)
{
    F_LIST_ITERATE(Collection, FEntity*, e) {
        f_entity_muteDec(e);
    }
}
