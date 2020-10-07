/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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
    #include "f_gamebuino.v.h"
}

#include <faur.v.h>

#if F_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

void setup(void)
{
    gb.begin();
    gb.setFrameRate(F_CONFIG_FPS_RATE_TICK);

    SerialUSB.begin(9600);

    #if F_CONFIG_DEBUG
        unsigned now = millis();

        while(!SerialUSB && millis() - now < 1000) {
            continue;
        }
    #endif

    f__main();
}

void loop(void)
{
    if(!gb.update()) {
        return;
    }

    f_state__runStep();

    #if F_CONFIG_DEBUG
        gb.display.setColor(WHITE);
        gb.display.setCursor(2, 46);
        gb.display.print(gb.getCpuLoad(), DEC);

        gb.display.setCursor(2, 52);
        gb.display.print(gb.getFreeRam(), DEC);

        gb.display.setCursor(2, 58);
        gb.display.print(gb.frameCount, DEC);
    #endif
}

uint32_t f_platform_api__timeMsGet(void)
{
    return millis();
}

void f_platform_api__timeMsWait(uint32_t Ms)
{
    f_time_msSpin(Ms);
}
#endif // F_CONFIG_SYSTEM_GAMEBUINO
