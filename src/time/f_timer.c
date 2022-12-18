/*
    Copyright 2011 Alex Margarit <alex@alxm.org>
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

#include "f_timer.v.h"
#include <faur.v.h>

#define F_TIMER__REPEAT F_FLAGS_BIT(0)
#define F_TIMER__RUNNING F_FLAGS_BIT(1)
#define F_TIMER__EXPIRED F_FLAGS_BIT(2)

static F_LISTINTR(g_running, FTimer, listNode);

void f_timer__tick(void)
{
    FFixu ticksNow = f_fixu_fromInt(f_fps_ticksGet());

    F_LISTINTR_ITERATE(&g_running, FTimer*, t) {
        if(!F_FLAGS_TEST_ANY(t->flags, F_TIMER__RUNNING)) {
            // Kick out timer that was marked as not running last frame
            f_listintr_removeNode(&t->listNode);

            F_FLAGS_CLEAR(t->flags, F_TIMER__EXPIRED);

            continue;
        }

        unsigned diff = ticksNow - t->start;

        if(diff >= t->period) {
            F_FLAGS_SET(t->flags, F_TIMER__EXPIRED);

            if(F_FLAGS_TEST_ANY(t->flags, F_TIMER__REPEAT)) {
                if(t->period > 0) {
                    t->start += (diff / t->period) * t->period;
                }
            } else {
                // Will be kicked out of running list next frame
                F_FLAGS_CLEAR(t->flags, F_TIMER__RUNNING);
            }

            if(t->period > 0) {
                t->diff = t->period - 1;
            }

            t->expiredCount++;
        } else {
            F_FLAGS_CLEAR(t->flags, F_TIMER__EXPIRED);

            t->diff = diff;
        }
    }
}

FTimer* f_timer_new(unsigned PeriodMs, bool Repeat)
{
    FTimer* t = f_pool__alloc(F_POOL__TIMER);

    f_listintr_nodeInit(&t->listNode);

    t->periodMs = PeriodMs;
    t->period = f_time_msToTicks(PeriodMs);

    if(Repeat) {
        F_FLAGS_SET(t->flags, F_TIMER__REPEAT);
    }

    return t;
}

FTimer* f_timer_dup(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    FTimer* t = f_pool__dup(F_POOL__TIMER, Timer);

    f_listintr_nodeInit(&t->listNode);

    F_FLAGS_CLEAR(t->flags, F_TIMER__RUNNING | F_TIMER__EXPIRED);
    t->diff = 0;

    return t;
}

void f_timer_free(FTimer* Timer)
{
    if(Timer == NULL) {
        return;
    }

    if(f_listintr_nodeIsLinked(&Timer->listNode)) {
        f_listintr_removeNode(&Timer->listNode);
    }

    f_pool_release(Timer);
}

unsigned f_timer_elapsedGet(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return Timer->diff;
}

FFixu f_timer_elapsedGetFraction(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return Timer->period > 0 ? f_fixu_div(Timer->diff, Timer->period) : 0;
}

unsigned f_timer_periodGet(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return Timer->periodMs;
}

void f_timer_periodSet(FTimer* Timer, unsigned PeriodMs)
{
    F__CHECK(Timer != NULL);

    Timer->periodMs = PeriodMs;
    Timer->period = f_time_msToTicks(PeriodMs);
    Timer->diff = 0;
    Timer->expiredCount = 0;

    if(PeriodMs == 0 && F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__RUNNING)) {
        F_FLAGS_SET(Timer->flags, F_TIMER__EXPIRED);
        Timer->expiredCount++;
    }
}

void f_timer_runStart(FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    Timer->start = f_fixu_fromInt(f_fps_ticksGet());
    Timer->diff = 0;
    Timer->expiredCount = 0;

    F_FLAGS_SET(Timer->flags, F_TIMER__RUNNING);

    if(Timer->period == 0) {
        F_FLAGS_SET(Timer->flags, F_TIMER__EXPIRED);
        Timer->expiredCount++;
    } else {
        F_FLAGS_CLEAR(Timer->flags, F_TIMER__EXPIRED);
    }

    if(!f_listintr_nodeIsLinked(&Timer->listNode)) {
        f_listintr_addLast(&g_running, Timer);
    }
}

void f_timer_runStop(FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    Timer->diff = 0;

    F_FLAGS_CLEAR(Timer->flags, F_TIMER__RUNNING | F_TIMER__EXPIRED);

    f_listintr_removeNode(&Timer->listNode);
}

bool f_timer_runGet(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__RUNNING);
}

bool f_timer_expiredGet(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__EXPIRED);
}

unsigned f_timer_expiredGetCount(const FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    return Timer->expiredCount;
}

void f_timer_expiredClear(FTimer* Timer)
{
    F__CHECK(Timer != NULL);

    if(Timer->period > 0) {
        F_FLAGS_CLEAR(Timer->flags, F_TIMER__EXPIRED);
    }
}
