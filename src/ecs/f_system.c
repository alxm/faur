/*
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "f_system.v.h"
#include <faur.v.h>

FSystem* const* f_system__array; // [f_system__num]
unsigned f_system__num;

void f_system__init(FSystem* const* Systems, size_t SystemsNum)
{
    f_system__array = Systems;
    f_system__num = (unsigned)SystemsNum;

    for(unsigned s = f_system__num; s--; ) {
        FSystem* sys = f_system__array[s];

        sys->entities = f_list_new();
        sys->componentBits = f_bitfield_new(f_component__num);

        for(unsigned c = sys->componentsNum; c--; ) {
            #if F_CONFIG_DEBUG
                if(sys->components[c] == NULL) {
                    F__FATAL("%s component %u/%u is NULL",
                             sys->stringId,
                             c + 1,
                             sys->componentsNum);
                }
            #endif

            f_bitfield_set(sys->componentBits, sys->components[c]->bitId);
        }
    }
}

void f_system__uninit(void)
{
    for(unsigned s = f_system__num; s--; ) {
        FSystem* sys = f_system__array[s];

        f_list_free(sys->entities);
        f_bitfield_free(sys->componentBits);
    }
}

void f_system_run(const FSystem* System)
{
    if(System->compare) {
        f_list_sort(System->entities, (FListCompare*)System->compare);
    }

    if(System->onlyActiveEntities) {
        F_LIST_ITERATE(System->entities, FEntity*, entity) {
            if(f_entity_activeGet(entity)) {
                System->handler(entity);
            } else {
                f_entity__flushFromSystemsActive(entity);
            }
        }
    } else {
        F_LIST_ITERATE(System->entities, FEntity*, entity) {
            System->handler(entity);
        }
    }

    f_entity__flushFromSystems();
}
