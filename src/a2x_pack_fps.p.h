/*
    Copyright 2010, 2017-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#pragma once

#include "a2x_system_includes.h"

#include "a2x_pack_settings.p.h"

static inline unsigned a_fps_rateTickGet(void)
{
    return A_CONFIG_FPS_TICK;
}

extern unsigned a_fps_rateDrawGet(void);
extern unsigned a_fps_rateDrawGetMax(void);

extern unsigned a_fps_ticksGet(void);
extern bool a_fps_ticksNth(unsigned N);
