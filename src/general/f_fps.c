/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_fps.v.h"
#include <faur.v.h>

static const struct {
    unsigned tickFrameMs;
    unsigned drawFrameMs;
} g_settings = {
    1000 / F_CONFIG_FPS_RATE_TICK,
    1000 / F_CONFIG_FPS_RATE_DRAW,
};

static struct {
    unsigned head;
    unsigned drawFrameMs[F_CONFIG_FPS_HISTORY];
    unsigned drawFrameMsSum;
    unsigned drawFrameMsMin[F_CONFIG_FPS_HISTORY];
    unsigned drawFrameMsMinSum;
} g_history;

static struct {
    unsigned drawFps;
    unsigned drawFpsMax;
    unsigned frameCounter;
    uint32_t lastFrameMs;
    unsigned tickCreditMs;
} g_run;

static void f_fps__init(void)
{
    f_fps__reset();
}

const FPack f_pack__fps = {
    "FPS",
    {
        [0] = f_fps__init,
    },
    {
        NULL,
    }
};

void f_fps__reset(void)
{
    g_run.drawFps = F_CONFIG_FPS_RATE_DRAW;
    g_run.drawFpsMax = g_run.drawFps;

    for(int i = F_CONFIG_FPS_HISTORY; i--; ) {
        g_history.drawFrameMs[i] = g_settings.drawFrameMs;
        g_history.drawFrameMsMin[i] = g_settings.drawFrameMs;
    }

    g_history.drawFrameMsSum = F_CONFIG_FPS_HISTORY * 1000
                                / F_CONFIG_FPS_RATE_DRAW;
    g_history.drawFrameMsMinSum = g_history.drawFrameMsSum;

    g_run.lastFrameMs = f_time_getMs();
    g_run.tickCreditMs = g_settings.tickFrameMs;
}

bool f_fps__tick(void)
{
    if(g_run.tickCreditMs >= g_settings.tickFrameMs) {
        g_run.tickCreditMs -= g_settings.tickFrameMs;
        g_run.frameCounter++;

        return true;
    }

    return false;
}

void f_fps__frame(void)
{
    uint32_t nowMs = f_time_getMs();
    uint32_t elapsedMs = nowMs - g_run.lastFrameMs;

    if(elapsedMs > 0) {
        g_history.drawFrameMsMinSum -= g_history.drawFrameMsMin[g_history.head];
        g_history.drawFrameMsMin[g_history.head] = elapsedMs;
        g_history.drawFrameMsMinSum += elapsedMs;

        g_run.drawFpsMax = F_CONFIG_FPS_HISTORY * 1000
                            / g_history.drawFrameMsMinSum;
    }

    if(!f_platform_api__screenVsyncGet()) {
        while(elapsedMs < g_settings.drawFrameMs) {
            f_time_waitMs(g_settings.drawFrameMs - elapsedMs);

            nowMs = f_time_getMs();
            elapsedMs = nowMs - g_run.lastFrameMs;
        }
    }

    if(elapsedMs > 0) {
        g_history.drawFrameMsSum -= g_history.drawFrameMs[g_history.head];
        g_history.drawFrameMs[g_history.head] = elapsedMs;
        g_history.drawFrameMsSum += g_history.drawFrameMs[g_history.head];
        g_run.drawFps = F_CONFIG_FPS_HISTORY * 1000 / g_history.drawFrameMsSum;
    }

    g_history.head = (g_history.head + 1) % F_CONFIG_FPS_HISTORY;

    g_run.lastFrameMs = nowMs;

    #if F_CONFIG_SYSTEM_GAMEBUINO
        g_run.tickCreditMs = g_settings.tickFrameMs;
    #elif F_CONFIG_FPS_CAP_LAG
        g_run.tickCreditMs += f_math_minu(
                                elapsedMs, g_settings.drawFrameMs * 2);
    #else
        g_run.tickCreditMs += elapsedMs;
    #endif
}

unsigned f_fps_rateDrawGet(void)
{
    return g_run.drawFps;
}

unsigned f_fps_rateDrawGetMax(void)
{
    return g_run.drawFpsMax;
}

unsigned f_fps_ticksGet(void)
{
    return g_run.frameCounter;
}

bool f_fps_ticksNth(unsigned N)
{
    return (g_run.frameCounter % N) == 0;
}

FFix f_fps_ticksSin(uint8_t Mul, uint8_t Div, unsigned Offset)
{
    uint64_t param = (uint64_t)g_run.frameCounter * Mul * F_FIX_ANGLES_NUM;

    if(Div > 1) {
        param /= Div;
    }

    return f_fix_sin((unsigned)(param / F_CONFIG_FPS_RATE_TICK) + Offset);
}
