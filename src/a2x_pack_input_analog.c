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
    AList* buttonBindings; // List of AInputSourceAxisToButtons
    int axisValue;
};

typedef struct AInputSourceAxisToButtons {
    AInputButtonSource* negative;
    AInputButtonSource* positive;
} AInputSourceAxisToButtons;

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
    a->buttonBindings = a_list_new();
    a->axisValue = 0;

    return a;
}

void a_input_analog__freeSource(AInputAnalogSource* Analog)
{
    A_LIST_ITERATE(Analog->buttonBindings, AInputSourceAxisToButtons*, b) {
        free(b);
    }

    a_list_free(Analog->buttonBindings);
    a_input__freeSourceHeader(&Analog->header);
}

void a_input_analog__axisButtonsBinding(AInputAnalogSource* Axis, AInputButtonSource* Negative, AInputButtonSource* Positive)
{
    AInputSourceAxisToButtons* b = a_mem_malloc(sizeof(AInputSourceAxisToButtons));

    b->negative = Negative;
    b->positive = Positive;

    a_list_addLast(Axis->buttonBindings, b);
}

AInputAnalog* a_analog_new(const char* Ids)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    a_input__initUserHeader(&a->header);

    AStrTok* tok = a_strtok_new(Ids, ", ");

    A_STRTOK_ITERATE(tok, id) {
        a_input__findSourceInput(NULL,
                                 a_controller__getAnalogCollection(),
                                 id,
                                 &a->header);
    }

    a_strtok_free(tok);

    if(a_list_empty(a->header.sourceInputs)) {
        a_out__error("No analog axes found for '%s'", Ids);
    }

    a_list_addLast(g_analogs, a);

    return a;
}

bool a_analog_working(const AInputAnalog* Analog)
{
    return !a_list_empty(Analog->header.sourceInputs);
}

int a_analog_valueRaw(const AInputAnalog* Analog)
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

AFix a_analog_valueFix(const AInputAnalog* Analog)
{
    return a_analog_valueRaw(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

void a_input_analog__setAxisValue(AInputAnalogSource* Analog, int Value)
{
    Analog->axisValue = Value;

    a_input__setFreshEvent(&Analog->header);

    #define PRESS_THRESHOLD ((1 << 15) / 2)

    A_LIST_ITERATE(Analog->buttonBindings, AInputSourceAxisToButtons*, b) {
        a_input_button__setState(b->negative, Value < -PRESS_THRESHOLD);
        a_input_button__setState(b->positive, Value > PRESS_THRESHOLD);
    }
}