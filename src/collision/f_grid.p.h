/*
    Copyright 2010-2011, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_COLLISION_GRID_P_H
#define A_INC_COLLISION_GRID_P_H

#include "general/f_system_includes.h"

typedef struct AGrid AGrid;
typedef struct AList AGridItem;

#include "data/f_list.p.h"
#include "math/f_fix.p.h"

extern AGrid* f_grid_new(AFix Width, AFix Height, AFix MaxObjectDim);
extern void f_grid_free(AGrid* Grid);

extern const AList* f_grid_nearGet(const AGrid* Grid, AVectorFix Coords);

extern AGridItem* f_grid_itemNew(void);
extern void f_grid_itemFree(AGridItem* Item);

extern void f_grid_itemCoordsSet(const AGrid* Grid, AGridItem* Item, void* Context, AVectorFix Coords);

#endif // A_INC_COLLISION_GRID_P_H
