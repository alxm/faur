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

#pragma once

#include "a2x_pack_input_touch.p.h"

typedef struct AInputSourceTouch AInputSourceTouch;

#include "a2x_pack_input.v.h"

extern void a_input_touch__init(void);
extern void a_input_touch__uninit(void);

extern AInputSourceTouch* a_input__newSourceTouch(const char* Name);

extern void a_input_touch__addMotion(AInputSourceTouch* Touch, int X, int Y);
extern void a_input_touch__setCoords(AInputSourceTouch* Touch, int X, int Y, bool Tapped);

extern void a_input_touch__clearMotion(void);
