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

static size_t g_bufferHead;
static size_t g_bufferLen;
static uint32_t* g_fpsBuffer;
static uint32_t* g_maxFpsBuffer;
static uint32_t g_fpsBufferSum;
static uint32_t g_maxFpsBufferSum;

static uint32_t g_frameCounter;

void a_fps__init(void)
{
    const int fps = a_settings_getInt("video.fps");

    g_milisPerFrame = 1000 / fps;

    g_timer = a_timer_new(g_milisPerFrame);
    a_timer_start(g_timer);

    g_fps = fps;
    g_maxFps = fps;

    g_bufferHead = 0;
    g_bufferLen = 5 * fps;
    g_fpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));
    g_maxFpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));

    for(int i = g_bufferLen; i--; ) {
        g_fpsBuffer[i] = g_milisPerFrame;
        g_maxFpsBuffer[i] = g_milisPerFrame;
    }

    g_fpsBufferSum = g_milisPerFrame * g_bufferLen;
    g_maxFpsBufferSum = g_milisPerFrame * g_bufferLen;

    g_frameCounter = 0;
}

void a_fps__uninit(void)
{
    a_timer_free(g_timer);
    free(g_fpsBuffer);
    free(g_maxFpsBuffer);
}

void a_fps_frame(void)
{
    a_screen_show();

    const bool track = a_settings_getBool("video.fps.track");
    const bool done = a_timer_check(g_timer);

    if(track) {
        g_maxFpsBufferSum -= g_maxFpsBuffer[g_bufferHead];
        g_maxFpsBuffer[g_bufferHead] = a_timer_diff(g_timer);
        g_maxFpsBufferSum += g_maxFpsBuffer[g_bufferHead];
        g_maxFps = 1000 / ((float)g_maxFpsBufferSum / g_bufferLen);
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
        g_fpsBufferSum -= g_fpsBuffer[g_bufferHead];
        g_fpsBuffer[g_bufferHead] = a_timer_diff(g_timer);
        g_fpsBufferSum += g_fpsBuffer[g_bufferHead];
        g_fps = 1000 / ((float)g_fpsBufferSum / g_bufferLen);
        g_bufferHead = (g_bufferHead + 1) % g_bufferLen;
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
