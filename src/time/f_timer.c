/*
    Copyright 2011, 2016-2019 Alex Margarit <alex@alxm.org>
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
    FTimerType type;
    FTimerFlags flags;
    unsigned period;
    unsigned start;
    unsigned diff;
    unsigned expiredCount;
    FListNode* runningListNode;
};

static struct {
    unsigned ms;
    unsigned ticks;
} g_now;

static FList* g_runningTimers; // list of FTimer

static inline void setNow(void)
{
    g_now.ms = f_time_getMs();
    g_now.ticks = f_fps_ticksGet();
}

static inline unsigned getNow(const FTimer* Timer)
{
    switch(Timer->type) {
        case F_TIMER_MS:
        case F_TIMER_SEC:
            return g_now.ms;

        case F_TIMER_TICKS:
            return g_now.ticks;

        default:
            return 0;
    }
}

static void f_timer__init(void)
{
    setNow();

    g_runningTimers = f_list_new();
}

static void f_timer__uninit(void)
{
    f_list_free(g_runningTimers);
}

const FPack f_pack__timer = {
    "Timer",
    {
        [0] = f_timer__init,
    },
    {
        [0] = f_timer__uninit,
    },
};

void f_timer__tick(void)
{
    setNow();

    F_LIST_ITERATE(g_runningTimers, FTimer*, t) {
        if(!F_FLAGS_TEST_ANY(t->flags, F_TIMER__RUNNING)) {
            // Kick out timer that was marked as not running last frame
            f_list_removeNode(t->runningListNode);
            t->runningListNode = NULL;

            F_FLAGS_CLEAR(t->flags, F_TIMER__EXPIRED);

            continue;
        }

        t->diff = getNow(t) - t->start;

        if(t->diff >= t->period) {
            F_FLAGS_SET(t->flags, F_TIMER__EXPIRED);
            t->expiredCount++;

            if(F_FLAGS_TEST_ANY(t->flags, F_TIMER__REPEAT)) {
                if(t->period > 0) {
                    t->start += (t->diff / t->period) * t->period;
                }
            } else {
                t->diff = 0;

                // Will be kicked out of running list next frame
                F_FLAGS_CLEAR(t->flags, F_TIMER__RUNNING);
            }
        } else {
            F_FLAGS_CLEAR(t->flags, F_TIMER__EXPIRED);
        }
    }
}

FTimer* f_timer_new(FTimerType Type, unsigned Period, bool Repeat)
{
    FTimer* t = f_mem_zalloc(sizeof(FTimer));

    if(Type == F_TIMER_SEC) {
        Period *= 1000;
    }

    t->type = Type;
    t->period = Period;

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

unsigned f_timer_periodGet(const FTimer* Timer)
{
    if(Timer->type == F_TIMER_SEC) {
        return Timer->period / 1000;
    }

    return Timer->period;
}

void f_timer_periodSet(FTimer* Timer, unsigned Period)
{
    if(Timer->type == F_TIMER_SEC) {
        Period *= 1000;
    }

    Timer->period = Period;
    Timer->expiredCount = 0;

    if(Period == 0 && F_FLAGS_TEST_ANY(Timer->flags, F_TIMER__RUNNING)) {
        F_FLAGS_SET(Timer->flags, F_TIMER__EXPIRED);
        Timer->expiredCount++;
    }
}

void f_timer_start(FTimer* Timer)
{
    Timer->start = getNow(Timer);
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

void f_timer_stop(FTimer* Timer)
{
    Timer->diff = 0;

    F_FLAGS_CLEAR(Timer->flags, F_TIMER__RUNNING | F_TIMER__EXPIRED);

    if(Timer->runningListNode) {
        f_list_removeNode(Timer->runningListNode);
        Timer->runningListNode = NULL;
    }
}

bool f_timer_isRunning(const FTimer* Timer)
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
