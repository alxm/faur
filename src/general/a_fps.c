/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_fps.v.h"
#include <a2x.v.h>

#define A__AVERAGE_WINDOW_SEC 2

static struct {
    unsigned tickFrameMs;
    unsigned drawFrameMs;
} g_settings;

static struct {
    unsigned head;
    unsigned len;
    unsigned* drawFrameMs;
    unsigned drawFrameMsSum;
    unsigned* drawFrameMsMin;
    unsigned drawFrameMsMinSum;
} g_history;

static struct {
    unsigned drawFps;
    unsigned drawFpsMax;
    unsigned frameCounter;
    uint32_t lastFrameMs;
    unsigned tickCreditMs;
} g_run;

static void a_fps__init(void)
{
    g_settings.tickFrameMs = 1000 / A_CONFIG_FPS_RATE_TICK;
    g_settings.drawFrameMs = 1000 / A_CONFIG_FPS_RATE_DRAW;

    g_history.head = 0;
    g_history.len = A_CONFIG_FPS_RATE_DRAW * A__AVERAGE_WINDOW_SEC;
    g_history.drawFrameMs = a_mem_malloc(g_history.len * sizeof(unsigned));
    g_history.drawFrameMsMin = a_mem_malloc(g_history.len * sizeof(unsigned));

    g_run.frameCounter = 0;

    a_fps__reset();
}

static void a_fps__uninit(void)
{
    free(g_history.drawFrameMs);
    free(g_history.drawFrameMsMin);
}

const APack a_pack__fps = {
    "FPS",
    {
        [0] = a_fps__init,
    },
    {
        [0] = a_fps__uninit,
    },
};

void a_fps__reset(void)
{
    g_run.drawFps = A_CONFIG_FPS_RATE_DRAW;
    g_run.drawFpsMax = g_run.drawFps;

    for(unsigned i = g_history.len; i--; ) {
        g_history.drawFrameMs[i] = g_settings.drawFrameMs;
        g_history.drawFrameMsMin[i] = g_settings.drawFrameMs;
    }

    g_history.drawFrameMsSum = g_history.len * 1000 / A_CONFIG_FPS_RATE_DRAW;
    g_history.drawFrameMsMinSum = g_history.drawFrameMsSum;

    g_run.lastFrameMs = a_time_getMs();
    g_run.tickCreditMs = g_settings.tickFrameMs;
}

bool a_fps__tick(void)
{
    if(g_run.tickCreditMs >= g_settings.tickFrameMs) {
        g_run.tickCreditMs -= g_settings.tickFrameMs;
        g_run.frameCounter++;

        return true;
    }

    return false;
}

void a_fps__frame(void)
{
    uint32_t nowMs = a_time_getMs();
    uint32_t elapsedMs = nowMs - g_run.lastFrameMs;

    if(elapsedMs > 0) {
        g_history.drawFrameMsMinSum -= g_history.drawFrameMsMin[g_history.head];
        g_history.drawFrameMsMin[g_history.head] = elapsedMs;
        g_history.drawFrameMsMinSum += elapsedMs;
        g_run.drawFpsMax = g_history.len * 1000 / g_history.drawFrameMsMinSum;
    }

    if(!a_platform_api__screenVsyncGet()) {
        while(elapsedMs < g_settings.drawFrameMs) {
            a_time_waitMs(g_settings.drawFrameMs - elapsedMs);

            nowMs = a_time_getMs();
            elapsedMs = nowMs - g_run.lastFrameMs;
        }
    }

    if(elapsedMs > 0) {
        g_history.drawFrameMsSum -= g_history.drawFrameMs[g_history.head];
        g_history.drawFrameMs[g_history.head] = elapsedMs;
        g_history.drawFrameMsSum += g_history.drawFrameMs[g_history.head];
        g_run.drawFps = g_history.len * 1000 / g_history.drawFrameMsSum;
    }

    g_history.head = (g_history.head + 1) % g_history.len;

    g_run.lastFrameMs = nowMs;

    #if A_CONFIG_FPS_CAP_LAG
        g_run.tickCreditMs += a_math_minu(
                                elapsedMs, g_settings.drawFrameMs * 2);
    #else
        g_run.tickCreditMs += elapsedMs;
    #endif
}

unsigned a_fps_rateDrawGet(void)
{
    return g_run.drawFps;
}

unsigned a_fps_rateDrawGetMax(void)
{
    return g_run.drawFpsMax;
}

unsigned a_fps_ticksGet(void)
{
    return g_run.frameCounter;
}

bool a_fps_ticksNth(unsigned N)
{
    return (g_run.frameCounter % N) == 0;
}

AFix a_fps_ticksSin(uint8_t Mul, uint8_t Div, unsigned Offset)
{
    uint64_t param = (uint64_t)g_run.frameCounter * Mul * A_FIX_ANGLES_NUM;

    if(Div > 1) {
        param /= Div;
    }

    return a_fix_sin((unsigned)(param / A_CONFIG_FPS_RATE_TICK) + Offset);
}
