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

static void f_ecs__uninit(void)
{
    if(f_ecs__isInit()) {
        f_entity__uninit();
        f_system__uninit();
        f_component__uninit();
    }
}

const FPack f_pack__ecs = {
    "ECS",
    NULL,
    f_ecs__uninit,
};

void f__ecs_init(const FComponent* const Components[], size_t ComponentsNum, const FSystem* const Systems[], size_t SystemsNum)
{
    F__CHECK(!f_ecs__isInit());
    F__CHECK(Components != NULL);
    F__CHECK(ComponentsNum > 0);
    F__CHECK(Systems != NULL);
    F__CHECK(SystemsNum > 0);

    f_out__info("ECS: %zu components, %zu systems", ComponentsNum, SystemsNum);

    f_component__init(Components, ComponentsNum);
    f_system__init(Systems, SystemsNum);
    f_entity__init();
}
