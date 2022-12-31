/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_touch.v.h"
#include <faur.v.h>

FVecInt f_touch_deltaGet(void)
{
    return f_platform_api__inputTouchDeltaGet();
}

bool f_touch_tapGet(void)
{
    return f_platform_api__inputTouchTapGet();
}

bool f_touch_pointGet(int X, int Y)
{
    return f_touch_boxGet(X - 1, Y - 1, 3, 3);
}

bool f_touch_boxGet(int X, int Y, int Width, int Height)
{
    F__CHECK(Width > 0);
    F__CHECK(Height > 0);

    if(f_platform_api__inputTouchTapGet()) {
        FVecInt coords = f_platform_api__inputTouchCoordsGet();

        if(f_collide_pointInBox(
            coords, (FVecInt){X, Y}, (FVecInt){Width, Height})) {

            return true;
        }
    }

    return false;
}
