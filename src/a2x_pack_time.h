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

#ifndef A2X_PACK_TIME_H
#define A2X_PACK_TIME_H

#include <stdint.h>

#if A_PLATFORM_WIZ
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/mman.h>
#endif

#include <SDL/SDL.h>

extern void a__time_set(void);
extern void a__time_free(void);

extern uint32_t a_time_getMilis(void);
extern void a_time_waitMilis(const uint32_t milis);

extern int a_time_overflowed(void);
extern void a_time_handledOverflow(void);

#endif // A2X_PACK_TIME_H
