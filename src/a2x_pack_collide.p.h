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
typedef struct ColPoint ColPoint;
typedef struct ColIt ColIt;

#include "a2x_pack_fix.p.h"

extern ColMap* a_colmap_new(const int totalWidth, const int totalHeight, const int gridDim);
extern void a_colmap_free(ColMap* const c);

extern ColPoint* a_colpoint_new(ColMap* const colmap);
extern void a_colpoint_free(ColPoint* const c);

extern void a_colpoint_setCoords(ColPoint* const b, const fix8 x, const fix8 y);
extern void a_colpoint_setParent(ColPoint* const b, void* parent);
extern void* a_colpoint_getParent(ColPoint* const b);

extern ColIt* a_colit_new(ColPoint* const b);
extern void a_colit_free(ColIt* const it);
extern bool a_colit_next(ColIt* const it);
extern ColPoint* a_colit_get(const ColIt* const it);

#define ColIterate(colpoint, var)                                                       \
    for(ColIt* a__ci = a_colit_new(colpoint); a__ci; a_colit_free(a__ci), a__ci = NULL) \
        for(ColPoint* var; a_colit_next(a__ci) && (var = a_colit_get(a__ci)); )

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

extern bool a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2);

#endif // A2X_PACK_COLLIDE_PH
