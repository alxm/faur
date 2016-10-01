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

#define AVERAGE_WINDOW_SEC 2
#define FRAMESKIP_ADJUST_DELAY_SEC 2
#define NO_SLEEP_RESET_SEC 20

static uint32_t g_idealFps;
static bool g_skipFrames;
static int g_skipMax;

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

static int g_skipNum;
static int g_skipCounter;
static uint32_t g_fpsThresholdFast;
static uint32_t g_fpsThresholdSlow;
static ATimer* g_skipAdjustTimer;
static ATimer* g_noSleepTimer;
static bool g_canSleep;

void a_fps__init(void)
{
    g_idealFps = a_settings_getInt("video.fps");
    g_skipFrames = a_settings_getBool("video.fps.skip");
    g_skipMax = a_settings_getInt("video.fps.skip.max");

    g_timer = NULL;
    g_frameCounter = 0;
    g_bufferHead = 0;
    g_bufferLen = g_idealFps * AVERAGE_WINDOW_SEC;
    g_fpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));
    g_maxFpsBuffer = a_mem_malloc(g_bufferLen * sizeof(uint32_t));

    g_skipAdjustTimer = a_timer_new(FRAMESKIP_ADJUST_DELAY_SEC * 1000);
    a_timer_start(g_skipAdjustTimer);

    g_noSleepTimer = a_timer_new(NO_SLEEP_RESET_SEC * 1000);
    g_canSleep = true;

    a_fps__reset(0);
}

void a_fps__uninit(void)
{
    a_timer_free(g_timer);
    a_timer_free(g_skipAdjustTimer);
    a_timer_free(g_noSleepTimer);

    free(g_fpsBuffer);
    free(g_maxFpsBuffer);
}

void a_fps__reset(int NumFramesToSkip)
{
    g_skipNum = NumFramesToSkip;
    g_skipCounter = 0;

    g_fpsRate = g_idealFps / (1 + g_skipNum);
    g_milisPerFrame = 1000 / g_fpsRate;

    g_fps = g_fpsRate;
    g_maxFps = g_fpsRate;

    for(int i = g_bufferLen; i--; ) {
        g_fpsBuffer[i] = g_fpsRate;
        g_maxFpsBuffer[i] = g_fpsRate;
    }

    g_fpsBufferSum = g_fpsRate * g_bufferLen;
    g_maxFpsBufferSum = g_fpsRate * g_bufferLen;

    if(g_skipNum > 0) {
        g_fpsThresholdFast = g_idealFps / g_skipNum;
    }

    g_fpsThresholdSlow = g_fpsRate - 2;

    if(g_timer == NULL) {
        g_timer = a_timer_new(g_milisPerFrame);
    } else {
        a_timer_setPeriod(g_timer, g_milisPerFrame);
    }

    a_timer_start(g_timer);
}

void a_fps_frame(void)
{
    if(a_fps_notSkipped()) {
        a_screen_show();

        const bool done = a_timer_check(g_timer);
        const uint32_t elapsedMs = a_timer_diff(g_timer);

        if(elapsedMs > 0) {
            g_maxFpsBufferSum -= g_maxFpsBuffer[g_bufferHead];
            g_maxFpsBuffer[g_bufferHead] = 1000 / elapsedMs;
            g_maxFpsBufferSum += g_maxFpsBuffer[g_bufferHead];
            g_maxFps = g_maxFpsBufferSum / g_bufferLen;
        }

        if(!done) {
            while(!a_timer_check(g_timer)) {
                if(!g_canSleep) {
                    continue;
                }

                const uint32_t waitMs = g_milisPerFrame - a_timer_diff(g_timer);

                #if A_PLATFORM_GP2X
                    // GP2X timer granularity is too coarse
                    if(waitMs >= 10) {
                        a_time_waitMilis(10);
                    }
                #else
                    a_time_waitMilis(waitMs);
                #endif
            }
        }

        g_fpsBufferSum -= g_fpsBuffer[g_bufferHead];
        g_fpsBuffer[g_bufferHead] = 1000 / a_timer_diff(g_timer);
        g_fpsBufferSum += g_fpsBuffer[g_bufferHead];
        g_fps = g_fpsBufferSum / g_bufferLen;

        g_bufferHead = (g_bufferHead + 1) % g_bufferLen;
    }

    a_input__get();
    g_frameCounter++;

    if(g_skipFrames) {
        if(a_fps_notSkipped() && a_timer_check(g_skipAdjustTimer)) {
            int newFrameSkip = -1;

            if(g_maxFps <= g_fpsThresholdSlow && g_skipNum < g_skipMax) {
                newFrameSkip = g_skipNum + 1;
            } else if(g_maxFps >= g_fpsThresholdFast && g_skipNum > 0) {
                newFrameSkip = g_skipNum - 1;
            }

            if(newFrameSkip != -1) {
                if(newFrameSkip == 0) {
                    a_timer_start(g_noSleepTimer);
                } else {
                    a_timer_stop(g_noSleepTimer);
                }

                g_canSleep = false;
                a_fps__reset(newFrameSkip);
            } else if(!g_canSleep && a_timer_check(g_noSleepTimer)) {
                g_canSleep = true;
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
