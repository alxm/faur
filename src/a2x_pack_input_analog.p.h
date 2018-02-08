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

typedef struct AInputAnalog AInputAnalog;

#include "a2x_pack_fix.p.h"

extern AInputAnalog* a_analog_new(const char* Ids);
extern void a_analog_free(AInputAnalog* Analog);

extern bool a_analog_isWorking(const AInputAnalog* Analog);

extern int a_analog_getValueRaw(const AInputAnalog* Analog);
extern AFix a_analog_getValueFix(const AInputAnalog* Analog);
