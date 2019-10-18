/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "f_ecs.v.h"
#include <faur.v.h>

#if F_CONFIG_ECS_ENABLED
static AList* g_lists[F_ECS__NUM]; // Each entity is in exactly one of these
static bool g_ignoreRefDec; // Set to prevent using freed entities

static void f_ecs__init(void)
{
    for(int i = F_ECS__NUM; i--; ) {
        g_lists[i] = f_list_new();
    }

    f_component__init();
    f_template__init();
}

static void f_ecs__uninit(void)
{
    f_ecs__refDecIgnoreSet(true);

    for(int i = F_ECS__NUM; i--; ) {
        f_list_freeEx(g_lists[i], (AFree*)f_entity__free);
    }

    f_template__uninit();
    f_system__uninit();
    f_component__uninit();
}

const APack f_pack__ecs = {
    "ECS",
    {
        [0] = f_ecs__init,
    },
    {
        [0] = f_ecs__uninit,
    },
};

AList* f_ecs__listGet(AEcsListId List)
{
    return g_lists[List];
}

unsigned f_ecs__listGetSum(void)
{
    unsigned sum = 0;

    for(int i = F_ECS__NUM; i--; ) {
        sum += f_list_sizeGet(g_lists[i]);
    }

    return sum;
}

bool f_ecs__refDecIgnoreGet(void)
{
    return g_ignoreRefDec;
}

void f_ecs__refDecIgnoreSet(bool IgnoreRefDec)
{
    g_ignoreRefDec = IgnoreRefDec;
}

void f_ecs__tick(void)
{
    f_ecs__flushEntitiesFromSystems();

    // Check what systems the new entities match
    F_LIST_ITERATE(g_lists[F_ECS__NEW], AEntity*, e) {
        for(int s = F_CONFIG_ECS_SYS_NUM; s--; ) {
            f_entity__systemsMatch(e, f_system__get(s));
        }

        f_entity__ecsListAdd(e, g_lists[F_ECS__RESTORE]);
    }

    // Add entities to the systems they match
    F_LIST_ITERATE(g_lists[F_ECS__RESTORE], AEntity*, e) {
        f_entity__systemsAddTo(e);
    }

    f_list_clear(g_lists[F_ECS__NEW]);
    f_list_clear(g_lists[F_ECS__RESTORE]);
    f_list_clearEx(g_lists[F_ECS__FREE], (AFree*)f_entity__free);
}

void f_ecs__flushEntitiesFromSystems(void)
{
    F_LIST_ITERATE(g_lists[F_ECS__FLUSH], AEntity*, e) {
        f_entity__systemsRemoveFromAll(e);

        f_entity__ecsListAdd(
            e, g_lists[f_entity__canDelete(e) ? F_ECS__FREE : F_ECS__DEFAULT]);
    }

    f_list_clear(g_lists[F_ECS__FLUSH]);
}
#else // !F_CONFIG_ECS_ENABLED
const APack f_pack__ecs;

void f_ecs__tick(void)
{
}
#endif // !F_CONFIG_ECS_ENABLED
