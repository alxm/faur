/*
    Copyright 2010, 2017 Alex Margarit

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

#include "a2x_system_includes.h"

#include "a2x_pack_fix.p.h"

typedef struct AInputButton AInputButton;
typedef struct AInputAnalog AInputAnalog;
typedef struct AInputTouch AInputTouch;

extern unsigned a_input_numControllers(void);
extern void a_input_setController(unsigned Index);

extern AInputButton* a_button_new(const char* Names);
extern bool a_button_working(const AInputButton* Button);
extern const char* a_button_name(const AInputButton* Button);
extern bool a_button_get(AInputButton* Button);
extern void a_button_release(const AInputButton* Button);
extern bool a_button_getOnce(AInputButton* Button);
extern void a_button_setRepeat(AInputButton* Button, unsigned RepeatFrames);

extern AInputAnalog* a_analog_new(const char* Names);
extern bool a_analog_working(const AInputAnalog* Analog);
extern int a_analog_axisRaw(const AInputAnalog* Analog);
extern AFix a_analog_axisFix(const AInputAnalog* Analog);

extern AInputTouch* a_touch_new(const char* Names);
extern bool a_touch_working(const AInputTouch* Touch);
extern bool a_touch_tapped(const AInputTouch* Touch);
extern bool a_touch_point(const AInputTouch* Touch, int X, int Y);
extern bool a_touch_box(const AInputTouch* Touch, int X, int Y, int W, int H);
