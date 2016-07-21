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

#pragma once

#include "a2x_system_includes.h"

typedef struct AColMap AColMap;
typedef struct AColObject AColObject;
typedef struct AColIt AColIt;

#include "a2x_pack_listit.p.h"

struct AColIt {
    AColObject* callerPoint;
    AListIt points; // list of points in the current submap
};

extern AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim);
extern void a_colmap_free(AColMap* Map);

extern AColObject* a_colobject_new(const AColMap* Map, void* Parent);
extern void a_colobject_free(AColObject* Object);

extern void a_colobject_setCoords(AColObject* Object, int X, int Y);
extern AList* a_colobject__getColList(const AColObject* Object);
extern void* a_colobject__getParent(const AColObject* Object);

#define A_COL_ITERATE(Object, VarType, VarName)                                          \
    A_LIST_FILTER(a_colobject__getColList(Object), AColObject, a__obj, Object != a__obj) \
        for(VarType* VarName = a_colobject__getParent(a__obj); VarName; VarName = NULL)

#define a_collide_boxes(X1, Y1, W1, H1, X2, Y2, W2, H2) \
(                                                       \
    !( (Y1) >= (Y2) + (H2)                              \
    || (Y2) >= (Y1) + (H1)                              \
    || (X1) >= (X2) + (W2)                              \
    || (X2) >= (X1) + (W1) )                            \
)

#define a_collide_boxOnScreen(X, Y, W, H)		            	 \
(                      						                     \
    a_collide_boxes((X), (Y), (W), (H), 0, 0, a_width, a_height) \
)

#define a_collide_boxInsideScreen(X, Y, W, H)                 \
(                                                             \
    X >= 0 && Y >= 0 && X + W <= a_width && Y + H <= a_height \
)

extern bool a_collide_circles(int X1, int Y1, int R1, int X2, int Y2, int R2);
