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
    AList* buttons; // List of APhysicalInput
    AList* analogs; // List of APhysicalInput
    AList* touchScreens; // List of APhysicalInput
    AList* combos; // List of AComboInput
};

typedef struct AComboInput {
    char* name;
    AList* buttons; // List of APhysicalInput
} AComboInput;

struct APhysicalInput {
    char* name;
    char* shortName;
    union {
        struct {
            bool pressed;
            bool waitingForUnpress;
            bool analogPushedPast; // used to simulate key events for analog
            bool freshEvent; // used to simulate separate directions from diagonals
        } button;
        struct {
            int xaxis;
            int yaxis;
        } analog;
        struct {
            int x;
            int y;
            bool tap;
            AList* motion; // APoints captured by motion event
        } touch;
    } u;
};

typedef struct APoint {
    int x;
    int y;
} APoint;

static AStrHash* g_buttons;
static AStrHash* g_analogs;
static AStrHash* g_touchScreens;

// all inputs returned by a_input_new()
static AList* g_userInputs;
static AList* g_callbacks;

static void addButton(const char* Name)
{
    APhysicalInput* b = a_strhash_get(g_buttons, Name);

    if(b) {
        a_out__error("Button '%s' is already defined", Name);
        return;
    }

    b = a_mem_malloc(sizeof(APhysicalInput));

    b->name = a_str_dup(Name);
    b->shortName = a_str_getSuffixLastFind(Name, '.');
    b->u.button.pressed = false;
    b->u.button.waitingForUnpress = false;
    b->u.button.analogPushedPast = false;
    b->u.button.freshEvent = false;

    a_strhash_add(g_buttons, Name, b);
    a_sdl__input_matchButton(Name, b);
}

#if !A_PLATFORM_GP2X && !A_PLATFORM_WIZ
static void addAnalog(const char* Name)
{
    APhysicalInput* a = a_strhash_get(g_analogs, Name);

    if(a) {
        a_out__error("Analog '%s' is already defined", Name);
        return;
    }

    a = a_mem_malloc(sizeof(APhysicalInput));

    a->name = a_str_dup(Name);
    a->shortName = a_str_getSuffixLastFind(Name, '.');
    a->u.analog.xaxis = 0;
    a->u.analog.yaxis = 0;

    a_strhash_add(g_analogs, Name, a);
    a_sdl__input_matchAnalog(Name, a);
}
#endif // !A_PLATFORM_GP2X && !A_PLATFORM_WIZ

static void addTouch(const char* Name)
{
    APhysicalInput* t = a_strhash_get(g_touchScreens, Name);

    if(t) {
        a_out__error("Touchscreen '%s' is already defined", Name);
        return;
    }

    t = a_mem_malloc(sizeof(APhysicalInput));

    t->name = a_str_dup(Name);
    t->shortName = a_str_getSuffixLastFind(Name, '.');
    t->u.touch.tap = false;
    t->u.touch.x = 0;
    t->u.touch.y = 0;
    t->u.touch.motion = a_list_new();

    a_strhash_add(g_touchScreens, Name, t);
    a_sdl__input_matchTouch(Name, t);
}

void a_input__init(void)
{
    g_buttons = a_strhash_new();
    g_analogs = a_strhash_new();
    g_touchScreens = a_strhash_new();

    #if A_PLATFORM_GP2X
        addButton("gp2x.up");
        addButton("gp2x.down");
        addButton("gp2x.left");
        addButton("gp2x.right");
        addButton("gp2x.upleft");
        addButton("gp2x.upright");
        addButton("gp2x.downleft");
        addButton("gp2x.downright");
        addButton("gp2x.l");
        addButton("gp2x.r");
        addButton("gp2x.a");
        addButton("gp2x.b");
        addButton("gp2x.x");
        addButton("gp2x.y");
        addButton("gp2x.start");
        addButton("gp2x.select");
        addButton("gp2x.volup");
        addButton("gp2x.voldown");
        addButton("gp2x.stickclick");
        addTouch("gp2x.touch");
    #elif A_PLATFORM_WIZ
        addButton("wiz.up");
        addButton("wiz.down");
        addButton("wiz.left");
        addButton("wiz.right");
        addButton("wiz.upleft");
        addButton("wiz.upright");
        addButton("wiz.downleft");
        addButton("wiz.downright");
        addButton("wiz.l");
        addButton("wiz.r");
        addButton("wiz.a");
        addButton("wiz.b");
        addButton("wiz.x");
        addButton("wiz.y");
        addButton("wiz.menu");
        addButton("wiz.select");
        addButton("wiz.volup");
        addButton("wiz.voldown");
        addTouch("wiz.touch");
    #elif A_PLATFORM_CAANOO
        addButton("caanoo.up");
        addButton("caanoo.down");
        addButton("caanoo.left");
        addButton("caanoo.right");
        addButton("caanoo.l");
        addButton("caanoo.r");
        addButton("caanoo.a");
        addButton("caanoo.b");
        addButton("caanoo.x");
        addButton("caanoo.y");
        addButton("caanoo.home");
        addButton("caanoo.hold");
        addButton("caanoo.1");
        addButton("caanoo.2");
        addAnalog("caanoo.stick");
        addTouch("caanoo.touch");
    #elif A_PLATFORM_PANDORA
        addButton("pandora.up");
        addButton("pandora.down");
        addButton("pandora.left");
        addButton("pandora.right");
        addButton("pandora.l");
        addButton("pandora.r");
        addButton("pandora.a");
        addButton("pandora.b");
        addButton("pandora.x");
        addButton("pandora.y");
        addButton("pandora.start");
        addButton("pandora.select");
        addTouch("pandora.touch");
        addAnalog("pandora.nub1");
        addAnalog("pandora.nub2");
        addButton("pandora.m");
        addButton("pandora.s");
    #elif A_PLATFORM_LINUXPC || A_PLATFORM_MINGW
        addButton("pc.up");
        addButton("pc.down");
        addButton("pc.left");
        addButton("pc.right");
        addButton("pc.z");
        addButton("pc.x");
        addButton("pc.c");
        addButton("pc.v");
        addButton("pc.m");
        addButton("pc.enter");
        addButton("pc.space");
        addButton("pc.f1");
        addButton("pc.f2");
        addButton("pc.f3");
        addButton("pc.f4");
        addButton("pc.f5");
        addButton("pc.f6");
        addButton("pc.f7");
        addButton("pc.f8");
        addButton("pc.f9");
        addButton("pc.f10");
        addButton("pc.f11");
        addButton("pc.f12");
        addButton("pc.1");
        addButton("pc.0");
        addTouch("pc.mouse");
        addAnalog("joypad.analog1");
        addAnalog("joypad.analog2");
    #endif

    g_userInputs = a_list_new();
    g_callbacks = a_list_new();
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(g_userInputs, AInput*, input) {
        a_input__free(input);
    }

    a_list_free(g_userInputs);
    a_list_free(g_callbacks);

    A_STRHASH_ITERATE(g_buttons, APhysicalInput*, b) {
        free(b->name);
        free(b->shortName);
        free(b);
    }

    A_STRHASH_ITERATE(g_analogs, APhysicalInput*, a) {
        free(a->name);
        free(a->shortName);
        free(a);
    }

    A_STRHASH_ITERATE(g_touchScreens, APhysicalInput*, t) {
        free(t->name);
        free(t->shortName);

        A_LIST_ITERATE(t->u.touch.motion, APoint*, p) {
            free(p);
        }

        a_list_free(t->u.touch.motion);
        free(t);
    }

    a_strhash_free(g_buttons);
    a_strhash_free(g_analogs);
    a_strhash_free(g_touchScreens);
}

void a_input__addCallback(AInputCallback Callback)
{
    a_list_addLast(g_callbacks, Callback);
}

void a_input__get(void)
{
    A_STRHASH_ITERATE(g_touchScreens, APhysicalInput*, touchScreen) {
        touchScreen->u.touch.tap = false;

        A_LIST_ITERATE(touchScreen->u.touch.motion, APoint*, p) {
            free(p);
        }

        a_list_clear(touchScreen->u.touch.motion);
    }

    A_STRHASH_ITERATE(g_buttons, APhysicalInput*, button) {
        button->u.button.freshEvent = false;
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
            APhysicalInput* upLeft = a_strhash_get(g_buttons, "gp2x.upleft");
            APhysicalInput* upRight = a_strhash_get(g_buttons, "gp2x.upright");
            APhysicalInput* downLeft = a_strhash_get(g_buttons, "gp2x.downleft");
            APhysicalInput* downRight = a_strhash_get(g_buttons, "gp2x.downright");
            APhysicalInput* up = a_strhash_get(g_buttons, "gp2x.up");
            APhysicalInput* down = a_strhash_get(g_buttons, "gp2x.down");
            APhysicalInput* left = a_strhash_get(g_buttons, "gp2x.left");
            APhysicalInput* right = a_strhash_get(g_buttons, "gp2x.right");
        #elif A_PLATFORM_WIZ
            APhysicalInput* upLeft = a_strhash_get(g_buttons, "wiz.upleft");
            APhysicalInput* upRight = a_strhash_get(g_buttons, "wiz.upright");
            APhysicalInput* downLeft = a_strhash_get(g_buttons, "wiz.downleft");
            APhysicalInput* downRight = a_strhash_get(g_buttons, "wiz.downright");
            APhysicalInput* up = a_strhash_get(g_buttons, "wiz.up");
            APhysicalInput* down = a_strhash_get(g_buttons, "wiz.down");
            APhysicalInput* left = a_strhash_get(g_buttons, "wiz.left");
            APhysicalInput* right = a_strhash_get(g_buttons, "wiz.right");
        #endif

        if(upLeft->u.button.freshEvent) {
            if(upLeft->u.button.pressed) {
                up->u.button.pressed = left->u.button.pressed = true;
            } else {
                // if the up button itself was not pressed, unpress it
                if(!up->u.button.freshEvent) {
                    up->u.button.pressed = false;
                }

                if(!left->u.button.freshEvent) {
                    left->u.button.pressed = false;
                }
            }
        }

        if(upRight->u.button.freshEvent) {
            if(upRight->u.button.pressed) {
                up->u.button.pressed = right->u.button.pressed = true;
            } else {
                if(!up->u.button.freshEvent) {
                    up->u.button.pressed = false;
                }

                if(!right->u.button.freshEvent) {
                    right->u.button.pressed = false;
                }
            }
        }

        if(downLeft->u.button.freshEvent) {
            if(downLeft->u.button.pressed) {
                down->u.button.pressed = left->u.button.pressed = true;
            } else {
                if(!down->u.button.freshEvent) {
                    down->u.button.pressed = false;
                }

                if(!left->u.button.freshEvent) {
                    left->u.button.pressed = false;
                }
            }
        }

        if(downRight->u.button.freshEvent) {
            if(downRight->u.button.pressed) {
                down->u.button.pressed = right->u.button.pressed = true;
            } else {
                if(!down->u.button.freshEvent) {
                    down->u.button.pressed = false;
                }

                if(!right->u.button.freshEvent) {
                    right->u.button.pressed = false;
                }
            }
        }
    #endif

    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        // pressed at least half-way
        #define ANALOG_TRESH ((1 << 15) / 2)

        APhysicalInput* stick = a_strhash_get(g_analogs, "caanoo.stick");
        APhysicalInput* up = a_strhash_get(g_buttons, "caanoo.up");
        APhysicalInput* down = a_strhash_get(g_buttons, "caanoo.down");
        APhysicalInput* left = a_strhash_get(g_buttons, "caanoo.left");
        APhysicalInput* right = a_strhash_get(g_buttons, "caanoo.right");

        if(stick->u.analog.xaxis < -ANALOG_TRESH) {
            // Tracking analog direction pushes with analogPushedPast lets us
            // call a_button_getAndUnpress and a_button_unpress on the simulated
            // dpad buttons while maintaining correct press/unpress states here.
            if(!left->u.button.analogPushedPast) {
                left->u.button.analogPushedPast = true;
                left->u.button.pressed = true;
            }
        } else {
            if(left->u.button.analogPushedPast) {
                left->u.button.analogPushedPast = false;
                left->u.button.pressed = false;
            }
        }

        if(stick->u.analog.xaxis > ANALOG_TRESH) {
            if(!right->u.button.analogPushedPast) {
                right->u.button.analogPushedPast = true;
                right->u.button.pressed = true;
            }
        } else {
            if(right->u.button.analogPushedPast) {
                right->u.button.analogPushedPast = false;
                right->u.button.pressed = false;
            }
        }

        if(stick->u.analog.yaxis < -ANALOG_TRESH) {
            if(!up->u.button.analogPushedPast) {
                up->u.button.analogPushedPast = true;
                up->u.button.pressed = true;
            }
        } else {
            if(up->u.button.analogPushedPast) {
                up->u.button.analogPushedPast = false;
                up->u.button.pressed = false;
            }
        }

        if(stick->u.analog.yaxis > ANALOG_TRESH) {
            if(!down->u.button.analogPushedPast) {
                down->u.button.analogPushedPast = true;
                down->u.button.pressed = true;
            }
        } else {
            if(down->u.button.analogPushedPast) {
                down->u.button.analogPushedPast = false;
                down->u.button.pressed = false;
            }
        }
    #endif
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
                APhysicalInput* button = a_strhash_get(g_buttons, part);

                if(button == NULL) {
                    missing = true;
                    a_list_free(buttons);
                    break;
                }

                a_list_addLast(buttons, button);
            }

            if(!missing) {
                AComboInput* combo = a_mem_malloc(sizeof(AComboInput));
                AStrBuilder* sb = a_strbuilder_new(128);

                A_LIST_ITERATE(buttons, APhysicalInput*, button) {
                    a_strbuilder_addString(sb, button->shortName);

                    if(!A_LIST_IS_LAST()) {
                        a_strbuilder_addString(sb, "+");
                    }
                }

                combo->name = a_str_dup(a_strbuilder_string(sb));
                combo->buttons = buttons;

                a_list_addLast(i->combos, combo);

                if(i->name == NULL) {
                    i->name = combo->name;
                }

                a_strbuilder_free(sb);
            }

            a_strtok_free(tok);
        } else {
            #define findNameInCollection(collection)                      \
            ({                                                            \
                APhysicalInput* pi = a_strhash_get(g_##collection, name); \
                if(pi) {                                                  \
                    a_list_addLast(i->collection, pi);                    \
                    if(i->name == NULL) {                                 \
                        i->name = pi->shortName;                          \
                    }                                                     \
                }                                                         \
            })

            findNameInCollection(buttons);
            findNameInCollection(analogs);
            findNameInCollection(touchScreens);
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

    A_LIST_ITERATE(Input->combos, AComboInput*, c) {
        free(c->name);
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
    A_LIST_ITERATE(Button->buttons, APhysicalInput*, b) {
        if(b->u.button.pressed) {
            return true;
        }
    }

    if(!a_list_empty(Button->combos)) {
        A_LIST_ITERATE(Button->combos, AComboInput*, c) {
            A_LIST_ITERATE(c->buttons, APhysicalInput*, b) {
                if(!b->u.button.pressed) {
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
    A_LIST_ITERATE(Button->buttons, APhysicalInput*, b) {
        b->u.button.pressed = false;
    }

    A_LIST_ITERATE(Button->combos, AComboInput*, c) {
        A_LIST_ITERATE(c->buttons, APhysicalInput*, b) {
            b->u.button.pressed = false;
        }
    }
}

bool a_button_getAndUnpress(const AInput* Button)
{
    bool foundPressed = false;

    A_LIST_ITERATE(Button->buttons, APhysicalInput*, b) {
        if(b->u.button.pressed) {
            b->u.button.pressed = false;
            b->u.button.waitingForUnpress = true;
            foundPressed = true;
        }
    }

    bool anyComboAllPressed = false;

    A_LIST_ITERATE(Button->combos, AComboInput*, c) {
        A_LIST_ITERATE(c->buttons, APhysicalInput*, b) {
            if(!b->u.button.pressed) {
                break;
            } else if(A_LIST_IS_LAST()) {
                anyComboAllPressed = true;

                A_LIST_ITERATE(c->buttons, APhysicalInput*, b) {
                    b->u.button.pressed = false;
                    b->u.button.waitingForUnpress = true;
                }
            }
        }
    }

    return foundPressed || anyComboAllPressed;
}

int a_analog_xaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, APhysicalInput*, a) {
        if(a_math_abs(a->u.analog.xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->u.analog.xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, APhysicalInput*, a) {
        if(a_math_abs(a->u.analog.yaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->u.analog.yaxis;
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
    A_LIST_ITERATE(Touch->touchScreens, APhysicalInput*, t) {
        if(t->u.touch.tap) {
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
    A_LIST_ITERATE(Touch->touchScreens, APhysicalInput*, t) {
        if(t->u.touch.tap
            && a_collide_boxes(X, Y, W, H,
                               t->u.touch.x, t->u.touch.y, 1, 1)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(APhysicalInput* Button, bool Pressed)
{
    if(Button->u.button.waitingForUnpress && Pressed) {
        // Ignore press until getting an unpress
        return;
    }

    Button->u.button.pressed = Pressed;
    Button->u.button.waitingForUnpress = false;
    Button->u.button.freshEvent = true;
}

void a_input__analog_setXAxis(APhysicalInput* Analog, int Value)
{
    Analog->u.analog.xaxis = Value;
}

void a_input__analog_setYAxis(APhysicalInput* Analog, int Value)
{
    Analog->u.analog.yaxis = Value;
}

void a_input__touch_addMotion(APhysicalInput* Touch, int X, int Y)
{
    Touch->u.touch.x = X;
    Touch->u.touch.y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        APoint* p = a_mem_malloc(sizeof(APoint));

        p->x = Touch->u.touch.x;
        p->y = Touch->u.touch.y;

        a_list_addLast(Touch->u.touch.motion, p);
    }
}

void a_input__touch_setCoords(APhysicalInput* Touch, int X, int Y, bool Tapped)
{
    Touch->u.touch.x = X;
    Touch->u.touch.y = Y;
    Touch->u.touch.tap = Tapped;
}
