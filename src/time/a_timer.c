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

#include "a_timer.v.h"
#include <a2x.v.h>

typedef enum {
    A_TIMER__REPEAT = A_FLAGS_BIT(0),
    A_TIMER__RUNNING = A_FLAGS_BIT(1),
    A_TIMER__EXPIRED = A_FLAGS_BIT(2),
} ATimerFlags;

struct ATimer {
    ATimerType type;
    ATimerFlags flags;
    unsigned period;
    unsigned start;
    unsigned diff;
    unsigned expiredCount;
    AListNode* runningListNode;
};

static struct {
    unsigned ms;
    unsigned ticks;
} g_now;

static AList* g_runningTimers; // list of ATimer

static inline void setNow(void)
{
    g_now.ms = a_time_getMs();
    g_now.ticks = a_fps_ticksGet();
}

static inline unsigned getNow(const ATimer* Timer)
{
    switch(Timer->type) {
        case A_TIMER_MS:
        case A_TIMER_SEC:
            return g_now.ms;

        case A_TIMER_TICKS:
            return g_now.ticks;

        default:
            return 0;
    }
}

static void a_timer__init(void)
{
    setNow();

    g_runningTimers = a_list_new();
}

static void a_timer__uninit(void)
{
    a_list_free(g_runningTimers);
}

const APack a_pack__timer = {
    "Timer",
    {
        [0] = a_timer__init,
    },
    {
        [0] = a_timer__uninit,
    },
};

void a_timer__tick(void)
{
    setNow();

    A_LIST_ITERATE(g_runningTimers, ATimer*, t) {
        if(!A_FLAGS_TEST_ANY(t->flags, A_TIMER__RUNNING)) {
            // Kick out timer that was marked as not running last frame
            a_list_removeNode(t->runningListNode);
            t->runningListNode = NULL;

            A_FLAGS_CLEAR(t->flags, A_TIMER__EXPIRED);

            continue;
        }

        t->diff = getNow(t) - t->start;

        if(t->diff >= t->period) {
            A_FLAGS_SET(t->flags, A_TIMER__EXPIRED);
            t->expiredCount++;

            if(A_FLAGS_TEST_ANY(t->flags, A_TIMER__REPEAT)) {
                if(t->period > 0) {
                    t->start += (t->diff / t->period) * t->period;
                }
            } else {
                t->diff = 0;

                // Will be kicked out of running list next frame
                A_FLAGS_CLEAR(t->flags, A_TIMER__RUNNING);
            }
        } else {
            A_FLAGS_CLEAR(t->flags, A_TIMER__EXPIRED);
        }
    }
}

ATimer* a_timer_new(ATimerType Type, unsigned Period, bool Repeat)
{
    ATimer* t = a_mem_zalloc(sizeof(ATimer));

    if(Type == A_TIMER_SEC) {
        Period *= 1000;
    }

    t->type = Type;
    t->period = Period;

    if(Repeat) {
        A_FLAGS_SET(t->flags, A_TIMER__REPEAT);
    }

    return t;
}

ATimer* a_timer_dup(const ATimer* Timer)
{
    ATimer* t = a_mem_dup(Timer, sizeof(ATimer));

    A_FLAGS_CLEAR(t->flags, A_TIMER__RUNNING | A_TIMER__EXPIRED);

    t->diff = 0;
    t->runningListNode = NULL;

    return t;
}

void a_timer_free(ATimer* Timer)
{
    if(Timer == NULL) {
        return;
    }

    if(Timer->runningListNode) {
        a_list_removeNode(Timer->runningListNode);
    }

    a_mem_free(Timer);
}

unsigned a_timer_elapsedGet(const ATimer* Timer)
{
    return Timer->diff;
}

unsigned a_timer_periodGet(const ATimer* Timer)
{
    if(Timer->type == A_TIMER_SEC) {
        return Timer->period / 1000;
    }

    return Timer->period;
}

void a_timer_periodSet(ATimer* Timer, unsigned Period)
{
    if(Timer->type == A_TIMER_SEC) {
        Period *= 1000;
    }

    Timer->period = Period;
    Timer->expiredCount = 0;

    if(Period == 0 && A_FLAGS_TEST_ANY(Timer->flags, A_TIMER__RUNNING)) {
        A_FLAGS_SET(Timer->flags, A_TIMER__EXPIRED);
        Timer->expiredCount++;
    }
}

void a_timer_start(ATimer* Timer)
{
    Timer->start = getNow(Timer);
    Timer->diff = 0;
    Timer->expiredCount = 0;

    A_FLAGS_SET(Timer->flags, A_TIMER__RUNNING);

    if(Timer->period == 0) {
        A_FLAGS_SET(Timer->flags, A_TIMER__EXPIRED);
        Timer->expiredCount++;
    } else {
        A_FLAGS_CLEAR(Timer->flags, A_TIMER__EXPIRED);
    }

    if(Timer->runningListNode == NULL) {
        Timer->runningListNode = a_list_addLast(g_runningTimers, Timer);
    }
}

void a_timer_stop(ATimer* Timer)
{
    Timer->diff = 0;

    A_FLAGS_CLEAR(Timer->flags, A_TIMER__RUNNING | A_TIMER__EXPIRED);

    if(Timer->runningListNode) {
        a_list_removeNode(Timer->runningListNode);
        Timer->runningListNode = NULL;
    }
}

bool a_timer_isRunning(const ATimer* Timer)
{
    return A_FLAGS_TEST_ANY(Timer->flags, A_TIMER__RUNNING);
}

bool a_timer_expiredGet(const ATimer* Timer)
{
    return A_FLAGS_TEST_ANY(Timer->flags, A_TIMER__EXPIRED);
}

unsigned a_timer_expiredGetCount(const ATimer* Timer)
{
    return Timer->expiredCount;
}

void a_timer_expiredClear(ATimer* Timer)
{
    if(Timer->period > 0) {
        A_FLAGS_CLEAR(Timer->flags, A_TIMER__EXPIRED);
    }
}
