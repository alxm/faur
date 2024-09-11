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

#include "f_ecs.v.h"
#include <faur.v.h>

#if F_CONFIG_ECS
#if F_CONFIG_ECS_COM_NUM <= 0 || F_CONFIG_ECS_SYS_NUM <= 0
    #error FAUR_ERROR: Invalid F_CONFIG_ECS
#endif

static void f_ecs__init(void)
{
    f_out__info("%u components, %u systems",
                F_CONFIG_ECS_COM_NUM,
                F_CONFIG_ECS_SYS_NUM);

    f_component__init();
    f_system__init();
    f_entity__init();
}

static void f_ecs__uninit(void)
{
    f_entity__uninit();
    f_system__uninit();
    f_component__uninit();
}

const FPack f_pack__ecs = {
    "ECS",
    f_ecs__init,
    f_ecs__uninit,
};
#endif // F_CONFIG_ECS
