/*
    Copyright 2010, 2016 Alex Margarit

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

static uint32_t g_milisPerFrame;
static ATimer* g_timer;

static uint32_t g_fps;
static uint32_t g_maxFps;

#define BUFFER_SIZE 60
static uint32_t g_fpsBuffer[BUFFER_SIZE];
static uint32_t g_maxFpsBuffer[BUFFER_SIZE];

static uint32_t g_frameCounter;

void a_fps__init(void)
{
    g_milisPerFrame = 1000 / a_settings_getInt("fps.rate");

    g_timer = a_timer_new(g_milisPerFrame);
    a_timer_start(g_timer);

    g_fps = 0;
    g_maxFps = 0;

    for(int i = BUFFER_SIZE; i--; ) {
        g_fpsBuffer[i] = g_milisPerFrame;
        g_maxFpsBuffer[i] = g_milisPerFrame;
    }

    g_frameCounter = 0;
}

void a_fps__uninit(void)
{
    a_timer_free(g_timer);
}

void a_fps_frame(void)
{
    a_screen_show();

    const bool track = a_settings_getBool("fps.track");
    const bool done = a_timer_check(g_timer);

    if(track) {
        g_maxFpsBuffer[BUFFER_SIZE - 1] = a_timer_diff(g_timer);
    } else {
        g_maxFps = 1000 / a_math_max(1, a_timer_diff(g_timer));
    }

    if(!done) {
        while(!a_timer_check(g_timer)) {
            const uint32_t waitMilis = g_milisPerFrame - a_timer_diff(g_timer);

            #if A_PLATFORM_GP2X
                // GP2X timer granularity is too coarse
                if(waitMilis >= 10) {
                    a_time_waitMilis(10);
                }
            #else
                a_time_waitMilis(waitMilis);
            #endif
        }
    }

    if(track) {
        uint32_t f = 0;
        uint32_t m = 0;

        for(int i = BUFFER_SIZE; i--; ) {
            f += g_fpsBuffer[i];
            m += g_maxFpsBuffer[i];
        }

        g_fps = 1000 / ((float)f / BUFFER_SIZE);
        g_maxFps = 1000 / ((float)m / BUFFER_SIZE);

        memmove(g_fpsBuffer, &g_fpsBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));
        memmove(g_maxFpsBuffer, &g_maxFpsBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));

        g_fpsBuffer[BUFFER_SIZE - 1] = a_timer_diff(g_timer);
    } else {
        g_fps = 1000 / a_math_max(1, a_timer_diff(g_timer));
    }

    a_input__get();
    g_frameCounter++;
}

uint32_t a_fps_getFps(void)
{
    return g_fps;
}

uint32_t a_fps_getMaxFps(void)
{
    return g_maxFps;
}

uint32_t a_fps_getCounter(void)
{
    return g_frameCounter;
}

bool a_fps_nthFrame(uint32_t N)
{
    return (g_frameCounter % N) == 0;
}
