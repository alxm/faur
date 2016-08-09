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

struct AColMap {
    int w, h; // width and height of map, in submaps
    int bitShift; // right-shift object coords by this to get submap index
    AList*** submaps; // AList*[h][w] of AColObjects
};

struct AColObject {
    const AColMap* colmap;
    int x, y; // coords on the Colmap
    AList* nodes; // AListNodes from submaps this object is in
    void* userObject; // the game object that owns this AColObject
    AListIt iterator;
};

AColMap* a_colmap_new(int Width, int Height, int MaxObjectDim)
{
    AColMap* const m = a_mem_malloc(sizeof(AColMap));

    #define nextpow(value)           \
    ({                               \
        int p = 0;                   \
        while((1 << p) < value) p++; \
        p;                           \
    })

    m->bitShift = nextpow(MaxObjectDim);
    m->w = 1 << a_math_max(0, nextpow(Width) - m->bitShift);
    m->h = 1 << a_math_max(0, nextpow(Height) - m->bitShift);

    m->submaps = a_mem_malloc(m->h * sizeof(AList**));

    for(int i = m->h; i--; ) {
        m->submaps[i] = a_mem_malloc(m->w * sizeof(AList*));

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
            a_list_free(Map->submaps[i][j]);
        }

        free(Map->submaps[i]);
    }

    free(Map->submaps);
    free(Map);
}

AColObject* a_colobject_new(const AColMap* Map, void* UserObject)
{
    AColObject* const o = a_mem_malloc(sizeof(AColObject));

    o->colmap = Map;
    o->nodes = a_list_new();
    o->userObject = UserObject;

    return o;
}

void a_colobject_free(AColObject* Object)
{
    AListNode* n;

    // Remove object from any lists it is in
    A_LIST_ITERATE(Object->nodes, n) {
        a_list_removeNode(n);
    }

    a_list_free(Object->nodes);
    free(Object);
}

void a_colobject_setCoords(AColObject* Object, int X, int Y)
{
    const AColMap* const m = Object->colmap;
    AList* const pt_nodes = Object->nodes;
    AList*** const submaps = m->submaps;
    AListNode* n;

    // remove point from all the submaps it was in
    A_LIST_ITERATE(pt_nodes, n) {
        a_list_removeNode(n);
    }

    // purge old information
    a_list_empty(pt_nodes);

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

void a_colobject__reset(AColObject* Object)
{
    AList* possibleCollisions;
    const AColMap* map = Object->colmap;

    int submap_x = a_math_constrain(Object->x >> map->bitShift, 0, map->w - 1);
    int submap_y = a_math_constrain(Object->y >> map->bitShift, 0, map->h - 1);

    possibleCollisions = map->submaps[submap_y][submap_x];
    Object->iterator = a_listit__new(possibleCollisions);
}

bool a_colobject__getNext(AColObject* Object, void** UserObject)
{
    while(true) {
        if(a_listit__getNext(&Object->iterator)) {
            AColObject* colObject = Object->iterator.currentItem;

            if(colObject == Object) {
                continue;
            }

            *UserObject = a_colobject__getUserObject(colObject);
            return true;
        }

        return false;
    }
}

void* a_colobject__getUserObject(const AColObject* Object)
{
    return Object->userObject;
}

bool a_collide_boxes(int X1, int Y1, int W1, int H1, int X2, int Y2, int W2, int H2)
{
    return !(Y1 >= Y2 + H2
          || Y2 >= Y1 + H1
          || X1 >= X2 + W2
          || X2 >= X1 + W1);
}

bool a_collide_boxOnScreen(int X, int Y, int W, int H)
{
    return a_collide_boxes(X, Y, W, H,
                           0, 0, a_screen__width, a_screen__height);
}

bool a_collide_boxInsideScreen(int X, int Y, int W, int H)
{
    return X >= 0 && Y >= 0
        && X + W <= a_screen__width && Y + H <= a_screen__height;
}

bool a_collide_circles(int X1, int Y1, int R1, int X2, int Y2, int R2)
{
    const int x = X1 - X2;
    const int y = Y1 - Y2;
    const int r = R1 + R2;

    return x * x + y * y < r * r;
}
