/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
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
    FListIntr* l = f_pool__alloc(F_POOL__LISTINTR);

    f_listintr_init(l, FEntity, collectionNode);

    return l;
}

void f_collection_free(FCollection* Collection)
{
    if(Collection == NULL) {
        return;
    }

    f_entity__bulkFreeInProgress = true;
    f_listintr_apply(Collection, (FCallFree*)f_entity__free);
    f_entity__bulkFreeInProgress = false;

    if(f__collection == Collection) {
        f__collection = NULL;
    }

    f_pool_release(Collection);
}

void f_collection_clear(FCollection* Collection)
{
    F__CHECK(Collection != NULL);

    f_listintr_clearEx(Collection, (FCallFree*)f_entity_removedSet);
}

void f_collection_muteInc(FCollection* Collection)
{
    F__CHECK(Collection != NULL);

    F_LISTINTR_ITERATE(Collection, FEntity*, e) {
        f_entity_muteInc(e);
    }
}

void f_collection_muteDec(FCollection* Collection)
{
    F__CHECK(Collection != NULL);

    F_LISTINTR_ITERATE(Collection, FEntity*, e) {
        f_entity_muteDec(e);
    }
}
