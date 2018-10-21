/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_input_analog.v.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"

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

    free(Analog);
}

void a_analog_bind(AAnalog* Analog, AAxisId Id)
{
    APlatformAnalog* pa = a_platform__analogGet(Id);

    if(pa == NULL) {
        return;
    }

    if(Analog->header.name == a__inputNameDefault) {
        Analog->header.name = a_platform__analogNameGet(pa);
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

    A_LIST_ITERATE(Analog->header.platformInputs, APlatformAnalog*, a) {
        value = a_platform__analogValueGet(a);

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
