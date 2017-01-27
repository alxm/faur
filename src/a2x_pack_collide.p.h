/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_pack_listit.p.h"

extern AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim);
extern void a_colmap_free(AColMap* Map);

extern AColObject* a_colobject_new(const AColMap* Map, void* UserObject);
extern void a_colobject_free(AColObject* Object);

extern void a_colobject_setCoords(AColObject* Object, int X, int Y);

extern void* a_colobject__getUserObject(const AColObject* Object);
extern AList* a_colobject__getPossibleCollisions(const AColObject* Object);

#define A_COL_ITERATE(ColObject, UserObjectPtrType, UserObjectName)     \
    for(UserObjectPtrType UserObjectName = (UserObjectPtrType)1;        \
        UserObjectName;                                                 \
        UserObjectName = NULL)                                          \
        A_LIST_FILTER(                                                  \
            a_colobject__getPossibleCollisions(ColObject),              \
            AColObject*, a__o,                                          \
            a__o != ColObject                                           \
                && (UserObjectName = a_colobject__getUserObject(a__o)))

extern bool a_collide_boxAndBox(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2);
extern bool a_collide_circleAndCircle(int X1, int Y1, int R1, int X2, int Y2, int R2);
extern bool a_collide_pointInBox(int X, int Y, int BoxX, int BoxY, int BoxW, int BoxH);
extern bool a_collide_pointInCircle(int X, int Y, int CircleX, int CircleY, int CircleR);
