/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_input_touch.v.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_str.v.h"

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

typedef struct {
    int x;
    int y;
} AInputTouchPoint;

static AStrHash* g_sourceTouchScreens;

void a_input_touch__init(void)
{
    g_sourceTouchScreens = a_strhash_new();
}

void a_input_touch__uninit(void)
{
    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputTouchSource*, t) {
        a_list_freeEx(t->motion, free);
        a_input__sourceHeaderFree(&t->header);
    }

    a_strhash_free(g_sourceTouchScreens);
}

AInputTouchSource* a_input_touch__newSource(const char* Id)
{
    AInputTouchSource* t = a_mem_malloc(sizeof(AInputTouchSource));

    a_input__sourceHeaderInit(&t->header, Id);

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

    a_input__userHeaderInit(&t->header);

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        a_input__userHeaderFindSource(
            &t->header, id, g_sourceTouchScreens, NULL);
    }

    a_list_freeEx(tok, free);

    if(a_list_isEmpty(t->header.sourceInputs)) {
        a_out__error("No touch screen found for '%s'", Ids);
    }

    return t;
}

void a_touch_free(AInputTouch* Touch)
{
    if(Touch == NULL) {
        return;
    }

    a_input__userHeaderFree(&Touch->header);

    free(Touch);
}

bool a_touch_isWorking(const AInputTouch* Touch)
{
    return !a_list_isEmpty(Touch->header.sourceInputs);
}

void a_touch_deltaGet(const AInputTouch* Touch, int* Dx, int* Dy)
{
    AInputTouchSource* t = a_list_getFirst(Touch->header.sourceInputs);

    *Dx = t->dx;
    *Dy = t->dy;
}

bool a_touch_tapGet(const AInputTouch* Touch)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputTouchSource*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_pointGet(const AInputTouch* Touch, int X, int Y)
{
    return a_touch_boxGet (Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_boxGet(const AInputTouch* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputTouchSource*, t) {
        if(t->tap && a_collide_pointInBox(t->x, t->y, X, Y, W, H)) {
            return true;
        }
    }

    return false;
}

void a_input_touch__motionAdd(AInputTouchSource* Touch, int X, int Y)
{
    Touch->x = X;
    Touch->y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        AInputTouchPoint* p = a_mem_malloc(sizeof(AInputTouchPoint));

        p->x = Touch->x;
        p->y = Touch->y;

        a_list_addLast(Touch->motion, p);
    }

    a_input__freshEventSet(&Touch->header);
}

void a_input_touch__motionClear(void)
{
    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputTouchSource*, touchScreen) {
        touchScreen->tap = false;
        a_list_clearEx(touchScreen->motion, free);
    }
}

void a_input_touch__coordsSet(AInputTouchSource* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;

    a_input__freshEventSet(&Touch->header);
}

void a_input_touch__deltaSet(AInputTouchSource* Touch, int Dx, int Dy)
{
    Touch->dx = Dx;
    Touch->dy = Dy;
}
