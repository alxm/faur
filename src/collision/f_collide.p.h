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

#ifndef F_INC_COLLISION_COLLIDE_P_H
#define F_INC_COLLISION_COLLIDE_P_H

#include "../general/f_system_includes.h"

#include "../general/f_errors.p.h"
#include "../math/f_vec.p.h"

static inline bool f_collide_boxAndBox(FVecInt Coords1, FVecInt Size1, FVecInt Coords2, FVecInt Size2)
{
    F_CHECK(Size1.x >= 0);
    F_CHECK(Size1.y >= 0);
    F_CHECK(Size2.x >= 0);
    F_CHECK(Size2.y >= 0);

    return !(Coords1.y >= Coords2.y + Size2.y
          || Coords2.y >= Coords1.y + Size1.y
          || Coords1.x >= Coords2.x + Size2.x
          || Coords2.x >= Coords1.x + Size1.x);
}

static inline bool f_collide_boxAndBoxf(FVecFix Coords1, FVecFix Size1, FVecFix Coords2, FVecFix Size2)
{
    F_CHECK(Size1.x >= 0);
    F_CHECK(Size1.y >= 0);
    F_CHECK(Size2.x >= 0);
    F_CHECK(Size2.y >= 0);

    return !(Coords1.y >= Coords2.y + Size2.y
          || Coords2.y >= Coords1.y + Size1.y
          || Coords1.x >= Coords2.x + Size2.x
          || Coords2.x >= Coords1.x + Size1.x);
}

static inline bool f_collide_circleAndCircle(FVecInt Coords1, int Radius1, FVecInt Coords2, int Radius2)
{
    F_CHECK(Radius1 >= 0);
    F_CHECK(Radius2 >= 0);

    int dx = Coords1.x - Coords2.x;
    int dy = Coords1.y - Coords2.y;
    int rSum = Radius1 + Radius2;

    return dx * dx + dy * dy < rSum * rSum;
}

static inline bool f_collide_circleAndCirclef(FVecFix Coords1, FFix Radius1, FVecFix Coords2, FFix Radius2)
{
    F_CHECK(Radius1 >= 0);
    F_CHECK(Radius2 >= 0);

    int64_t dx = Coords1.x - Coords2.x;
    int64_t dy = Coords1.y - Coords2.y;
    int64_t rSum = Radius1 + Radius2;

    return dx * dx + dy * dy < rSum * rSum;
}

static inline bool f_collide_pointInBox(FVecInt Point, FVecInt BoxCoords, FVecInt BoxSize)
{
    F_CHECK(BoxSize.x >= 0);
    F_CHECK(BoxSize.y >= 0);

    return Point.x >= BoxCoords.x && Point.x < BoxCoords.x + BoxSize.x
        && Point.y >= BoxCoords.y && Point.y < BoxCoords.y + BoxSize.y;
}

static inline bool f_collide_pointInBoxf(FVecFix Point, FVecFix BoxCoords, FVecFix BoxSize)
{
    F_CHECK(BoxSize.x >= 0);
    F_CHECK(BoxSize.y >= 0);

    return Point.x >= BoxCoords.x && Point.x < BoxCoords.x + BoxSize.x
        && Point.y >= BoxCoords.y && Point.y < BoxCoords.y + BoxSize.y;
}

static inline bool f_collide_pointInCircle(FVecInt Point, FVecInt CircleCoords, int CircleRadius)
{
    F_CHECK(CircleRadius >= 0);

    int dx = Point.x - CircleCoords.x;
    int dy = Point.y - CircleCoords.y;

    return dx * dx + dy * dy < CircleRadius * CircleRadius;
}

static inline bool f_collide_pointInCirclef(FVecFix Point, FVecFix CircleCoords, FFix CircleRadius)
{
    F_CHECK(CircleRadius >= 0);

    int64_t dx = Point.x - CircleCoords.x;
    int64_t dy = Point.y - CircleCoords.y;

    return dx * dx + dy * dy < (int64_t)CircleRadius * CircleRadius;
}

#endif // F_INC_COLLISION_COLLIDE_P_H
