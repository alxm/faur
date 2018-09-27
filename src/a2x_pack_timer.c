/*
    Copyright 2011, 2016-2018 Alex Margarit

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

#include "a2x_pack_timer.v.h"

#include "a2x_pack_fps.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_time.v.h"

typedef enum {
    A_TIMER__REPEAT = A_FLAG_BIT(0),
    A_TIMER__RUNNING = A_FLAG_BIT(1),
    A_TIMER__EXPIRED = A_FLAG_BIT(2),
} ATimerFlags;

struct ATimer {
    ATimerType type;
    ATimerFlags flags;
    unsigned period;
    unsigned start;
    unsigned diff;
    AListNode* runningListNode;
};

static struct {
    unsigned ms;
    unsigned ticks;
} g_now;

static AList* g_runningTimers; // list of ATimer

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

void a_timer__init(void)
{
    g_runningTimers = a_list_new();
}

void a_timer__uninit(void)
{
    a_list_free(g_runningTimers);
}

void a_timer__tick(void)
{
    g_now.ms = a_time_msGet();
    g_now.ticks = a_fps_ticksGet();

    A_LIST_ITERATE(g_runningTimers, ATimer*, t) {
        if(!(t->flags & A_TIMER__RUNNING)) {
            // Kick out timer that was marked as not running last frame
            A_LIST_REMOVE_CURRENT();
            t->runningListNode = NULL;

            A_FLAG_CLEAR(t->flags, A_TIMER__EXPIRED);

            continue;
        }

        t->diff = getNow(t) - t->start;

        if(t->diff >= t->period) {
            A_FLAG_SET(t->flags, A_TIMER__EXPIRED);

            if(t->flags & A_TIMER__REPEAT) {
                if(t->period > 0) {
                    t->start += (t->diff / t->period) * t->period;
                }
            } else {
                t->diff = 0;

                // Will be kicked out of running list next frame
                A_FLAG_CLEAR(t->flags, A_TIMER__RUNNING);
            }
        } else {
            A_FLAG_CLEAR(t->flags, A_TIMER__EXPIRED);
        }
    }
}

ATimer* a_timer_new(ATimerType Type, unsigned Period, bool Repeat)
{
    ATimer* t = a_mem_malloc(sizeof(ATimer));

    if(Type == A_TIMER_SEC) {
        Period *= 1000;
    }

    t->type = Type;
    t->flags = 0;
    t->period = Period;
    t->start = 0;
    t->diff = 0;
    t->runningListNode = NULL;

    if(Repeat) {
        A_FLAG_SET(t->flags, A_TIMER__REPEAT);
    }

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

    free(Timer);
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

    if(Period == 0 && Timer->flags & A_TIMER__RUNNING) {
        A_FLAG_SET(Timer->flags, A_TIMER__EXPIRED);
    }
}

void a_timer_start(ATimer* Timer)
{
    Timer->start = getNow(Timer);
    Timer->diff = 0;

    A_FLAG_SET(Timer->flags, A_TIMER__RUNNING);

    if(Timer->period == 0) {
        A_FLAG_SET(Timer->flags, A_TIMER__EXPIRED);
    } else {
        A_FLAG_CLEAR(Timer->flags, A_TIMER__EXPIRED);
    }

    if(Timer->runningListNode == NULL) {
        Timer->runningListNode = a_list_addLast(g_runningTimers, Timer);
    }
}

void a_timer_stop(ATimer* Timer)
{
    Timer->diff = 0;

    A_FLAG_CLEAR(Timer->flags, A_TIMER__RUNNING | A_TIMER__EXPIRED);

    if(Timer->runningListNode) {
        a_list_removeNode(Timer->runningListNode);
        Timer->runningListNode = NULL;
    }
}

bool a_timer_isRunning(const ATimer* Timer)
{
    return Timer->flags & A_TIMER__RUNNING;
}

bool a_timer_isExpired(const ATimer* Timer)
{
    return Timer->flags & A_TIMER__EXPIRED;
}
