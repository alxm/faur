/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_TIME_TIME_P_H
#define F_INC_TIME_TIME_P_H

#include "../general/f_system_includes.h"

#include "../general/f_fps.p.h"

extern uint32_t f_time_msGet(void);
extern void f_time_msWait(uint32_t Ms);
extern void f_time_msSpin(uint32_t Ms);

static inline FFixu f_time_msToTicks(unsigned Ms)
{
    return (FFixu)((uint64_t)f_fixu_fromInt(f_fps_rateTickGet()) * Ms / 1000);
}

#endif // F_INC_TIME_TIME_P_H
