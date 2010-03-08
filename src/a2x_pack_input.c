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

struct Button {
    char* name;
    int code;
    int pressed;
};

struct Touch {
    List* motion;
    int x;
    int y;
    int tap;
};

struct Input {
    List* buttons;
    char* name;
    int working;
};

struct Inputs {
    List* buttons;
    Touch mouse;
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        SDL_Joystick* joystick;
    #endif
};

static Inputs a__input;

#if A_PLATFORM_LINUXPC
    static Input* a__fullScreen;
#endif

static void a__input_button(const char* const name, const int code);

void a_input__set(void)
{
    a__input.buttons = a_list_set();

    #if A_PLATFORM_GP2X
        a__input_button("gp2x.Up", 0);
        a__input_button("gp2x.Down", 4);
        a__input_button("gp2x.Left", 2);
        a__input_button("gp2x.Right", 6);
        a__input_button("gp2x.UpLeft", 1);
        a__input_button("gp2x.UpRight", 7);
        a__input_button("gp2x.DownLeft", 3);
        a__input_button("gp2x.DownRight", 5);
        a__input_button("gp2x.L", 10);
        a__input_button("gp2x.R", 11);
        a__input_button("gp2x.A", 12);
        a__input_button("gp2x.B", 13);
        a__input_button("gp2x.X", 14);
        a__input_button("gp2x.Y", 15);
        a__input_button("gp2x.Start", 8);
        a__input_button("gp2x.Select", 9);
        a__input_button("gp2x.VolUp", 16);
        a__input_button("gp2x.VolDown", 17);
        a__input_button("gp2x.StickClick", 18);

        a__input.joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_WIZ
        a__input_button("wiz.Up", 0);
        a__input_button("wiz.Down", 4);
        a__input_button("wiz.Left", 2);
        a__input_button("wiz.Right", 6);
        a__input_button("wiz.UpLeft", 1);
        a__input_button("wiz.UpRight", 7);
        a__input_button("wiz.DownLeft", 3);
        a__input_button("wiz.DownRight", 5);
        a__input_button("wiz.L", 10);
        a__input_button("wiz.R", 11);
        a__input_button("wiz.A", 12);
        a__input_button("wiz.B", 13);
        a__input_button("wiz.X", 14);
        a__input_button("wiz.Y", 15);
        a__input_button("wiz.Menu", 8);
        a__input_button("wiz.Select", 9);
        a__input_button("wiz.VolUp", 16);
        a__input_button("wiz.VolDown", 17);

        a__input.joystick = SDL_JoystickOpen(0);
    #elif A_PLATFORM_LINUXPC
        // my laptop's arrow keys drowned in vodka, so I use IJKL instead
        a__input_button("pc.Up", SDLK_i); // SDLK_UP
        a__input_button("pc.Down", SDLK_k); // SDLK_DOWN
        a__input_button("pc.Left", SDLK_j); // SDLK_LEFT
        a__input_button("pc.Right", SDLK_l); // SDLK_RIGHT
        a__input_button("pc.z", SDLK_z);
        a__input_button("pc.x", SDLK_x);
        a__input_button("pc.c", SDLK_c);
        a__input_button("pc.v", SDLK_v);
        a__input_button("pc.Enter", SDLK_RETURN);
        a__input_button("pc.Space", SDLK_SPACE);
        a__input_button("pc.F1", SDLK_F1);
        a__input_button("pc.1", SDLK_1);
        a__input_button("pc.0", SDLK_0);
    #endif

    if(a2x_bool("trackMouse")) {
        a__input.mouse.motion = a_list_set();
    }

    #if A_PLATFORM_LINUXPC
        a__fullScreen = a_input_set("pc.F1");
    #endif
}

static void a__input_button(const char* const name, const int code)
{
    Button* const b = malloc(sizeof(Button));

    b->name = a_str_dup(name);
    b->code = code;
    b->pressed = 0;

    a_list_addLast(a__input.buttons, b);
}

void a_input__free(void)
{
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

    #if A_PLATFORM_LINUXPC
        a_input_free(a__fullScreen);
    #endif
}

void a__input_get(void)
{
    if(a2x_bool("trackMouse")) {
        a_list_freeContent(a__input.mouse.motion);
    }

    a__input.mouse.tap = 0;

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        int button;
        int action = -1;

        switch(event.type) {
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

            case SDL_QUIT: {
                a_state_exit();
            } break;

            default:break;
        }

        if(action != -1) {
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
        if(a_input_getUnpress(a__fullScreen)){
            a__screen_switchFull();
        }
    #endif
}

Input* a_input_set(const char* const names)
{
    Input* const i = malloc(sizeof(Input));
    StringTok* const t = a_str_makeTok(names, ", ");

    i->buttons = a_list_set();
    i->name = NULL;

    while(a_str_nextTok(t)) {
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

    i->working = a_list_size(i->buttons) > 0;

    a_str_freeTok(t);

    return i;
}

void a_input_free(Input* const i)
{
    a_list_free(i->buttons);
    free(i->name);

    free(i);
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
