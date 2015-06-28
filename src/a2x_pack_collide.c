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
    int w, h; // width and height of map, in submaps
    int bitShift; // right-shift object coords by this to get submap index
    List*** submaps; // List*[h][w] of ColObjects
};

struct ColObject {
    int x;
    int y;
    ColMap* colmap; // the colmap this point belongs to
    List* nodes; // ListNodes from submaps this point is in
    void* parent; // the object that uses this ColObject
};

ColMap* a_colmap_new(int width, int height, int maxObjectDim)
{
    ColMap* const m = malloc(sizeof(ColMap));

    #define nextpow(value)           \
    ({                               \
        int p = 0;                   \
        while((1 << p) < value) p++; \
        p;                           \
    })

    m->bitShift = nextpow(maxObjectDim);
    m->w = 1 << a_math_max(0, nextpow(width) - m->bitShift);
    m->h = 1 << a_math_max(0, nextpow(height) - m->bitShift);

    m->submaps = malloc(m->h * sizeof(List**));

    for(int i = m->h; i--; ) {
        m->submaps[i] = malloc(m->w * sizeof(List*));

        for(int j = m->w; j--; ) {
            m->submaps[i][j] = a_list_new();
        }
    }

    return m;
}

void a_colmap_free(ColMap* m)
{
    for(int i = m->h; i--; ) {
        for(int j = m->w; j--; ) {
            a_list_free(m->submaps[i][j]);
        }

        free(m->submaps[i]);
    }

    free(m->submaps);
    free(m);
}

ColObject* a_colobject_new(ColMap* colmap, void* parent)
{
    ColObject* const o = malloc(sizeof(ColObject));

    o->colmap = colmap;
    o->nodes = a_list_new();
    o->parent = parent;

    return o;
}

void a_colobject_free(ColObject* const o)
{
    A_LIST_ITERATE(o->nodes, ListNode, n) {
        a_list_removeNode(n);
    }

    a_list_free(o->nodes);
    free(o);
}

void a_colobject_setCoords(ColObject* o, int x, int y)
{
    ColMap* const m = o->colmap;
    List* const pt_nodes = o->nodes;

    // remove point from all the submaps it was in
    A_LIST_ITERATE(pt_nodes, ListNode, n) {
        a_list_removeNode(n);
    }

    // purge old information
    a_list_empty(pt_nodes);

    // set new coords
    o->x = x;
    o->y = y;

    // center submap coords
    const int submap_x = o->x >> m->bitShift;
    const int submap_y = o->y >> m->bitShift;

    // submap perimeter
    const int startx = a_math_constrain(submap_x - 1, 0, m->w - 1);
    const int endx = a_math_constrain(submap_x + 1, 0, m->w - 1);
    const int starty = a_math_constrain(submap_y - 1, 0, m->h - 1);
    const int endy = a_math_constrain(submap_y + 1, 0, m->h - 1);

    // submap matrix
    List*** const submaps = m->submaps;

    // add object to every submap in its surrounding perimeter
    for(int i = starty; i <= endy; i++) {
        for(int j = startx; j <= endx; j++) {
            // add point to the submap, save node to point's nodes list
            a_list_addFirst(pt_nodes, a_list_addFirst(submaps[i][j], o));
        }
    }
}

void* a_colobject_getParent(ColObject* const o)
{
    return o->parent;
}

ColIt a_colit__new(ColObject* const p)
{
    ColIt it;
    ColMap* const colmap = p->colmap;

    const int submap_x = p->x >> colmap->bitShift;
    const int submap_y = p->y >> colmap->bitShift;

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

ColObject* a_colit__get(ColIt* it)
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
