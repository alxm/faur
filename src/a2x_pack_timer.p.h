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

#ifndef A2X_PACK_TIMER_PH
#define A2X_PACK_TIMER_PH

#include "a2x_app_includes.h"

typedef struct Timer Timer;

extern Timer* a_timer_new(const uint32_t milisPeriod);
extern void a_timer_free(Timer* const t);

extern bool a_timer_check(Timer* const t);
extern uint32_t a_timer_diff(Timer* const t);
extern void a_timer_start(Timer* const t);

#endif // A2X_PACK_TIMER_PH
