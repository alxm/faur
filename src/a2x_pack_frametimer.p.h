/*
    Copyright 2011, 2016 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

typedef struct AFrameTimer AFrameTimer;

extern AFrameTimer* a_frametimer_new(unsigned FramesPeriod);
extern void a_frametimer_free(AFrameTimer* Timer);

extern void a_frametimer_start(AFrameTimer* Timer);
extern void a_frametimer_stop(AFrameTimer* Timer);
extern bool a_frametimer_running(AFrameTimer* Timer);
extern bool a_frametimer_expired(AFrameTimer* Timer);
extern unsigned a_frametimer_elapsed(AFrameTimer* Timer);
