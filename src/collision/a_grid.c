/*
    Copyright 2010-2011, 2015-2018 Alex Margarit <alex@alxm.org>
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

#include <a2x.v.h>

struct AGrid {
    int coordsShift; // right-shift item coords to get cell index
    int w, h; // width and height of grid in cells
    AList*** cells; // AList*[h][w] of AGridItem
    AList** cellsData; // AList*[h * w] of AGridItem
};

struct AGridItem {
    const AGrid* grid;
    void* context; // the game item that owns this AGridItem
    AList* nodes; // list of AListNode from cells this item is in
    AFix x, y; // coords on the grid
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
            A_LIST_ITERATE(Grid->cells[i][j], AGridItem*, i) {
                a_list_free(i->nodes);
                i->nodes = NULL; // In case grid is freed before the items
            }

            a_list_free(Grid->cells[i][j]);
        }
    }

    free(Grid->cells);
    free(Grid->cellsData);
    free(Grid);
}

AGridItem* a_griditem_new(const AGrid* Grid, void* Context)
{
    AGridItem* i = a_mem_malloc(sizeof(AGridItem));

    i->grid = Grid;
    i->context = Context;
    i->nodes = a_list_new();
    i->x = 0;
    i->y = 0;

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

void a_griditem_coordsSet(AGridItem* Item, AFix X, AFix Y)
{
    const AGrid* grid = Item->grid;
    AList* cellNodes = Item->nodes;
    AList*** cells = grid->cells;

    Item->x = X;
    Item->y = Y;

    // remove item from all the cells it was previously in
    a_list_clearEx(cellNodes, (AFree*)a_list_removeNode);

    // center cell coords
    int cellX = a_fix_toInt(Item->x >> grid->coordsShift);
    int cellY = a_fix_toInt(Item->y >> grid->coordsShift);

    AFix cellDim = A_FIX_ONE << grid->coordsShift;

    AFix cellOffsetX = Item->x & (cellDim - 1);
    AFix cellOffsetY = Item->y & (cellDim - 1);

    int cellStartX, cellStartY;
    int cellEndX, cellEndY;

    if(cellOffsetX < cellDim / 2) {
        cellStartX = a_math_clamp(cellX - 1, 0, grid->w - 1);
        cellEndX = a_math_clamp(cellX, 0, grid->w - 1);
    } else {
        cellStartX = a_math_clamp(cellX, 0, grid->w - 1);
        cellEndX = a_math_clamp(cellX + 1, 0, grid->w - 1);
    }

    if(cellOffsetY < cellDim / 2) {
        cellStartY = a_math_clamp(cellY - 1, 0, grid->h - 1);
        cellEndY = a_math_clamp(cellY, 0, grid->h - 1);
    } else {
        cellStartY = a_math_clamp(cellY, 0, grid->h - 1);
        cellEndY = a_math_clamp(cellY + 1, 0, grid->h - 1);
    }

    // add item to every cell in its surrounding perimeter
    for(int y = cellStartY; y <= cellEndY; y++) {
        for(int x = cellStartX; x <= cellEndX; x++) {
            a_list_addFirst(cellNodes, a_list_addFirst(cells[y][x], Item));
        }
    }
}

void* a__griditem_contextGet(const AGridItem* Item)
{
    return Item->context;
}

AList* a__griditem_nearbyListGet(const AGridItem* Item)
{
    const AGrid* grid = Item->grid;

    int cellX = a_math_clamp(
                    a_fix_toInt(Item->x >> grid->coordsShift), 0, grid->w - 1);
    int cellY = a_math_clamp(
                    a_fix_toInt(Item->y >> grid->coordsShift), 0, grid->h - 1);

    return grid->cells[cellY][cellX];
}
