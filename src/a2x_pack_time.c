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

#if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    #define TIMER_BASE3  0x1980
    #define TIMER_REG(x) memregs[(TIMER_BASE3 + x) >> 2]

    static int memfd;
    static volatile uint32_t* memregs;
#endif

void a_time__init(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        memfd = open("/dev/mem", O_RDWR);
        memregs = mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, 0xc0000000);

        TIMER_REG(0x44) = 0x922;
        TIMER_REG(0x40) = 0x0c;
        TIMER_REG(0x08) = 0x6b;
    #endif
}

void a_time__free(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        TIMER_REG(0x40) = 0x0c;
        TIMER_REG(0x08) = 0x23;
        TIMER_REG(0x00) = 0;
        TIMER_REG(0x40) = 0;
        TIMER_REG(0x44) = 0;

        close(memfd);
    #endif
}

uint32_t a_time_getMilis(void)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        TIMER_REG(0x08) = 0x4b; // run timer, latch value
        const uint32_t t = TIMER_REG(0) / 1000;
    #else
        const uint32_t t = SDL_GetTicks();
    #endif

    return t;
}

void a_time_waitMilis(const uint32_t milis)
{
    #if A_PLATFORM_WIZ || A_PLATFORM_CAANOO
        const uint32_t start = a_time_getMilis();

        while(a_time_getMilis() - start < milis) {
            continue;
        }
    #else
        SDL_Delay(milis);
    #endif
}
