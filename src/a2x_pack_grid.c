/*
    Copyright 2010, 2011, 2015-2018 Alex Margarit

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

#include "a2x_pack_grid.v.h"

#include "a2x_pack_listit.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"

struct AGrid {
    int w, h; // width and height of grid in cells
    int coordsShift; // right-shift item coords to get cell index
    AList*** cells; // AList*[h][w] of AGridItem
    AList** cellsData; // AList*[h * w] of AGridItem
};

struct AGridItem {
    const AGrid* grid;
    int x, y; // coords on the grid
    AList* nodes; // list of AListNode from cells this item is in
    void* context; // the game item that owns this AGridItem
};

static inline int nextpow(int X)
{
    int power = 0;

    while((1 << power) < X) {
        power++;
    }

    return power;
}

AGrid* a_grid_new(int Width, int Height, int MaxObjectDim)
{
    AGrid* g = a_mem_malloc(sizeof(AGrid));

    g->coordsShift = nextpow(MaxObjectDim);
    g->w = 1 << a_math_max(0, nextpow(Width) - g->coordsShift);
    g->h = 1 << a_math_max(0, nextpow(Height) - g->coordsShift);

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
    i->nodes = a_list_new();
    i->context = Context;

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

void a_griditem_coordsSet(AGridItem* Item, int X, int Y)
{
    const AGrid* grid = Item->grid;
    AList* cellNodes = Item->nodes;
    AList*** cells = grid->cells;

    Item->x = X;
    Item->y = Y;

    // remove item from all the cells it was previously in
    a_list_clearEx(cellNodes, (AFree*)a_list_removeNode);

    // center cell coords
    const int cellX = Item->x >> grid->coordsShift;
    const int cellY = Item->y >> grid->coordsShift;

    const int cellDim = 1 << grid->coordsShift;
    int cellOffsetX = Item->x & (cellDim - 1);
    int cellOffsetY = Item->y & (cellDim - 1);

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

    int cellX = a_math_clamp(Item->x >> grid->coordsShift, 0, grid->w - 1);
    int cellY = a_math_clamp(Item->y >> grid->coordsShift, 0, grid->h - 1);

    return grid->cells[cellY][cellX];
}
