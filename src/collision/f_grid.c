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

#include "f_grid.v.h"
#include <faur.v.h>

FGrid* f_grid_new(FFix Width, FFix Height, FFix MaxItemDiameter)
{
    F__CHECK(Width > 0);
    F__CHECK(Height > 0);
    F__CHECK(MaxItemDiameter > 0);

    FGrid* g = f_mem_malloc(sizeof(FGrid));

    int shift = 0;

    while((F_FIX_ONE << shift) < MaxItemDiameter) {
        shift++;
    }

    g->shift = shift + 1;

    FFix cellDim = F_FIX_ONE << g->shift;

    g->w = f_fix_toInt((Width + cellDim - 1) >> g->shift);
    g->h = f_fix_toInt((Height + cellDim - 1) >> g->shift);

    g->cells = f_mem_malloc((unsigned)g->h * sizeof(FList**));
    g->cellsData = f_mem_malloc((unsigned)(g->h * g->w) * sizeof(FList*));

    for(int i = g->h; i--; ) {
        g->cells[i] = g->cellsData + i * g->w;

        for(int j = g->w; j--; ) {
            g->cells[i][j] = f_list_new();
        }
    }

    return g;
}

void f_grid_free(FGrid* Grid)
{
    if(Grid == NULL) {
        return;
    }

    for(int i = Grid->h; i--; ) {
        for(int j = Grid->w; j--; ) {
            f_list_free(Grid->cells[i][j]);
        }
    }

    f_mem_free(Grid->cells);
    f_mem_free(Grid->cellsData);
    f_mem_free(Grid);
}

const FList* f_grid_nearGet(const FGrid* Grid, FVecFix Coords)
{
    F__CHECK(Grid != NULL);

    int x = f_math_clamp(f_fix_toInt(Coords.x >> Grid->shift), 0, Grid->w - 1);
    int y = f_math_clamp(f_fix_toInt(Coords.y >> Grid->shift), 0, Grid->h - 1);

    return Grid->cells[y][x];
}

FGridItem* f_grid_itemNew(void)
{
    return f_list_new();
}

void f_grid_itemFree(FGridItem* Item)
{
    if(Item == NULL) {
        return;
    }

    // Remove item from any lists it is in
    f_list_freeEx(Item, (FCallFree*)f_list_removeNode);
}

void f_grid_itemCoordsSet(const FGrid* Grid, FGridItem* Item, void* Context, FVecFix Coords)
{
    F__CHECK(Grid != NULL);
    F__CHECK(Item != NULL);
    F__CHECK(Context != NULL);

    // remove item from all the cells it was previously in
    f_list_clearEx(Item, (FCallFree*)f_list_removeNode);

    // center cell coords
    int cellX = f_fix_toInt(Coords.x >> Grid->shift);
    int cellY = f_fix_toInt(Coords.y >> Grid->shift);

    FVecInt cellStart, cellEnd;
    FFix cellDim = F_FIX_ONE << Grid->shift;
    FVecFix cellOffset = {Coords.x & (cellDim - 1), Coords.y & (cellDim - 1)};

    if(cellOffset.x < cellDim / 2) {
        cellStart.x = f_math_clamp(cellX - 1, 0, Grid->w - 1);
        cellEnd.x = f_math_clamp(cellX, 0, Grid->w - 1);
    } else {
        cellStart.x = f_math_clamp(cellX, 0, Grid->w - 1);
        cellEnd.x = f_math_clamp(cellX + 1, 0, Grid->w - 1);
    }

    if(cellOffset.y < cellDim / 2) {
        cellStart.y = f_math_clamp(cellY - 1, 0, Grid->h - 1);
        cellEnd.y = f_math_clamp(cellY, 0, Grid->h - 1);
    } else {
        cellStart.y = f_math_clamp(cellY, 0, Grid->h - 1);
        cellEnd.y = f_math_clamp(cellY + 1, 0, Grid->h - 1);
    }

    // add item to every cell in its surrounding perimeter
    for(int y = cellStart.y; y <= cellEnd.y; y++) {
        for(int x = cellStart.x; x <= cellEnd.x; x++) {
            f_list_addFirst(Item, f_list_addFirst(Grid->cells[y][x], Context));
        }
    }
}
