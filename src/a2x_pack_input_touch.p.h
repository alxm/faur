/*
    Copyright 2010, 2017, 2018 Alex Margarit

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

#pragma once

typedef struct AInputTouch AInputTouch;

extern AInputTouch* a_touch_new(const char* Ids);
extern void a_touch_free(AInputTouch* Touch);

extern bool a_touch_isWorking(const AInputTouch* Touch);

extern void a_touch_deltaGet(const AInputTouch* Touch, int* Dx, int* Dy);
extern bool a_touch_tapGet(const AInputTouch* Touch);
extern bool a_touch_pointGet(const AInputTouch* Touch, int X, int Y);
extern bool a_touch_boxGet(const AInputTouch* Touch, int X, int Y, int W, int H);
