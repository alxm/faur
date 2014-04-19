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

#if A_PLATFORM_LINUXPC || A_PLATFORM_CAANOO || A_PLATFORM_PANDORA
    #define A_INPUT_SUPPORTS_ANALOG 1
#else
    #define A_INPUT_SUPPORTS_ANALOG 0
#endif

#define A_ANALOG_MAX_DISTANCE (1 << 15)
#define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

#define A_MAX_BUTTON_CODES 4

#define A_MAX_JOYSTICKS 8

struct Input {
    char* name;
    List* buttons;
    List* analogs;
    List* touches;
};

typedef struct InputInstance {
    char* name;
    int device_index;
    char* device_name;
    union {
        struct {
            int numCodes;
            int codes[A_MAX_BUTTON_CODES]; // SDL button/key code
            bool pressed;
            bool previouslyPressed; // used to simulate key events for analog
            bool freshEvent; // used to simulate separate directions from diagonals
        } button;
        struct {
            int xaxis_index;
            int yaxis_index;
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
    };
} InputInstance;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct InputCollection {
    List* list; // inputs registered during init
    StrHash* names; // hash table of above inputs' names
} InputCollection;

typedef enum InputAction {
    A_ACTION_NONE, A_ACTION_PRESSED, A_ACTION_UNPRESSED
} InputAction;

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

static int joysticks_num;
static SDL_Joystick* joysticks[A_MAX_JOYSTICKS];

static InputCollection buttons;
static InputCollection analogs;
static InputCollection touches;

static List* userInputs; // all inputs returned by a_input_new()

static Input* screenshot;

#if A_PLATFORM_LINUXPC
    static Input* normalRes;
    static Input* doubleRes;
    static Input* tripleRes;
    static Input* fullScreen;
#endif

static void addButton(const char* name, int code);
#if A_INPUT_SUPPORTS_ANALOG
    static void addAnalog(const char* name, int device_index, char* device_name, int xaxis_index, int yaxis_index);
#endif
static void addTouch(const char* name);

void a_input__init(void)
{
    joysticks_num = a_math_min(A_MAX_JOYSTICKS, SDL_NumJoysticks());

    if(joysticks_num > 0) {
        a_out("Found %d joysticks", joysticks_num);
        for(int j = joysticks_num; j--; ) {
            joysticks[j] = SDL_JoystickOpen(j);
        }
    }

    buttons = a_inputs_new();
    analogs = a_inputs_new();
    touches = a_inputs_new();

    addButton("all.any", -1);

    #if A_PLATFORM_GP2X
        addButton("gp2x.Up", 0);
        addButton("gp2x.Down", 4);
        addButton("gp2x.Left", 2);
        addButton("gp2x.Right", 6);
        addButton("gp2x.UpLeft", 1);
        addButton("gp2x.UpRight", 7);
        addButton("gp2x.DownLeft", 3);
        addButton("gp2x.DownRight", 5);
        addButton("gp2x.L", 10);
        addButton("gp2x.R", 11);
        addButton("gp2x.A", 12);
        addButton("gp2x.B", 13);
        addButton("gp2x.X", 14);
        addButton("gp2x.Y", 15);
        addButton("gp2x.Start", 8);
        addButton("gp2x.Select", 9);
        addButton("gp2x.VolUp", 16);
        addButton("gp2x.VolDown", 17);
        addButton("gp2x.StickClick", 18);
        addTouch("gp2x.Touch");
    #elif A_PLATFORM_WIZ
        addButton("wiz.Up", 0);
        addButton("wiz.Down", 4);
        addButton("wiz.Left", 2);
        addButton("wiz.Right", 6);
        addButton("wiz.UpLeft", 1);
        addButton("wiz.UpRight", 7);
        addButton("wiz.DownLeft", 3);
        addButton("wiz.DownRight", 5);
        addButton("wiz.L", 10);
        addButton("wiz.R", 11);
        addButton("wiz.A", 12);
        addButton("wiz.B", 13);
        addButton("wiz.X", 14);
        addButton("wiz.Y", 15);
        addButton("wiz.Menu", 8);
        addButton("wiz.Select", 9);
        addButton("wiz.VolUp", 16);
        addButton("wiz.VolDown", 17);
        addTouch("wiz.Touch");
    #elif A_PLATFORM_CAANOO
        addButton("caanoo.Up", -1);
        addButton("caanoo.Down", -1);
        addButton("caanoo.Left", -1);
        addButton("caanoo.Right", -1);
        addButton("caanoo.A", 0);
        addButton("caanoo.X", 1);
        addButton("caanoo.B", 2);
        addButton("caanoo.Y", 3);
        addButton("caanoo.L", 4);
        addButton("caanoo.R", 5);
        addButton("caanoo.Home", 6);
        addButton("caanoo.Hold", 7);
        addButton("caanoo.Help1", 8);
        addButton("caanoo.Help2", 9);
        addAnalog("caanoo.Stick", 0, NULL, 0, 1);
        addTouch("caanoo.Touch");
    #elif A_PLATFORM_PANDORA
        addButton("pandora.Up", SDLK_UP);
        addButton("pandora.Down", SDLK_DOWN);
        addButton("pandora.Left", SDLK_LEFT);
        addButton("pandora.Right", SDLK_RIGHT);
        addButton("pandora.L", SDLK_RSHIFT);
        addButton("pandora.R", SDLK_RCTRL);
        addButton("pandora.A", SDLK_HOME);
        addButton("pandora.B", SDLK_END);
        addButton("pandora.X", SDLK_PAGEDOWN);
        addButton("pandora.Y", SDLK_PAGEUP);
        addButton("pandora.Start", SDLK_LALT);
        addButton("pandora.Select", SDLK_LCTRL);
        addTouch("pandora.Touch");
        addAnalog("pandora.Nub1", -1, "nub0", 0, 1);
        addAnalog("pandora.Nub2", -1, "nub1", 0, 1);
        addButton("pandora.m", SDLK_m);
        addButton("pandora.s", SDLK_s);
    #elif A_PLATFORM_LINUXPC
        addButton("pc.Up", SDLK_i);
        addButton("pc.Up", SDLK_UP);
        addButton("pc.Down", SDLK_k);
        addButton("pc.Down", SDLK_DOWN);
        addButton("pc.Left", SDLK_j);
        addButton("pc.Left", SDLK_LEFT);
        addButton("pc.Right", SDLK_l);
        addButton("pc.Right", SDLK_RIGHT);
        addButton("pc.z", SDLK_z);
        addButton("pc.x", SDLK_x);
        addButton("pc.c", SDLK_c);
        addButton("pc.v", SDLK_v);
        addButton("pc.m", SDLK_m);
        addButton("pc.Enter", SDLK_RETURN);
        addButton("pc.Space", SDLK_SPACE);
        addButton("pc.F1", SDLK_F1);
        addButton("pc.F2", SDLK_F2);
        addButton("pc.F3", SDLK_F3);
        addButton("pc.F4", SDLK_F4);
        addButton("pc.F5", SDLK_F5);
        addButton("pc.F6", SDLK_F6);
        addButton("pc.F7", SDLK_F7);
        addButton("pc.F8", SDLK_F8);
        addButton("pc.F9", SDLK_F9);
        addButton("pc.F10", SDLK_F10);
        addButton("pc.F11", SDLK_F11);
        addButton("pc.F12", SDLK_F12);
        addButton("pc.1", SDLK_1);
        addButton("pc.0", SDLK_0);
        addTouch("pc.Mouse");
        addAnalog("joypad.Analog1", 0, NULL, 0, 1);
        addAnalog("joypad.Analog2", 0, NULL, 3, 4);
    #endif

    userInputs = a_list_new();

    #if A_PLATFORM_LINUXPC
        normalRes = a_input_new("pc.F1");
        doubleRes = a_input_new("pc.F2");
        tripleRes = a_input_new("pc.F3");
        fullScreen = a_input_new("pc.F4");
    #endif

    screenshot = a_input_new(a2x_str("screenshot.button"));
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(touches.list, InputInstance, t) {
        A_LIST_ITERATE(t->touch.motion, Point, p) {
            free(p);
        }
        a_list_free(t->touch.motion);
    }

    A_LIST_ITERATE(userInputs, Input, i) {
        a_input__free(i);
    }
    a_list_free(userInputs);

    a_inputs_free(buttons);
    a_inputs_free(analogs);
    a_inputs_free(touches);

    for(int j = joysticks_num; j--; ) {
        SDL_JoystickClose(joysticks[j]);
    }
}

void a_input__get(void)
{
    A_LIST_ITERATE(touches.list, InputInstance, t) {
        t->touch.tap = false;

        A_LIST_ITERATE(t->touch.motion, Point, p) {
            free(p);
        }
        a_list_empty(t->touch.motion);
    }

    A_LIST_ITERATE(buttons.list, InputInstance, b) {
        b->button.freshEvent = false;
    }

    InputInstance* const any = a_list__first(buttons.list);
    any->button.pressed = false;

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        InputAction action = A_ACTION_NONE;
        int code = -1;

        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            case SDL_KEYDOWN: {
                action = A_ACTION_PRESSED;
                code = event.key.keysym.sym;

                if(code == SDLK_ESCAPE) {
                    a_state_exit();
                }
            } break;

            case SDL_KEYUP: {
                action = A_ACTION_UNPRESSED;
                code = event.key.keysym.sym;
            } break;

            case SDL_JOYBUTTONDOWN: {
                action = A_ACTION_PRESSED;
                code = event.jbutton.button;
            } break;

            case SDL_JOYBUTTONUP: {
                action = A_ACTION_UNPRESSED;
                code = event.jbutton.button;
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(analogs.list, InputInstance, a) {
                    if(a->device_index == event.jaxis.which) {
                        if(event.jaxis.axis == a->analog.xaxis_index) {
                            a->analog.xaxis = event.jaxis.value;
                        } else if(event.jaxis.axis == a->analog.yaxis_index) {
                            a->analog.yaxis = event.jaxis.value;
                        }
                    }
                }
            } break;

            case SDL_MOUSEMOTION: {
                A_LIST_ITERATE(touches.list, InputInstance, t) {
                    t->touch.x = event.button.x;
                    t->touch.y = event.button.y;

                    if(a2x_bool("input.trackMouse")) {
                        Point* const p = malloc(sizeof(Point));

                        p->x = t->touch.x;
                        p->y = t->touch.y;

                        a_list_addLast(t->touch.motion, p);
                    }
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        action = A_ACTION_PRESSED;

                        A_LIST_ITERATE(touches.list, InputInstance, t) {
                            t->touch.tap = true;
                            t->touch.x = event.button.x;
                            t->touch.y = event.button.y;
                        }
                    break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        A_LIST_ITERATE(touches.list, InputInstance, t) {
                            t->touch.x = event.button.x;
                            t->touch.y = event.button.y;
                        }
                    break;
                }
            } break;

            default:break;
        }

        if(action != A_ACTION_NONE) {
            if(action == A_ACTION_PRESSED) {
                any->button.pressed = true;
            }

            A_LIST_ITERATE(buttons.list, InputInstance, b) {
                for(int c = b->button.numCodes; c--; ) {
                    if(b->button.codes[c] == code) {
                        b->button.pressed = action == A_ACTION_PRESSED;
                        b->button.freshEvent = true;
                        break;
                    }
                }
            }
        }
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
            int scale = a2x_int("video.scale");

            A_LIST_ITERATE(touches.list, InputInstance, t) {
                t->touch.scale = scale;
            }

            a_screen__change();
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

        if(upLeft->button.freshEvent) {
            if(upLeft->button.pressed) {
                up->button.pressed = left->button.pressed = true;
            } else {
                // if the up button itself was not pressed, unpress it
                if(!up->button.freshEvent) {
                    up->button.pressed = false;
                }

                if(!left->button.freshEvent) {
                    left->button.pressed = false;
                }
            }
        }

        if(upRight->button.freshEvent) {
            if(upRight->button.pressed) {
                up->button.pressed = right->button.pressed = true;
            } else {
                if(!up->button.freshEvent) {
                    up->button.pressed = false;
                }

                if(!right->button.freshEvent) {
                    right->button.pressed = false;
                }
            }
        }

        if(downLeft->button.freshEvent) {
            if(downLeft->button.pressed) {
                down->button.pressed = left->pressed = true;
            } else {
                if(!down->button.freshEvent) {
                    down->button.pressed = false;
                }

                if(!left->button.freshEvent) {
                    left->button.pressed = false;
                }
            }
        }

        if(downRight->button.freshEvent) {
            if(downRight->button.pressed) {
                down->button.pressed = right->button.pressed = true;
            } else {
                if(!down->button.freshEvent) {
                    down->button.pressed = false;
                }

                if(!right->button.freshEvent) {
                    right->button.pressed = false;
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

        if(stick->analog.xaxis < -ANALOG_TRESH) {
            // saving previouslyPressed allows us to call a_button_getAndUnpress
            // or a_button_unpress without losing actual button state
            if(!left->button.previouslyPressed) {
                left->button.previouslyPressed = true;
                left->button.pressed = true;
            }
        } else {
            if(left->button.previouslyPressed) {
                left->button.previouslyPressed = false;
                left->button.pressed = false;
            }
        }

        if(stick->analog.xaxis > ANALOG_TRESH) {
            if(!right->button.previouslyPressed) {
                right->button.previouslyPressed = true;
                right->button.pressed = true;
            }
        } else {
            if(right->previouslyPressed) {
                right->button.previouslyPressed = false;
                right->button.pressed = false;
            }
        }

        if(stick->analog.yaxis < -ANALOG_TRESH) {
            if(!up->button.previouslyPressed) {
                up->button.previouslyPressed = true;
                up->button.pressed = true;
            }
        } else {
            if(up->button.previouslyPressed) {
                up->button.previouslyPressed = false;
                up->button.pressed = false;
            }
        }

        if(stick->analog.yaxis > ANALOG_TRESH) {
            if(!down->button.previouslyPressed) {
                down->button.previouslyPressed = true;
                down->button.pressed = true;
            }
        } else {
            if(down->button.previouslyPressed) {
                down->button.previouslyPressed = false;
                down->button.pressed = false;
            }
        }
    #endif
}

Input* a_input_new(const char* names)
{
    Input* const i = malloc(sizeof(Input));

    i->name = NULL;
    i->buttons = a_list_new();
    i->analogs = a_list_new();
    i->touches = a_list_new();

    A_STRTOK_ITERATE(names, ", ", name) {
        #define registerInput(instances)                      \
        ({                                                          \
            InputInstance* const var = a_strhash_get(instances.names, name); \
            if(var) {                                               \
                a_list_addLast(i->instances, var);                  \
                if(i->name == NULL) {                               \
                    i->name = a_str_getSuffixLastFind(name, '.');   \
                }                                                   \
            }                                                       \
        })

        registerInput(buttons);
        registerInput(analogs);
        registerInput(touches);
    }

    if(a_list_isEmpty(i->buttons)
        && a_list_isEmpty(i->analogs)
        && a_list_isEmpty(i->touches)) {
        a_error("No inputs found for '%s'", names);
    }

    a_list_addLast(userInputs, i);

    return i;
}

void a_input__free(Input* i)
{
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
        if(b->button.pressed) {
            return true;
        }
    }

    return false;
}

void a_button_unpress(const Input* i)
{
    A_LIST_ITERATE(i->buttons, InputInstance, b) {
        b->button.pressed = false;
    }
}

bool a_button_getAndUnpress(const Input* i)
{
    bool foundPressed = false;

    A_LIST_ITERATE(i->buttons, InputInstance, b) {
        if(b->button.pressed) {
            b->button.pressed = false;
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
        if(a_math_abs(a->analog.xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->analog.xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const Input* i)
{
    A_LIST_ITERATE(i->analogs, InputInstance, a) {
        if(a_math_abs(a->analog.yaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->analog.yaxis;
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
        if(t->touch.tap) {
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
        const int scale = t->touch.scale;

        if(t->touch.tap
            && a_collide_boxes(
                x * scale, y * scale, w * scale, h * scale,
                t->touch.x, t->touch.y, 1, 1)) {
            return true;
        }
    }

    return false;
}

static void addButton(const char* name, int code)
{
    InputInstance* b = a_strhash_get(buttons.names, name);

    if(!b) {
        b = malloc(sizeof(InputInstance));

        b->name = a_str_dup(name);
        b->button.numCodes = 1;
        b->button.codes[0] = code;
        b->button.pressed = false;
        b->button.previouslyPressed = false;
        b->button.freshEvent = false;

        a_inputs_add(buttons, b, name);
    } else {
        if(b->button.numCodes < A_MAX_BUTTON_CODES) {
            b->button.codes[b->button.numCodes++] = code;
        } else {
            a_error("Button '%s' has too many codes", name);
        }
    }
}

#if A_INPUT_SUPPORTS_ANALOG
    static void addAnalog(const char* name, int device_index, char* device_name, int xaxis_index, int yaxis_index)
    {
        if(device_index == -1 && device_name == NULL) {
            a_error("Inputs must specify device index or name");
            return;
        }

        InputInstance* a = a_strhash_get(analogs.names, name);

        if(!a) {
            a = malloc(sizeof(InputInstance));

            a->name = a_str_dup(name);
            a->device_index = device_index;
            a->device_name = device_name;
            a->analog.xaxis_index = xaxis_index;
            a->analog.yaxis_index = yaxis_index;
            a->analog.xaxis = 0;
            a->analog.yaxis = 0;

            // check if we requested a specific device by name
            if(device_name) {
                for(int j = joysticks_num; j--; ) {
                    if(a_str_same(device_name, SDL_JoystickName(j))) {
                        a->device_index = j;
                        break;
                    }
                }
            }

            a_inputs_add(analogs, a, name);
        } else {
            a_error("Analog '%s' is already defined", name);
        }
    }
#endif

static void addTouch(const char* name)
{
    InputInstance* t = a_strhash_get(touches.names, name);

    if(!t) {
        t = malloc(sizeof(InputInstance));

        t->name = a_str_dup(name);
        t->touch.tap = false;
        t->touch.x = 0;
        t->touch.y = 0;
        t->touch.scale = a2x_int("video.scale");
        t->touch.motion = a_list_new();

        a_inputs_add(touches, t, name);
    } else {
        a_error("Touch '%s' is already defined", name);
    }
}
