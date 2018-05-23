/*
    Copyright 2010, 2018 Alex Margarit

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

#pragma once

#include "a2x_pack_fps.p.h"

extern uint32_t a_time_getMs(void);

extern void a_time_waitMs(uint32_t Ms);
extern void a_time_waitSec(uint32_t Sec);

extern void a_time_spinMs(uint32_t Ms);
extern void a_time_spinSec(uint32_t Sec);

static inline unsigned a_time_msToTicks(unsigned Ms)
{
    return (a_fps_getTickRate() * Ms + 500) / 1000;
}

static inline unsigned a_time_secToTicks(unsigned Sec)
{
    return a_fps_getTickRate() * Sec;
}
