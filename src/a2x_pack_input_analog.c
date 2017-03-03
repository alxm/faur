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
    int axisValue;
};

static AList* g_analogs;
static AStrHash* g_sourceAnalogs;

void a_input_analog__init(void)
{
    g_analogs = a_list_new();
    g_sourceAnalogs = a_strhash_new();
}

void a_input_analog__uninit(void)
{
    A_LIST_ITERATE(g_analogs, AInputAnalog*, a) {
        a_input__freeHeader(&a->header);
        free(a);
    }

    A_STRHASH_ITERATE(g_sourceAnalogs, AInputSourceAnalog*, a) {
        a_input__freeSourceHeader(&a->header);
    }

    a_list_free(g_analogs);
    a_strhash_free(g_sourceAnalogs);
}

AInputSourceAnalog* a_input__newSourceAnalog(const char* Name)
{
    AInputSourceAnalog* a = a_mem_malloc(sizeof(AInputSourceAnalog));

    a_input__initSourceHeader(&a->header, Name);

    a->axisValue = 0;

    if(a_input__activeController == NULL) {
        a_strhash_add(g_sourceAnalogs, Name, a);
    } else {
        a_strhash_add(a_input__activeController->axes, Name, a);
    }

    return a;
}

AInputAnalog* a_analog_new(const char* Names)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    a_input__initHeader(&a->header);

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        if(!a_input__findSourceInput(name, g_sourceAnalogs, &a->header)) {
            if(a_input__activeController != NULL) {
                a_input__findSourceInput(name, a_input__activeController->axes, &a->header);
            }
        }
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
}

void a_input_analog__adjust(void)
{
    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        // Pressed at least half-way
        #define ANALOG_TRESH ((1 << 15) / 2)

        AInputSourceAnalog* stickx = a_strhash_get(g_sourceAnalogs, "caanoo.stickX");
        AInputSourceAnalog* sticky = a_strhash_get(g_sourceAnalogs, "caanoo.stickY");

        if(a_input__hasFreshEvent(&stickx->header)) {
            AInputSourceButton* left = a_input__getSourceButton("caanoo.left");
            a_input__button_setState(left, stickx->axisValue < -ANALOG_TRESH);

            AInputSourceButton* right = a_input__getSourceButton("caanoo.right");
            a_input__button_setState(right, stickx->axisValue > ANALOG_TRESH);
        }

        if(a_input__hasFreshEvent(&sticky->header)) {
            AInputSourceButton* up = a_input__getSourceButton("caanoo.up");
            a_input__button_setState(up, sticky->axisValue < -ANALOG_TRESH);

            AInputSourceButton* down = a_input__getSourceButton("caanoo.down");
            a_input__button_setState(down, sticky->axisValue > ANALOG_TRESH);
        }
    #endif
}
