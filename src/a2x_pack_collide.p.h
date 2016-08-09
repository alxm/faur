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

#include "a2x_pack_listit.p.h"

extern AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim);
extern void a_colmap_free(AColMap* Map);

extern AColObject* a_colobject_new(const AColMap* Map, void* Parent);
extern void a_colobject_free(AColObject* Object);

extern void a_colobject_setCoords(AColObject* Object, int X, int Y);
extern void* a_colobject__getParent(const AColObject* Object);

extern void a_colobject__reset(AColObject* Object);
extern bool a_colobject__getNext(AColObject* Object, void** UserObject);

#define A_COL_ITERATE(ColObject, UserObject)                    \
    for(a_colobject__reset(ColObject);                          \
        a_colobject__getNext(ColObject, (void**)&UserObject); )

extern bool a_collide_boxes(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2);
extern bool a_collide_boxOnScreen(int X, int Y, int W, int H);
extern bool a_collide_boxInsideScreen(int X, int Y, int W, int H);
extern bool a_collide_circles(int X1, int Y1, int R1, int X2, int Y2, int R2);
