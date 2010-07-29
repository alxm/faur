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

#ifndef A2X_PACK_COLLIDE_PH
#define A2X_PACK_COLLIDE_PH

#include <stdlib.h>

#include "a2x_pack_fix.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_math.p.h"

typedef struct ColMap ColMap;
typedef struct ColBox ColBox;
typedef struct ColIterator ColIterator;

extern ColMap* a_colmap_set(const int dim, const int w, const int h);
extern void a_colmap_free(ColMap* const c);

extern ColBox* a_colbox_set(ColMap* const colmap, const int w, const int h);
extern void a_colbox_free(ColBox* const c);

extern void a_colbox_setCoords(ColBox* const b, const fix8 x, const fix8 y);
extern void a_colbox_setParent(ColBox* const b, void* parent);
extern void* a_colbox_getParent(ColBox* const b);

extern ColIterator* a_colbox_setIterator(ColBox* const b);
extern void a_colbox_freeIterator(ColIterator* const it);
extern int a_colbox_iteratorNext(ColIterator* const it);
extern ColBox* a_colbox_iteratorGet(const ColIterator* const it);

#define a_collide_boxes(x1, y1, w1, h1, x2, y2, w2, h2) \
(                                                       \
	!( (y1) >= (y2) + (h2)                              \
	|| (y2) >= (y1) + (h1)                              \
	|| (x1) >= (x2) + (w2)                              \
	|| (x2) >= (x1) + (w1) )                            \
)

#define a_collide_screen(x, y, w, h) (                           \
	a_collide_boxes((x), (y), (w), (h), 0, 0, a_width, a_height) \
)

extern int a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2);

#endif // A2X_PACK_COLLIDE_PH
