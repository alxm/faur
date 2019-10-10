/*
    Copyright 2010, 2017 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_GENERAL_FPS_V_H
#define A_INC_GENERAL_FPS_V_H

#include "general/a_fps.p.h"

#include "general/a_main.v.h"

extern const APack a_pack__fps;

extern void a_fps__reset(void);
extern bool a_fps__tick(void);
extern void a_fps__frame(void);

#endif // A_INC_GENERAL_FPS_V_H
