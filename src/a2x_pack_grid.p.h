/*
    Copyright 2010, 2011, 2016-2018 Alex Margarit

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

typedef struct AGrid AGrid;
typedef struct AGridItem AGridItem;

#include "a2x_pack_fix.p.h"
#include "a2x_pack_list.p.h"

extern AGrid* a_grid_new(int Width, int Height, int MaxObjectDim);
extern void a_grid_free(AGrid* Grid);

extern AGridItem* a_griditem_new(const AGrid* Grid, void* Context);
extern void a_griditem_free(AGridItem* Item);

extern void a_griditem_coordsSet(AGridItem* Item, int X, int Y);

extern void* a__griditem_contextGet(const AGridItem* Item);
extern AList* a__griditem_nearbyListGet(const AGridItem* Item);

#define A_GRID_ITERATE(GridItem, ContextPtrType, ContextVarName)              \
    for(const AGridItem* a__gi = GridItem; a__gi; a__gi = NULL)               \
        A_LIST_FILTER(a__griditem_nearbyListGet(a__gi),                       \
                      const AGridItem*, a__i,                                 \
                      a__i != a__gi)                                          \
            for(ContextPtrType ContextVarName = a__griditem_contextGet(a__i); \
                a__i != NULL; a__i = NULL)
