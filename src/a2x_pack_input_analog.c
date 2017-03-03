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
    AInputHeader header;
};

struct AInputSourceAnalog {
    AInputSourceHeader header;
    AList* buttonBindings; // List of AInputSourceAxisToButtons
    int axisValue;
};

typedef struct AInputSourceAxisToButtons {
    AInputSourceButton* negative;
    AInputSourceButton* positive;
} AInputSourceAxisToButtons;

static AList* g_analogs;

void a_input_analog__init(void)
{
    g_analogs = a_list_new();
}

void a_input_analog__uninit(void)
{
    A_LIST_ITERATE(g_analogs, AInputAnalog*, a) {
        a_input__freeHeader(&a->header);
        free(a);
    }

    a_list_free(g_analogs);
}

AInputSourceAnalog* a_input__newSourceAnalog(const char* Name)
{
    AInputSourceAnalog* a = a_mem_malloc(sizeof(AInputSourceAnalog));

    a_input__initSourceHeader(&a->header, Name);
    a->buttonBindings = a_list_new();
    a->axisValue = 0;

    return a;
}

void a_input__freeSourceAnalog(AInputSourceAnalog* Analog)
{
    A_LIST_ITERATE(Analog->buttonBindings, AInputSourceAxisToButtons*, b) {
        free(b);
    }

    a_list_free(Analog->buttonBindings);
}

void a_input__axisButtonsBinding(AInputSourceAnalog* Axis, AInputSourceButton* Negative, AInputSourceButton* Positive)
{
    AInputSourceAxisToButtons* b = a_mem_malloc(sizeof(AInputSourceAxisToButtons));

    b->negative = Negative;
    b->positive = Positive;

    a_list_addLast(Axis->buttonBindings, b);
}

AInputAnalog* a_analog_new(const char* Names)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    a_input__initHeader(&a->header);

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        a_input__findSourceInput(name,
                                 NULL,
                                 a_controller__getAnalogsCollection(),
                                 &a->header);
    }

    a_strtok_free(tok);

    if(a_list_empty(a->header.sourceInputs)) {
        a_out__error("No analog axes found for '%s'", Names);
    }

    a_list_addLast(g_analogs, a);

    return a;
}

bool a_analog_working(const AInputAnalog* Analog)
{
    return !a_list_empty(Analog->header.sourceInputs);
}

int a_analog_axisRaw(const AInputAnalog* Analog)
{
    #define A_ANALOG_MAX_DISTANCE (1 << 15)
    #define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->header.sourceInputs, AInputSourceAnalog*, a) {
        if(a_math_abs(a->axisValue) > A_ANALOG_ERROR_MARGIN) {
            return a->axisValue;
        }
    }

    return 0;
}

AFix a_analog_axisFix(const AInputAnalog* Analog)
{
    return a_analog_axisRaw(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

void a_input__analog_setAxisValue(AInputSourceAnalog* Analog, int Value)
{
    Analog->axisValue = Value;

    a_input__setFreshEvent(&Analog->header);

    #define PRESS_THRESHOLD ((1 << 15) / 2)

    A_LIST_ITERATE(Analog->buttonBindings, AInputSourceAxisToButtons*, b) {
        a_input__button_setState(b->negative, Value < -PRESS_THRESHOLD);
        a_input__button_setState(b->positive, Value > PRESS_THRESHOLD);
    }
}

void a_input_analog__adjust(void)
{
    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        // Pressed at least half-way
        #define ANALOG_TRESH ((1 << 15) / 2)

        AInputSourceAnalog* stickx = a_controller__getAnalog("caanoo.stickX");
        AInputSourceAnalog* sticky = a_controller__getAnalog("caanoo.stickY");

        if(a_input__hasFreshEvent(&stickx->header)) {
            AInputSourceButton* left = a_controller__getButton("caanoo.left");
            a_input__button_setState(left, stickx->axisValue < -ANALOG_TRESH);

            AInputSourceButton* right = a_controller__getButton("caanoo.right");
            a_input__button_setState(right, stickx->axisValue > ANALOG_TRESH);
        }

        if(a_input__hasFreshEvent(&sticky->header)) {
            AInputSourceButton* up = a_controller__getButton("caanoo.up");
            a_input__button_setState(up, sticky->axisValue < -ANALOG_TRESH);

            AInputSourceButton* down = a_controller__getButton("caanoo.down");
            a_input__button_setState(down, sticky->axisValue > ANALOG_TRESH);
        }
    #endif
}
