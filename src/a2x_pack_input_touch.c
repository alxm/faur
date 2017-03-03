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
    AInputHeader header;
};

struct AInputSourceTouch {
    AInputSourceHeader header;
    int x;
    int y;
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
        a_input__freeHeader(&t->header);
        free(t);
    }

    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputSourceTouch*, t) {
        A_LIST_ITERATE(t->motion, AInputTouchPoint*, p) {
            free(p);
        }

        a_list_free(t->motion);

        a_input__freeSourceHeader(&t->header);
    }

    a_list_free(g_touchScreens);
    a_strhash_free(g_sourceTouchScreens);
}

AInputSourceTouch* a_input__newSourceTouch(const char* Name)
{
    AInputSourceTouch* t = a_mem_malloc(sizeof(AInputSourceTouch));

    a_input__initSourceHeader(&t->header, Name);

    t->tap = false;
    t->x = 0;
    t->y = 0;
    t->motion = a_list_new();

    a_strhash_add(g_sourceTouchScreens, Name, t);

    return t;
}

AInputTouch* a_touch_new(const char* Names)
{
    AInputTouch* t = a_mem_malloc(sizeof(AInputTouch));

    a_input__initHeader(&t->header);

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        a_input__findSourceInput(name, g_sourceTouchScreens, &t->header);
    }

    a_strtok_free(tok);

    if(a_list_empty(t->header.sourceInputs)) {
        a_out__error("No touch screen found for '%s'", Names);
    }

    a_list_addLast(g_touchScreens, t);

    return t;
}

bool a_touch_working(const AInputTouch* Touch)
{
    return !a_list_empty(Touch->header.sourceInputs);
}

bool a_touch_tapped(const AInputTouch* Touch)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputSourceTouch*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_point(const AInputTouch* Touch, int X, int Y)
{
    return a_touch_box(Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_box(const AInputTouch* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputSourceTouch*, t) {
        if(t->tap && a_collide_pointInBox(t->x, t->y, X, Y, W, H)) {
            return true;
        }
    }

    return false;
}

void a_input_touch__addMotion(AInputSourceTouch* Touch, int X, int Y)
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

void a_input_touch__setCoords(AInputSourceTouch* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;

    a_input__setFreshEvent(&Touch->header);
}

void a_input_touch__clearMotion(void)
{
    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputSourceTouch*, touchScreen) {
        touchScreen->tap = false;

        A_LIST_ITERATE(touchScreen->motion, AInputTouchPoint*, p) {
            free(p);
        }

        a_list_clear(touchScreen->motion);
    }
}
