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

#include "f_ecs.v.h"
#include <faur.v.h>

static void f_ecs__uninit(void)
{
    f_entity__uninit();
    f_template__uninit();
    f_system__uninit();
    f_component__uninit();
}

const FPack f_pack__ecs = {
    "ECS",
    {
        NULL,
    },
    {
        [0] = f_ecs__uninit,
    },
};

void f_ecs_init(FComponent** Components, size_t ComponentsNum, FSystem** Systems, size_t SystemsNum)
{
    f_component__init(Components, ComponentsNum);
    f_system__init(Systems, SystemsNum);
    f_template__init();
    f_entity__init();
}
