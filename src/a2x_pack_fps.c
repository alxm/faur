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

#include "a2x_pack_fps.h"

#define MILIS (1000 / a2xSet.fps)
#define FPS_SIZE (60)

static struct {
    uint32_t f[FPS_SIZE];
    uint32_t f2[FPS_SIZE];
    uint32_t m[FPS_SIZE];
    uint32_t m2[FPS_SIZE];

    uint32_t fps;
    uint32_t max;
    uint32_t num;
    uint32_t last;
    uint32_t counter;
} a__fps;

#define overflow()                   \
{                                    \
    a__time_getMilisOverflow = 0;    \
    a__fps.num = 0;                  \
    a__fps.last = a_time_getMilis(); \
    return;                          \
}

void a__fps_set(void)
{
    if(a2xSet.trackFps) {
        a__fps.fps = 0;
        a__fps.max = 0;

        for(int i = 0; i < FPS_SIZE; i++) {
            a__fps.f[i] = a__fps.m[i] = MILIS;
        }
    }

    a__fps.num = 0;
    a__fps.last = a_time_getMilis();

    a__fps.counter = 0;
}

void a_fps_start(void)
{
    a__input_get();
    a_sound_adjustVolume();
}

void a_fps_end(void)
{
    a_screen_show();

    a__fps.counter++;

    if(a2xSet.trackFps) {
        a__fps.fps = a__fps.max = 0;

        for(int i = 0; i < FPS_SIZE; i++) {
            a__fps.fps += a__fps.f[i];
            a__fps.max += a__fps.m[i];
        }

        a__fps.fps = 1000 / ((float)a__fps.fps / FPS_SIZE);
        a__fps.max = 1000 / ((float)a__fps.max / FPS_SIZE);

        memcpy(&a__fps.f2[0], &a__fps.f[0], FPS_SIZE * sizeof(uint32_t));
        memcpy(&a__fps.f[0], &a__fps.f2[1], (FPS_SIZE - 1) * sizeof(uint32_t));

        memcpy(&a__fps.m2[0], &a__fps.m[0], FPS_SIZE * sizeof(uint32_t));
        memcpy(&a__fps.m[0], &a__fps.m2[1], (FPS_SIZE - 1) * sizeof(uint32_t));
    }

    a__fps.num++;

    const uint32_t wantedTicks = a__fps.last + (uint32_t)(MILIS * a__fps.num);
    if(wantedTicks < a__fps.last) overflow();

    uint32_t currentTicks = a_time_getMilis();
    if(a__time_getMilisOverflow) overflow();

    if(a2xSet.trackFps) {
        const uint32_t diff = wantedTicks - currentTicks;

        if(diff < MILIS) {
            a__fps.m[FPS_SIZE - 1] = MILIS - diff;
        } else {
            a__fps.m[FPS_SIZE - 1] = 1;
        }
    } else {
        a__fps.max = 1000 / a_math_max(1, wantedTicks - currentTicks);
    }

    if(currentTicks < wantedTicks) {
        do {
            #if !A_PLATFORM_WIZ
                if(wantedTicks - currentTicks >= 10) a_time_waitMilis(10);
            #endif

            currentTicks = a_time_getMilis();
            if(a__time_getMilisOverflow) overflow();
        } while(currentTicks < wantedTicks);
    } else if(currentTicks > wantedTicks) {
        a__fps.num = 0;
        a__fps.last = a_time_getMilis();
        if(a__time_getMilisOverflow) a__time_getMilisOverflow = 0;
    }

    if(a2xSet.trackFps) {
        a__fps.f[FPS_SIZE - 1] = MILIS - (wantedTicks - currentTicks);
    } else {
        a__fps.fps = 1000 / (MILIS - (wantedTicks - currentTicks));
    }
}

uint32_t a_fps_fps(void)
{
    return a__fps.fps;
}

uint32_t a_fps_max(void)
{
    return a__fps.max;
}

uint32_t a_fps_counter(void)
{
    return a__fps.counter;
}
