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

#include "a2x_pack_input_analog.p.h"

typedef struct AInputAnalogSource AInputAnalogSource;

#include "a2x_pack_input_button.v.h"

extern AInputAnalogSource* a_input_analog__newSource(const char* Id);
extern void a_input_analog__freeSource(AInputAnalogSource* Analog);

extern void a_input_analog__forwardToButtons(AInputAnalogSource* Axis, AInputButtonSource* Negative, AInputButtonSource* Positive);
extern void a_input_analog__setAxisValue(AInputAnalogSource* Analog, int Value);
