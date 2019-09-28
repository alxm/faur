/*
    Copyright 2010-2011, 2015-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_grid.v.h"
#include <a2x.v.h>

struct AGrid {
    int shift; // right-shift item coords to get cell index
    int w, h; // width and height of grid in cells
    AList*** cells; // AList*[h][w] of void*
    AList** cellsData; // AList*[h * w] of void*
};

AGrid* a_grid_new(AFix Width, AFix Height, AFix MaxObjectDim)
{
    AGrid* g = a_mem_malloc(sizeof(AGrid));

    g->shift = 0;

    while((A_FIX_ONE << g->shift) < MaxObjectDim) {
        g->shift++;
    }

    AFix cellDim = A_FIX_ONE << g->shift;

    g->w = a_fix_toInt((Width + cellDim - 1) >> g->shift);
    g->h = a_fix_toInt((Height + cellDim - 1) >> g->shift);

    g->cells = a_mem_malloc((unsigned)g->h * sizeof(AList**));
    g->cellsData = a_mem_malloc((unsigned)(g->h * g->w) * sizeof(AList*));

    for(int i = g->h; i--; ) {
        g->cells[i] = g->cellsData + i * g->w;

        for(int j = g->w; j--; ) {
            g->cells[i][j] = a_list_new();
        }
    }

    return g;
}

void a_grid_free(AGrid* Grid)
{
    if(Grid == NULL) {
        return;
    }

    for(int i = Grid->h; i--; ) {
        for(int j = Grid->w; j--; ) {
            a_list_free(Grid->cells[i][j]);
        }
    }

    a_mem_free(Grid->cells);
    a_mem_free(Grid->cellsData);
    a_mem_free(Grid);
}

const AList* a_grid_nearGet(const AGrid* Grid, AVectorFix Coords)
{
    int x = a_math_clamp(a_fix_toInt(Coords.x >> Grid->shift), 0, Grid->w - 1);
    int y = a_math_clamp(a_fix_toInt(Coords.y >> Grid->shift), 0, Grid->h - 1);

    return Grid->cells[y][x];
}

AGridItem* a_grid_itemNew(void)
{
    return a_list_new();
}

void a_grid_itemFree(AGridItem* Item)
{
    // Remove item from any lists it is in
    a_list_freeEx(Item, (AFree*)a_list_removeNode);
}

void a_grid_itemCoordsSet(const AGrid* Grid, AGridItem* Item, void* Context, AVectorFix Coords)
{
    // remove item from all the cells it was previously in
    a_list_clearEx(Item, (AFree*)a_list_removeNode);

    // center cell coords
    int cellX = a_fix_toInt(Coords.x >> Grid->shift);
    int cellY = a_fix_toInt(Coords.y >> Grid->shift);

    AVectorInt cellStart, cellEnd;
    AFix cellDim = A_FIX_ONE << Grid->shift;
    AVectorFix cellOffset = {Coords.x & (cellDim - 1),
                             Coords.y & (cellDim - 1)};

    if(cellOffset.x < cellDim / 2) {
        cellStart.x = a_math_clamp(cellX - 1, 0, Grid->w - 1);
        cellEnd.x = a_math_clamp(cellX, 0, Grid->w - 1);
    } else {
        cellStart.x = a_math_clamp(cellX, 0, Grid->w - 1);
        cellEnd.x = a_math_clamp(cellX + 1, 0, Grid->w - 1);
    }

    if(cellOffset.y < cellDim / 2) {
        cellStart.y = a_math_clamp(cellY - 1, 0, Grid->h - 1);
        cellEnd.y = a_math_clamp(cellY, 0, Grid->h - 1);
    } else {
        cellStart.y = a_math_clamp(cellY, 0, Grid->h - 1);
        cellEnd.y = a_math_clamp(cellY + 1, 0, Grid->h - 1);
    }

    // add item to every cell in its surrounding perimeter
    for(int y = cellStart.y; y <= cellEnd.y; y++) {
        for(int x = cellStart.x; x <= cellEnd.x; x++) {
            a_list_addFirst(Item, a_list_addFirst(Grid->cells[y][x], Context));
        }
    }
}
