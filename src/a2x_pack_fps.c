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

#include "a2x_pack_fps.p.h"
#include "a2x_pack_fps.v.h"

#define BUFFER_SIZE 60

static uint32_t fpsBuffer[BUFFER_SIZE];
static uint32_t maxBuffer[BUFFER_SIZE];

static uint32_t milisPerFrame;
static uint32_t fps;
static uint32_t max;
static uint32_t numFrames;
static uint32_t lastMilis;
static uint32_t counter;

#define getMilis()                        \
({                                        \
    const uint32_t m = a_time_getMilis(); \
    if(a_time_overflowed()) {             \
        overflow();                       \
    }                                     \
    m;                                    \
})

#define overflow()                 \
({                                 \
    numFrames = 0;                 \
    lastMilis = a_time_getMilis(); \
    a_time_handledOverflow();      \
    return;                        \
})

void a_fps__set(void)
{
    milisPerFrame = 1000 / a2x_int("fps");

    fps = 0;
    max = 0;

    if(a2x_bool("trackFps")) {
        for(int i = BUFFER_SIZE; i--; ) {
            fpsBuffer[i] = milisPerFrame;
            maxBuffer[i] = milisPerFrame;
        }
    }

    numFrames = 0;
    lastMilis = a_time_getMilis();
    counter = 0;
}

void a_fps_start(void)
{
    a_input__get();
    a_sound_adjustVolume();
}

void a_fps_end(void)
{
    a_screen_show();

    counter++;
    numFrames++;

    const uint32_t wantedMilis = lastMilis + numFrames * milisPerFrame;

    if(wantedMilis < lastMilis) {
        overflow();
    }

    uint32_t currentMilis = getMilis();

    if(a2x_bool("trackFps")) {
        const uint32_t diff = wantedMilis - currentMilis;

        if(diff < milisPerFrame) {
            maxBuffer[BUFFER_SIZE - 1] = milisPerFrame - diff;
        } else {
            maxBuffer[BUFFER_SIZE - 1] = 1;
        }
    } else {
        max = 1000 / a_math_max(1, wantedMilis - currentMilis);
    }

    if(currentMilis < wantedMilis) {
        do {
            #if !A_PLATFORM_WIZ
                if(wantedMilis - currentMilis >= 10) {
                    a_time_waitMilis(10);
                }
            #endif

            currentMilis = getMilis();
        } while(currentMilis < wantedMilis);
    } else if(currentMilis > wantedMilis) {
        numFrames = 0;
        lastMilis = getMilis();
    }

    if(a2x_bool("trackFps")) {
        int f = 0;
        int m = 0;

        for(int i = BUFFER_SIZE; i--; ) {
            f += fpsBuffer[i];
            m += maxBuffer[i];
        }

        fps = 1000 / ((float)f / BUFFER_SIZE);
        max = 1000 / ((float)m / BUFFER_SIZE);

        memmove(&fpsBuffer[0], &fpsBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));
        memmove(&maxBuffer[0], &maxBuffer[1], (BUFFER_SIZE - 1) * sizeof(uint32_t));
    }

    if(a2x_bool("trackFps")) {
        fpsBuffer[BUFFER_SIZE - 1] = milisPerFrame - (wantedMilis - currentMilis);
    } else {
        fps = 1000 / (milisPerFrame - (wantedMilis - currentMilis));
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
