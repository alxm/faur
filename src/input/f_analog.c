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

#include "f_analog.v.h"
#include <faur.v.h>

struct FAnalog {
    const char* name; // friendly name
    FList* platformInputs; // FList<FPlatformAnalog*>
};

static const char* g_analogNames[F_AXIS_NUM] = {
    #if F_CONFIG_SYSTEM_CAANOO
        [F_AXIS_LEFTX] = "Stick",
        [F_AXIS_LEFTY] = "Stick",
    #else
        [F_AXIS_LEFTX] = "Left-Stick",
        [F_AXIS_LEFTY] = "Left-Stick",
    #endif
    [F_AXIS_RIGHTX] = "Right-Stick",
    [F_AXIS_RIGHTY] = "Right-Stick",
    [F_AXIS_LEFTTRIGGER] = "Left-Trigger",
    [F_AXIS_RIGHTTRIGGER] = "Right-Trigger",
};

static const char* g_defaultName = "FAnalog";

FAnalog* f_analog_new(void)
{
    FAnalog* a = f_mem_malloc(sizeof(FAnalog));

    a->name = g_defaultName;
    a->platformInputs = f_list_new();

    return a;
}

void f_analog_free(FAnalog* Analog)
{
    if(Analog == NULL) {
        return;
    }

    f_list_free(Analog->platformInputs);

    f_mem_free(Analog);
}

void f_analog_bind(FAnalog* Analog, const FController* Controller, FAnalogId Id)
{
    const FPlatformAnalog* a = f_platform_api__inputAnalogGet(Controller, Id);

    if(a == NULL) {
        return;
    }

    if(Analog->name == g_defaultName) {
        Analog->name = g_analogNames[Id];
    }

    f_list_addLast(Analog->platformInputs, (FPlatformAnalog*)a);
}

bool f_analog_isWorking(const FAnalog* Analog)
{
    return !f_list_sizeIsEmpty(Analog->platformInputs);
}

const char* f_analog_nameGet(const FAnalog* Analog)
{
    return Analog->name;
}

FFix f_analog_valueGet(const FAnalog* Analog)
{
    int value = 0;

    #define F__ANALOG_BITS 15
    #define F__ANALOG_MAX_DISTANCE (1 << F__ANALOG_BITS)
    #define F__ANALOG_ERROR_MARGIN (F__ANALOG_MAX_DISTANCE / 20)

    F_LIST_ITERATE(Analog->platformInputs, FPlatformAnalog*, a) {
        value = f_platform_api__inputAnalogValueGet(a);

        if(f_math_abs(value) > F__ANALOG_ERROR_MARGIN) {
            break;
        }
    }

    #if F_FIX_BIT_PRECISION < F__ANALOG_BITS
        return value >> (F__ANALOG_BITS - F_FIX_BIT_PRECISION);
    #else
        return value << (F_FIX_BIT_PRECISION - F__ANALOG_BITS);
    #endif
}
