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

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Button {
    char* name;
    int numCodes;
    int codes[MAX_CODES];
    int pressed;
    int oldEvent;
    int freshEvent;
} Button;

typedef struct Touch {
    bool tap;
    int x;
    int y;
    int shift;
    List* motion;
} Touch;

typedef struct Analog {
    int id;
    int xaxis;
    int yaxis;
} Analog;

struct Input {
    List* buttons;
    char* name;
    bool working;
};

static Hash* buttonNames;
static List* buttonList;

static Touch mouse;

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    static SDL_Joystick* joystick;
#endif

static List* inputs;

#if A_PLATFORM_LINUXPC
    static Input* fullScreen;
    static Input* doubleRes;
#endif

#if A_PLATFORM_CAANOO
    static Analog caanooAnalog;
#endif

static Input* screenshot;

static void registerButton(const char* const name, const int code);
static void registerButtonFake(const char* const name);

void a_input__set(void)
{
    buttonNames = a_hash_set();
    buttonList = a_list_set();

    inputs = a_list_set();

    registerButtonFake("all.any");

    #if A_PLATFORM_GP2X
        registerButton("gp2x.Up", 0);
        registerButton("gp2x.Down", 4);
        registerButton("gp2x.Left", 2);
        registerButton("gp2x.Right", 6);
        registerButton("gp2x.UpLeft", 1);
        registerButton("gp2x.UpRight", 7);
        registerButton("gp2x.DownLeft", 3);
        registerButton("gp2x.DownRight", 5);
        registerButton("gp2x.L", 10);
        registerButton("gp2x.R", 11);
        registerButton("gp2x.A", 12);
        registerButton("gp2x.B", 13);
        registerButton("gp2x.X", 14);
        registerButton("gp2x.Y", 15);
        registerButton("gp2x.Start", 8);
        registerButton("gp2x.Select", 9);
        registerButton("gp2x.VolUp", 16);
        registerButton("gp2x.VolDown", 17);
        registerButton("gp2x.StickClick", 18);

        joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_WIZ
        registerButton("wiz.Up", 0);
        registerButton("wiz.Down", 4);
        registerButton("wiz.Left", 2);
        registerButton("wiz.Right", 6);
        registerButton("wiz.UpLeft", 1);
        registerButton("wiz.UpRight", 7);
        registerButton("wiz.DownLeft", 3);
        registerButton("wiz.DownRight", 5);
        registerButton("wiz.L", 10);
        registerButton("wiz.R", 11);
        registerButton("wiz.A", 12);
        registerButton("wiz.B", 13);
        registerButton("wiz.X", 14);
        registerButton("wiz.Y", 15);
        registerButton("wiz.Menu", 8);
        registerButton("wiz.Select", 9);
        registerButton("wiz.VolUp", 16);
        registerButton("wiz.VolDown", 17);

        joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_CAANOO
        registerButtonFake("caanoo.Up");
        registerButtonFake("caanoo.Down");
        registerButtonFake("caanoo.Left");
        registerButtonFake("caanoo.Right");
        registerButton("caanoo.A", 0);
        registerButton("caanoo.X", 1);
        registerButton("caanoo.B", 2);
        registerButton("caanoo.Y", 3);
        registerButton("caanoo.L", 4);
        registerButton("caanoo.R", 5);
        registerButton("caanoo.Home", 6);
        registerButton("caanoo.Hold", 7);
        registerButton("caanoo.Help1", 8);
        registerButton("caanoo.Help2", 9);

        caanooAnalog.id = 0;
        caanooAnalog.xaxis = 0;
        caanooAnalog.yaxis = 0;

        joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_LINUXPC
        registerButton("pc.Up", SDLK_i);
        registerButton("pc.Up", SDLK_UP);
        registerButton("pc.Down", SDLK_k);
        registerButton("pc.Down", SDLK_DOWN);
        registerButton("pc.Left", SDLK_j);
        registerButton("pc.Left", SDLK_LEFT);
        registerButton("pc.Right", SDLK_l);
        registerButton("pc.Right", SDLK_RIGHT);
        registerButton("pc.z", SDLK_z);
        registerButton("pc.x", SDLK_x);
        registerButton("pc.c", SDLK_c);
        registerButton("pc.v", SDLK_v);
        registerButton("pc.Enter", SDLK_RETURN);
        registerButton("pc.Space", SDLK_SPACE);
        registerButton("pc.F1", SDLK_F1);
        registerButton("pc.F2", SDLK_F2);
        registerButton("pc.F3", SDLK_F3);
        registerButton("pc.1", SDLK_1);
        registerButton("pc.0", SDLK_0);
    #endif

    mouse.shift = a2x_bool("video.double");

    if(a2x_bool("input.trackMouse")) {
        mouse.motion = a_list_set();
    }

    #if A_PLATFORM_LINUXPC
        fullScreen = a_input_set("pc.F1");
        doubleRes = a_input_set("pc.F2");
    #endif

    screenshot = a_input_set(a2x_str("screenshot.button"));
}

void a_input__free(void)
{
    while(a_list_iterate(inputs)) {
        a_input_free(a_list_current(inputs));
    }

    a_list_free(inputs);

    while(a_list_iterate(buttonList)) {
        Button* const b = a_list_current(buttonList);
        free(b->name);
        free(b);
    }

    a_list_free(buttonList);
    a_hash_free(buttonNames);

    if(a2x_bool("input.trackMouse")) {
        a_list_freeContent(mouse.motion);
    }

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        SDL_JoystickClose(joystick);
    #endif
}

void a_input__get(void)
{
    if(a2x_bool("input.trackMouse")) {
        a_list_freeContent(mouse.motion);
    }

    mouse.tap = false;

    Button* const any = a_list__first(buttonList);
    any->pressed = 0;

    while(a_list_iterate(buttonList)) {
        Button* const b = a_list_current(buttonList);
        b->freshEvent = 0;
    }

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        int button;
        int action = -1;

        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            #if A_PLATFORM_LINUXPC
                case SDL_KEYDOWN: {
                    button = event.key.keysym.sym;
                    action = 1;

                    if(button == SDLK_ESCAPE) {
                        a_state_exit();
                    }
                } break;

                case SDL_KEYUP: {
                    button = event.key.keysym.sym;
                    action = 0;
                } break;
            #endif

            #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
                case SDL_JOYBUTTONDOWN: {
                    button = event.jbutton.button;
                    action = 1;
                } break;

                case SDL_JOYBUTTONUP: {
                    button = event.jbutton.button;
                    action = 0;
                } break;
            #endif

            #if A_PLATFORM_CAANOO
                case SDL_JOYAXISMOTION: {
                    if(event.jaxis.which == caanooAnalog.id) {
                        if(event.jaxis.axis == 0) {
                            caanooAnalog.xaxis = event.jaxis.value;
                        } else {
                            caanooAnalog.yaxis = event.jaxis.value;
                        }
                    }
                } break;
            #endif

            case SDL_MOUSEMOTION: {
                mouse.x = event.button.x;
                mouse.y = event.button.y;

                if(a2x_bool("input.trackMouse")) {
                    Point* const p = malloc(sizeof(Point));

                    p->x = mouse.x;
                    p->y = mouse.y;

                    a_list_addLast(mouse.motion, p);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        mouse.tap = true;
                        mouse.x = event.button.x;
                        mouse.y = event.button.y;
                    break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        mouse.x = event.button.x;
                        mouse.y = event.button.y;
                    break;
                }
            } break;

            default:break;
        }

        if(action != -1) {
            if(action == 1) {
                any->pressed = 1;
            }

            List* const l = buttonList;

            while(a_list_iterate(l)) {
                Button* const b = a_list_current(l);

                for(int i = b->numCodes; i--; ) {
                    if(b->codes[i] == button) {
                        b->pressed = action;
                        b->freshEvent = 1;
                        break;
                    }
                }
            }
        }
    }

    if(a_input_getUnpress(screenshot)) {
        a_screenshot_save();
    }

    // PC-only options
    #if A_PLATFORM_LINUXPC
        if(a_input_getUnpress(fullScreen)) {
            a_screen__switchFull();
        }

        if(a_input_getUnpress(doubleRes)) {
            a2x__flip("video.double");
            a_screen__doubleRes();
            mouse.shift = a2x_bool("video.double");
        }
    #endif

    // simulate seperate direction events
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            Button* const upLeft = a_hash_get(buttonNames, "gp2x.UpLeft");
            Button* const upRight = a_hash_get(buttonNames, "gp2x.UpRight");
            Button* const downLeft = a_hash_get(buttonNames, "gp2x.DownLeft");
            Button* const downRight = a_hash_get(buttonNames, "gp2x.DownRight");

            Button* const up = a_hash_get(buttonNames, "gp2x.Up");
            Button* const down = a_hash_get(buttonNames, "gp2x.Down");
            Button* const left = a_hash_get(buttonNames, "gp2x.Left");
            Button* const right = a_hash_get(buttonNames, "gp2x.Right");
        #elif A_PLATFORM_WIZ
            Button* const upLeft = a_hash_get(buttonNames, "wiz.UpLeft");
            Button* const upRight = a_hash_get(buttonNames, "wiz.UpRight");
            Button* const downLeft = a_hash_get(buttonNames, "wiz.DownLeft");
            Button* const downRight = a_hash_get(buttonNames, "wiz.DownRight");

            Button* const up = a_hash_get(buttonNames, "wiz.Up");
            Button* const down = a_hash_get(buttonNames, "wiz.Down");
            Button* const left = a_hash_get(buttonNames, "wiz.Left");
            Button* const right = a_hash_get(buttonNames, "wiz.Right");
        #endif

        if(upLeft->freshEvent) {
            if(upLeft->pressed) {
                up->pressed = left->pressed = 1;
            } else {
                if(!up->freshEvent) {
                    up->pressed = 0;
                }

                if(!left->freshEvent) {
                    left->pressed = 0;
                }
            }
        }

        if(upRight->freshEvent) {
            if(upRight->pressed) {
                up->pressed = right->pressed = 1;
            } else {
                if(!up->freshEvent) {
                    up->pressed = 0;
                }

                if(!right->freshEvent) {
                    right->pressed = 0;
                }
            }
        }

        if(downLeft->freshEvent) {
            if(downLeft->pressed) {
                down->pressed = left->pressed = 1;
            } else {
                if(!down->freshEvent) {
                    down->pressed = 0;
                }

                if(!left->freshEvent) {
                    left->pressed = 0;
                }
            }
        }

        if(downRight->freshEvent) {
            if(downRight->pressed) {
                down->pressed = right->pressed = 1;
            } else {
                if(!down->freshEvent) {
                    down->pressed = 0;
                }

                if(!right->freshEvent) {
                    right->pressed = 0;
                }
            }
        }
    #endif

    // simulate a DPAD with Caanoo's analog stick
    #if A_PLATFORM_CAANOO
        #define ANALOG_TRESH (1 << 14)

        Button* const up = a_hash_get(buttonNames, "caanoo.Up");
        Button* const down = a_hash_get(buttonNames, "caanoo.Down");
        Button* const left = a_hash_get(buttonNames, "caanoo.Left");
        Button* const right = a_hash_get(buttonNames, "caanoo.Right");

        if(caanooAnalog.xaxis < -ANALOG_TRESH) {
            if(left->oldEvent == 0) {
                left->oldEvent = 1;
                left->pressed = 1;
            }
        } else {
            if(left->oldEvent == 1) {
                left->oldEvent = 0;
                left->pressed = 0;
            }
        }

        if(caanooAnalog.xaxis > ANALOG_TRESH) {
            if(right->oldEvent == 0) {
                right->oldEvent = 1;
                right->pressed = 1;
            }
        } else {
            if(right->oldEvent == 1) {
                right->oldEvent = 0;
                right->pressed = 0;
            }
        }

        if(caanooAnalog.yaxis < -ANALOG_TRESH) {
            if(up->oldEvent == 0) {
                up->oldEvent = 1;
                up->pressed = 1;
            }
        } else {
            if(up->oldEvent == 1) {
                up->oldEvent = 0;
                up->pressed = 0;
            }
        }

        if(caanooAnalog.yaxis > ANALOG_TRESH) {
            if(down->oldEvent == 0) {
                down->oldEvent = 1;
                down->pressed = 1;
            }
        } else {
            if(down->oldEvent == 1) {
                down->oldEvent = 0;
                down->pressed = 0;
            }
        }
    #endif
}

Input* a_input_set(const char* const names)
{
    Input* const i = malloc(sizeof(Input));
    StringTok* const t = a_str_makeTok(names, ", ");

    i->buttons = a_list_set();
    i->name = NULL;

    while(a_str_hasTok(t)) {
        List* const l = buttonList;
        const char* const name = a_str_getTok(t);

        while(a_list_iterate(l)) {
            Button* const b = a_list_current(l);

            if(a_str_equal(name, b->name)) {
                a_list_addLast(i->buttons, b);

                if(a_list_size(i->buttons) == 1) {
                    i->name = a_str_getSuffixLastFind(b->name, '.');
                }
            }
        }
    }

    a_str_freeTok(t);

    i->working = a_list_size(i->buttons) > 0;

    a_list_addLast(inputs, i);

    return i;
}

void a_input_free(Input* const i)
{
    a_list_free(i->buttons);
    free(i->name);

    free(i);

    a_list_remove(inputs, i);
}

bool a_input_get(Input* const i)
{
    List* const l = i->buttons;

    while(a_list_iterate(l)) {
        Button* const b = a_list_current(l);

        if(b->pressed) {
            a_list_reset(l);
            return true;
        }
    }

    return false;
}

void a_input_unpress(Input* const i)
{
    List* const l = i->buttons;

    while(a_list_iterate(l)) {
        Button* const b = a_list_current(l);
        b->pressed = 0;
    }
}

bool a_input_getUnpress(Input* const i)
{
    if(a_input_get(i)) {
        a_input_unpress(i);
        return true;
    }

    return false;
}

char* a_input_name(const Input* const i)
{
    return i->name;
}

bool a_input_working(const Input* const i)
{
    return i->working;
}

void a_input_waitFor(Input* const i)
{
    a_input_unpress(i);

    do {
        a_fps_start();
        a_fps_end();
    } while(!a_input_getUnpress(i));
}

bool a_input_tappedScreen(void)
{
    return mouse.tap;
}

bool a_input_touchedPoint(const int x, const int y)
{
    const int shift = mouse.shift;

    return mouse.tap
        && a_collide_boxes(
            (x - 1) << shift, (y - 1) << shift, 3 << shift, 3 << shift,
            mouse.x, mouse.y, 1, 1
        );
}

bool a_input_touchedRect(const int x, const int y, const int w, const int h)
{
    const int shift = mouse.shift;

    return mouse.tap
        && a_collide_boxes(
            x << shift, y << shift, w << shift, h << shift,
            mouse.x, mouse.y, 1, 1
        );
}

static void registerButton(const char* const name, const int code)
{
    Button* const bt = a_hash_get(buttonNames, name);

    if(bt == NULL) {
        Button* const b = malloc(sizeof(Button));

        b->name = a_str_dup(name);
        b->numCodes = 1;
        b->codes[0] = code;
        b->pressed = 0;
        b->oldEvent = 0;
        b->freshEvent = 0;

        a_list_addLast(buttonList, b);
        a_hash_add(buttonNames, name, b);
    } else if(bt->numCodes < MAX_CODES) {
        bt->codes[bt->numCodes++] = code;
    }
}

static void registerButtonFake(const char* const name)
{
    registerButton(name, -1);
}
