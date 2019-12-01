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
    #include "f_odroid_go.v.h"
}

#include <faur.v.h>

#if F_CONFIG_SYSTEM_ODROID_GO
#include <odroid_go.h>
#include <SD.h>

#define F__SD_PIN 22

void setup(void)
{
    GO.begin();

    if(!SD.begin(F__SD_PIN)) {
        f_out__error("SD.begin(%d) failed", F__SD_PIN);
    }

    f__main();
}

void loop(void)
{
    GO.update();

    f_state__runStep();
}

uint32_t f_platform_api__timeMsGet(void)
{
    return millis();
}

void f_platform_api__timeMsWait(uint32_t Ms)
{
    f_time_spinMs(Ms);
}
#endif // F_CONFIG_SYSTEM_ODROID_GO

