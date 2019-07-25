/*
    Copyright 2010-2011, 2015-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

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
    int coordsShift; // right-shift item coords to get cell index
    int w, h; // width and height of grid in cells
    AList*** cells; // AList*[h][w] of void*
    AList** cellsData; // AList*[h * w] of void*
};

struct AGridItem {
    AList* nodes; // list of AListNode from cells this item is in
};

AGrid* a_grid_new(AFix Width, AFix Height, AFix MaxObjectDim)
{
    AGrid* g = a_mem_malloc(sizeof(AGrid));

    g->coordsShift = 0;

    while((A_FIX_ONE << g->coordsShift) < MaxObjectDim) {
        g->coordsShift++;
    }

    AFix cellDim = A_FIX_ONE << g->coordsShift;

    g->w = a_fix_toInt((Width + cellDim - 1) >> g->coordsShift);
    g->h = a_fix_toInt((Height + cellDim - 1) >> g->coordsShift);

    g->cells = a_mem_malloc((unsigned)g->h * sizeof(AList**));
    g->cellsData = a_mem_malloc(
                        (unsigned)g->h * (unsigned)g->w * sizeof(AList*));

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

    free(Grid->cells);
    free(Grid->cellsData);
    free(Grid);
}

AGridItem* a_griditem_new(void)
{
    AGridItem* i = a_mem_malloc(sizeof(AGridItem));

    i->nodes = a_list_new();

    return i;
}

void a_griditem_free(AGridItem* Item)
{
    if(Item == NULL) {
        return;
    }

    // Remove item from any lists it is in
    a_list_freeEx(Item->nodes, (AFree*)a_list_removeNode);

    free(Item);
}

void a_griditem_coordsSet(const AGrid* Grid, AGridItem* Item, void* Context, AFix X, AFix Y)
{
    // remove item from all the cells it was previously in
    a_list_clearEx(Item->nodes, (AFree*)a_list_removeNode);

    // center cell coords
    int cellX = a_fix_toInt(X >> Grid->coordsShift);
    int cellY = a_fix_toInt(Y >> Grid->coordsShift);

    AFix cellDim = A_FIX_ONE << Grid->coordsShift;

    AFix cellOffsetX = X & (cellDim - 1);
    AFix cellOffsetY = Y & (cellDim - 1);

    int cellStartX, cellStartY;
    int cellEndX, cellEndY;

    if(cellOffsetX < cellDim / 2) {
        cellStartX = a_math_clamp(cellX - 1, 0, Grid->w - 1);
        cellEndX = a_math_clamp(cellX, 0, Grid->w - 1);
    } else {
        cellStartX = a_math_clamp(cellX, 0, Grid->w - 1);
        cellEndX = a_math_clamp(cellX + 1, 0, Grid->w - 1);
    }

    if(cellOffsetY < cellDim / 2) {
        cellStartY = a_math_clamp(cellY - 1, 0, Grid->h - 1);
        cellEndY = a_math_clamp(cellY, 0, Grid->h - 1);
    } else {
        cellStartY = a_math_clamp(cellY, 0, Grid->h - 1);
        cellEndY = a_math_clamp(cellY + 1, 0, Grid->h - 1);
    }

    // add item to every cell in its surrounding perimeter
    for(int y = cellStartY; y <= cellEndY; y++) {
        for(int x = cellStartX; x <= cellEndX; x++) {
            a_list_addFirst(
                Item->nodes, a_list_addFirst(Grid->cells[y][x], Context));
        }
    }
}

const AList* a_grid_nearGet(const AGrid* Grid, AVectorFix Coords)
{
    int x = a_math_clamp(
                a_fix_toInt(Coords.x >> Grid->coordsShift), 0, Grid->w - 1);
    int y = a_math_clamp(
                a_fix_toInt(Coords.y >> Grid->coordsShift), 0, Grid->h - 1);

    return Grid->cells[y][x];
}
