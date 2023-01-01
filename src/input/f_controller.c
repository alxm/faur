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

#include "f_controller.v.h"
#include <faur.v.h>

FController* f_controller_new(FCallControllerBind* Callback)
{
    F__CHECK(Callback != NULL);

    return f_platform_api__inputControllerClaim(Callback);
}

void f_controller_free(FController* Controller)
{
    if(Controller == NULL) {
        return;
    }

    f_platform_api__inputControllerRelease(Controller);
}
