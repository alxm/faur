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
        AInputAnalogSource* x = a_strhash_get(c->axes, "gamepad.a.leftX");
        AInputAnalogSource* y = a_strhash_get(c->axes, "gamepad.a.leftY");
        AInputAnalogSource* lt = a_strhash_get(c->axes, "gamepad.a.leftTrigger");
        AInputAnalogSource* rt = a_strhash_get(c->axes, "gamepad.a.rightTrigger");
        AInputButtonSource* u = a_strhash_get(c->buttons, "gamepad.b.up");
        AInputButtonSource* d = a_strhash_get(c->buttons, "gamepad.b.down");
        AInputButtonSource* l = a_strhash_get(c->buttons, "gamepad.b.left");
        AInputButtonSource* r = a_strhash_get(c->buttons, "gamepad.b.right");
        AInputButtonSource* ul = a_strhash_get(c->buttons, "gamepad.b.upLeft");
        AInputButtonSource* ur = a_strhash_get(c->buttons, "gamepad.b.upRight");
        AInputButtonSource* dl = a_strhash_get(c->buttons, "gamepad.b.downLeft");
        AInputButtonSource* dr = a_strhash_get(c->buttons, "gamepad.b.downRight");
        AInputButtonSource* lb = a_strhash_get(c->buttons, "gamepad.b.l");
        AInputButtonSource* rb = a_strhash_get(c->buttons, "gamepad.b.r");

        // GP2X and Wiz dpad diagonals are dedicated buttons, split them into
        // their cardinal directions.
        if(u && d && l && r && ul && ur && dl && dr) {
            a_input_button__forwardToButton(ul, u);
            a_input_button__forwardToButton(ul, l);

            a_input_button__forwardToButton(ur, u);
            a_input_button__forwardToButton(ur, r);

            a_input_button__forwardToButton(dl, d);
            a_input_button__forwardToButton(dl, l);

            a_input_button__forwardToButton(dr, d);
            a_input_button__forwardToButton(dr, r);
        }

        // Forward the left analog stick to the direction buttons
        if(x && y && u && d && l && r) {
            if(!c->mapped) {
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

            a_input_analog__forwardToButtons(x, l, r);
            a_input_analog__forwardToButtons(y, u, d);
        }

        // Forward analog shoulder triggers to the shoulder buttons
        if(lt && rt && lb && rb) {
            a_input_analog__forwardToButtons(lt, NULL, lb);
            a_input_analog__forwardToButtons(rt, NULL, rb);
        }

        #if A_PLATFORM_PANDORA
            if(!c->generic && x && y) {
                // Pandora buttons are keyboard keys, not controller buttons
                u = a_input_button__getKey("gamepad.b.up");
                d = a_input_button__getKey("gamepad.b.down");
                l = a_input_button__getKey("gamepad.b.left");
                r = a_input_button__getKey("gamepad.b.right");

                // Forward the left analog nub to the direction buttons
                a_input_analog__forwardToButtons(x, l, r);
                a_input_analog__forwardToButtons(y, u, d);
            }
        #endif
    }

    if(a_input_getNumControllers() > 0) {
        a_input_setController(0);

        // Set built-in controller as default, if one exists
        A_LIST_ITERATE(g_controllers, AInputController*, c) {
            if(!c->generic) {
                a_input_setController(A_LIST_INDEX());
                break;
            }
        }
    }
}

void a_input_controller__uninit(void)
{
    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        a_strhash_freeEx(c->buttons, (AFree*)a_input_button__freeSource);
        a_strhash_freeEx(c->axes, (AFree*)a_input_analog__freeSource);
        free(c);
    }

    a_list_free(g_controllers);
}

unsigned a_input_getNumControllers(void)
{
    return a_list_getSize(g_controllers);
}

void a_input_setController(unsigned Index)
{
    if(Index >= a_list_getSize(g_controllers)) {
        a_out__error("Controller %u not present", Index);
        return;
    }

    g_activeController = a_list_getIndex(g_controllers, Index);
}

void a_controller__new(bool Generic, bool IsMapped)
{
    #if A_PLATFORM_PANDORA
        // Assign both analog nubs to the same logical controller
        if(!Generic) {
            A_LIST_ITERATE(g_controllers, AInputController*, c) {
                if(!c->generic) {
                    g_activeController = c;
                    return;
                }
            }
        }
    #endif

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
