/*
    Copyright 2016 Alex Margarit

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

#include "a2x_pack_random.v.h"

void a_random__init(void)
{
    srand(time(NULL));
}

int a_random_int(int max)
{
    if(max == 0) {
        a_fatal("a_random_int invalid arg: 0");
    }

    return rand() % max;
}

int a_random_range(int min, int max)
{
    if(min >= max) {
        a_fatal("a_random_range invalid args: %d, %d", min, max);
    }

    return min + (rand() % (max - min));
}
