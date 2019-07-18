/*
    Copyright 2010, 2017-2019 Alex Margarit <alex@alxm.org>
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

#pragma once

#include "general/a_system_includes.h"

#include "math/a_fix.p.h"

static inline unsigned a_fps_rateTickGet(void)
{
    return A_CONFIG_FPS_RATE_TICK;
}

extern unsigned a_fps_rateDrawGet(void);
extern unsigned a_fps_rateDrawGetMax(void);

extern unsigned a_fps_ticksGet(void);
extern bool a_fps_ticksNth(unsigned N);
extern AFix a_fps_ticksSin(uint8_t Mul, uint8_t Div, unsigned Offset);
