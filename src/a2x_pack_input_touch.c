/*
    Copyright 2010, 2016-2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_input_touch.v.h"

#include "a2x_pack_collide.v.h"
#include "a2x_pack_input.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_platform.v.h"

struct ATouch {
    AInputUserHeader header;
};

ATouch* a_touch_new(void)
{
    ATouch* t = a_mem_malloc(sizeof(ATouch));

    a_input__userHeaderInit(&t->header);

    APlatformInputTouch* pt = a_platform_api__inputTouchGet();

    if(pt) {
        a_list_addLast(t->header.platformInputs, pt);
    }

    return t;
}

void a_touch_free(ATouch* Touch)
{
    if(Touch == NULL) {
        return;
    }

    a_input__userHeaderFree(&Touch->header);

    free(Touch);
}

bool a_touch_isWorking(const ATouch* Touch)
{
    return !a_list_isEmpty(Touch->header.platformInputs);
}

void a_touch_deltaGet(const ATouch* Touch, int* Dx, int* Dy)
{
    APlatformInputTouch* pt = a_list_getFirst(Touch->header.platformInputs);

    a_platform_api__inputTouchDeltaGet(pt, Dx, Dy);
}

bool a_touch_tapGet(const ATouch* Touch)
{
    A_LIST_ITERATE(Touch->header.platformInputs, APlatformInputTouch*, pt) {
        if(a_platform_api__inputTouchTapGet(pt)) {
            return true;
        }
    }

    return false;
}

bool a_touch_pointGet(const ATouch* Touch, int X, int Y)
{
    return a_touch_boxGet (Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_boxGet(const ATouch* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->header.platformInputs, APlatformInputTouch*, pt) {
        int x, y;
        a_platform_api__inputTouchCoordsGet(pt, &x, &y);

        if(a_platform_api__inputTouchTapGet(pt)
            && a_collide_pointInBox(x, y, X, Y, W, H)) {

            return true;
        }
    }

    return false;
}
