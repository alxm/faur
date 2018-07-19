/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_time.v.h"

#include "a2x_pack_platform.v.h"

uint32_t a_time_msGet(void)
{
    return a_platform__msGet();
}

void a_time_msWait(uint32_t Ms)
{
    a_platform__msWait(Ms);
}

void a_time_secWait(uint32_t Sec)
{
    a_platform__msWait(Sec * 1000);
}

void a_time_msSpin(uint32_t Ms)
{
    const uint32_t start = a_platform__msGet();

    while(a_platform__msGet() - start < Ms) {
        continue;
    }
}

void a_time_secSpin(uint32_t Sec)
{
    a_time_msSpin(Sec * 1000);
}
