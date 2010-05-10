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
    int dim; // dimension of a submap, in pixels
    int w; // width of map, in submaps
    int h; // height of map, in submaps
    List*** submaps; // each is a list of colboxes that are in a submap
};

struct ColBox {
    fix8 x;
    fix8 y;
    fix8 w;
    fix8 h;
    ColMap* colmap; // the colmap this box moves in
    List* submaps; // list of submaps this box is in
    List* nodes;
    void* parent;
};

struct ColIterator {
    ColBox* box;
    ListIterator* submaps; // list of submaps this box is in
    ListIterator* boxes; // list of boxes in the current submap
};

ColMap* a_colmap_set(const int dim, const int w, const int h)
{
    ColMap* const c = malloc(sizeof(ColMap));

    c->dim = dim;
    c->w = w / dim;
    c->h = h / dim;

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

ColBox* a_colbox_set(ColMap* const colmap, const int w, const int h)
{
    ColBox* const c = malloc(sizeof(ColBox));

    c->w = a_fix8_itofix(w);
    c->h = a_fix8_itofix(h);

    c->colmap = colmap;

    c->submaps = a_list_set();
    c->nodes = a_list_set();

    return c;
}

void a_colbox_free(ColBox* const c)
{
    List* const nodes = c->nodes;

    while(a_list_iterate(nodes)) {
        ListNode* const n = a_list__current(nodes);
        a_list_removeNode(n);
    }

    a_list_free(c->nodes);
    a_list_free(c->submaps);

    free(c);
}

void a_colbox_setCoords(ColBox* const b, const fix8 x, const fix8 y)
{
    b->x = x;
    b->y = y;

    a_colmap_update(b->colmap, b);
}

void a_colbox_setParent(ColBox* const b, void* parent)
{
    b->parent = parent;
}

void* a_colbox_getParent(ColBox* const b)
{
    return b->parent;
}

void a_colmap_update(ColMap* const m, ColBox* const b)
{
    List* const submaps = b->submaps;
    List* const nodes = b->nodes;

    // remove box from all the submaps it was in
    while(a_list_iterate(nodes)) {
        a_list_removeNode(a_list__current(nodes));
    }

    a_list_empty(nodes);
    a_list_empty(submaps);

    const fix8 bx = b->x;
    const fix8 by = b->y;
    const fix8 bw = b->w;
    const fix8 bh = b->h;

    const fix8 bw_div2 = (bw >> 1) + FONE8;
    const fix8 bw_add2 = bw + 2 * FONE8;
    const fix8 bh_div2 = (bh >> 1) + FONE8;
    const fix8 bh_add2 = bh + 2 * FONE8;

    const int cx = a_fix8_fixtoi(bx) / m->dim;
    const int cy = a_fix8_fixtoi(by) / m->dim;

    const int starty = a_math_max(0, cy - 1);
    const int endy = a_math_min(bh - 1, cy + 1);
    const int startx = a_math_max(0, cx - 1);
    const int endx = a_math_min(bw - 1, cx + 1);

    List*** const msubmaps = m->submaps;
    const fix8 mdim = a_fix8_itofix(m->dim);

    int counter = 0;

    for(int i = starty; i <= endy; i++) {
        for(int j = startx; j <= endx; j++) {
            if(a_collide_boxes(
                bx - bw_div2, by - bh_div2, bw_add2, bh_add2,
                j * mdim, i * mdim, mdim, mdim
            )) {
                List* const submap = msubmaps[i][j];

                // add box to the submap, save node to box's nodes list
                a_list_addFirst(nodes, a_list_addFirst(submap, b));

                // add submap to box's list of submaps
                a_list_addFirst(submaps, submap);

                // 4 submaps are enough
                if(++counter == 4) {
                    return;
                }
            }
        }
    }
}

ColIterator* a_colbox_setIterator(ColBox* const b)
{
    ColIterator* const it = malloc(sizeof(ColIterator));

    it->box = b;

    it->submaps = a_list_setIterator(b->submaps);
    it->boxes = a_list_setIterator(a_list_iteratorGet(it->submaps));

    return it;
}

void a_colbox_freeIterator(ColIterator* const it)
{
    a_list_freeIterator(it->submaps);
    a_list_freeIterator(it->boxes);

    free(it);
}

int a_colbox_iteratorNext(ColIterator* const it)
{
    if(a_list_iteratorNext(it->boxes)) {
        return 1;
    } else {
        if(a_list_iteratorNext(it->submaps)) {
            it->boxes = a_list_setIterator(a_list_iteratorGet(it->submaps));
            return a_colbox_iteratorNext(it);
        } else {
            return 0;
        }
    }
}

ColBox* a_colbox_iteratorGet(const ColIterator* const it)
{
    return a_list_iteratorGet(it->boxes);
}

int a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2)
{
    const int x = x1 - x2;
    const int y = y1 - y2;
    const int r = r1 + r2;

    return x * x + y * y < r * r;
}
