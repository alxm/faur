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

typedef struct AInputSourceAnalog AInputSourceAnalog;

#include "a2x_pack_input.v.h"

extern void a_input_analog__init(void);
extern void a_input_analog__uninit(void);

extern AInputSourceAnalog* a_input__newSourceAnalog(const char* Name);
extern void a_input__freeSourceAnalog(AInputSourceAnalog* Analog);

extern void a_input__axisButtonsBinding(AInputSourceAnalog* Axis, AInputSourceButton* Negative, AInputSourceButton* Positive);

extern void a_input__analog_setAxisValue(AInputSourceAnalog* Analog, int Value);

extern void a_input_analog__adjust(void);
