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

struct Input {
    char* name;
    List* buttons; // Lists of InputInstance
    List* analogs;
    List* touches;
};

struct InputInstance {
    char* name;
    union {
        struct {
            bool pressed;
            bool previouslyPressed; // used to simulate key events for analog
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
            int scale; // for zoomed screens
            List* motion; // Points captured by motion event
        } touch;
    } u;
};

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct InputCollection {
    List* list; // inputs registered during init
    StrHash* names; // hash table of above inputs' names
} InputCollection;

#define a_inputs_new() ((InputCollection){a_list_new(), a_strhash_new()})

#define a_inputs_free(i)              \
({                                    \
    A_LIST_ITERATE(i.list, void, v) { \
        free(v);                      \
    }                                 \
    a_list_free(i.list);              \
    a_strhash_free(i.names);          \
})

#define a_inputs_add(i, ptr, name)     \
({                                     \
    a_list_addLast(i.list, ptr);       \
    a_strhash_add(i.names, name, ptr); \
})

static InputCollection buttons;
static InputCollection analogs;
static InputCollection touches;

static List* userInputs; // all inputs returned by a_input_new()

static Input* console;
static Input* screenshot;

#if A_PLATFORM_LINUXPC
    static Input* normalRes;
    static Input* doubleRes;
    static Input* tripleRes;
    static Input* fullScreen;
#endif

static void addButton(const char* name)
{
    InputInstance* b = a_strhash_get(buttons.names, name);

    if(b) {
        a_out__error("Button '%s' is already defined", name);
        return;
    }

    b = a_mem_malloc(sizeof(InputInstance));

    b->name = a_str_dup(name);
    b->u.button.pressed = false;
    b->u.button.previouslyPressed = false;
    b->u.button.freshEvent = false;

    a_inputs_add(buttons, b, name);
    a_sdl__input_matchButton(name, b);
}

static void addAnalog(const char* name)
{
    InputInstance* a = a_strhash_get(analogs.names, name);

    if(a) {
        a_out__error("Analog '%s' is already defined", name);
        return;
    }

    a = a_mem_malloc(sizeof(InputInstance));

    a->name = a_str_dup(name);
    a->u.analog.xaxis = 0;
    a->u.analog.yaxis = 0;

    a_inputs_add(analogs, a, name);
    a_sdl__input_matchAnalog(name, a);
}

static void addTouch(const char* name)
{
    InputInstance* t = a_strhash_get(touches.names, name);

    if(t) {
        a_out__error("Touch '%s' is already defined", name);
        return;
    }

    t = a_mem_malloc(sizeof(InputInstance));

    t->name = a_str_dup(name);
    t->u.touch.tap = false;
    t->u.touch.x = 0;
    t->u.touch.y = 0;
    t->u.touch.scale = a2x_int("video.scale");
    t->u.touch.motion = a_list_new();

    a_inputs_add(touches, t, name);
    a_sdl__input_matchTouch(name, t);
}

void a_input__init(void)
{
    buttons = a_inputs_new();
    analogs = a_inputs_new();
    touches = a_inputs_new();

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
        addButton("wiz.Menu");
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

    userInputs = a_list_new();

    #if A_PLATFORM_LINUXPC
        normalRes = a_input_new("pc.F1");
        doubleRes = a_input_new("pc.F2");
        tripleRes = a_input_new("pc.F3");
        fullScreen = a_input_new("pc.F4");
    #endif

    console = a_input_new(a2x_str("console.button"));
    screenshot = a_input_new(a2x_str("screenshot.button"));
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(buttons.list, InputInstance, i) {
        free(i->name);
    }

    A_LIST_ITERATE(analogs.list, InputInstance, i) {
        free(i->name);
    }

    A_LIST_ITERATE(touches.list, InputInstance, i) {
        free(i->name);

        A_LIST_ITERATE(i->u.touch.motion, Point, p) {
            free(p);
        }
        a_list_free(i->u.touch.motion);
    }

    A_LIST_ITERATE(userInputs, Input, i) {
        a_input__free(i);
    }
    a_list_free(userInputs);

    a_inputs_free(buttons);
    a_inputs_free(analogs);
    a_inputs_free(touches);
}

void a_input__get(void)
{
    A_LIST_ITERATE(touches.list, InputInstance, t) {
        t->u.touch.tap = false;

        A_LIST_ITERATE(t->u.touch.motion, Point, p) {
            free(p);
        }
        a_list_empty(t->u.touch.motion);
    }

    A_LIST_ITERATE(buttons.list, InputInstance, b) {
        b->u.button.freshEvent = false;
    }

    a_sdl__input_get();

    if(a_button_getAndUnpress(console)) {
        a_console__show();
    }

    if(a_button_getAndUnpress(screenshot)) {
        a_screenshot_save();
    }

    // PC-only options
    #if A_PLATFORM_LINUXPC
        bool changed = false;

        if(a_button_getAndUnpress(normalRes)) {
            if(a2x_int("video.scale") != 1) {
                a2x__set("video.scale", "1");
                changed = true;
            }
        } else if(a_button_getAndUnpress(doubleRes)) {
            if(a2x_int("video.scale") != 2) {
                a2x__set("video.scale", "2");
                changed = true;
            }
        } else if(a_button_getAndUnpress(tripleRes)) {
            if(a2x_int("video.scale") != 3) {
                a2x__set("video.scale", "3");
                changed = true;
            }
        } else if(a_button_getAndUnpress(fullScreen)) {
            a2x__flip("video.fullscreen");
            a2x__set("video.scale", "1");
            changed = true;
        }

        if(changed) {
            if(a_screen__change()) {
                int scale = a2x_int("video.scale");

                A_LIST_ITERATE(touches.list, InputInstance, t) {
                    t->u.touch.scale = scale;
                }
            } else {
                a2x__undo("video.scale");
            }
        }
    #endif

    // simulate seperate direction events from diagonals
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            InputInstance* const upLeft = a_strhash_get(buttons.names, "gp2x.UpLeft");
            InputInstance* const upRight = a_strhash_get(buttons.names, "gp2x.UpRight");
            InputInstance* const downLeft = a_strhash_get(buttons.names, "gp2x.DownLeft");
            InputInstance* const downRight = a_strhash_get(buttons.names, "gp2x.DownRight");

            InputInstance* const up = a_strhash_get(buttons.names, "gp2x.Up");
            InputInstance* const down = a_strhash_get(buttons.names, "gp2x.Down");
            InputInstance* const left = a_strhash_get(buttons.names, "gp2x.Left");
            InputInstance* const right = a_strhash_get(buttons.names, "gp2x.Right");
        #elif A_PLATFORM_WIZ
            InputInstance* const upLeft = a_strhash_get(buttons.names, "wiz.UpLeft");
            InputInstance* const upRight = a_strhash_get(buttons.names, "wiz.UpRight");
            InputInstance* const downLeft = a_strhash_get(buttons.names, "wiz.DownLeft");
            InputInstance* const downRight = a_strhash_get(buttons.names, "wiz.DownRight");

            InputInstance* const up = a_strhash_get(buttons.names, "wiz.Up");
            InputInstance* const down = a_strhash_get(buttons.names, "wiz.Down");
            InputInstance* const left = a_strhash_get(buttons.names, "wiz.Left");
            InputInstance* const right = a_strhash_get(buttons.names, "wiz.Right");
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

    // simulate a DPAD with Caanoo's analog stick
    #if A_PLATFORM_CAANOO
        // pressed at least half-way
        #define ANALOG_TRESH (1 << 14)

        InputInstance* const stick = a_strhash_get(analogs.names, "caanoo.Stick");
        InputInstance* const up = a_strhash_get(buttons.names, "caanoo.Up");
        InputInstance* const down = a_strhash_get(buttons.names, "caanoo.Down");
        InputInstance* const left = a_strhash_get(buttons.names, "caanoo.Left");
        InputInstance* const right = a_strhash_get(buttons.names, "caanoo.Right");

        if(stick->u.analog.xaxis < -ANALOG_TRESH) {
            // saving previouslyPressed allows us to call a_button_getAndUnpress
            // or a_button_unpress without losing actual button state
            if(!left->u.button.previouslyPressed) {
                left->u.button.previouslyPressed = true;
                left->u.button.pressed = true;
            }
        } else {
            if(left->u.button.previouslyPressed) {
                left->u.button.previouslyPressed = false;
                left->u.button.pressed = false;
            }
        }

        if(stick->u.analog.xaxis > ANALOG_TRESH) {
            if(!right->u.button.previouslyPressed) {
                right->u.button.previouslyPressed = true;
                right->u.button.pressed = true;
            }
        } else {
            if(right->u.button.previouslyPressed) {
                right->u.button.previouslyPressed = false;
                right->u.button.pressed = false;
            }
        }

        if(stick->u.analog.yaxis < -ANALOG_TRESH) {
            if(!up->u.button.previouslyPressed) {
                up->u.button.previouslyPressed = true;
                up->u.button.pressed = true;
            }
        } else {
            if(up->u.button.previouslyPressed) {
                up->u.button.previouslyPressed = false;
                up->u.button.pressed = false;
            }
        }

        if(stick->u.analog.yaxis > ANALOG_TRESH) {
            if(!down->u.button.previouslyPressed) {
                down->u.button.previouslyPressed = true;
                down->u.button.pressed = true;
            }
        } else {
            if(down->u.button.previouslyPressed) {
                down->u.button.previouslyPressed = false;
                down->u.button.pressed = false;
            }
        }
    #endif
}

Input* a_input_new(const char* names)
{
    Input* const i = a_mem_malloc(sizeof(Input));
    StringTok* tok = a_strtok_new(names, ", ");

    i->name = NULL;
    i->buttons = a_list_new();
    i->analogs = a_list_new();
    i->touches = a_list_new();

    A_STRTOK_ITERATE(tok, name) {
        #define registerInput(instances)                                     \
        ({                                                                   \
            InputInstance* const var = a_strhash_get(instances.names, name); \
            if(var) {                                                        \
                a_list_addLast(i->instances, var);                           \
                if(i->name == NULL) {                                        \
                    i->name = a_str_getSuffixLastFind(name, '.');            \
                }                                                            \
            }                                                                \
        })

        registerInput(buttons);
        registerInput(analogs);
        registerInput(touches);
    }

    a_strtok_free(tok);

    if(a_list_isEmpty(i->buttons)
        && a_list_isEmpty(i->analogs)
        && a_list_isEmpty(i->touches)) {
        a_out__error("No inputs found for '%s'", names);
    }

    a_list_addLast(userInputs, i);

    return i;
}

void a_input__free(Input* i)
{
    free(i->name);

    a_list_free(i->buttons);
    a_list_free(i->analogs);
    a_list_free(i->touches);

    free(i);
}

char* a_input_name(const Input* i)
{
    return i->name;
}

bool a_input_working(const Input* i)
{
    return !a_list_isEmpty(i->buttons)
        || !a_list_isEmpty(i->analogs)
        || !a_list_isEmpty(i->touches);
}

bool a_button_get(const Input* i)
{
    A_LIST_ITERATE(i->buttons, InputInstance, b) {
        if(b->u.button.pressed) {
            return true;
        }
    }

    return false;
}

void a_button_unpress(const Input* i)
{
    A_LIST_ITERATE(i->buttons, InputInstance, b) {
        b->u.button.pressed = false;
    }
}

bool a_button_getAndUnpress(const Input* i)
{
    bool foundPressed = false;

    A_LIST_ITERATE(i->buttons, InputInstance, b) {
        if(b->u.button.pressed) {
            b->u.button.pressed = false;
            foundPressed = true;
        }
    }

    return foundPressed;
}

void a_button_waitFor(const Input* i)
{
    if(a_list_isEmpty(i->buttons)) {
        return;
    }

    a_button_unpress(i);

    do {
        a_fps_start();
        a_fps_end();
    } while(!a_button_getAndUnpress(i));
}

int a_analog_xaxis(const Input* i)
{
    A_LIST_ITERATE(i->analogs, InputInstance, a) {
        if(a_math_abs(a->u.analog.xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->u.analog.xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const Input* i)
{
    A_LIST_ITERATE(i->analogs, InputInstance, a) {
        if(a_math_abs(a->u.analog.yaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->u.analog.yaxis;
        }
    }

    return 0;
}

fix a_analog_xaxis_fix(const Input* i)
{
    return a_analog_xaxis(i) >> (15 - A_FIX_BIT_PRECISION);
}

fix a_analog_yaxis_fix(const Input* i)
{
    return a_analog_yaxis(i) >> (15 - A_FIX_BIT_PRECISION);
}

bool a_touch_tapped(const Input* i)
{
    A_LIST_ITERATE(i->touches, InputInstance, t) {
        if(t->u.touch.tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_point(const Input* i, int x, int y)
{
    return a_touch_rect(i, x - 1, y - 1, 3, 3);
}

bool a_touch_rect(const Input* i, int x, int y, int w, int h)
{
    A_LIST_ITERATE(i->touches, InputInstance, t) {
        const int scale = t->u.touch.scale;

        if(t->u.touch.tap
            && a_collide_boxes(
                x * scale, y * scale, w * scale, h * scale,
                t->u.touch.x, t->u.touch.y, 1, 1)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(InputInstance* b, bool pressed)
{
    b->u.button.pressed = pressed;
    b->u.button.freshEvent = true;
}

void a_input__analog_setXAxis(InputInstance* a, int value)
{
    a->u.analog.xaxis = value;
}

void a_input__analog_setYAxis(InputInstance* a, int value)
{
    a->u.analog.yaxis = value;
}

void a_input__touch_addMotion(InputInstance* t, int x, int y)
{
    t->u.touch.x = x;
    t->u.touch.y = y;

    if(a2x_bool("input.trackMouse")) {
        Point* const p = a_mem_malloc(sizeof(Point));

        p->x = t->u.touch.x;
        p->y = t->u.touch.y;

        a_list_addLast(t->u.touch.motion, p);
    }
}

void a_input__touch_setCoords(InputInstance* t, int x, int y, bool tapped)
{
    t->u.touch.tap = tapped;

    t->u.touch.x = x;
    t->u.touch.y = y;
}
