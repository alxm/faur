/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#include "a2x_pack_list.p.h"

extern AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim);
extern void a_colmap_free(AColMap* Map);

extern AColObject* a_colobject_new(const AColMap* Map, void* Context);
extern void a_colobject_free(AColObject* Object);

extern void a_colobject_coordsSet(AColObject* Object, int X, int Y);

extern void* a__colobject_contextGet(const AColObject* Object);
extern AList* a__colobject_nearbyListGet(const AColObject* Object);

#define A_COL_ITERATE(ColObject, ContextPtrType, ContextVarName)           \
    for(const AColObject* a__co = ColObject; a__co; a__co = NULL)          \
        A_LIST_FILTER(a__colobject_nearbyListGet(a__co),                   \
                      const AColObject*, a__o,                             \
                      a__o != a__co)                                       \
        for(ContextPtrType ContextVarName = a__colobject_contextGet(a__o); \
            a__o != NULL; a__o = NULL)

extern bool a_collide_boxAndBox(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2);
extern bool a_collide_circleAndCircle(int X1, int Y1, int R1, int X2, int Y2, int R2);
extern bool a_collide_pointInBox(int X, int Y, int BoxX, int BoxY, int BoxW, int BoxH);
extern bool a_collide_pointInCircle(int X, int Y, int CircleX, int CircleY, int CircleR);
