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

#include "a2x_pack_fix.p.h"

typedef struct Input Input;

extern Input* a_input_new(const char* names);

extern char* a_input_name(const Input* i);
extern bool a_input_working(const Input* i);

extern bool a_button_get(const Input* i);
extern void a_button_unpress(const Input* i);
extern bool a_button_getAndUnpress(const Input* i);
extern void a_button_waitFor(const Input* i);

extern int a_analog_xaxis(const Input* i);
extern int a_analog_yaxis(const Input* i);
extern fix a_analog_xaxis_fix(const Input* i);
extern fix a_analog_yaxis_fix(const Input* i);

extern bool a_touch_tapped(const Input* i);
extern bool a_touch_point(const Input* i, int x, int y);
extern bool a_touch_rect(const Input* i, int x, int y, int w, int h);

#endif // A2X_PACK_INPUT_PH
