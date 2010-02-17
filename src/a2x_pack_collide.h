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

#ifndef A2X_PACK_COLLIDE_H
#define A2X_PACK_COLLIDE_H

#include <stdlib.h>

#include "a2x_pack_fix.h"
#include "a2x_pack_list.h"
#include "a2x_pack_math.h"

typedef struct ColMap {
    int w;
    int h;
    int dim;
    List*** maps;
} ColMap;

typedef struct ColBox {
    fix8 x;
    fix8 y;
    fix8 w;
    fix8 h;
    List* nodes;
    List* maps;
    void* parent;
} ColBox;

#define a_collide_rects(r1, r2)    \
(                                  \
	!( (r1).y >= (r2).y + (r2).h   \
	|| (r2).y >= (r1).y + (r1).h   \
	|| (r1).x >= (r2).x + (r2).w   \
	|| (r2).x >= (r1).x + (r1).w ) \
)

#define a_collide_boxes(x1, y1, w1, h1, x2, y2, w2, h2) \
(                                                       \
	!( (y1) >= (y2) + (h2)                              \
	|| (y2) >= (y1) + (h1)                              \
	|| (x1) >= (x2) + (w2)                              \
	|| (x2) >= (x1) + (w1) )                            \
)

#define a_collide_screen(r1) (                         \
	a_collide_rects((r1), (Rect){0, 0, WIDTH, HEIGHT}) \
)

extern int a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2);

extern ColMap* a_collide_makeColMap(const int dim, const int w, const int h);
extern void a_collide_freeColMap(ColMap* const c);
extern ColBox* a_collide_makeColBox(const fix8 x, const fix8 y, const int w, const int h, void* const parent);
extern void a_collide_freeColBox(ColBox* const c);
extern void a_collide_assignColMaps(ColMap* const cm, ColBox* const o);

#endif // A2X_PACK_COLLIDE_H
