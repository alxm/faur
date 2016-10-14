/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_pack_input.v.h"

#define A_ANALOG_MAX_DISTANCE (1 << 15)
#define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

struct AInput {
    const char* name;
    AList* buttons; // List of AInputButton
    AList* analogs; // List of AInputAnalog
    AList* touchScreens; // List of AInputTouch
    AList* combos; // List of AInputButtonCombo
};

typedef struct AInputHeader {
    char* name;
    char* shortName;
} AInputHeader;

struct AInputButton {
    AInputHeader header;
    bool pressed;
    bool waitingForUnpress;
    bool analogPushedPast; // used to simulate key events for analog
    bool freshEvent; // used to simulate separate directions from diagonals
};

struct AInputAnalog {
    AInputHeader header;
    int xaxis;
    int yaxis;
};

struct AInputTouch {
    AInputHeader header;
    int x;
    int y;
    bool tap;
    AList* motion; // APoints captured by motion event
};

typedef struct AInputButtonCombo {
    AInputHeader header;
    AList* buttons; // List of AInputButton
} AInputButtonCombo;

typedef struct AInputController {
    AStrHash* buttons;
} AInputController;

typedef struct APoint {
    int x;
    int y;
} APoint;

static AStrHash* g_buttons;
static AStrHash* g_analogs;
static AStrHash* g_touchScreens;
static AStrHash* g_umbrellas;
static AList* g_controllers;
static AInputController* g_activeController;

// all inputs returned by a_input_new()
static AList* g_userInputs;
static AList* g_callbacks;

static void freeHeader(AInputHeader* Header)
{
    free(Header->name);
    free(Header->shortName);
}

static void addUmbrella(const char* Name, const char* Inputs)
{
    AInput* umbrella = a_input_new(Inputs);
    a_strhash_add(g_umbrellas, Name, umbrella);
}

static void findInput(const char* Name, const AStrHash* Collection, AInput* Input, AList* AddTo)
{
    AInputHeader* h = a_strhash_get(Collection, Name);

    if(h) {
        a_list_addLast(AddTo, h);

        if(Input->name == NULL) {
            Input->name = h->shortName;
        }
    }
}

void a_input__init(void)
{
    g_buttons = a_strhash_new();
    g_analogs = a_strhash_new();
    g_touchScreens = a_strhash_new();
    g_umbrellas = a_strhash_new();
    g_controllers = a_list_new();
    g_activeController = NULL;

    g_userInputs = a_list_new();
    g_callbacks = a_list_new();

    a_sdl__input_bind();

    if(a_input_numControllers() > 0) {
        a_input_setController(0);
    }

    addUmbrella("generic.up", "pc.up controller.up gp2x.up wiz.up caanoo.up pandora.up");
    addUmbrella("generic.down", "pc.down controller.down gp2x.down wiz.down caanoo.down pandora.down");
    addUmbrella("generic.left", "pc.left controller.left gp2x.left wiz.left caanoo.left pandora.left");
    addUmbrella("generic.right", "pc.right controller.right gp2x.right wiz.right caanoo.right pandora.right");

    addUmbrella("generic.b0", "pc.z controller.b0 gp2x.x wiz.x caanoo.x pandora.x");
    addUmbrella("generic.b1", "pc.x controller.b1 gp2x.b wiz.b caanoo.b pandora.b");
    addUmbrella("generic.b2", "pc.c controller.b2 gp2x.a wiz.a caanoo.a pandora.a");
    addUmbrella("generic.b3", "pc.v controller.b3 gp2x.y wiz.y caanoo.y pandora.y");
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(g_userInputs, AInput*, input) {
        a_input__free(input);
    }

    a_list_free(g_userInputs);
    a_list_free(g_callbacks);

    A_STRHASH_ITERATE(g_buttons, AInputButton*, b) {
        freeHeader(&b->header);
        free(b);
    }

    A_STRHASH_ITERATE(g_analogs, AInputAnalog*, a) {
        freeHeader(&a->header);
        free(a);
    }

    A_STRHASH_ITERATE(g_touchScreens, AInputTouch*, t) {
        freeHeader(&t->header);

        A_LIST_ITERATE(t->motion, APoint*, p) {
            free(p);
        }

        a_list_free(t->motion);
        free(t);
    }

    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, AInputButton*, b) {
            freeHeader(&b->header);
            free(b);
        }

        a_strhash_free(c->buttons);
        free(c);
    }

    a_strhash_free(g_buttons);
    a_strhash_free(g_analogs);
    a_strhash_free(g_touchScreens);
    a_strhash_free(g_umbrellas);
    a_list_free(g_controllers);
}

void a_input__newController(void)
{
    AInputController* c = a_mem_malloc(sizeof(AInputController));

    c->buttons = a_strhash_new();

    a_list_addLast(g_controllers, c);
    g_activeController = c;
}

AInputButton* a_input__newButton(const char* Name)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    b->header.name = a_str_dup(Name);
    b->header.shortName = a_str_getSuffixLastFind(Name, '.');
    b->pressed = false;
    b->waitingForUnpress = false;
    b->analogPushedPast = false;
    b->freshEvent = false;

    if(g_activeController == NULL) {
        a_strhash_add(g_buttons, Name, b);
    } else {
        a_strhash_add(g_activeController->buttons, Name, b);
    }

    return b;
}

AInputAnalog* a_input__newAnalog(const char* Name)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    a->header.name = a_str_dup(Name);
    a->header.shortName = a_str_getSuffixLastFind(Name, '.');
    a->xaxis = 0;
    a->yaxis = 0;

    a_strhash_add(g_analogs, Name, a);

    return a;
}

AInputTouch* a_input__newTouch(const char* Name)
{
    AInputTouch* t = a_mem_malloc(sizeof(AInputTouch));

    t->header.name = a_str_dup(Name);
    t->header.shortName = a_str_getSuffixLastFind(Name, '.');
    t->tap = false;
    t->x = 0;
    t->y = 0;
    t->motion = a_list_new();

    a_strhash_add(g_touchScreens, Name, t);

    return t;
}

void a_input__addCallback(AInputCallback Callback)
{
    a_list_addLast(g_callbacks, Callback);
}

void a_input__get(void)
{
    A_STRHASH_ITERATE(g_touchScreens, AInputTouch*, touchScreen) {
        touchScreen->tap = false;

        A_LIST_ITERATE(touchScreen->motion, APoint*, p) {
            free(p);
        }

        a_list_clear(touchScreen->motion);
    }

    A_STRHASH_ITERATE(g_buttons, AInputButton*, button) {
        button->freshEvent = false;
    }

    a_sdl__input_get();

    A_LIST_ITERATE(g_callbacks, AInputCallback, callback) {
        callback();
    }

    // GP2X and Wiz dpad diagonals show up as dedicated buttons instead of a
    // combination of two separate buttons. This code checks diagonal events
    // and sets the state of each actual button accordingly.
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            AInputButton* upLeft = a_strhash_get(g_buttons, "gp2x.upleft");
            AInputButton* upRight = a_strhash_get(g_buttons, "gp2x.upright");
            AInputButton* downLeft = a_strhash_get(g_buttons, "gp2x.downleft");
            AInputButton* downRight = a_strhash_get(g_buttons, "gp2x.downright");
            AInputButton* up = a_strhash_get(g_buttons, "gp2x.up");
            AInputButton* down = a_strhash_get(g_buttons, "gp2x.down");
            AInputButton* left = a_strhash_get(g_buttons, "gp2x.left");
            AInputButton* right = a_strhash_get(g_buttons, "gp2x.right");
        #elif A_PLATFORM_WIZ
            AInputButton* upLeft = a_strhash_get(g_buttons, "wiz.upleft");
            AInputButton* upRight = a_strhash_get(g_buttons, "wiz.upright");
            AInputButton* downLeft = a_strhash_get(g_buttons, "wiz.downleft");
            AInputButton* downRight = a_strhash_get(g_buttons, "wiz.downright");
            AInputButton* up = a_strhash_get(g_buttons, "wiz.up");
            AInputButton* down = a_strhash_get(g_buttons, "wiz.down");
            AInputButton* left = a_strhash_get(g_buttons, "wiz.left");
            AInputButton* right = a_strhash_get(g_buttons, "wiz.right");
        #endif

        if(upLeft->freshEvent) {
            if(upLeft->pressed) {
                up->pressed = left->pressed = true;
            } else {
                // if the up button itself was not pressed, unpress it
                if(!up->freshEvent) {
                    up->pressed = false;
                }

                if(!left->freshEvent) {
                    left->pressed = false;
                }
            }
        }

        if(upRight->freshEvent) {
            if(upRight->pressed) {
                up->pressed = right->pressed = true;
            } else {
                if(!up->freshEvent) {
                    up->pressed = false;
                }

                if(!right->freshEvent) {
                    right->pressed = false;
                }
            }
        }

        if(downLeft->freshEvent) {
            if(downLeft->pressed) {
                down->pressed = left->pressed = true;
            } else {
                if(!down->freshEvent) {
                    down->pressed = false;
                }

                if(!left->freshEvent) {
                    left->pressed = false;
                }
            }
        }

        if(downRight->freshEvent) {
            if(downRight->pressed) {
                down->pressed = right->pressed = true;
            } else {
                if(!down->freshEvent) {
                    down->pressed = false;
                }

                if(!right->freshEvent) {
                    right->pressed = false;
                }
            }
        }
    #endif

    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        // pressed at least half-way
        #define ANALOG_TRESH ((1 << 15) / 2)

        AInputAnalog* stick = a_strhash_get(g_analogs, "caanoo.stick");
        AInputButton* up = a_strhash_get(g_buttons, "caanoo.up");
        AInputButton* down = a_strhash_get(g_buttons, "caanoo.down");
        AInputButton* left = a_strhash_get(g_buttons, "caanoo.left");
        AInputButton* right = a_strhash_get(g_buttons, "caanoo.right");

        if(stick->xaxis < -ANALOG_TRESH) {
            // Tracking analog direction pushes with analogPushedPast lets us
            // call a_button_getAndUnpress and a_button_unpress on the simulated
            // dpad buttons while maintaining correct press/unpress states here.
            if(!left->analogPushedPast) {
                left->analogPushedPast = true;
                left->pressed = true;
            }
        } else {
            if(left->analogPushedPast) {
                left->analogPushedPast = false;
                left->pressed = false;
            }
        }

        if(stick->xaxis > ANALOG_TRESH) {
            if(!right->analogPushedPast) {
                right->analogPushedPast = true;
                right->pressed = true;
            }
        } else {
            if(right->analogPushedPast) {
                right->analogPushedPast = false;
                right->pressed = false;
            }
        }

        if(stick->yaxis < -ANALOG_TRESH) {
            if(!up->analogPushedPast) {
                up->analogPushedPast = true;
                up->pressed = true;
            }
        } else {
            if(up->analogPushedPast) {
                up->analogPushedPast = false;
                up->pressed = false;
            }
        }

        if(stick->yaxis > ANALOG_TRESH) {
            if(!down->analogPushedPast) {
                down->analogPushedPast = true;
                down->pressed = true;
            }
        } else {
            if(down->analogPushedPast) {
                down->analogPushedPast = false;
                down->pressed = false;
            }
        }
    #endif
}

size_t a_input_numControllers(void)
{
    return a_list_size(g_controllers);
}

void a_input_setController(size_t Index)
{
    if(Index >= a_list_size(g_controllers)) {
        a_out__error("Controller %u not present", Index);
        return;
    }

    g_activeController = a_list_get(g_controllers, Index);
}

AInput* a_input_new(const char* Names)
{
    AInput* i = a_mem_malloc(sizeof(AInput));
    AStrTok* tok = a_strtok_new(Names, ", ");

    i->name = NULL;
    i->buttons = a_list_new();
    i->analogs = a_list_new();
    i->touchScreens = a_list_new();
    i->combos = a_list_new();

    A_STRTOK_ITERATE(tok, name) {
        if(a_str_firstIndex(name, '+') > 0) {
            AList* buttons = a_list_new();
            AStrTok* tok = a_strtok_new(name, "+");
            bool missing = false;

            A_STRTOK_ITERATE(tok, part) {
                AInputButton* button = a_strhash_get(g_buttons, part);

                if(button == NULL) {
                    missing = true;
                    a_list_free(buttons);
                    break;
                }

                a_list_addLast(buttons, button);
            }

            if(!missing) {
                AInputButtonCombo* combo = a_mem_malloc(sizeof(AInputButtonCombo));
                AStrBuilder* sb = a_strbuilder_new(128);

                A_LIST_ITERATE(buttons, AInputButton*, button) {
                    a_strbuilder_addString(sb, button->header.shortName);

                    if(!A_LIST_IS_LAST()) {
                        a_strbuilder_addString(sb, "+");
                    }
                }

                combo->header.name = a_str_dup(a_strbuilder_string(sb));
                combo->header.shortName = NULL;
                combo->buttons = buttons;

                a_list_addLast(i->combos, combo);

                if(i->name == NULL) {
                    i->name = combo->header.name;
                }

                a_strbuilder_free(sb);
            }

            a_strtok_free(tok);
        } else if(a_strhash_contains(g_umbrellas, name)) {
            const AInput* umbrella = a_strhash_get(g_umbrellas, name);

            A_LIST_ITERATE(umbrella->buttons, AInputButton*, b) {
                a_list_addLast(i->buttons, b);
            }

            A_LIST_ITERATE(umbrella->analogs, AInputAnalog*, a) {
                a_list_addLast(i->analogs, a);
            }

            A_LIST_ITERATE(umbrella->touchScreens, AInputTouch*, t) {
                a_list_addLast(i->touchScreens, t);
            }

            A_LIST_ITERATE(umbrella->combos, AInputButtonCombo*, c) {
                a_list_addLast(i->combos, c);
            }
        } else if(g_activeController != NULL && a_str_startsWith(name, "controller.")) {
            findInput(name, g_activeController->buttons, i, i->buttons);
        } else {
            findInput(name, g_buttons, i, i->buttons);
            findInput(name, g_analogs, i, i->analogs);
            findInput(name, g_touchScreens, i, i->touchScreens);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(i->buttons)
        && a_list_empty(i->analogs)
        && a_list_empty(i->touchScreens)
        && a_list_empty(i->combos)) {

        a_out__error("No inputs found for '%s'", Names);
    }

    a_list_addLast(g_userInputs, i);

    return i;
}

void a_input__free(AInput* Input)
{
    a_list_free(Input->buttons);
    a_list_free(Input->analogs);
    a_list_free(Input->touchScreens);

    A_LIST_ITERATE(Input->combos, AInputButtonCombo*, c) {
        freeHeader(&c->header);
        a_list_free(c->buttons);
    }

    a_list_free(Input->combos);

    free(Input);
}

const char* a_input_name(const AInput* Input)
{
    return Input->name;
}

bool a_input_working(const AInput* Input)
{
    return !a_list_empty(Input->buttons)
        || !a_list_empty(Input->analogs)
        || !a_list_empty(Input->touchScreens)
        || !a_list_empty(Input->combos);
}

bool a_button_get(const AInput* Button)
{
    A_LIST_ITERATE(Button->buttons, AInputButton*, b) {
        if(b->pressed) {
            return true;
        }
    }

    if(!a_list_empty(Button->combos)) {
        A_LIST_ITERATE(Button->combos, AInputButtonCombo*, c) {
            A_LIST_ITERATE(c->buttons, AInputButton*, b) {
                if(!b->pressed) {
                    break;
                } else if(A_LIST_IS_LAST()) {
                    return true;
                }
            }
        }
    }

    return false;
}

void a_button_unpress(const AInput* Button)
{
    A_LIST_ITERATE(Button->buttons, AInputButton*, b) {
        b->pressed = false;
    }

    A_LIST_ITERATE(Button->combos, AInputButtonCombo*, c) {
        A_LIST_ITERATE(c->buttons, AInputButton*, b) {
            b->pressed = false;
        }
    }
}

bool a_button_getAndUnpress(const AInput* Button)
{
    bool foundPressed = false;

    A_LIST_ITERATE(Button->buttons, AInputButton*, b) {
        if(b->pressed) {
            b->pressed = false;
            b->waitingForUnpress = true;
            foundPressed = true;
        }
    }

    bool anyComboAllPressed = false;

    A_LIST_ITERATE(Button->combos, AInputButtonCombo*, c) {
        A_LIST_ITERATE(c->buttons, AInputButton*, b) {
            if(!b->pressed) {
                break;
            } else if(A_LIST_IS_LAST()) {
                anyComboAllPressed = true;

                A_LIST_ITERATE(c->buttons, AInputButton*, b) {
                    b->pressed = false;
                    b->waitingForUnpress = true;
                }
            }
        }
    }

    return foundPressed || anyComboAllPressed;
}

int a_analog_xaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, AInputAnalog*, a) {
        if(a_math_abs(a->xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, AInputAnalog*, a) {
        if(a_math_abs(a->yaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->yaxis;
        }
    }

    return 0;
}

AFix a_analog_xaxis_fix(const AInput* Analog)
{
    return a_analog_xaxis(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

AFix a_analog_yaxis_fix(const AInput* Analog)
{
    return a_analog_yaxis(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

bool a_touch_tapped(const AInput* Touch)
{
    A_LIST_ITERATE(Touch->touchScreens, AInputTouch*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_point(const AInput* Touch, int X, int Y)
{
    return a_touch_rect(Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_rect(const AInput* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->touchScreens, AInputTouch*, t) {
        if(t->tap
            && a_collide_boxes(X, Y, W, H,
                               t->x, t->y, 1, 1)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(AInputButton* Button, bool Pressed)
{
    if(Button->waitingForUnpress && Pressed) {
        // Ignore press until getting an unpress
        return;
    }

    Button->pressed = Pressed;
    Button->waitingForUnpress = false;
    Button->freshEvent = true;
}

void a_input__analog_setXAxis(AInputAnalog* Analog, int Value)
{
    Analog->xaxis = Value;
}

void a_input__analog_setYAxis(AInputAnalog* Analog, int Value)
{
    Analog->yaxis = Value;
}

void a_input__touch_addMotion(AInputTouch* Touch, int X, int Y)
{
    Touch->x = X;
    Touch->y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        APoint* p = a_mem_malloc(sizeof(APoint));

        p->x = Touch->x;
        p->y = Touch->y;

        a_list_addLast(Touch->motion, p);
    }
}

void a_input__touch_setCoords(AInputTouch* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;
}
