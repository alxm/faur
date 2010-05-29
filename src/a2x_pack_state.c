/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_state.p.h"
#include "a2x_pack_state.v.h"

static int changed;
static StateRunner current = NULL;

void a_state_go(StateRunner s)
{
    current = s;
    changed = 1;
}

void a_state__run(void)
{
    while(current) {
        changed = 0;
        current();
    }
}

int a_state_running(void)
{
    static int first = 1;

    if(first) {
        first = 0;
    } else {
        a_fps_end();
    }

    a_fps_start();

    if(changed) {
        first = 1;
    }

    return !changed;
}
