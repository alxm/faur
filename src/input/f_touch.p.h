/*
    Copyright 2010, 2017-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_INPUTS_TOUCH_P_H
#define A_INC_INPUTS_TOUCH_P_H

#include "general/a_system_includes.h"

#include "math/a_fix.p.h"

extern AVectorInt a_touch_deltaGet(void);
extern bool a_touch_tapGet(void);
extern bool a_touch_pointGet(int X, int Y);
extern bool a_touch_boxGet(int X, int Y, int W, int H);

#endif // A_INC_INPUTS_TOUCH_P_H
