/*
    Copyright 2010 Alex Margarit

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

struct Timer {
    bool running;
    uint32_t period;
    uint32_t start;
    uint32_t diff;
};

Timer* a_timer_set(const uint32_t milisPeriod)
{
    Timer* const t = malloc(sizeof(Timer));

    t->running = false;
    t->period = milisPeriod;
    t->start = 0;
    t->diff = 0;

    return t;
}

void a_timer_free(Timer* const t)
{
    free(t);
}

bool a_timer_check(Timer* const t)
{
    if(t->running) {
        t->diff = a_time_getMilis() - t->start;

        if(t->diff >= t->period) {
            t->start += (t->diff / t->period) * t->period;
            return true;
        }
    }

    return false;
}

uint32_t a_timer_diff(Timer* const t)
{
    return t->diff;
}

void a_timer_start(Timer* const t)
{
    t->running = true;
    t->start = a_time_getMilis();
}
