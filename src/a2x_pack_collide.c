/*
    Copyright 2010, 2016 Alex Margarit

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

struct AColMap {
    int w, h; // width and height of map, in submaps
    int bitShift; // right-shift object coords by this to get submap index
    AList*** submaps; // AList*[h][w] of AColObjects
    AList** submapsData; // AList*[h * w] of AColObjects
};

struct AColObject {
    const AColMap* colmap;
    int x, y; // coords on the Colmap
    AList* nodes; // AListNodes from submaps this object is in
    void* userObject; // the game object that owns this AColObject
};

static inline int nextpow(int X)
{
    int power = 0;

    while((1 << power) < X) {
        power++;
    }

    return power;
}

AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim)
{
    AColMap* m = a_mem_malloc(sizeof(AColMap));

    m->bitShift = nextpow(MaxObjectDim);
    m->w = 1 << a_math_max(0, nextpow(Width) - m->bitShift);
    m->h = 1 << a_math_max(0, nextpow(Height) - m->bitShift);

    m->submaps = a_mem_malloc((unsigned)m->h * sizeof(AList**));
    m->submapsData = a_mem_malloc((unsigned)m->h * (unsigned)m->w
                                    * sizeof(AList*));

    for(int i = m->h; i--; ) {
        m->submaps[i] = m->submapsData + i * m->w;

        for(int j = m->w; j--; ) {
            m->submaps[i][j] = a_list_new();
        }
    }

    return m;
}

void a_colmap_free(AColMap* Map)
{
    for(int i = Map->h; i--; ) {
        for(int j = Map->w; j--; ) {
            // In case ColMap was freed before the ColObjects
            A_LIST_ITERATE(Map->submaps[i][j], AColObject*, o) {
                if(o->nodes != NULL) {
                    a_list_free(o->nodes);
                    o->nodes = NULL;
                }
            }

            a_list_free(Map->submaps[i][j]);
        }
    }

    free(Map->submaps);
    free(Map->submapsData);
    free(Map);
}

AColObject* a_colobject_new(const AColMap* Map, void* UserObject)
{
    AColObject* o = a_mem_malloc(sizeof(AColObject));

    o->colmap = Map;
    o->nodes = a_list_new();
    o->userObject = UserObject;

    return o;
}

void a_colobject_free(AColObject* Object)
{
    // Remove object from any lists it is in
    if(Object->nodes != NULL) {
        A_LIST_ITERATE(Object->nodes, AListNode*, n) {
            a_list_removeNode(n);
        }

        a_list_free(Object->nodes);
    }

    free(Object);
}

void a_colobject_setCoords(AColObject* Object, int X, int Y)
{
    const AColMap* m = Object->colmap;
    AList* pt_nodes = Object->nodes;
    AList*** submaps = m->submaps;

    // remove point from all the submaps it was in
    A_LIST_ITERATE(pt_nodes, AListNode*, n) {
        a_list_removeNode(n);
    }

    // purge old information
    a_list_clear(pt_nodes);

    // set new coords
    Object->x = X;
    Object->y = Y;

    // center submap coords
    const int submap_x = Object->x >> m->bitShift;
    const int submap_y = Object->y >> m->bitShift;

    // offset inside submap
    const int submapDim = 1 << m->bitShift;
    int offset_x = Object->x & (submapDim - 1);
    int offset_y = Object->y & (submapDim - 1);

    // submaps perimeter
    int start_x, end_x;
    int start_y, end_y;

    if(offset_x < submapDim / 2) {
        start_x = a_math_constrain(submap_x - 1, 0, m->w - 1);
        end_x = a_math_constrain(submap_x, 0, m->w - 1);
    } else {
        start_x = a_math_constrain(submap_x, 0, m->w - 1);
        end_x = a_math_constrain(submap_x + 1, 0, m->w - 1);
    }

    if(offset_y < submapDim / 2) {
        start_y = a_math_constrain(submap_y - 1, 0, m->h - 1);
        end_y = a_math_constrain(submap_y, 0, m->h - 1);
    } else {
        start_y = a_math_constrain(submap_y, 0, m->h - 1);
        end_y = a_math_constrain(submap_y + 1, 0, m->h - 1);
    }

    // add object to every submap in its surrounding perimeter
    for(int i = start_y; i <= end_y; i++) {
        for(int j = start_x; j <= end_x; j++) {
            // add point to the submap, save node to point's nodes list
            a_list_addFirst(pt_nodes, a_list_addFirst(submaps[i][j], Object));
        }
    }
}

void* a_colobject__getUserObject(const AColObject* Object)
{
    return Object->userObject;
}

AList* a_colobject__getPossibleCollisions(const AColObject* Object)
{
    const AColMap* map = Object->colmap;

    int submap_x = a_math_constrain(Object->x >> map->bitShift, 0, map->w - 1);
    int submap_y = a_math_constrain(Object->y >> map->bitShift, 0, map->h - 1);

    return map->submaps[submap_y][submap_x];
}

bool a_collide_boxAndBox(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2)
{
    return !(Y1 >= Y2 + H2
          || Y2 >= Y1 + H1
          || X1 >= X2 + W2
          || X2 >= X1 + W1);
}

bool a_collide_circleAndCircle(int X1, int Y1, int R1, int X2, int Y2, int R2)
{
    const int x = X1 - X2;
    const int y = Y1 - Y2;
    const int r = R1 + R2;

    return x * x + y * y < r * r;
}

bool a_collide_pointInBox(int X, int Y, int BoxX, int BoxY, int BoxW, int BoxH)
{
    return X >= BoxX && X < BoxX + BoxW && Y >= BoxY && Y < BoxY + BoxH;
}

bool a_collide_pointInCircle(int X, int Y, int CircleX, int CircleY, int CircleR)
{
    const int dx = X - CircleX;
    const int dy = Y - CircleY;

    return dx * dx + dy * dy < CircleR * CircleR;
}
