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

#include "a2x_pack_frametimer.v.h"

struct FrameTimer {
    bool running;
    uint32_t period;
    uint32_t start;
    uint32_t diff;
};

FrameTimer* a_frametimer_new(const uint32_t framesPeriod)
{
    FrameTimer* const t = malloc(sizeof(FrameTimer));

    t->running = false;
    t->period = framesPeriod;
    t->start = 0;
    t->diff = 0;

    return t;
}

void a_frametimer_free(FrameTimer* const t)
{
    free(t);
}

bool a_frametimer_check(FrameTimer* const t)
{
    if(t->running) {
        t->diff = a_fps_getCounter() - t->start;

        if(t->diff >= t->period) {
            t->start += (t->diff / t->period) * t->period;
            return true;
        }
    }

    return false;
}

uint32_t a_frametimer_diff(FrameTimer* const t)
{
    return t->diff;
}

void a_frametimer_start(FrameTimer* const t)
{
    t->running = true;
    t->start = a_fps_getCounter();
}
