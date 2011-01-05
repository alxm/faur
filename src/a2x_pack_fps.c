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

#include "a2x_pack_fps.v.h"

static uint32_t milisPerFrame;
static Timer* timer;

static uint32_t fps;
static uint32_t max;

#define BUFFER_SIZE 60
static uint32_t fpsBuffer[BUFFER_SIZE];
static uint32_t maxBuffer[BUFFER_SIZE];

static uint32_t counter;

void a_fps__set(void)
{
    milisPerFrame = 1000 / a2x_int("fps");

    timer = a_timer_set(milisPerFrame);
    a_timer_start(timer);

    fps = 0;
    max = 0;

    if(a2x_bool("trackFps")) {
        for(int i = BUFFER_SIZE; i--; ) {
            fpsBuffer[i] = milisPerFrame;
            maxBuffer[i] = milisPerFrame;
        }
    }

    counter = 0;
}

void a_fps__free(void)
{
    a_timer_free(timer);
}

void a_fps_start(void)
{
    a_input__get();
    a_sound_adjustVolume();
}

void a_fps_end(void)
{
    counter++;

    a_screen_show();

    const bool track = a2x_bool("trackFps");
    const bool done = a_timer_check(timer);

    if(track) {
        maxBuffer[BUFFER_SIZE - 1] = a_timer_diff(timer);
    } else {
        max = 1000 / a_math_max(1, a_timer_diff(timer));
    }

    if(!done) {
        while(!a_timer_check(timer)) {
            if(milisPerFrame - a_timer_diff(timer) >= 10) {
                a_time_waitMilis(10);
            }
        }
    }

    if(track) {
        uint32_t f = 0;
        uint32_t m = 0;

        for(int i = BUFFER_SIZE; i--; ) {
            f += fpsBuffer[i];
            m += maxBuffer[i];
        }

        fps = 1000 / ((float)f / BUFFER_SIZE);
        max = 1000 / ((float)m / BUFFER_SIZE);

        memmove(fpsBuffer, &fpsBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));
        memmove(maxBuffer, &maxBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));

        fpsBuffer[BUFFER_SIZE - 1] = a_timer_diff(timer);
    } else {
        fps = 1000 / a_math_max(1, a_timer_diff(timer));
    }
}

uint32_t a_fps_getFps(void)
{
    return fps;
}

uint32_t a_fps_getMaxFps(void)
{
    return max;
}

uint32_t a_fps_getCounter(void)
{
    return counter;
}
