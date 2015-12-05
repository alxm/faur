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

#include "a2x_app_includes.h"

typedef struct ColMap ColMap;
typedef struct ColObject ColObject;
typedef struct ColIt ColIt;

#include "a2x_pack_listit.p.h"

struct ColIt {
    ColObject* callerPoint;
    ListIt points; // list of points in the current submap
};

extern ColMap* a_colmap_new(int width, int height, int maxObjectDim);
extern void a_colmap_free(ColMap* m);

extern ColObject* a_colobject_new(const ColMap* m, void* parent);
extern void a_colobject_free(ColObject* o);

extern void a_colobject_setCoords(ColObject* o, int x, int y);
extern List* a_colobject__getColList(const ColObject* o);
extern void* a_colobject__getParent(const ColObject* o);

#define A_COL_ITERATE(o, type, var)                                 \
    A_LIST_FILTER(a_colobject__getColList(o), ColObject, c, o != c) \
        for(type* var = a_colobject__getParent(c); var; var = NULL)

#define a_collide_boxes(x1, y1, w1, h1, x2, y2, w2, h2) \
(                                                       \
    !( (y1) >= (y2) + (h2)                              \
    || (y2) >= (y1) + (h1)                              \
    || (x1) >= (x2) + (w2)                              \
    || (x2) >= (x1) + (w1) )                            \
)

#define a_collide_boxOnScreen(x, y, w, h)		            	 \
(                      						                     \
    a_collide_boxes((x), (y), (w), (h), 0, 0, a_width, a_height) \
)

#define a_collide_boxInsideScreen(x, y, w, h)                 \
(                                                             \
    x >= 0 && y >= 0 && x + w <= a_width && y + h <= a_height \
)

extern bool a_collide_circles(int x1, int y1, int r1, int x2, int y2, int r2);

#endif // A2X_PACK_COLLIDE_PH
