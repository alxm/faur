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

#define MAX_CODES 4

struct Input {
    char* name;
    List* buttons;
    List* analogs;
    List* touches;
};

typedef struct Button {
    char* name;
    int numCodes;
    int codes[MAX_CODES]; // SDL button/key code
    bool pressed;
    bool previouslyPressed; // used to simulate key events for analog
    bool freshEvent; // used to simulate separate directions from diagonals
} Button;

typedef struct Analog {
    char* name;
    int id;
    int xaxis;
    int yaxis;
} Analog;

typedef struct Touch {
    char* name;
    bool tap;
    int x;
    int y;
    int scale; // for zoomed screens
    List* motion; // Points captured by motion event
} Touch;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Inputs {
    List* list; // inputs registered during init
    StrHash* names; // hash table of above inputs' names
} Inputs;

typedef enum InputAction {
    A_ACTION_NONE, A_ACTION_PRESSED, A_ACTION_UNPRESSED
} InputAction;

#define a_inputs_new() ((Inputs){a_list_new(), a_strhash_new()})

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

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    static SDL_Joystick* joystick;
#endif

static Inputs buttons;
static Inputs analogs;
static Inputs touches;

static List* userInputs; // all inputs returned by a_input_new()

static Input* screenshot;

#if A_PLATFORM_LINUXPC
    //static Input* fullScreen;
    static Input* normalRes;
    static Input* doubleRes;
    static Input* tripleRes;
#endif

static void addButton(const char* name, int code);
#if A_PLATFORM_CAANOO
    static void addAnalog(const char* name, int id);
#endif
static void addTouch(const char* name);

void a_input__init(void)
{
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

        joystick = SDL_JoystickOpen(0);
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

        joystick = SDL_JoystickOpen(0);
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
        addAnalog("caanoo.Stick", 0);
        addTouch("caanoo.Touch");

        joystick = SDL_JoystickOpen(0);
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
        addButton("pc.Enter", SDLK_RETURN);
        addButton("pc.Space", SDLK_SPACE);
        addButton("pc.F1", SDLK_F1);
        addButton("pc.F2", SDLK_F2);
        addButton("pc.F3", SDLK_F3);
        addButton("pc.F12", SDLK_F12);
        addButton("pc.1", SDLK_1);
        addButton("pc.0", SDLK_0);
        addTouch("pc.Mouse");
    #endif

    userInputs = a_list_new();

    #if A_PLATFORM_LINUXPC
        //fullScreen = a_input_new("pc.");
        normalRes = a_input_new("pc.F1");
        doubleRes = a_input_new("pc.F2");
        tripleRes = a_input_new("pc.F3");
    #endif

    screenshot = a_input_new(a2x_str("screenshot.button"));
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(touches.list, Touch, t) {
        A_LIST_ITERATE(t->motion, Point, p) {
            free(p);
        }
        a_list_free(t->motion);
    }

    A_LIST_ITERATE(userInputs, Input, i) {
        a_input__free(i);
    }
    a_list_free(userInputs);

    a_inputs_free(buttons);
    a_inputs_free(analogs);
    a_inputs_free(touches);

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        SDL_JoystickClose(joystick);
    #endif
}

void a_input__get(void)
{
    A_LIST_ITERATE(touches.list, Touch, t) {
        t->tap = false;

        A_LIST_ITERATE(t->motion, Point, p) {
            free(p);
        }
        a_list_empty(t->motion);
    }

    A_LIST_ITERATE(buttons.list, Button, b) {
        b->freshEvent = false;
    }

    Button* const any = a_list__first(buttons.list);
    any->pressed = false;

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        InputAction action = A_ACTION_NONE;
        int buttonCode = -1;

        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            case SDL_KEYDOWN: {
                action = A_ACTION_PRESSED;
                buttonCode = event.key.keysym.sym;

                if(buttonCode == SDLK_ESCAPE) {
                    a_state_exit();
                }
            } break;

            case SDL_KEYUP: {
                action = A_ACTION_UNPRESSED;
                buttonCode = event.key.keysym.sym;
            } break;

            case SDL_JOYBUTTONDOWN: {
                action = A_ACTION_PRESSED;
                buttonCode = event.jbutton.button;
            } break;

            case SDL_JOYBUTTONUP: {
                action = A_ACTION_UNPRESSED;
                buttonCode = event.jbutton.button;
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(analogs.list, Analog, a) {
                    if(a->id == event.jaxis.which) {
                        if(event.jaxis.axis == 0) {
                            a->xaxis = event.jaxis.value;
                        } else {
                            a->yaxis = event.jaxis.value;
                        }

                        break;
                    }
                }
            } break;

            case SDL_MOUSEMOTION: {
                A_LIST_ITERATE(touches.list, Touch, t) {
                    t->x = event.button.x;
                    t->y = event.button.y;

                    if(a2x_bool("input.trackMouse")) {
                        Point* const p = malloc(sizeof(Point));

                        p->x = t->x;
                        p->y = t->y;

                        a_list_addLast(t->motion, p);
                    }
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        action = A_ACTION_PRESSED;

                        A_LIST_ITERATE(touches.list, Touch, t) {
                            t->tap = true;
                            t->x = event.button.x;
                            t->y = event.button.y;
                        }
                    break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        A_LIST_ITERATE(touches.list, Touch, t) {
                            t->x = event.button.x;
                            t->y = event.button.y;
                        }
                    break;
                }
            } break;

            default:break;
        }

        if(action != A_ACTION_NONE) {
            if(action == A_ACTION_PRESSED) {
                any->pressed = true;
            }

            A_LIST_ITERATE(buttons.list, Button, b) {
                for(int c = b->numCodes; c--; ) {
                    if(b->codes[c] == buttonCode) {
                        b->pressed = action == A_ACTION_PRESSED;
                        b->freshEvent = true;
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
        /*if(a_button_getAndUnpress(fullScreen)) {
            a_screen__full();
        }*/

        bool changed = false;

        if(a_button_getAndUnpress(normalRes) && a2x_int("video.scale") > 1) {
            a2x__set("video.scale", "1");
            changed = true;
        } else if(a_button_getAndUnpress(doubleRes) && a2x_int("video.scale") != 2) {
            a2x__set("video.scale", "2");
            changed = true;
        } else if(a_button_getAndUnpress(tripleRes) && a2x_int("video.scale") != 3) {
            a2x__set("video.scale", "3");
            changed = true;
        }

        if(changed) {
            int scale = a2x_int("video.scale");

            A_LIST_ITERATE(touches.list, Touch, t) {
                t->scale = scale;
            }

            a_screen__applyScale();
        }
    #endif

    // simulate seperate direction events from diagonals
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            Button* const upLeft = a_strhash_get(buttons.names, "gp2x.UpLeft");
            Button* const upRight = a_strhash_get(buttons.names, "gp2x.UpRight");
            Button* const downLeft = a_strhash_get(buttons.names, "gp2x.DownLeft");
            Button* const downRight = a_strhash_get(buttons.names, "gp2x.DownRight");

            Button* const up = a_strhash_get(buttons.names, "gp2x.Up");
            Button* const down = a_strhash_get(buttons.names, "gp2x.Down");
            Button* const left = a_strhash_get(buttons.names, "gp2x.Left");
            Button* const right = a_strhash_get(buttons.names, "gp2x.Right");
        #elif A_PLATFORM_WIZ
            Button* const upLeft = a_strhash_get(buttons.names, "wiz.UpLeft");
            Button* const upRight = a_strhash_get(buttons.names, "wiz.UpRight");
            Button* const downLeft = a_strhash_get(buttons.names, "wiz.DownLeft");
            Button* const downRight = a_strhash_get(buttons.names, "wiz.DownRight");

            Button* const up = a_strhash_get(buttons.names, "wiz.Up");
            Button* const down = a_strhash_get(buttons.names, "wiz.Down");
            Button* const left = a_strhash_get(buttons.names, "wiz.Left");
            Button* const right = a_strhash_get(buttons.names, "wiz.Right");
        #endif

        if(upLeft->freshEvent) {
            if(upLeft->pressed) {
                up->pressed = left->pressed = true;
            } else {
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

    // simulate a DPAD with Caanoo's analog stick
    #if A_PLATFORM_CAANOO
        // pressed at least half-way
        #define ANALOG_TRESH (1 << 14)

        Analog* const stick = a_strhash_get(analogs.names, "caanoo.Stick");
        Button* const up = a_strhash_get(buttons.names, "caanoo.Up");
        Button* const down = a_strhash_get(buttons.names, "caanoo.Down");
        Button* const left = a_strhash_get(buttons.names, "caanoo.Left");
        Button* const right = a_strhash_get(buttons.names, "caanoo.Right");

        if(stick->xaxis < -ANALOG_TRESH) {
            if(!left->previouslyPressed) {
                left->previouslyPressed = true;
                left->pressed = true;
            }
        } else {
            if(left->previouslyPressed) {
                left->previouslyPressed = false;
                left->pressed = false;
            }
        }

        if(stick->xaxis > ANALOG_TRESH) {
            if(!right->previouslyPressed) {
                right->previouslyPressed = true;
                right->pressed = true;
            }
        } else {
            if(right->previouslyPressed) {
                right->previouslyPressed = false;
                right->pressed = false;
            }
        }

        if(stick->yaxis < -ANALOG_TRESH) {
            if(!up->previouslyPressed) {
                up->previouslyPressed = true;
                up->pressed = true;
            }
        } else {
            if(up->previouslyPressed) {
                up->previouslyPressed = false;
                up->pressed = false;
            }
        }

        if(stick->yaxis > ANALOG_TRESH) {
            if(!down->previouslyPressed) {
                down->previouslyPressed = true;
                down->pressed = true;
            }
        } else {
            if(down->previouslyPressed) {
                down->previouslyPressed = false;
                down->pressed = false;
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
        #define addInput(type, collection)                           \
        ({                                                           \
            type* const var = a_strhash_get(collection.names, name); \
            if(var) {                                                \
                a_list_addLast(i->collection, var);                  \
                if(i->name == NULL) {                                \
                    i->name = a_str_getSuffixLastFind(name, '.');    \
                }                                                    \
            }                                                        \
        })

        addInput(Button, buttons);
        addInput(Analog, analogs);
        addInput(Touch, touches);
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
    A_LIST_ITERATE(i->buttons, Button, b) {
        if(b->pressed) {
            return true;
        }
    }

    return false;
}

void a_button_unpress(const Input* i)
{
    A_LIST_ITERATE(i->buttons, Button, b) {
        b->pressed = false;
    }
}

bool a_button_getAndUnpress(const Input* i)
{
    bool foundPressed = false;

    A_LIST_ITERATE(i->buttons, Button, b) {
        if(b->pressed) {
            b->pressed = false;
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

bool a_touch_tapped(const Input* i)
{
    A_LIST_ITERATE(i->touches, Touch, t) {
        if(t->tap) {
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
    A_LIST_ITERATE(i->touches, Touch, t) {
        const int scale = t->scale;

        if(t->tap
            && a_collide_boxes(
                x * scale, y * scale, w * scale, h * scale,
                t->x, t->y, 1, 1)) {
            return true;
        }
    }

    return false;
}

static void addButton(const char* name, int code)
{
    Button* b = a_strhash_get(buttons.names, name);

    if(!b) {
        b = malloc(sizeof(Button));

        b->name = a_str_dup(name);
        b->numCodes = 1;
        b->codes[0] = code;
        b->pressed = false;
        b->previouslyPressed = false;
        b->freshEvent = false;

        a_inputs_add(buttons, b, name);
    } else {
        if(b->numCodes < MAX_CODES) {
            b->codes[b->numCodes++] = code;
        } else {
            a_error("Button '%s' has too many codes", name);
        }
    }
}

#if A_PLATFORM_CAANOO
    static void addAnalog(const char* name, int id)
    {
        Analog* a = a_strhash_get(analogs.names, name);

        if(!a) {
            a = malloc(sizeof(Analog));

            a->name = a_str_dup(name);
            a->id = id;
            a->xaxis = 0;
            a->yaxis = 0;

            a_inputs_add(analogs, a, name);
        } else {
            a_error("Analog '%s' is already defined", name);
        }
    }
#endif

static void addTouch(const char* name)
{
    Touch* t = a_strhash_get(touches.names, name);

    if(!t) {
        t = malloc(sizeof(Touch));

        t->name = a_str_dup(name);
        t->tap = false;
        t->x = 0;
        t->y = 0;
        t->scale = a2x_int("video.scale");
        t->motion = a_list_new();

        a_inputs_add(touches, t, name);
    } else {
        a_error("Touch '%s' is already defined", name);
    }
}
