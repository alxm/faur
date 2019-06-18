/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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

#include "time/a_time.v.h"

#include "platform/a_platform.v.h"

uint32_t a_time_getMs(void)
{
    return a_platform_api__timeMsGet();
}

void a_time_waitMs(uint32_t Ms)
{
    a_platform_api__timeMsWait(Ms);
}

void a_time_waitSec(uint32_t Sec)
{
    a_platform_api__timeMsWait(Sec * 1000);
}

void a_time_spinMs(uint32_t Ms)
{
    const uint32_t start = a_platform_api__timeMsGet();

    while(a_platform_api__timeMsGet() - start < Ms) {
        continue;
    }
}

void a_time_spinSec(uint32_t Sec)
{
    a_time_spinMs(Sec * 1000);
}
