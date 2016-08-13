/*
    Copyright 2010 Alex Margarit

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
    char* name;
    AList* g_buttons; // ALists of AInputInstance
    AList* g_analogs;
    AList* g_touchScreens;
};

struct AInputInstance {
    char* name;
    union {
        struct {
            bool pressed;
            bool analogPushedPast; // used to simulate key events for analog
            bool freshEvent; // used to simulate separate directions from diagonals
        } button;
        struct {
            int xaxis;
            int yaxis;
        } analog;
        struct {
            bool tap;
            int x;
            int y;
            AList* motion; // APoints captured by motion event
        } touch;
    } u;
};

typedef struct APoint {
    int x;
    int y;
} APoint;

static AInputCollection* g_buttons;
static AInputCollection* g_analogs;
static AInputCollection* g_touchScreens;

// all inputs returned by a_input_new()
static AList* g_userInputs;

static AInput* g_consoleToggleButton;
static AInput* g_screenshotButton;

static void addButton(const char* Name)
{
    AInputInstance* b = a_strhash_get(g_buttons->names, Name);

    if(b) {
        a_out__error("Button '%s' is already defined", Name);
        return;
    }

    b = a_mem_malloc(sizeof(AInputInstance));

    b->name = a_str_dup(Name);
    b->u.button.pressed = false;
    b->u.button.analogPushedPast = false;
    b->u.button.freshEvent = false;

    a_input__collection_add(g_buttons, b, Name);
    a_sdl__input_matchButton(Name, b);
}

#if !A_PLATFORM_GP2X && !A_PLATFORM_WIZ
static void addAnalog(const char* Name)
{
    AInputInstance* a = a_strhash_get(g_analogs->names, Name);

    if(a) {
        a_out__error("Analog '%s' is already defined", Name);
        return;
    }

    a = a_mem_malloc(sizeof(AInputInstance));

    a->name = a_str_dup(Name);
    a->u.analog.xaxis = 0;
    a->u.analog.yaxis = 0;

    a_input__collection_add(g_analogs, a, Name);
    a_sdl__input_matchAnalog(Name, a);
}
#endif // !A_PLATFORM_GP2X && !A_PLATFORM_WIZ

static void addTouch(const char* Name)
{
    AInputInstance* t = a_strhash_get(g_touchScreens->names, Name);

    if(t) {
        a_out__error("Touchscreen '%s' is already defined", Name);
        return;
    }

    t = a_mem_malloc(sizeof(AInputInstance));

    t->name = a_str_dup(Name);
    t->u.touch.tap = false;
    t->u.touch.x = 0;
    t->u.touch.y = 0;
    t->u.touch.motion = a_list_new();

    a_input__collection_add(g_touchScreens, t, Name);
    a_sdl__input_matchTouch(Name, t);
}

void a_input__init(void)
{
    g_buttons = a_input__collection_new();
    g_analogs = a_input__collection_new();
    g_touchScreens = a_input__collection_new();

    #if A_PLATFORM_GP2X
        addButton("gp2x.Up");
        addButton("gp2x.Down");
        addButton("gp2x.Left");
        addButton("gp2x.Right");
        addButton("gp2x.UpLeft");
        addButton("gp2x.UpRight");
        addButton("gp2x.DownLeft");
        addButton("gp2x.DownRight");
        addButton("gp2x.L");
        addButton("gp2x.R");
        addButton("gp2x.A");
        addButton("gp2x.B");
        addButton("gp2x.X");
        addButton("gp2x.Y");
        addButton("gp2x.Start");
        addButton("gp2x.Select");
        addButton("gp2x.VolUp");
        addButton("gp2x.VolDown");
        addButton("gp2x.StickClick");
        addTouch("gp2x.Touch");
    #elif A_PLATFORM_WIZ
        addButton("wiz.Up");
        addButton("wiz.Down");
        addButton("wiz.Left");
        addButton("wiz.Right");
        addButton("wiz.UpLeft");
        addButton("wiz.UpRight");
        addButton("wiz.DownLeft");
        addButton("wiz.DownRight");
        addButton("wiz.L");
        addButton("wiz.R");
        addButton("wiz.A");
        addButton("wiz.B");
        addButton("wiz.X");
        addButton("wiz.Y");
        addButton("wiz.AMenu");
        addButton("wiz.Select");
        addButton("wiz.VolUp");
        addButton("wiz.VolDown");
        addTouch("wiz.Touch");
    #elif A_PLATFORM_CAANOO
        addButton("caanoo.Up");
        addButton("caanoo.Down");
        addButton("caanoo.Left");
        addButton("caanoo.Right");
        addButton("caanoo.A");
        addButton("caanoo.X");
        addButton("caanoo.B");
        addButton("caanoo.Y");
        addButton("caanoo.L");
        addButton("caanoo.R");
        addButton("caanoo.Home");
        addButton("caanoo.Hold");
        addButton("caanoo.Help1");
        addButton("caanoo.Help2");
        addAnalog("caanoo.Stick");
        addTouch("caanoo.Touch");
    #elif A_PLATFORM_PANDORA
        addButton("pandora.Up");
        addButton("pandora.Down");
        addButton("pandora.Left");
        addButton("pandora.Right");
        addButton("pandora.L");
        addButton("pandora.R");
        addButton("pandora.A");
        addButton("pandora.B");
        addButton("pandora.X");
        addButton("pandora.Y");
        addButton("pandora.Start");
        addButton("pandora.Select");
        addTouch("pandora.Touch");
        addAnalog("pandora.Nub1");
        addAnalog("pandora.Nub2");
        addButton("pandora.m");
        addButton("pandora.s");
    #elif A_PLATFORM_LINUXPC
        addButton("pc.Up");
        addButton("pc.Down");
        addButton("pc.Left");
        addButton("pc.Right");
        addButton("pc.z");
        addButton("pc.x");
        addButton("pc.c");
        addButton("pc.v");
        addButton("pc.m");
        addButton("pc.Enter");
        addButton("pc.Space");
        addButton("pc.F1");
        addButton("pc.F2");
        addButton("pc.F3");
        addButton("pc.F4");
        addButton("pc.F5");
        addButton("pc.F6");
        addButton("pc.F7");
        addButton("pc.F8");
        addButton("pc.F9");
        addButton("pc.F10");
        addButton("pc.F11");
        addButton("pc.F12");
        addButton("pc.1");
        addButton("pc.0");
        addTouch("pc.Mouse");
        addAnalog("joypad.Analog1");
        addAnalog("joypad.Analog2");
    #endif

    g_userInputs = a_list_new();

    g_consoleToggleButton = a_input_new(a_settings_getString("console.button"));
    g_screenshotButton = a_input_new(a_settings_getString("screenshot.button"));
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(g_userInputs, AInput*, input) {
        a_input__free(input);
    }

    a_list_free(g_userInputs);

    A_LIST_ITERATE(g_buttons->list, AInputInstance*, inputInstance) {
        free(inputInstance->name);
    }

    A_LIST_ITERATE(g_analogs->list, AInputInstance*, inputInstance) {
        free(inputInstance->name);
    }

    A_LIST_ITERATE(g_touchScreens->list, AInputInstance*, inputInstance) {
        free(inputInstance->name);

        A_LIST_ITERATE(inputInstance->u.touch.motion, APoint*, p) {
            free(p);
        }
        a_list_free(inputInstance->u.touch.motion);
    }

    a_input__collection_free(g_buttons);
    a_input__collection_free(g_analogs);
    a_input__collection_free(g_touchScreens);
}

AInputCollection* a_input__collection_new(void)
{
    AInputCollection* c = a_mem_malloc(sizeof(AInputCollection));

    c->list = a_list_new();
    c->names = a_strhash_new();

    return c;
}

void a_input__collection_free(AInputCollection* Collection)
{
    A_LIST_ITERATE(Collection->list, void*, v) {
        free(v);
    }

    a_list_free(Collection->list);
    a_strhash_free(Collection->names);

    free(Collection);
}

void a_input__collection_add(AInputCollection* Collection, void* Instance, const char* Name)
{
    a_list_addLast(Collection->list, Instance);
    a_strhash_add(Collection->names, Name, Instance);
}

void a_input__get(void)
{
    A_LIST_ITERATE(g_touchScreens->list, AInputInstance*, inputInstance) {
        inputInstance->u.touch.tap = false;

        A_LIST_ITERATE(inputInstance->u.touch.motion, APoint*, p) {
            free(p);
        }

        a_list_empty(inputInstance->u.touch.motion);
    }

    A_LIST_ITERATE(g_buttons->list, AInputInstance*, inputInstance) {
        inputInstance->u.button.freshEvent = false;
    }

    a_sdl__input_get();

    if(a_button_getAndUnpress(g_consoleToggleButton)) {
        a_console__show();
    }

    if(a_button_getAndUnpress(g_screenshotButton)) {
        a_screenshot_save();
    }

    // GP2X and Wiz dpad diagonals show up as dedicated buttons instead of a
    // combination of two separate buttons. This code checks diagonal events
    // and sets the state of each actual button accordingly.
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            AInputInstance* upLeft = a_strhash_get(g_buttons->names, "gp2x.UpLeft");
            AInputInstance* upRight = a_strhash_get(g_buttons->names, "gp2x.UpRight");
            AInputInstance* downLeft = a_strhash_get(g_buttons->names, "gp2x.DownLeft");
            AInputInstance* downRight = a_strhash_get(g_buttons->names, "gp2x.DownRight");
            AInputInstance* up = a_strhash_get(g_buttons->names, "gp2x.Up");
            AInputInstance* down = a_strhash_get(g_buttons->names, "gp2x.Down");
            AInputInstance* left = a_strhash_get(g_buttons->names, "gp2x.Left");
            AInputInstance* right = a_strhash_get(g_buttons->names, "gp2x.Right");
        #elif A_PLATFORM_WIZ
            AInputInstance* upLeft = a_strhash_get(g_buttons->names, "wiz.UpLeft");
            AInputInstance* upRight = a_strhash_get(g_buttons->names, "wiz.UpRight");
            AInputInstance* downLeft = a_strhash_get(g_buttons->names, "wiz.DownLeft");
            AInputInstance* downRight = a_strhash_get(g_buttons->names, "wiz.DownRight");
            AInputInstance* up = a_strhash_get(g_buttons->names, "wiz.Up");
            AInputInstance* down = a_strhash_get(g_buttons->names, "wiz.Down");
            AInputInstance* left = a_strhash_get(g_buttons->names, "wiz.Left");
            AInputInstance* right = a_strhash_get(g_buttons->names, "wiz.Right");
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

        AInputInstance* stick = a_strhash_get(g_analogs->names, "caanoo.Stick");
        AInputInstance* up = a_strhash_get(g_buttons->names, "caanoo.Up");
        AInputInstance* down = a_strhash_get(g_buttons->names, "caanoo.Down");
        AInputInstance* left = a_strhash_get(g_buttons->names, "caanoo.Left");
        AInputInstance* right = a_strhash_get(g_buttons->names, "caanoo.Right");

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
    AInput* const i = a_mem_malloc(sizeof(AInput));
    AStrTok* tok = a_strtok_new(Names, ", ");

    i->name = NULL;
    i->g_buttons = a_list_new();
    i->g_analogs = a_list_new();
    i->g_touchScreens = a_list_new();

    A_STRTOK_ITERATE(tok, name) {
        #define registerInput(instances)                                      \
        ({                                                                    \
            AInputInstance* const var = a_strhash_get(instances->names, name); \
            if(var) {                                                         \
                a_list_addLast(i->instances, var);                            \
                if(i->name == NULL) {                                         \
                    i->name = a_str_getSuffixLastFind(name, '.');             \
                }                                                             \
            }                                                                 \
        })

        registerInput(g_buttons);
        registerInput(g_analogs);
        registerInput(g_touchScreens);
    }

    a_strtok_free(tok);

    if(a_list_isEmpty(i->g_buttons)
        && a_list_isEmpty(i->g_analogs)
        && a_list_isEmpty(i->g_touchScreens)) {
        a_out__error("No inputs found for '%s'", Names);
    }

    a_list_addLast(g_userInputs, i);

    return i;
}

void a_input__free(AInput* Input)
{
    free(Input->name);

    a_list_free(Input->g_buttons);
    a_list_free(Input->g_analogs);
    a_list_free(Input->g_touchScreens);

    free(Input);
}

char* a_input_name(const AInput* Input)
{
    return Input->name;
}

bool a_input_working(const AInput* Input)
{
    return !a_list_isEmpty(Input->g_buttons)
        || !a_list_isEmpty(Input->g_analogs)
        || !a_list_isEmpty(Input->g_touchScreens);
}

bool a_button_get(const AInput* Button)
{
    A_LIST_ITERATE(Button->g_buttons, AInputInstance*, b) {
        if(b->u.button.pressed) {
            return true;
        }
    }

    return false;
}

void a_button_unpress(const AInput* Input)
{
    A_LIST_ITERATE(Input->g_buttons, AInputInstance*, b) {
        b->u.button.pressed = false;
    }
}

bool a_button_getAndUnpress(const AInput* Button)
{
    bool foundPressed = false;

    A_LIST_ITERATE(Button->g_buttons, AInputInstance*, b) {
        if(b->u.button.pressed) {
            b->u.button.pressed = false;
            foundPressed = true;
        }
    }

    return foundPressed;
}

void a_button_waitFor(const AInput* Button)
{
    if(a_list_isEmpty(Button->g_buttons)) {
        return;
    }

    a_button_unpress(Button);

    do {
        a_fps_start();
        a_fps_end();
    } while(!a_button_getAndUnpress(Button));
}

int a_analog_xaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->g_analogs, AInputInstance*, a) {
        if(a_math_abs(a->u.analog.xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->u.analog.xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->g_analogs, AInputInstance*, a) {
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
    A_LIST_ITERATE(Touch->g_touchScreens, AInputInstance*, t) {
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
    A_LIST_ITERATE(Touch->g_touchScreens, AInputInstance*, t) {
        if(t->u.touch.tap
            && a_collide_boxes(X, Y, W, H,
                               t->u.touch.x, t->u.touch.y, 1, 1)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(AInputInstance* Button, bool Pressed)
{
    Button->u.button.pressed = Pressed;
    Button->u.button.freshEvent = true;
}

void a_input__analog_setXAxis(AInputInstance* Analog, int Value)
{
    Analog->u.analog.xaxis = Value;
}

void a_input__analog_setYAxis(AInputInstance* Analog, int Value)
{
    Analog->u.analog.yaxis = Value;
}

void a_input__touch_addMotion(AInputInstance* Touch, int X, int Y)
{
    Touch->u.touch.x = X;
    Touch->u.touch.y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        APoint* const p = a_mem_malloc(sizeof(APoint));

        p->x = Touch->u.touch.x;
        p->y = Touch->u.touch.y;

        a_list_addLast(Touch->u.touch.motion, p);
    }
}

void a_input__touch_setCoords(AInputInstance* Touch, int X, int Y, bool Tapped)
{
    Touch->u.touch.tap = Tapped;

    Touch->u.touch.x = X;
    Touch->u.touch.y = Y;
}
