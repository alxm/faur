/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#if F_CONFIG_ECS
void f_system__init(void)
{
    for(unsigned s = F_CONFIG_ECS_SYS_NUM; s--; ) {
        const FSystem* sys = f_system__array[s];

        sys->runtime->entities = f_list_new();
        sys->runtime->componentBits = f_bitfield_new(F_CONFIG_ECS_COM_NUM);

        for(unsigned c = sys->componentsNum; c--; ) {
            if(sys->components[c] == NULL) {
                F__FATAL("%s component %u/%u is NULL",
                         sys->stringId,
                         c + 1,
                         sys->componentsNum);
            }

            f_bitfield_set(sys->runtime->componentBits,
                           sys->components[c]->runtime->bitId);
        }
    }
}

void f_system__uninit(void)
{
    for(unsigned s = F_CONFIG_ECS_SYS_NUM; s--; ) {
        const FSystem* system = f_system__array[s];

        f_list_free(system->runtime->entities);
        f_bitfield_free(system->runtime->componentBits);
    }
}

void f_system_run(const FSystem* System)
{
    F__CHECK(System != NULL);

    if(System->compare) {
        f_list_sort(
            System->runtime->entities, (FCallListCompare*)System->compare);
    }

    if(System->onlyActiveEntities) {
        F_LIST_ITERATE(System->runtime->entities, FEntity*, entity) {
            if(f_entity_activeGet(entity)) {
                System->handler(entity);
            } else {
                f_entity__flushFromSystemsActive(entity);
            }
        }
    } else {
        F_LIST_ITERATE(System->runtime->entities, FEntity*, entity) {
            System->handler(entity);
        }
    }

    f_entity__flushFromSystems();
}
#endif // F_CONFIG_ECS
