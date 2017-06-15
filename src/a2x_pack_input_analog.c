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

#include "a2x_pack_input_analog.v.h"

struct AInputAnalog {
    AInputUserHeader header;
};

struct AInputAnalogSource {
    AInputSourceHeader header;
    AList* forwardButtons; // List of AInputSourceAxisButtons
    int axisValue;
};

typedef struct AInputSourceAxisButtons {
    AInputButtonSource* negative;
    AInputButtonSource* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} AInputSourceAxisButtons;

static AList* g_analogs;

void a_input_analog__init(void)
{
    g_analogs = a_list_new();
}

void a_input_analog__uninit(void)
{
    A_LIST_ITERATE(g_analogs, AInputAnalog*, a) {
        a_input__freeUserHeader(&a->header);
        free(a);
    }

    a_list_free(g_analogs);
}

AInputAnalogSource* a_input_analog__newSource(const char* Id)
{
    AInputAnalogSource* a = a_mem_malloc(sizeof(AInputAnalogSource));

    a_input__initSourceHeader(&a->header, Id);
    a->forwardButtons = a_list_new();
    a->axisValue = 0;

    return a;
}

void a_input_analog__freeSource(AInputAnalogSource* Analog)
{
    A_LIST_ITERATE(Analog->forwardButtons, AInputSourceAxisButtons*, b) {
        free(b);
    }

    a_list_free(Analog->forwardButtons);
    a_input__freeSourceHeader(&Analog->header);
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

    a_input__initUserHeader(&a->header);

    AList* tok = a_str_split(Ids, ", ");

    A_LIST_ITERATE(tok, char*, id) {
        a_input__findSourceInput(NULL,
                                 a_controller__getAnalogCollection(),
                                 id,
                                 &a->header);
    }

    a_list_free(tok);

    if(a_list_isEmpty(a->header.sourceInputs)) {
        a_out__error("No analog axes found for '%s'", Ids);
    }

    a_list_addLast(g_analogs, a);

    return a;
}

bool a_analog_isWorking(const AInputAnalog* Analog)
{
    return !a_list_isEmpty(Analog->header.sourceInputs);
}

int a_analog_getValueRaw(const AInputAnalog* Analog)
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

AFix a_analog_getValueFix(const AInputAnalog* Analog)
{
    return a_analog_getValueRaw(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

void a_input_analog__setAxisValue(AInputAnalogSource* Analog, int Value)
{
    Analog->axisValue = Value;
    a_input__setFreshEvent(&Analog->header);

    #define PRESS_THRESHOLD ((1 << 15) / 3)
    bool pressedNegative = Value < -PRESS_THRESHOLD;
    bool pressedPositive = Value > PRESS_THRESHOLD;

    A_LIST_ITERATE(Analog->forwardButtons, AInputSourceAxisButtons*, b) {
        if(b->negative && pressedNegative != b->lastPressedNegative) {
            a_input_button__setState(b->negative, pressedNegative);
            b->lastPressedNegative = pressedNegative;
        }

        if(b->positive && pressedPositive != b->lastPressedPositive) {
            a_input_button__setState(b->positive, pressedPositive);
            b->lastPressedPositive = pressedPositive;
        }
    }
}
