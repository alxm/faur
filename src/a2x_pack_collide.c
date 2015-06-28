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

#include "a2x_pack_collide.v.h"

struct ColMap {
    int w; // width of map, in submaps
    int h; // height of map, in submaps
    int submapShift; // right-shift coords by this to get submap
    List*** submaps; // matrix of lists of colpoints
};

struct ColPoint {
    fix x;
    fix y;
    ColMap* colmap; // the colmap this point belongs to
    List* nodes; // ListNodes from submaps this point is in
    void* parent; // the object that uses this ColPoint
};

ColMap* a_colmap_new(int totalWidth, int totalHeight, int gridDim)
{
    ColMap* const c = malloc(sizeof(ColMap));

    #define nextpow(x) \
    ({                 \
        int p = 1;     \
        while(p < x) { \
            p <<= 1;   \
        }              \
        p;             \
    })

    const int submapDim = nextpow(gridDim);

    c->submapShift = log2(submapDim);

    c->w = nextpow(totalWidth) / submapDim;
    c->h = nextpow(totalHeight) / submapDim;

    #undef nextpow

    c->submaps = malloc(c->h * sizeof(List**));

    for(int i = c->h; i--; ) {
        c->submaps[i] = malloc(c->w * sizeof(List*));

        for(int j = c->w; j--; ) {
            c->submaps[i][j] = a_list_new();
        }
    }

    return c;
}

void a_colmap_free(ColMap* c)
{
    for(int i = c->h; i--; ) {
        for(int j = c->w; j--; ) {
            a_list_free(c->submaps[i][j]);
        }

        free(c->submaps[i]);
    }

    free(c->submaps);
    free(c);
}

ColPoint* a_colpoint_new(ColMap* colmap, void* parent)
{
    ColPoint* const p = malloc(sizeof(ColPoint));

    p->colmap = colmap;
    p->nodes = a_list_new();
    p->parent = parent;

    return p;
}

void a_colpoint_free(ColPoint* const p)
{
    List* const nodes = p->nodes;

    A_LIST_ITERATE(nodes, ListNode, n) {
        a_list_removeNode(n);
    }

    a_list_free(p->nodes);

    free(p);
}

void a_colpoint_setCoords(ColPoint* const p, const fix x, const fix y)
{
    p->x = x;
    p->y = y;

    ColMap* const colmap = p->colmap;
    List* const pt_nodes = p->nodes;

    // remove point from all the submaps it was in
    A_LIST_ITERATE(pt_nodes, ListNode, n) {
        a_list_removeNode(n);
    }

    // purge old information
    a_list_empty(pt_nodes);

    // center submap coords
    const int submap_x = a_fix_fixtoi(p->x) >> colmap->submapShift;
    const int submap_y = a_fix_fixtoi(p->y) >> colmap->submapShift;

    // submap perimeter
    const int startx = a_math_max(0, submap_x - 1);
    const int endx = a_math_min(colmap->w - 1, submap_x + 1);
    const int starty = a_math_max(0, submap_y - 1);
    const int endy = a_math_min(colmap->h - 1, submap_y + 1);

    // submap matrix
    List*** const submaps = colmap->submaps;

    for(int i = starty; i <= endy; i++) {
        for(int j = startx; j <= endx; j++) {
            // add point to the submap, save node to point's nodes list
            a_list_addFirst(pt_nodes, a_list_addFirst(submaps[i][j], p));
        }
    }
}

void* a_colpoint_getParent(ColPoint* const p)
{
    return p->parent;
}

ColIt a_colit__new(ColPoint* const p)
{
    ColIt it;
    ColMap* const colmap = p->colmap;

    const int submap_x = a_fix_fixtoi(p->x) >> colmap->submapShift;
    const int submap_y = a_fix_fixtoi(p->y) >> colmap->submapShift;

    it.callerPoint = p;
    it.points = a_listit__new(colmap->submaps[submap_y][submap_x]);

    return it;
}

bool a_colit__next(ColIt* it)
{
    if(a_listit__next(&it->points)) {
        // don't return the point we iterate on
        if(a_listit__peek(&it->points) == it->callerPoint) {
            a_listit__get(&it->points);
            return a_colit__next(it);
        }

        return true;
    }

    return false;
}

ColPoint* a_colit__get(ColIt* it)
{
    return a_listit__get(&it->points);
}

bool a_collide_circles(int x1, int y1, int r1, int x2, int y2, int r2)
{
    const int x = x1 - x2;
    const int y = y1 - y2;
    const int r = r1 + r2;

    return x * x + y * y < r * r;
}
