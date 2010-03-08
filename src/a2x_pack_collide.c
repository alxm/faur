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

int a_collide_circles(const int x1, const int y1, const int r1, const int x2, const int y2, const int r2)
{
    const int x = x1 - x2;
    const int y = y1 - y2;
    const int r = r1 + r2;

    return x * x + y * y < r * r;
}

ColMap* a_collide_makeColMap(const int dim, const int w, const int h)
{
    ColMap* const c = malloc(sizeof(ColMap));

    c->w = w;
    c->h = h;
    c->dim = dim;
    c->maps = malloc(h * sizeof(List**));

    for(int i = h; i--; ) {
        c->maps[i] = malloc(w * sizeof(List*));

        for(int j = w; j--; ) {
            c->maps[i][j] = a_list_set();
        }
    }

    return c;
}

void a_collide_freeColMap(ColMap* const c)
{
    for(int i = c->h; i--; ) {
        for(int j = c->w; j--; ) {
            a_list_free(c->maps[i][j]);
        }

        free(c->maps[i]);
    }

    free(c->maps);
    free(c);
}

ColBox* a_collide_makeColBox(const fix8 x, const fix8 y, const int w, const int h, void* const parent)
{
    ColBox* const c = malloc(sizeof(ColBox));

    c->x = x;
    c->y = y;
    c->w = a_fix8_itofix(w);
    c->h = a_fix8_itofix(h);
    c->nodes = a_list_set();
    c->maps = a_list_set();
    c->parent = parent;

    return c;
}

void a_collide_freeColBox(ColBox* const c)
{
    List* const nodes = c->nodes;

    while(a_list_iterate(nodes)) {
        ListNode* const n = a_list__current(nodes);
        a_list_removeNode(n);
    }

    a_list_free(c->nodes);
    a_list_free(c->maps);
    free(c);
}

void a_collide_assignColMaps(ColMap* const cm, ColBox* const o)
{
    const fix8 cdim = a_fix8_itofix(cm->dim);
    const int cw = cm->w;
    const int ch = cm->h;
    List*** const m = cm->maps;

    List* const nodes = o->nodes;
    List* const maps = o->maps;

    while(a_list_iterate(nodes)) {
        ListNode* const n = a_list__current(nodes);
        a_list_removeNode(n);
    }

    a_list_empty(nodes);
    a_list_empty(maps);

    int counter = 0;

    const fix8 x = o->x;
    const fix8 y = o->y;
    const fix8 w = o->w;
    const fix8 h = o->h;

    const int cx = a_fix8_fixtoi(x) / cm->dim;
    const int cy = a_fix8_fixtoi(y) / cm->dim;

    const int starty = (cy > 0) ? (cy - 1) : (0);
    const int endy = (cy < ch - 1) ? (cy + 1) : (cy);

    const int startx = (cx > 0) ? (cx - 1) : (0);
    const int endx = (cx < cw - 1) ? (cx + 1) : (cx);

    for(int i = starty; i <= endy; i++) {
        for(int j = startx; j <= endx; j++) {
            if(a_collide_boxes(
                x - (w >> 1) - FONE8,
                y - (h >> 1) - FONE8,
                w + 2 * FONE8,
                h + 2 * FONE8,
                j * cdim,
                i * cdim,
                cdim,
                cdim
            )) {
                List* const map = m[i][j];

                a_list_addFirst(nodes, a_list_addFirst(map, o));
                a_list_addFirst(maps, map);

                if(++counter == 4) return;
            }
        }
    }
}
