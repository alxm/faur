/*
    Copyright 2010-2011, 2016-2018 Alex Margarit <alex@alxm.org>
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

#include "a2x_system_includes.h"

#include "a2x_pack_fix.p.h"

extern bool a_collide_boxAndBox(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2);
extern bool a_collide_circleAndCircle(int X1, int Y1, int R1, int X2, int Y2, int R2);
extern bool a_collide_circleAndCirclef(AFix X1, AFix Y1, AFix R1, AFix X2, AFix Y2, AFix R2);
extern bool a_collide_pointInBox(int X, int Y, int BoxX, int BoxY, int BoxW, int BoxH);
extern bool a_collide_pointInCircle(int X, int Y, int CircleX, int CircleY, int CircleR);
