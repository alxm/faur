/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_system_includes.h"

#include "a2x_pack_fps.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_time.v.h"
#include "a2x_pack_timer.v.h"

#define AVERAGE_WINDOW_SEC 2
#define FRAMESKIP_ADJUST_DELAY_SEC 2
#define NO_SLEEP_RESET_SEC 20

static unsigned g_tickRate;
static unsigned g_drawRate;
static bool g_skipFrames;
static unsigned g_skipMax;
static bool g_vsync;

static unsigned g_skipNum;
static unsigned g_fpsRate;
static unsigned g_msPerFrame;

static unsigned g_fps;
static unsigned g_maxFps;
static unsigned g_frameCounter;

static unsigned g_bufferHead;
static unsigned g_bufferLen;
static unsigned* g_fpsBuffer;
static unsigned* g_maxFpsBuffer;
static unsigned g_fpsBufferSum;
static unsigned g_maxFpsBufferSum;

static unsigned g_fpsThresholdFast;
static unsigned g_fpsThresholdSlow;
static ATimer* g_skipAdjustTimer;
static bool g_allowSleep;
static ATimer* g_noSleepTimer;
static bool g_canSleep;

static uint32_t g_lastMs;
static unsigned g_tickCredit;

void a_fps__init(void)
{
    g_tickRate = a_settings_getUnsigned("fps.tick");
    g_drawRate = a_settings_getUnsigned("fps.draw");
    g_skipFrames = a_settings_getBool("fps.draw.skip");
    g_skipMax = a_settings_getUnsigned("fps.draw.skip.max");
    g_vsync = a_settings_getBool("video.vsync");

    if(g_tickRate < 1) {
        a_out__fatal("Invalid setting fps.tick=0");
    }

    if(g_drawRate < 1) {
        a_out__fatal("Invalid setting fps.draw=0");
    } else if(g_skipMax >= g_drawRate) {
        a_out__fatal("Invalid setting fps.draw.skip.max=%u for fps.draw=%u",
                     g_skipMax,
                     g_drawRate);
    }

    g_frameCounter = 0;

    g_bufferHead = 0;
    g_bufferLen = g_drawRate * AVERAGE_WINDOW_SEC;
    g_fpsBuffer = a_mem_malloc(g_bufferLen * sizeof(unsigned));
    g_maxFpsBuffer = a_mem_malloc(g_bufferLen * sizeof(unsigned));

    g_skipAdjustTimer = a_timer_new(A_TIMER_SEC, FRAMESKIP_ADJUST_DELAY_SEC);
    a_timer_start(g_skipAdjustTimer);

    #if A_PLATFORM_SYSTEM_EMSCRIPTEN
        g_allowSleep = false;
    #else
        g_allowSleep = true;
    #endif

    g_noSleepTimer = a_timer_new(A_TIMER_SEC, NO_SLEEP_RESET_SEC);
    g_canSleep = g_allowSleep;

    a_fps__reset(0);
}

void a_fps__uninit(void)
{
    a_timer_free(g_skipAdjustTimer);
    a_timer_free(g_noSleepTimer);

    free(g_fpsBuffer);
    free(g_maxFpsBuffer);
}

void a_fps__reset(unsigned NumFramesToSkip)
{
    g_skipNum = NumFramesToSkip;
    g_fpsRate = g_drawRate / (1 + g_skipNum);
    g_msPerFrame = 1000 / g_fpsRate;

    g_fps = g_fpsRate;
    g_maxFps = g_fpsRate;

    for(unsigned i = g_bufferLen; i--; ) {
        g_fpsBuffer[i] = g_fpsRate;
        g_maxFpsBuffer[i] = g_fpsRate;
    }

    g_fpsBufferSum = g_fpsRate * g_bufferLen;
    g_maxFpsBufferSum = g_fpsRate * g_bufferLen;

    if(g_skipNum > 0) {
        g_fpsThresholdFast = g_drawRate / g_skipNum;
    }

    g_fpsThresholdSlow = (g_fpsRate > 3) ? (g_fpsRate - 2) : 1;

    g_lastMs = a_time_getMs();
    g_tickCredit = 1000;
}

bool a_fps__tick(void)
{
    if(g_tickCredit >= 1000) {
        g_tickCredit -= 1000;
        g_frameCounter++;

        return true;
    }

    return false;
}

void a_fps__frame(void)
{
    uint32_t nowMs = a_time_getMs();
    uint32_t elapsedMs = nowMs - g_lastMs;

    if(elapsedMs > 0) {
        g_maxFpsBufferSum -= g_maxFpsBuffer[g_bufferHead];
        g_maxFpsBuffer[g_bufferHead] = 1000 / elapsedMs;
        g_maxFpsBufferSum += g_maxFpsBuffer[g_bufferHead];
        g_maxFps = g_maxFpsBufferSum / g_bufferLen;
    }

    if(!g_vsync) {
        while(elapsedMs < g_msPerFrame) {
            if(g_canSleep) {
                uint32_t waitMs = g_msPerFrame - elapsedMs;

                #if A_PLATFORM_SYSTEM_GP2X
                    // GP2X timer granularity is too coarse
                    if(waitMs >= 10) {
                        a_time_waitMs(10);
                    }
                #else
                    a_time_waitMs(waitMs);
                #endif
            }

            nowMs = a_time_getMs();
            elapsedMs = nowMs - g_lastMs;
        }
    }

    if(elapsedMs > 0) {
        g_fpsBufferSum -= g_fpsBuffer[g_bufferHead];
        g_fpsBuffer[g_bufferHead] = 1000 / elapsedMs;
        g_fpsBufferSum += g_fpsBuffer[g_bufferHead];
        g_fps = g_fpsBufferSum / g_bufferLen;
    }

    g_bufferHead = (g_bufferHead + 1) % g_bufferLen;
    g_lastMs = nowMs;

    if(!g_vsync && g_skipFrames && a_timer_isExpired(g_skipAdjustTimer)) {
        unsigned newFrameSkip = UINT_MAX;

        if(g_maxFps <= g_fpsThresholdSlow && g_skipNum < g_skipMax) {
            newFrameSkip = g_skipNum + 1;
        } else if(g_maxFps >= g_fpsThresholdFast && g_skipNum > 0) {
            newFrameSkip = g_skipNum - 1;
        }

        if(newFrameSkip != UINT_MAX) {
            if(newFrameSkip == 0) {
                a_timer_start(g_noSleepTimer);
            } else {
                a_timer_stop(g_noSleepTimer);
            }

            g_canSleep = false;
            a_fps__reset(newFrameSkip);
        } else if(!g_canSleep && a_timer_isExpired(g_noSleepTimer)) {
            g_canSleep = g_allowSleep;
        }
    }

    g_tickCredit += elapsedMs * g_tickRate;
}

unsigned a_fps_getFps(void)
{
    return g_fps;
}

unsigned a_fps_getMaxFps(void)
{
    return g_maxFps;
}

unsigned a_fps_getFrameSkip(void)
{
    return g_skipNum;
}

unsigned a_fps_getCounter(void)
{
    return g_frameCounter;
}

bool a_fps_isNthFrame(unsigned N)
{
    return (g_frameCounter % N) == 0;
}

unsigned a_fps_msToFrames(unsigned Ms)
{
    return Ms * g_tickRate / 1000;
}
