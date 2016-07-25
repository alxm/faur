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

/*
    Many thanks:

    - to Notaz for the accurate Wiz timer code
*/

#include "a2x_pack_time.v.h"

uint32_t a_time_getMilis(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        return a_hw__getMilis();
    #else
        return a_sdl__getTicks();
    #endif
}

void a_time_waitMilis(uint32_t Milis)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        const uint32_t start = a_hw__getMilis();

        while(a_hw__getMilis() - start < Milis) {
            continue;
        }
    #else
        a_sdl__delay(Milis);
    #endif
}
