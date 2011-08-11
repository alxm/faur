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

#ifndef A2X_PACK_INPUT_PH
#define A2X_PACK_INPUT_PH

#include "a2x_app_includes.h"

typedef struct Input Input;

extern Input* a_input_new(const char* const names);
extern void a_input_free(Input* const i);
extern bool a_input_get(Input* const i);
extern void a_input_unpress(Input* const i);
extern bool a_input_getUnpress(Input* const i);

extern char* a_input_name(const Input* const i);
extern bool a_input_working(const Input* const i);

extern void a_input_waitFor(Input* const i);

extern bool a_input_tappedScreen(void);
extern bool a_input_touchedPoint(const int x, const int y);
extern bool a_input_touchedRect(const int x, const int y, const int w, const int h);

#endif // A2X_PACK_INPUT_PH
