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

#include "f_timer.v.h"
#include <faur.v.h>

typedef enum {
    F_TIMER__REPEAT = F_FLAGS_BIT(0),
    F_TIMER__RUNNING = F_FLAGS_BIT(1),
    F_TIMER__EXPIRED = F_FLAGS_BIT(2),
} FTimerFlags;

struct FTimer {
    FTimerFlags flags;
    unsigned periodMs;
    FFixu period;
    FFixu start;
    FFixu diff;
    unsigned expiredCount;
    FListNode* runningListNode;
};

FFixu g_ticksNow;
static FList* g_runningTimers; // FList<FTimer*>

static void f_timer__init(void)
{
    g_ticksNow = f_fixu_fromInt(f_fps_ticksGet());
    g_runningTimers = f_list_new();
}

static void f_timer__uninit(void)
{
    f_list_free(g_runningTimers);
}

const FPack f_pack__timer = {
    "Timer",
    f_timer__init,
    f_timer__uninit,
};

void f_timer__tick(void)
{
    g_ticksNow = f_fixu_fromInt(f_fps_ticksGet());

    F_LIST_ITERATE(g_runningTimers, FTimer*, t) {
        if(!F_FLAGS_TEST_ANY(t->flags, F_TIMER__RUNNING)) {
            // Kick out timer that was marked as not running last frame
            f_list_removeNode(t->runningListNode);
            t->runningListNode = NULL;

            F_FLAGS_CLEAR(t->flags, F_TIMER__EXPIRED);

            continue;
        }

        unsigned diff = g_ticksNow - t->start;

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
    FTimer* t = f_mem_mallocz(sizeof(FTimer));

    t->periodMs = PeriodMs;
    t->period = f_time_msToTicks(PeriodMs);

    if(Repeat) {
        F_FLAGS_SET(t->flags, F_TIMER__REPEAT);
    }

    return t;
}

FTimer* f_timer_dup(const FTimer* Timer)
{
    FTimer* t = f_mem_dup(Timer, sizeof(FTimer));

    F_FLAGS_CLEAR(t->flags, F_TIMER__RUNNING | F_TIMER__EXPIRED);

    t->diff = 0;
    t->runningListNode = NULL;

    return t;
}

void f_timer_free(FTimer* Timer)
{
    if(Timer == NULL) {
        return;
    }

    if(Timer->runningListNode) {
        f_list_removeNode(Timer->runningListNode);
    }

    f_mem_free(Timer);
}

unsigned f_timer_elapsedGet(const FTimer* Timer)
{
    return Timer->diff;
}

FFixu f_timer_elapsedGetFraction(const FTimer* Timer)
{
    return Timer->period > 0 ? f_fixu_div(Timer->diff, Timer->period) : 0;
}

unsigned f_timer_periodGet(const FTimer* Timer)
{
    return Timer->periodMs;
}

void f_timer_periodSet(FTimer* Timer, unsigned PeriodMs)
{
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
    Timer->start = g_ticksNow;
    Timer->diff = 0;
    Timer->expiredCount = 0;

    F_FLAGS_SET(Timer->flags, F_TIMER__RUNNING);

    if(Timer->period == 0) {
        F_FLAGS_SET(Timer->flags, F_TIMER__EXPIRED);
        Timer->expiredCount++;
    } else {
        F_FLAGS_CLEAR(Timer->flags, F_TIMER__EXPIRED);
    }

    if(Timer->runningListNode == NULL) {
        Timer->runningListNode = f_list_addLast(g_runningTimers, Timer);
    }
}

void f_timer_runStop(FTimer* Timer)
{
    Timer->diff = 0;

    F_FLAGS_CLEAR(Timer->flags, F_TIMER__RUNNING | F_TIMER__EXPIRED);

    if(Timer->runningListNode) {
        f_list_removeNode(Timer->runningListNode);
        Timer->runningListNode = NULL;
    }
}

bool f_timer_runGet(const FTimer* Timer)
{
    return F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__RUNNING);
}

bool f_timer_expiredGet(const FTimer* Timer)
{
    return F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__EXPIRED);
}

unsigned f_timer_expiredGetCount(const FTimer* Timer)
{
    return Timer->expiredCount;
}

void f_timer_expiredClear(FTimer* Timer)
{
    if(Timer->period > 0) {
        F_FLAGS_CLEAR(Timer->flags, F_TIMER__EXPIRED);
    }
}
