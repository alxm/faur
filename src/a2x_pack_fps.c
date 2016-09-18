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

#define AVG_WINDOW_SECONDS 2

static uint32_t g_fpsRate;
static uint32_t g_milisPerFrame;
static ATimer* g_timer;

static uint32_t g_fps;
static uint32_t g_maxFps;
static uint32_t g_frameCounter;

static size_t g_bufferHead;
static size_t g_bufferLen;
static uint32_t* g_fpsBuffer;
static uint32_t* g_maxFpsBuffer;
static uint32_t g_fpsBufferSum;
static uint32_t g_maxFpsBufferSum;

static bool g_skipFrames;
static int g_skipMax;
static int g_skipNum;
static int g_skipCounter;
static uint32_t g_fpsThresholdFast;
static uint32_t g_fpsThresholdSlow;
static AFrameTimer* g_fastTimer;
static AFrameTimer* g_slowTimer;

void a_fps__init(void)
{
    g_fpsRate = a_settings_getInt("video.fps");
    g_milisPerFrame = 1000 / g_fpsRate;
    g_timer = a_timer_new(g_milisPerFrame);
    g_frameCounter = 0;

    g_bufferHead = 0;
    g_bufferLen = g_fpsRate * AVG_WINDOW_SECONDS;
    g_fpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));
    g_maxFpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));

    g_skipFrames = a_settings_getBool("video.fps.skip");
    g_skipMax = a_settings_getInt("video.fps.skip.max");
    g_fpsThresholdFast = g_fpsRate * 0.95;
    g_fpsThresholdSlow = g_fpsRate * 0.90;
    g_fastTimer = a_frametimer_new(g_fpsRate * AVG_WINDOW_SECONDS);
    g_slowTimer = a_frametimer_new(g_fpsRate * AVG_WINDOW_SECONDS);

    a_fps__reset();
}

void a_fps__uninit(void)
{
    a_timer_free(g_timer);
    a_frametimer_free(g_fastTimer);
    a_frametimer_free(g_slowTimer);

    free(g_fpsBuffer);
    free(g_maxFpsBuffer);
}

void a_fps__reset(void)
{
    g_fps = g_fpsRate;
    g_maxFps = g_fpsRate;

    for(int i = g_bufferLen; i--; ) {
        g_fpsBuffer[i] = g_milisPerFrame;
        g_maxFpsBuffer[i] = g_milisPerFrame;
    }

    g_fpsBufferSum = g_milisPerFrame * g_bufferLen;
    g_maxFpsBufferSum = g_milisPerFrame * g_bufferLen;

    g_skipNum = 0;
    g_skipCounter = 0;

    a_frametimer_stop(g_fastTimer);
    a_frametimer_stop(g_slowTimer);
    a_timer_start(g_timer);
}

void a_fps_frame(void)
{
    if(g_skipCounter == g_skipNum) {
        a_screen_show();
    }

    const bool done = a_timer_check(g_timer);

    g_maxFpsBufferSum -= g_maxFpsBuffer[g_bufferHead];
    g_maxFpsBuffer[g_bufferHead] = a_timer_diff(g_timer);
    g_maxFpsBufferSum += g_maxFpsBuffer[g_bufferHead];
    g_maxFps = 1000 / ((float)g_maxFpsBufferSum / g_bufferLen);

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

    g_fpsBufferSum -= g_fpsBuffer[g_bufferHead];
    g_fpsBuffer[g_bufferHead] = a_timer_diff(g_timer);
    g_fpsBufferSum += g_fpsBuffer[g_bufferHead];
    g_fps = 1000 / ((float)g_fpsBufferSum / g_bufferLen);
    g_bufferHead = (g_bufferHead + 1) % g_bufferLen;

    a_input__get();
    g_frameCounter++;

    if(g_skipFrames) {
        if(g_fps < g_fpsThresholdSlow && g_skipNum < g_skipMax) {
            a_frametimer_stop(g_fastTimer);

            if(!a_frametimer_running(g_slowTimer)
                || a_frametimer_check(g_slowTimer)) {

                g_skipNum++;
                g_skipCounter = 0;
            }

            if(!a_frametimer_running(g_slowTimer)) {
                a_frametimer_start(g_slowTimer);
            }
        } else if(g_fps > g_fpsThresholdFast && g_skipNum > 0) {
            a_frametimer_stop(g_slowTimer);

            if(!a_frametimer_running(g_fastTimer)
                || a_frametimer_check(g_fastTimer)) {

                g_skipNum--;
                g_skipCounter = 0;
            }

            if(!a_frametimer_running(g_fastTimer)) {
                a_frametimer_start(g_fastTimer);
            }
        }

        if(g_skipCounter++ == g_skipNum) {
            g_skipCounter = 0;
        }
    }
}

bool a_fps_notSkipped(void)
{
    return g_skipCounter == g_skipNum;
}

uint32_t a_fps_getFps(void)
{
    return g_fps;
}

uint32_t a_fps_getMaxFps(void)
{
    return g_maxFps;
}

int a_fps_getFrameSkip(void)
{
    return g_skipNum;
}

uint32_t a_fps_getCounter(void)
{
    return g_frameCounter;
}

bool a_fps_nthFrame(uint32_t N)
{
    return (g_frameCounter % N) == 0;
}
