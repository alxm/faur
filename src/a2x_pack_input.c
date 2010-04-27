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

#include "a2x_pack_input.p.h"
#include "a2x_pack_input.v.h"

typedef struct Button {
    char* name;
    int code;
    int pressed;
} Button;

typedef struct Touch {
    List* motion;
    int x;
    int y;
    int tap;
} Touch;

typedef struct Inputs {
    List* buttons;
    Touch mouse;
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        SDL_Joystick* joystick;
    #endif
} Inputs;

struct Input {
    List* buttons;
    char* name;
    int working;
};

static List* inputs;
static Inputs a__input;

#if A_PLATFORM_LINUXPC
    static Input* fullScreen;
    static Input* doubleRes;
#endif

static void registerButton(const char* const name, const int code);

void a_input__set(void)
{
    inputs = a_list_set();
    a__input.buttons = a_list_set();

    registerButton("all.any", -1);

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

        a__input.joystick = SDL_JoystickOpen(0);
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

        a__input.joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_LINUXPC
        // my laptop's arrow keys drowned in vodka, so I use IJKL instead
        registerButton("pc.Up", SDLK_i); // SDLK_UP
        registerButton("pc.Down", SDLK_k); // SDLK_DOWN
        registerButton("pc.Left", SDLK_j); // SDLK_LEFT
        registerButton("pc.Right", SDLK_l); // SDLK_RIGHT
        registerButton("pc.z", SDLK_z);
        registerButton("pc.x", SDLK_x);
        registerButton("pc.c", SDLK_c);
        registerButton("pc.v", SDLK_v);
        registerButton("pc.Enter", SDLK_RETURN);
        registerButton("pc.Space", SDLK_SPACE);
        registerButton("pc.F1", SDLK_F1);
        registerButton("pc.F2", SDLK_F2);
        registerButton("pc.1", SDLK_1);
        registerButton("pc.0", SDLK_0);
    #endif

    if(a2x_bool("trackMouse")) {
        a__input.mouse.motion = a_list_set();
    }

    #if A_PLATFORM_LINUXPC
        fullScreen = a_input_set("pc.F1");
        doubleRes = a_input_set("pc.F2");
    #endif
}

void a_input__free(void)
{
    while(a_list_iterate(inputs)) {
        a_input_free(a_list_current(inputs));
    }

    a_list_free(inputs);

    while(a_list_iterate(a__input.buttons)) {
        Button* const b = a_list_current(a__input.buttons);
        free(b->name);
        free(b);
    }

    a_list_free(a__input.buttons);

    if(a2x_bool("trackMouse")) {
        a_list_freeContent(a__input.mouse.motion);
    }

    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        SDL_JoystickClose(a__input.joystick);
    #endif
}

void a_input__get(void)
{
    if(a2x_bool("trackMouse")) {
        a_list_freeContent(a__input.mouse.motion);
    }

    a__input.mouse.tap = 0;

    Button* const any = a_list__first(a__input.buttons);
    any->pressed = 0;

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        int button;
        int action = -1;

        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
                case SDL_JOYBUTTONDOWN: {
                    button = event.jbutton.button;
                    action = 1;
                } break;

                case SDL_JOYBUTTONUP: {
                    button = event.jbutton.button;
                    action = 0;
                } break;
            #else
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

            case SDL_MOUSEMOTION: {
                a__input.mouse.x = event.button.x;
                a__input.mouse.y = event.button.y;

                if(a2x_bool("trackMouse")) {
                    Point* const p = malloc(sizeof(Point));

                    p->x = a__input.mouse.x;
                    p->y = a__input.mouse.y;

                    a_list_addLast(a__input.mouse.motion, p);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        a__input.mouse.tap = 1;
                        a__input.mouse.x = event.button.x;
                        a__input.mouse.y = event.button.y;
                    break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        a__input.mouse.x = event.button.x;
                        a__input.mouse.y = event.button.y;
                    break;
                }
            } break;

            default:break;
        }

        if(action != -1) {
            if(action == 1) {
                any->pressed = 1;
            }

            List* const l = a__input.buttons;

            while(a_list_iterate(l)) {
                Button* const b = a_list_current(l);

                if(b->code == button) {
                    b->pressed = action;
                }
            }
        }
    }

    #if A_PLATFORM_LINUXPC
        if(a_input_getUnpress(fullScreen)) {
            a_screen__switchFull();
        }

        if(a_input_getUnpress(doubleRes)) {
            a_screen__doubleRes();
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
        List* const l = a__input.buttons;
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

int a_input_get(Input* const i)
{
    List* const l = i->buttons;

    while(a_list_iterate(l)) {
        Button* const b = a_list_current(l);

        if(b->pressed) {
            a_list_reset(l);
            return 1;
        }
    }

    return 0;
}

void a_input_unpress(Input* const i)
{
    List* const l = i->buttons;

    while(a_list_iterate(l)) {
        Button* const b = a_list_current(l);
        b->pressed = 0;
    }
}

int a_input_getUnpress(Input* const i)
{
    if(a_input_get(i)) {
        a_input_unpress(i);
        return 1;
    }

    return 0;
}

char* a_input_name(const Input* const i)
{
    return i->name;
}

int a_input_working(const Input* const i)
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

int a_input_touchedPoint(const int x, const int y)
{
    return a__input.mouse.tap
        && a_collide_rects(
            ((Rect){x - 1, y - 1, 3, 3}),
            ((Rect){a__input.mouse.x, a__input.mouse.y, 1, 1})
        );
}

int a_input_touchedRect(const int x, const int y, const int w, const int h)
{
    return a__input.mouse.tap
        && a_collide_rects(
            ((Rect){x, y, w, h}),
            ((Rect){a__input.mouse.x, a__input.mouse.y, 1, 1})
        );
}

static void registerButton(const char* const name, const int code)
{
    Button* const b = malloc(sizeof(Button));

    b->name = a_str_dup(name);
    b->code = code;
    b->pressed = 0;

    a_list_addLast(a__input.buttons, b);
}
