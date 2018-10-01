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
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_str.v.h"

struct AAnalog {
    AInputUserHeader header;
};

AAnalog* a_analog_new(const char* Ids)
{
    AAnalog* a = a_mem_malloc(sizeof(AAnalog));

    a_input__userHeaderInit(&a->header);

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        APlatformAnalog* pa = a_platform__analogGet(id);

        if(pa) {
            a_list_addLast(a->header.platformInputs, pa);
        }
    }

    a_list_freeEx(tok, free);

    if(a_list_isEmpty(a->header.platformInputs)) {
        a_out__error("a_analog_new: No analog axes found for '%s'", Ids);
    }

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

bool a_analog_isWorking(const AAnalog* Analog)
{
    return !a_list_isEmpty(Analog->header.platformInputs);
}

int a_analog_valueGetRaw(const AAnalog* Analog)
{
    #define A__ANALOG_MAX_DISTANCE (1 << 15)
    #define A__ANALOG_ERROR_MARGIN (A__ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->header.platformInputs, APlatformAnalog*, a) {
        int value = a_platform__analogValueGet(a);

        if(a_math_abs(value) > A__ANALOG_ERROR_MARGIN) {
            return value;
        }
    }

    return 0;
}

AFix a_analog_valueGetFix(const AAnalog* Analog)
{
    #define A__ANALOG_BITS 15

    #if A_FIX_BIT_PRECISION < A__ANALOG_BITS
        return a_analog_valueGetRaw(Analog)
                >> (A__ANALOG_BITS - A_FIX_BIT_PRECISION);
    #else
        return a_analog_valueGetRaw(Analog)
                << (A_FIX_BIT_PRECISION - A__ANALOG_BITS);
    #endif
}
