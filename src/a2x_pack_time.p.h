/*
    Copyright 2010, 2018-2019 Alex Margarit <alex@alxm.org>
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

#pragma once

#include "a2x_system_includes.h"

#include "a2x_pack_fps.p.h"

extern uint32_t a_time_getMs(void);

extern void a_time_waitMs(uint32_t Ms);
extern void a_time_waitSec(uint32_t Sec);

extern void a_time_spinMs(uint32_t Ms);
extern void a_time_spinSec(uint32_t Sec);

static inline unsigned a_time_ticksFromMs(unsigned Ms)
{
    return (a_fps_rateTickGet() * Ms + 500) / 1000;
}

static inline unsigned a_time_ticksFromSec(unsigned Sec)
{
    return a_fps_rateTickGet() * Sec;
}
