/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_input_touch.v.h"

struct AInputTouch {
    AInputUserHeader header;
};

struct AInputTouchSource {
    AInputSourceHeader header;
    int x, y;
    int dx, dy;
    bool tap;
    AList* motion; // AInputTouchPoints captured by motion event
};

typedef struct AInputTouchPoint {
    int x;
    int y;
} AInputTouchPoint;

static AList* g_touchScreens;
static AStrHash* g_sourceTouchScreens;

void a_input_touch__init(void)
{
    g_touchScreens = a_list_new();
    g_sourceTouchScreens = a_strhash_new();
}

void a_input_touch__uninit(void)
{
    A_LIST_ITERATE(g_touchScreens, AInputTouch*, t) {
        a_input__freeUserHeader(&t->header);
        free(t);
    }

    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputTouchSource*, t) {
        a_list_freeEx(t->motion, free);
        a_input__freeSourceHeader(&t->header);
    }

    a_list_free(g_touchScreens);
    a_strhash_free(g_sourceTouchScreens);
}

AInputTouchSource* a_input_touch__newSource(const char* Id)
{
    AInputTouchSource* t = a_mem_malloc(sizeof(AInputTouchSource));

    a_input__initSourceHeader(&t->header, Id);

    t->tap = false;
    t->x = 0;
    t->y = 0;
    t->dx = 0;
    t->dy = 0;
    t->motion = a_list_new();

    a_strhash_add(g_sourceTouchScreens, Id, t);

    return t;
}

AInputTouch* a_touch_new(const char* Ids)
{
    AInputTouch* t = a_mem_malloc(sizeof(AInputTouch));

    a_input__initUserHeader(&t->header);

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        a_input__findSourceInput(g_sourceTouchScreens, NULL, id, &t->header);
    }

    a_list_freeEx(tok, free);

    if(a_list_isEmpty(t->header.sourceInputs)) {
        a_out__error("No touch screen found for '%s'", Ids);
    }

    a_list_addLast(g_touchScreens, t);

    return t;
}

bool a_touch_isWorking(const AInputTouch* Touch)
{
    return !a_list_isEmpty(Touch->header.sourceInputs);
}

void a_touch_getDelta(const AInputTouch* Touch, int* Dx, int* Dy)
{
    AInputTouchSource* t = a_list_getFirst(Touch->header.sourceInputs);

    *Dx = t->dx;
    *Dy = t->dy;
}

bool a_touch_getTap(const AInputTouch* Touch)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputTouchSource*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_getPoint(const AInputTouch* Touch, int X, int Y)
{
    return a_touch_getBox(Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_getBox(const AInputTouch* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputTouchSource*, t) {
        if(t->tap && a_collide_pointInBox(t->x, t->y, X, Y, W, H)) {
            return true;
        }
    }

    return false;
}

void a_input_touch__addMotion(AInputTouchSource* Touch, int X, int Y)
{
    Touch->x = X;
    Touch->y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        AInputTouchPoint* p = a_mem_malloc(sizeof(AInputTouchPoint));

        p->x = Touch->x;
        p->y = Touch->y;

        a_list_addLast(Touch->motion, p);
    }

    a_input__setFreshEvent(&Touch->header);
}

void a_input_touch__setCoords(AInputTouchSource* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;

    a_input__setFreshEvent(&Touch->header);
}

void a_input_touch__setDelta(AInputTouchSource* Touch, int Dx, int Dy)
{
    Touch->dx = Dx;
    Touch->dy = Dy;
}

void a_input_touch__clearMotion(void)
{
    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputTouchSource*, touchScreen) {
        touchScreen->tap = false;
        a_list_clearEx(touchScreen->motion, free);
    }
}
