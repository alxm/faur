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

#include "a2x_pack_collide.p.h"
#include "a2x_pack_collide.v.h"

struct ColMap {
    int w; // width of map, in submaps
    int h; // height of map, in submaps
    int submapShift; // right-shift coords by this to get submap
    List*** submaps; // matrix of lists of colpoints
};

struct ColPoint {
    fix8 x;
    fix8 y;
    ColMap* colmap; // the colmap this point belongs to
    List* submaps; // submaps this point is in
    List* nodes; // ListNodes from submaps this point is in
    void* parent; // the object that uses this ColPoint
};

struct ColIterator {
    ColPoint* callerPoint;
    ListIterator* submaps; // list of submaps this point is in
    ListIterator* points; // list of points in the current submap
};

ColMap* a_colmap_set(const int totalWidth, const int totalHeight, const int gridDim)
{
    ColMap* const c = malloc(sizeof(ColMap));

    #define nextpow(x) \
    ({ \
        int p = 1; \
        while(p < x) { \
            p <<= 1; \
        } \
        p; \
    })

    const int submapDim = nextpow(gridDim);

    c->submapShift = a_math_log2(submapDim);

    c->w = nextpow(totalWidth) / submapDim;
    c->h = nextpow(totalHeight) / submapDim;

    c->submaps = malloc(c->h * sizeof(List**));

    for(int i = c->h; i--; ) {
        c->submaps[i] = malloc(c->w * sizeof(List*));

        for(int j = c->w; j--; ) {
            c->submaps[i][j] = a_list_set();
        }
    }

    return c;
}

void a_colmap_free(ColMap* const c)
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

ColPoint* a_colpoint_set(ColMap* const colmap)
{
    ColPoint* const p = malloc(sizeof(ColPoint));

    p->colmap = colmap;

    p->submaps = a_list_set();
    p->nodes = a_list_set();

    return p;
}

void a_colpoint_free(ColPoint* const p)
{
    List* const nodes = p->nodes;

    while(a_list_iterate(nodes)) {
        ListNode* const n = a_list__current(nodes);
        a_list_removeNode(n);
    }

    a_list_free(p->nodes);
    a_list_free(p->submaps);

    free(p);
}

void a_colpoint_setCoords(ColPoint* const p, const fix8 x, const fix8 y)
{
    p->x = x;
    p->y = y;

    ColMap* const colmap = p->colmap;

    List* const pt_submaps = p->submaps;
    List* const pt_nodes = p->nodes;

    // remove point from all the submaps it was in
    while(a_list_iterate(pt_nodes)) {
        a_list_removeNode(a_list__current(pt_nodes));
    }

    // purge old information
    a_list_empty(pt_nodes);
    a_list_empty(pt_submaps);

    // center submap coords
    const int submap_x = a_fix8_fixtoi(p->x) >> colmap->submapShift;
    const int submap_y = a_fix8_fixtoi(p->y) >> colmap->submapShift;

    // submap perimeter
    const int startx = a_math_max(0, submap_x - 1);
    const int endx = a_math_min(colmap->w - 1, submap_x + 1);
    const int starty = a_math_max(0, submap_y - 1);
    const int endy = a_math_min(colmap->h - 1, submap_y + 1);

    // submap matrix
    List*** const submaps = colmap->submaps;

    for(int i = starty; i <= endy; i++) {
        for(int j = startx; j <= endx; j++) {
            List* const submap = submaps[i][j];

            // add point to the submap, save node to point's nodes list
            a_list_addFirst(pt_nodes, a_list_addFirst(submap, p));

            // add submap to point's list of submaps
            a_list_addFirst(pt_submaps, submap);
        }
    }
}

void a_colpoint_setParent(ColPoint* const p, void* parent)
{
    p->parent = parent;
}

void* a_colpoint_getParent(ColPoint* const p)
{
    return p->parent;
}

ColIterator* a_colpoint_setIterator(ColPoint* const p)
{
    ColIterator* const it = malloc(sizeof(ColIterator));

    it->callerPoint = p;
    it->submaps = a_list_setIterator(p->submaps);
    it->points = a_list_setIterator(a_list_iteratorGet(it->submaps));

    return it;
}

void a_colpoint_freeIterator(ColIterator* const it)
{
    a_list_freeIterator(it->submaps);
    a_list_freeIterator(it->points);

    free(it);
}

int a_colpoint_iteratorNext(ColIterator* const it)
{
    if(a_list_iteratorNext(it->points)) {
        // don't return the point we iterate on
        if(a_list_iteratorGet(it->points) == it->callerPoint) {
            return a_colpoint_iteratorNext(it);
        } else {
            a_list__iteratorRewind(it->points);
            return 1;
        }
    } else {
        if(a_list_iteratorNext(it->submaps)) {
            a_list_freeIterator(it->points);
            it->points = a_list_setIterator(a_list_iteratorGet(it->submaps));
            return a_colpoint_iteratorNext(it);
        } else {
            return 0;
        }
    }
}

ColPoint* a_colpoint_iteratorGet(const ColIterator* const it)
{
    return a_list_iteratorGet(it->points);
}

int a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2)
{
    const int x = x1 - x2;
    const int y = y1 - y2;
    const int r = r1 + r2;

    return x * x + y * y < r * r;
}
