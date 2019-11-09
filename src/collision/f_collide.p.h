/*
    Copyright 2010-2011, 2016-2018 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_COLLISION_COLLIDE_P_H
#define F_INC_COLLISION_COLLIDE_P_H

#include "../general/f_system_includes.h"

#include "../math/f_fix.p.h"

extern bool f_collide_boxAndBox(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2);
extern bool f_collide_circleAndCircle(int X1, int Y1, int R1, int X2, int Y2, int R2);
extern bool f_collide_circleAndCirclef(FFix X1, FFix Y1, FFix R1, FFix X2, FFix Y2, FFix R2);
extern bool f_collide_pointInBox(int X, int Y, int BoxX, int BoxY, int BoxW, int BoxH);
extern bool f_collide_pointInCircle(int X, int Y, int CircleX, int CircleY, int CircleR);

#endif // F_INC_COLLISION_COLLIDE_P_H
