/*
    Copyright 2016, 2017 Alex Margarit

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

#include "a2x_pack_input_controller.v.h"

typedef struct AInputController {
    AStrHash* buttons; // table of AInputButtonSource
    AStrHash* axes; // table of AInputAnalogSource
    bool generic;
    bool mapped;
} AInputController;

static AList* g_controllers;
AInputController* g_activeController;

void a_input_controller__init(void)
{
    g_controllers = a_list_new();
    g_activeController = NULL;
}

void a_input_controller__init2(void)
{
    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        if(!c->generic) {
            // GP2X and Wiz dpad diagonals are dedicated buttons instead of a
            // combination of two separate buttons. This splits them into
            // individual directions.
            #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
                #if A_PLATFORM_GP2X
                    #define PREFIX "gp2x"
                #elif A_PLATFORM_WIZ
                    #define PREFIX "wiz"
                #endif

                AInputButtonSource* ul = a_strhash_get(c->buttons, PREFIX ".upLeft");
                AInputButtonSource* ur = a_strhash_get(c->buttons, PREFIX ".upRight");
                AInputButtonSource* dl = a_strhash_get(c->buttons, PREFIX ".downLeft");
                AInputButtonSource* dr = a_strhash_get(c->buttons, PREFIX ".downRight");
                AInputButtonSource* u = a_strhash_get(c->buttons, PREFIX ".up");
                AInputButtonSource* d = a_strhash_get(c->buttons, PREFIX ".down");
                AInputButtonSource* l = a_strhash_get(c->buttons, PREFIX ".left");
                AInputButtonSource* r = a_strhash_get(c->buttons, PREFIX ".right");

                a_input_button__forwardTo(ul, u);
                a_input_button__forwardTo(ul, l);

                a_input_button__forwardTo(ur, u);
                a_input_button__forwardTo(ur, r);

                a_input_button__forwardTo(dl, d);
                a_input_button__forwardTo(dl, l);

                a_input_button__forwardTo(dr, d);
                a_input_button__forwardTo(dr, r);
            #endif

            // Caanoo has an analog stick instead of direction buttons,
            // this lets us use it as a dpad like on the other platforms.
            #if A_PLATFORM_CAANOO
                AInputAnalogSource* x = a_strhash_get(c->axes, "caanoo.stickX");
                AInputAnalogSource* y = a_strhash_get(c->axes, "caanoo.stickY");

                AInputButtonSource* u = a_strhash_get(c->buttons, "caanoo.up");
                AInputButtonSource* d = a_strhash_get(c->buttons, "caanoo.down");
                AInputButtonSource* l = a_strhash_get(c->buttons, "caanoo.left");
                AInputButtonSource* r = a_strhash_get(c->buttons, "caanoo.right");

                a_input_analog__axisButtonsBinding(x, l, r);
                a_input_analog__axisButtonsBinding(y, u, d);
            #endif

            continue;
        }

        if(a_strhash_size(c->axes) < 2) {
            continue;
        }

        AInputAnalogSource *x, *y;
        AInputButtonSource* u = a_strhash_get(c->buttons, "gamepad.b.up");
        AInputButtonSource* d = a_strhash_get(c->buttons, "gamepad.b.down");
        AInputButtonSource* l = a_strhash_get(c->buttons, "gamepad.b.left");
        AInputButtonSource* r = a_strhash_get(c->buttons, "gamepad.b.right");

        if(c->mapped) {
            x = a_strhash_get(c->axes, "gamepad.a.leftX");
            y = a_strhash_get(c->axes, "gamepad.a.leftY");

            if(!x || !y) {
                continue;
            }
        } else {
            // Assume that the first two axes are for X and Y movement.
            x = a_strhash_get(c->axes, "gamepad.a.0");
            y = a_strhash_get(c->axes, "gamepad.a.1");

            if(a_settings_getBool("input.switchAxes")) {
                AInputAnalogSource* save = x;

                x = y;
                y = save;
            }

            if(a_settings_getBool("input.invertAxes")) {
                AInputButtonSource* save;

                save = u;
                u = d;
                d = save;

                save = l;
                l = r;
                r = save;
            }
        }

        a_input_analog__axisButtonsBinding(x, l, r);
        a_input_analog__axisButtonsBinding(y, u, d);
    }

    if(a_input_numControllers() > 0) {
        a_input_setController(0);
    }
}

void a_input_controller__uninit(void)
{
    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, AInputButtonSource*, b) {
            a_input_button__freeSource(b);
        }

        A_STRHASH_ITERATE(c->axes, AInputAnalogSource*, a) {
            a_input_analog__freeSource(a);
        }

        a_strhash_free(c->buttons);
        a_strhash_free(c->axes);
        free(c);
    }

    a_list_free(g_controllers);
}

unsigned a_input_numControllers(void)
{
    return a_list_size(g_controllers);
}

void a_input_setController(unsigned Index)
{
    if(Index >= a_list_size(g_controllers)) {
        a_out__error("Controller %u not present", Index);
        return;
    }

    g_activeController = a_list_get(g_controllers, Index);
}

void a_controller__new(bool Generic, bool IsMapped)
{
    AInputController* c = a_mem_malloc(sizeof(AInputController));

    c->buttons = a_strhash_new();
    c->axes = a_strhash_new();
    c->generic = Generic;
    c->mapped = IsMapped;

    a_list_addLast(g_controllers, c);
    g_activeController = c;
}

void a_controller__addButton(AInputButtonSource* Button, const char* Id)
{
    a_strhash_add(g_activeController->buttons, Id, Button);
}

AInputButtonSource* a_controller__getButton(const char* Id)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->buttons, Id);
}

AStrHash* a_controller__getButtonCollection(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->buttons;
}

void a_controller__addAnalog(AInputAnalogSource* Analog, const char* Id)
{
    a_strhash_add(g_activeController->axes, Id, Analog);
}

AInputAnalogSource* a_controller__getAnalog(const char* Id)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->axes, Id);
}

AStrHash* a_controller__getAnalogCollection(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->axes;
}
