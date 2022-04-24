/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_COLLISION_GRID_P_H
#define F_INC_COLLISION_GRID_P_H

#include "../general/f_system_includes.h"

typedef struct FGrid FGrid;
typedef struct FList FGridItem;

#include "../data/f_list.p.h"
#include "../math/f_vec.p.h"

extern FGrid* f_grid_new(FFix Width, FFix Height, FFix MaxItemDiameter);
extern void f_grid_free(FGrid* Grid);

extern const FList* f_grid_nearGet(const FGrid* Grid, FVecFix Coords);

extern FGridItem* f_grid_itemNew(void);
extern void f_grid_itemFree(FGridItem* Item);

extern void f_grid_itemCoordsSet(const FGrid* Grid, FGridItem* Item, void* Context, FVecFix Coords);

#endif // F_INC_COLLISION_GRID_P_H
