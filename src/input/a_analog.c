/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "a_analog.v.h"
#include <a2x.v.h>

struct AAnalog {
    const char* name; // friendly name
    AList* platformInputs; // list of APlatformAnalog
};

static const char* g_analogNames[A_AXIS_NUM] = {
    #if A_CONFIG_SYSTEM_CAANOO
        [A_AXIS_LEFTX] = "Stick",
        [A_AXIS_LEFTY] = "Stick",
    #else
        [A_AXIS_LEFTX] = "Left-Stick",
        [A_AXIS_LEFTY] = "Left-Stick",
    #endif
    [A_AXIS_RIGHTX] = "Right-Stick",
    [A_AXIS_RIGHTY] = "Right-Stick",
    [A_AXIS_LEFTTRIGGER] = "Left-Trigger",
    [A_AXIS_RIGHTTRIGGER] = "Right-Trigger",
};

static const char* g_defaultName = "AAnalog";

AFix a_analog_read(AAnalogId Axis)
{
    APlatformAnalog* a = a_platform_api__inputAnalogGet(NULL, Axis);

    return a ? a_platform_api__inputAnalogValueGet(a) : 0;
}

AAnalog* a_analog_new(void)
{
    AAnalog* a = a_mem_malloc(sizeof(AAnalog));

    a->name = g_defaultName;
    a->platformInputs = a_list_new();

    return a;
}

void a_analog_free(AAnalog* Analog)
{
    if(Analog == NULL) {
        return;
    }

    a_list_free(Analog->platformInputs);

    a_mem_free(Analog);
}

void a_analog_bind(AAnalog* Analog, AController* Controller, AAnalogId Id)
{
    APlatformAnalog* a = a_platform_api__inputAnalogGet(Controller, Id);

    if(a == NULL) {
        return;
    }

    if(Analog->name == g_defaultName) {
        Analog->name = g_analogNames[Id];
    }

    a_list_addLast(Analog->platformInputs, a);
}

bool a_analog_isWorking(const AAnalog* Analog)
{
    return !a_list_isEmpty(Analog->platformInputs);
}

const char* a_analog_nameGet(const AAnalog* Analog)
{
    return Analog->name;
}

AFix a_analog_valueGet(const AAnalog* Analog)
{
    int value = 0;

    #define A__ANALOG_BITS 15
    #define A__ANALOG_MAX_DISTANCE (1 << A__ANALOG_BITS)
    #define A__ANALOG_ERROR_MARGIN (A__ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->platformInputs, APlatformAnalog*, a) {
        value = a_platform_api__inputAnalogValueGet(a);

        if(a_math_abs(value) > A__ANALOG_ERROR_MARGIN) {
            break;
        }
    }

    #if A_FIX_BIT_PRECISION < A__ANALOG_BITS
        return value >> (A__ANALOG_BITS - A_FIX_BIT_PRECISION);
    #else
        return value << (A_FIX_BIT_PRECISION - A__ANALOG_BITS);
    #endif
}
