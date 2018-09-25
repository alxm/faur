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
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_time.v.h"

typedef enum {
    A_TIMER__REPEAT = A_FLAG_BIT(0),
    A_TIMER__RUNNING = A_FLAG_BIT(1),
} ATimerFlags;

struct ATimer {
    ATimerType type;
    ATimerFlags flags;
    unsigned period;
    unsigned start;
    unsigned diff;
};

static inline unsigned getNow(const ATimer* Timer)
{
    switch(Timer->type) {
        case A_TIMER_MS:
        case A_TIMER_SEC:
            return a_time_msGet();

        case A_TIMER_TICKS:
            return a_fps_ticksGet();

        default:
            return 0;
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
    t->period = a_math_maxu(Period, 1);
    t->start = 0;
    t->diff = 0;

    if(Repeat) {
        A_FLAG_SET(t->flags, A_TIMER__REPEAT);
    }

    return t;
}

void a_timer_free(ATimer* Timer)
{
    free(Timer);
}

unsigned a_timer_elapsedGet(ATimer* Timer)
{
    return Timer->diff;
}

void a_timer_periodSet(ATimer* Timer, unsigned Period)
{
    if(Timer->type == A_TIMER_SEC) {
        Period *= 1000;
    }

    Timer->period = a_math_maxu(Period, 1);
}

void a_timer_start(ATimer* Timer)
{
    Timer->start = getNow(Timer);
    Timer->diff = 0;

    A_FLAG_SET(Timer->flags, A_TIMER__RUNNING);
}

void a_timer_stop(ATimer* Timer)
{
    Timer->diff = 0;

    A_FLAG_CLEAR(Timer->flags, A_TIMER__RUNNING);
}

bool a_timer_isRunning(ATimer* Timer)
{
    return Timer->flags & A_TIMER__RUNNING;
}

bool a_timer_isExpired(ATimer* Timer)
{
    bool expired = false;

    if(Timer->flags & A_TIMER__RUNNING) {
        Timer->diff = getNow(Timer) - Timer->start;

        if(Timer->diff >= Timer->period) {
            expired = true;

            if(Timer->flags & A_TIMER__REPEAT) {
                Timer->start += (Timer->diff / Timer->period) * Timer->period;
            } else {
                a_timer_stop(Timer);
            }
        }
    }

    return expired;
}
