/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#include "f_time.v.h"
#include <faur.v.h>

uint32_t f_time_msGet(void)
{
    return f_platform_api__timeMsGet();
}

void f_time_msWait(uint32_t Ms)
{
    f_platform_api__timeMsWait(Ms);
}

void f_time_msSpin(uint32_t Ms)
{
    const uint32_t start = f_platform_api__timeMsGet();

    while(f_platform_api__timeMsGet() - start < Ms) {
        continue;
    }
}
