/*
    Copyright 2011, 2016, 2017 Alex Margarit

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

struct ATimer {
    ATimerType type;
    unsigned period;
    unsigned start;
    unsigned diff;
    bool running;
};

static inline unsigned getNow(const ATimer* Timer)
{
    switch(Timer->type) {
        case A_TIMER_MS:
            return a_time_getMs();

        case A_TIMER_SEC:
            return a_time_getMs() / 1000;

        case A_TIMER_FRAMES:
            return a_fps_getCounter();

        default:
            return 0;
    }
}

ATimer* a_timer_new(ATimerType Type, unsigned Period)
{
    ATimer* t = a_mem_malloc(sizeof(ATimer));

    t->type = Type;
    t->period = a_math_maxu(Period, 1);
    t->start = 0;
    t->diff = 0;
    t->running = false;

    return t;
}

void a_timer_free(ATimer* Timer)
{
    free(Timer);
}

void a_timer_start(ATimer* Timer)
{
    Timer->start = getNow(Timer);
    Timer->diff = 0;
    Timer->running = true;
}

void a_timer_stop(ATimer* Timer)
{
    Timer->diff = 0;
    Timer->running = false;
}

bool a_timer_running(ATimer* Timer)
{
    return Timer->running;
}

bool a_timer_expired(ATimer* Timer)
{
    if(Timer->running) {
        Timer->diff = getNow(Timer) - Timer->start;

        if(Timer->diff >= Timer->period) {
            Timer->start += (Timer->diff / Timer->period) * Timer->period;
            return true;
        }
    }

    return false;
}

unsigned a_timer_elapsed(ATimer* Timer)
{
    return Timer->diff;
}

void a_timer_setPeriod(ATimer* Timer, unsigned Period)
{
    Timer->period = a_math_maxu(Period, 1);
}
