/*
    Copyright 2010, 2018-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_TIME_TIME_P_H
#define A_INC_TIME_TIME_P_H

#include "general/f_system_includes.h"

#include "general/f_fps.p.h"

extern uint32_t f_time_getMs(void);

extern void f_time_waitMs(uint32_t Ms);
extern void f_time_waitSec(uint32_t Sec);

extern void f_time_spinMs(uint32_t Ms);
extern void f_time_spinSec(uint32_t Sec);

static inline unsigned f_time_ticksFromMs(unsigned Ms)
{
    return (f_fps_rateTickGet() * Ms + 500) / 1000;
}

static inline unsigned f_time_ticksFromSec(unsigned Sec)
{
    return f_fps_rateTickGet() * Sec;
}

#endif // A_INC_TIME_TIME_P_H
