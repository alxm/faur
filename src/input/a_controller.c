/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "a_controller.v.h"
#include <a2x.v.h>

unsigned a_controller_numGet(void)
{
    return a_platform_api__inputControllerNumGet();
}

void a_controller_set(unsigned Index)
{
    #if A_CONFIG_BUILD_DEBUG
        if(Index >= a_platform_api__inputControllerNumGet()) {
            A__FATAL("a_controller_set(%d): %d controllers total",
                     Index,
                     a_platform_api__inputControllerNumGet());
        }
    #endif

    a_platform_api__inputControllerSet(Index);
}
