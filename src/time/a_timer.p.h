/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_TIME_TIMER_P_H
#define A_INC_TIME_TIMER_P_H

#include "general/a_system_includes.h"

typedef struct ATimer ATimer;

typedef enum {
    A_TIMER_INVALID = -1,
    A_TIMER_MS,
    A_TIMER_SEC,
    A_TIMER_TICKS,
    A_TIMER_NUM
} ATimerType;

extern ATimer* a_timer_new(ATimerType Type, unsigned Period, bool Repeat);
extern ATimer* a_timer_dup(const ATimer* Timer);
extern void a_timer_free(ATimer* Timer);

extern unsigned a_timer_elapsedGet(const ATimer* Timer);

extern unsigned a_timer_periodGet(const ATimer* Timer);
extern void a_timer_periodSet(ATimer* Timer, unsigned Period);

extern void a_timer_start(ATimer* Timer);
extern void a_timer_stop(ATimer* Timer);
extern bool a_timer_isRunning(const ATimer* Timer);

extern bool a_timer_expiredGet(const ATimer* Timer);
extern unsigned a_timer_expiredGetCount(const ATimer* Timer);
extern void a_timer_expiredClear(ATimer* Timer);

#endif // A_INC_TIME_TIMER_P_H
