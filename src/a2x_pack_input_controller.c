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

typedef struct AInputSourceController {
    AStrHash* buttons;
    AStrHash* axes;
} AInputSourceController;

static AList* g_controllers;
AInputSourceController* g_activeController;

void a_input_controller__init(void)
{
    g_controllers = a_list_new();
    g_activeController = NULL;
}

void a_input_controller__init2(void)
{
    if(a_input_numControllers() == 0) {
        return;
    }

    A_LIST_ITERATE(g_controllers, AInputSourceController*, c) {
        if(a_strhash_size(c->axes) < 2) {
            continue;
        }

        AInputSourceAnalog* x = a_strhash_get(c->axes, "controller.axis0");
        AInputSourceAnalog* y = a_strhash_get(c->axes, "controller.axis1");

        AInputSourceButton* u = a_strhash_get(c->buttons, "controller.up");
        AInputSourceButton* d = a_strhash_get(c->buttons, "controller.down");
        AInputSourceButton* l = a_strhash_get(c->buttons, "controller.left");
        AInputSourceButton* r = a_strhash_get(c->buttons, "controller.right");

        if(x && y && u && d && l && r) {
            a_input__axisButtonsBinding(x, l, r);
            a_input__axisButtonsBinding(y, u, d);
        }
    }

    a_input_setController(0);
}

void a_input_controller__uninit(void)
{
    A_LIST_ITERATE(g_controllers, AInputSourceController*, c) {
        A_STRHASH_ITERATE(c->buttons, AInputSourceHeader*, b) {
            a_input__freeSourceHeader(b);
        }

        A_STRHASH_ITERATE(c->axes, AInputSourceAnalog*, a) {
            a_input__freeSourceAnalog(a);
            a_input__freeSourceHeader((AInputSourceHeader*)a);
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

void a_controller__new(void)
{
    AInputSourceController* c = a_mem_malloc(sizeof(AInputSourceController));

    c->buttons = a_strhash_new();
    c->axes = a_strhash_new();

    a_list_addLast(g_controllers, c);
    g_activeController = c;
}

void a_controller__addButton(AInputSourceButton* Button, const char* Name)
{
    a_strhash_add(g_activeController->buttons, Name, Button);
}

AInputSourceButton* a_controller__getButton(const char* Name)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->buttons, Name);
}

AStrHash* a_controller__getButtonsCollection(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->buttons;
}

void a_controller__addAnalog(AInputSourceAnalog* Analog, const char* Name)
{
    a_strhash_add(g_activeController->axes, Name, Analog);
}

AInputSourceAnalog* a_controller__getAnalog(const char* Name)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return a_strhash_get(g_activeController->axes, Name);
}

AStrHash* a_controller__getAnalogsCollection(void)
{
    if(g_activeController == NULL) {
        return NULL;
    }

    return g_activeController->axes;
}
