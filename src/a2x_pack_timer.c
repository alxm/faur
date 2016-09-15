/*
    Copyright 2011, 2016 Alex Margarit

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
    bool running;
    uint32_t period;
    uint32_t start;
    uint32_t diff;
};

ATimer* a_timer_new(uint32_t MilisPeriod)
{
    ATimer* const t = a_mem_malloc(sizeof(ATimer));

    t->running = false;
    t->period = MilisPeriod;
    t->start = 0;
    t->diff = 0;

    return t;
}

void a_timer_free(ATimer* Timer)
{
    free(Timer);
}

void a_timer_start(ATimer* Timer)
{
    Timer->running = true;
    Timer->start = a_time_getMilis();
}

void a_timer_stop(ATimer* Timer)
{
    Timer->running = false;
}

bool a_timer_running(ATimer* Timer)
{
    return Timer->running;
}

bool a_timer_check(ATimer* Timer)
{
    if(Timer->running) {
        Timer->diff = a_time_getMilis() - Timer->start;

        if(Timer->diff >= Timer->period) {
            Timer->start += (Timer->diff / Timer->period) * Timer->period;
            return true;
        }
    }

    return false;
}

uint32_t a_timer_diff(ATimer* Timer)
{
    return Timer->diff;
}
