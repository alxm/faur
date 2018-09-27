/*
    Copyright 2016-2018 Alex Margarit

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

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"

typedef struct {
    AStrHash* buttons; // table of AButtonSource
    AStrHash* axes; // table of AAnalogSource
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
        AAnalogSource* x = a_strhash_get(c->axes, "gamepad.a.leftX");
        AAnalogSource* y = a_strhash_get(c->axes, "gamepad.a.leftY");
        AAnalogSource* lt = a_strhash_get(c->axes, "gamepad.a.leftTrigger");
        AAnalogSource* rt = a_strhash_get(c->axes, "gamepad.a.rightTrigger");
        AButtonSource* u = a_strhash_get(c->buttons, "gamepad.b.up");
        AButtonSource* d = a_strhash_get(c->buttons, "gamepad.b.down");
        AButtonSource* l = a_strhash_get(c->buttons, "gamepad.b.left");
        AButtonSource* r = a_strhash_get(c->buttons, "gamepad.b.right");
        AButtonSource* ul = a_strhash_get(c->buttons, "gamepad.b.upLeft");
        AButtonSource* ur = a_strhash_get(c->buttons, "gamepad.b.upRight");
        AButtonSource* dl = a_strhash_get(c->buttons, "gamepad.b.downLeft");
        AButtonSource* dr = a_strhash_get(c->buttons, "gamepad.b.downRight");
        AButtonSource* lb = a_strhash_get(c->buttons, "gamepad.b.l");
        AButtonSource* rb = a_strhash_get(c->buttons, "gamepad.b.r");

        // GP2X and Wiz dpad diagonals are dedicated buttons, split them into
        // their cardinal directions.
        if(u && d && l && r && ul && ur && dl && dr) {
            a_input_button__sourceForward(ul, u);
            a_input_button__sourceForward(ul, l);

            a_input_button__sourceForward(ur, u);
            a_input_button__sourceForward(ur, r);

            a_input_button__sourceForward(dl, d);
            a_input_button__sourceForward(dl, l);

            a_input_button__sourceForward(dr, d);
            a_input_button__sourceForward(dr, r);
        }

        // Forward the left analog stick to the direction buttons
        if(x && y && u && d && l && r) {
            if(!c->mapped) {
                if(a_settings_getBool("input.switchAxes")) {
                    AAnalogSource* save = x;

                    x = y;
                    y = save;
                }

                if(a_settings_getBool("input.invertAxes")) {
                    AButtonSource* save;

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

        #if A_BUILD_SYSTEM_PANDORA
            if(!c->generic && x && y) {
                // Pandora buttons are keyboard keys, not controller buttons
                u = a_input_button__sourceKeyGet("gamepad.b.up");
                d = a_input_button__sourceKeyGet("gamepad.b.down");
                l = a_input_button__sourceKeyGet("gamepad.b.left");
                r = a_input_button__sourceKeyGet("gamepad.b.right");

                // Forward the left analog nub to the direction buttons
                a_input_analog__forwardToButtons(x, l, r);
                a_input_analog__forwardToButtons(y, u, d);
            }
        #endif
    }

    if(a_input_controllerNumGet() > 0) {
        a_input_controllerSet(0);

        // Set built-in controller as default, if one exists
        A_LIST_ITERATE(g_controllers, AInputController*, c) {
            if(!c->generic) {
                a_input_controllerSet(A_LIST_INDEX());
                break;
            }
        }
    }
}

void a_input_controller__uninit(void)
{
    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        a_strhash_freeEx(c->buttons, (AFree*)a_input_button__sourceFree);
        a_strhash_freeEx(c->axes, (AFree*)a_input_analog__freeSource);

        free(c);
    }

    a_list_free(g_controllers);
}

unsigned a_input_controllerNumGet(void)
{
    return a_list_sizeGet(g_controllers);
}

void a_input_controllerSet(unsigned Index)
{
    if(Index >= a_list_sizeGet(g_controllers)) {
        a_out__error("a_input_controllerSet: Controller %u not present", Index);
        return;
    }

    g_activeController = a_list_getIndex(g_controllers, Index);
}

void a_controller__new(bool Generic, bool IsMapped)
{
    #if A_BUILD_SYSTEM_PANDORA
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

void a_controller__buttonAdd(AButtonSource* Button, const char* Id)
{
    a_strhash_add(g_activeController->buttons, Id, Button);
}

AButtonSource* a_controller__buttonGet(const char* Id)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->buttons, Id);
}

AStrHash* a_controller__buttonCollectionGet(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->buttons;
}

void a_controller__analogAdd(AAnalogSource* Analog, const char* Id)
{
    a_strhash_add(g_activeController->axes, Id, Analog);
}

AAnalogSource* a_controller__analogGet(const char* Id)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->axes, Id);
}

AStrHash* a_controller__analogCollectionGet(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->axes;
}
