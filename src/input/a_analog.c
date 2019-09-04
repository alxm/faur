/*
    Copyright 2010, 2016-2018 Alex Margarit <alex@alxm.org>
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
    AInputUserHeader header;
};

AAnalog* a_analog_new(void)
{
    AAnalog* a = a_mem_malloc(sizeof(AAnalog));

    a_input__userHeaderInit(&a->header);

    return a;
}

void a_analog_free(AAnalog* Analog)
{
    if(Analog == NULL) {
        return;
    }

    a_input__userHeaderFree(&Analog->header);

    a_mem_free(Analog);
}

void a_analog_bind(AAnalog* Analog, AAxisId Id)
{
    APlatformInputAnalog* pa = a_platform_api__inputAnalogGet(Id);

    if(pa == NULL) {
        return;
    }

    if(Analog->header.name == a__inputNameDefault) {
        Analog->header.name = a_platform_api__inputAnalogNameGet(pa);
    }

    a_list_addLast(Analog->header.platformInputs, pa);
}

bool a_analog_isWorking(const AAnalog* Analog)
{
    return !a_list_isEmpty(Analog->header.platformInputs);
}

const char* a_analog_nameGet(const AAnalog* Analog)
{
    return Analog->header.name;
}

AFix a_analog_valueGet(const AAnalog* Analog)
{
    int value = 0;

    #define A__ANALOG_BITS 15
    #define A__ANALOG_MAX_DISTANCE (1 << A__ANALOG_BITS)
    #define A__ANALOG_ERROR_MARGIN (A__ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->header.platformInputs, APlatformInputAnalog*, a) {
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
