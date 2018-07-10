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

#include "a2x_system_includes.h"
#include "a2x_pack_input_analog.v.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_input_controller.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"

struct AInputAnalog {
    AInputUserHeader header;
};

struct AInputAnalogSource {
    AInputSourceHeader header;
    AList* forwardButtons; // List of AInputSourceAxisButtons
    int axisValue;
};

typedef struct {
    AInputButtonSource* negative;
    AInputButtonSource* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} AInputSourceAxisButtons;

AInputAnalogSource* a_input_analog__newSource(const char* Id)
{
    AInputAnalogSource* a = a_mem_malloc(sizeof(AInputAnalogSource));

    a_input__sourceHeaderInit(&a->header, Id);
    a->forwardButtons = a_list_new();
    a->axisValue = 0;

    return a;
}

void a_input_analog__freeSource(AInputAnalogSource* Analog)
{
    a_list_freeEx(Analog->forwardButtons, free);
    a_input__sourceHeaderFree(&Analog->header);
}

void a_input_analog__forwardToButtons(AInputAnalogSource* Axis, AInputButtonSource* Negative, AInputButtonSource* Positive)
{
    AInputSourceAxisButtons* b = a_mem_malloc(sizeof(AInputSourceAxisButtons));

    b->negative = Negative;
    b->positive = Positive;
    b->lastPressedNegative = false;
    b->lastPressedPositive = false;

    a_list_addLast(Axis->forwardButtons, b);
}

AInputAnalog* a_analog_new(const char* Ids)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    a_input__userHeaderInit(&a->header);

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        a_input__userHeaderFindSource(
            &a->header, id, NULL, a_controller__analogCollectionGet());
    }

    a_list_freeEx(tok, free);

    if(a_list_isEmpty(a->header.sourceInputs)) {
        a_out__error("No analog axes found for '%s'", Ids);
    }

    return a;
}

void a_analog_free(AInputAnalog* Analog)
{
    a_input__userHeaderFree(&Analog->header);
    free(Analog);
}

bool a_analog_isWorking(const AInputAnalog* Analog)
{
    return !a_list_isEmpty(Analog->header.sourceInputs);
}

int a_analog_valueGetRaw(const AInputAnalog* Analog)
{
    #define A_ANALOG_MAX_DISTANCE (1 << 15)
    #define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->header.sourceInputs, AInputAnalogSource*, a) {
        if(a_math_abs(a->axisValue) > A_ANALOG_ERROR_MARGIN) {
            return a->axisValue;
        }
    }

    return 0;
}

AFix a_analog_valueGetFix(const AInputAnalog* Analog)
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

void a_input_analog__axisValueSet(AInputAnalogSource* Analog, int Value)
{
    Analog->axisValue = Value;
    a_input__freshEventSet(&Analog->header);

    #define PRESS_THRESHOLD ((1 << 15) / 3)
    bool pressedNegative = Value < -PRESS_THRESHOLD;
    bool pressedPositive = Value > PRESS_THRESHOLD;

    A_LIST_ITERATE(Analog->forwardButtons, AInputSourceAxisButtons*, b) {
        if(b->negative && pressedNegative != b->lastPressedNegative) {
            a_input_button__stateSet(b->negative, pressedNegative);
            b->lastPressedNegative = pressedNegative;
        }

        if(b->positive && pressedPositive != b->lastPressedPositive) {
            a_input_button__stateSet(b->positive, pressedPositive);
            b->lastPressedPositive = pressedPositive;
        }
    }
}
