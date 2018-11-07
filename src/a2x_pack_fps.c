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

#include "a2x_pack_fps.v.h"

#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_time.v.h"

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

void a_fps__init(void)
{
    unsigned fpsTick = a_settings_intuGet(A_SETTING_FPS_TICK);
    unsigned fpsDraw = a_settings_intuGet(A_SETTING_FPS_DRAW);

    if(fpsTick < 1) {
        a_out__fatal("%s < 1", a_settings__idToString(A_SETTING_FPS_TICK));
    }

    if(fpsDraw < 1) {
        a_out__fatal("%s < 1", a_settings__idToString(A_SETTING_FPS_DRAW));
    }

    if(fpsTick < fpsDraw) {
        a_out__warning("Changing %s from %u to %u",
                       a_settings__idToString(A_SETTING_FPS_DRAW),
                       fpsDraw,
                       fpsTick);

        fpsDraw = fpsTick;
        a_settings_intuSet(A_SETTING_FPS_DRAW, fpsDraw);
    }

    g_settings.tickFrameMs = 1000 / fpsTick;
    g_settings.drawFrameMs = 1000 / fpsDraw;

    g_history.head = 0;
    g_history.len = fpsDraw * A__AVERAGE_WINDOW_SEC;
    g_history.drawFrameMs = a_mem_malloc(g_history.len * sizeof(unsigned));
    g_history.drawFrameMsMin = a_mem_malloc(g_history.len * sizeof(unsigned));

    g_run.frameCounter = 0;

    a_fps__reset();
}

void a_fps__uninit(void)
{
    free(g_history.drawFrameMs);
    free(g_history.drawFrameMsMin);
}

void a_fps__reset(void)
{
    g_run.drawFps = a_settings_intuGet(A_SETTING_FPS_DRAW);
    g_run.drawFpsMax = g_run.drawFps;

    for(unsigned i = g_history.len; i--; ) {
        g_history.drawFrameMs[i] = g_settings.drawFrameMs;
        g_history.drawFrameMsMin[i] = g_settings.drawFrameMs;
    }

    g_history.drawFrameMsSum =
        g_history.len * 1000 / a_settings_intuGet(A_SETTING_FPS_DRAW);
    g_history.drawFrameMsMinSum = g_history.drawFrameMsSum;

    g_run.lastFrameMs = a_time_msGet();
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
    uint32_t nowMs = a_time_msGet();
    uint32_t elapsedMs = nowMs - g_run.lastFrameMs;

    if(elapsedMs > 0) {
        g_history.drawFrameMsMinSum -= g_history.drawFrameMsMin[g_history.head];
        g_history.drawFrameMsMin[g_history.head] = elapsedMs;
        g_history.drawFrameMsMinSum += elapsedMs;
        g_run.drawFpsMax = g_history.len * 1000 / g_history.drawFrameMsMinSum;
    }

    if(!a_settings_boolGet(A_SETTING_VIDEO_VSYNC)) {
        while(elapsedMs < g_settings.drawFrameMs) {
            a_time_msWait(g_settings.drawFrameMs - elapsedMs);

            nowMs = a_time_msGet();
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
    g_run.tickCreditMs += a_math_minu(elapsedMs, g_settings.drawFrameMs * 2);
}

unsigned a_fps_drawRateGet(void)
{
    return g_run.drawFps;
}

unsigned a_fps_drawRateGetMax(void)
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
