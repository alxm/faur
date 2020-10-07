/*
    Copyright 2011, 2016-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_TIME_TIMER_P_H
#define F_INC_TIME_TIMER_P_H

#include "../general/f_system_includes.h"

typedef struct FTimer FTimer;

typedef enum {
    F_TIMER_INVALID = -1,
    F_TIMER_MS,
    F_TIMER_TICKS,
    F_TIMER_NUM
} FTimerType;

#include "../math/f_fix.p.h"

extern FTimer* f_timer_new(FTimerType Type, unsigned Period, bool Repeat);
extern FTimer* f_timer_dup(const FTimer* Timer);
extern void f_timer_free(FTimer* Timer);

extern unsigned f_timer_elapsedGet(const FTimer* Timer);
extern FFixu f_timer_elapsedGetFraction(const FTimer* Timer);

extern unsigned f_timer_periodGet(const FTimer* Timer);
extern void f_timer_periodSet(FTimer* Timer, unsigned Period);

extern void f_timer_runStart(FTimer* Timer);
extern void f_timer_runStop(FTimer* Timer);
extern bool f_timer_runGet(const FTimer* Timer);

extern bool f_timer_expiredGet(const FTimer* Timer);
extern unsigned f_timer_expiredGetCount(const FTimer* Timer);
extern void f_timer_expiredClear(FTimer* Timer);

#endif // F_INC_TIME_TIMER_P_H
