/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

extern "C" {
    #include "a_gamebuino.v.h"
}

#include <faur.v.h>

#if A_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

void setup(void)
{
    gb.begin();
    gb.setFrameRate(A_CONFIG_FPS_RATE_TICK);

    SerialUSB.begin(9600);

    #if A_CONFIG_BUILD_DEBUG
        unsigned now = millis();

        while(!SerialUSB && millis() - now < 1000) {
            continue;
        }
    #endif

    a__main();
}

void loop(void)
{
    if(!gb.update()) {
        return;
    }

    a_state__runStep();

    #if A_CONFIG_BUILD_DEBUG
        gb.display.setColor(WHITE);
        gb.display.setCursor(2, 46);
        gb.display.print(gb.getCpuLoad(), DEC);

        gb.display.setCursor(2, 52);
        gb.display.print(gb.getFreeRam(), DEC);

        gb.display.setCursor(2, 58);
        gb.display.print(gb.frameCount, DEC);
    #endif
}

uint32_t a_platform_api__timeMsGet(void)
{
    return millis();
}

void a_platform_api__timeMsWait(uint32_t Ms)
{
    a_time_spinMs(Ms);
}
#endif // A_CONFIG_SYSTEM_GAMEBUINO
