/*
    Copyright 2010, 2016, 2017 Alex Margarit

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

#include "a2x_pack_sdl_input.v.h"

#include <SDL.h>

typedef struct ASdlInputHeader {
    char* name;
    int device_index;
} ASdlInputHeader;

typedef struct ASdlInputButton {
    ASdlInputHeader header;
    AInputButton* logicalButton;
    union {
        #if A_USE_LIB_SDL == 1
            SDLKey key;
        #elif A_USE_LIB_SDL == 2
            SDL_Keycode key;
        #endif
        uint8_t button;
        int code;
    } code;
    bool lastStatePressed;
} ASdlInputButton;

typedef struct ASdlInputAnalog {
    ASdlInputHeader header;
    AInputAnalog* logicalAnalog;
    int xaxis_index;
    int yaxis_index;
} ASdlInputAnalog;

typedef struct ASdlInputTouch {
    ASdlInputHeader header;
    AInputTouch* logicalTouch;
} ASdlInputTouch;

typedef struct ASdlInputController {
    SDL_Joystick* joystick;
    #if A_USE_LIB_SDL == 1
        uint8_t id;
    #elif A_USE_LIB_SDL == 2
        SDL_JoystickID id;
    #endif
    int numButtons;
    int numHats;
    AStrHash* buttons;
} ASdlInputController;

static AStrHash* g_buttons;
static AStrHash* g_analogs;
static AStrHash* g_touchScreens;
static AList* g_controllers;

static void freeHeader(ASdlInputHeader* Header)
{
    free(Header->name);
}

static void addButton(const char* Name, int Code)
{
    ASdlInputButton* b = a_strhash_get(g_buttons, Name);

    if(b) {
        a_out__error("Button '%s' already defined", Name);
        return;
    }

    b = a_mem_malloc(sizeof(ASdlInputButton));

    b->header.name = a_str_dup(Name);
    b->code.code = Code;
    b->lastStatePressed = false;

    a_strhash_add(g_buttons, Name, b);
}

#if A_PLATFORM_CAANOO || A_PLATFORM_PANDORA
    static void addAnalog(const char* Name, int DeviceIndex, char* DeviceName, int XAxisIndex, int YAxisIndex)
    {
        if(DeviceIndex == -1 && DeviceName == NULL) {
            a_out__error("Inputs must specify device index or name");
            return;
        }

        ASdlInputAnalog* a = a_strhash_get(g_analogs, Name);

        if(a) {
            a_out__error("Analog '%s' is already defined", Name);
            return;
        }

        a = a_mem_malloc(sizeof(ASdlInputAnalog));

        a->header.name = a_str_dup(Name);
        a->header.device_index = DeviceIndex;
        a->xaxis_index = XAxisIndex;
        a->yaxis_index = YAxisIndex;

        // check if we requested a specific device by Name
        if(DeviceName) {
            A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                if(a_str_equal(DeviceName, SDL_JoystickName(c->id))) {
                    a->header.device_index = c->id;
                    break;
                }
            }
        }

        a_strhash_add(g_analogs, Name, a);
    }
#endif

static void addTouch(const char* Name)
{
    ASdlInputTouch* t = a_strhash_get(g_touchScreens, Name);

    if(t) {
        a_out__error("Touchscreen '%s' is already defined", Name);
        return;
    }

    t = a_mem_malloc(sizeof(ASdlInputTouch));

    t->header.name = a_str_dup(Name);

    a_strhash_add(g_touchScreens, Name, t);
}

void a_sdl_input__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_buttons = a_strhash_new();
    g_analogs = a_strhash_new();
    g_touchScreens = a_strhash_new();
    g_controllers = a_list_new();

    const int joysticksNum = SDL_NumJoysticks();
    a_out__message("Found %d controllers", joysticksNum);

    for(int i = 0; i < joysticksNum; i++) {
        SDL_Joystick* joystick = SDL_JoystickOpen(i);

        if(joystick == NULL) {
            a_out__error("SDL_JoystickOpen(%d) failed: %s",
                         i,
                         SDL_GetError());
            continue;
        }

        #if A_USE_LIB_SDL == 2
            SDL_JoystickID id = SDL_JoystickInstanceID(joystick);

            if(id < 0) {
                a_out__error("SDL_JoystickInstanceID(%d) failed: %s",
                             i,
                             SDL_GetError());
                SDL_JoystickClose(joystick);
                continue;
            }
        #endif

        ASdlInputController* c = a_mem_malloc(sizeof(ASdlInputController));

        c->joystick = joystick;
        #if A_USE_LIB_SDL == 1
            c->id = (uint8_t)i;
        #elif A_USE_LIB_SDL == 2
            c->id = id;
        #endif
        c->numButtons = SDL_JoystickNumButtons(c->joystick);
        c->numHats = SDL_JoystickNumHats(c->joystick);
        c->buttons = a_strhash_new();

        a_list_addLast(g_controllers, c);

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
            if(i == 0) {
                // Joystick 0 is the built-in controls on these platforms
                continue;
            }
        #elif A_PLATFORM_PANDORA
            if(i < 2) {
                // Joysticks 0 and 1 are the built-in nubs
                continue;
            }
        #endif

        AStrHash* savedButtons = g_buttons;
        g_buttons = c->buttons;

        for(int j = 0; j < c->numButtons; j++) {
            char name[32];
            snprintf(name, sizeof(name), "controller.b%d", j);
            addButton(name, j);
        }

        if(c->numHats > 0) {
            addButton("controller.up", -1);
            addButton("controller.down", -1);
            addButton("controller.left", -1);
            addButton("controller.right", -1);
        }

        g_buttons = savedButtons;
    }

    #if A_PLATFORM_GP2X
        addButton("gp2x.up", 0);
        addButton("gp2x.down", 4);
        addButton("gp2x.left", 2);
        addButton("gp2x.right", 6);
        addButton("gp2x.upleft", 1);
        addButton("gp2x.upright", 7);
        addButton("gp2x.downleft", 3);
        addButton("gp2x.downright", 5);
        addButton("gp2x.l", 10);
        addButton("gp2x.r", 11);
        addButton("gp2x.a", 12);
        addButton("gp2x.b", 13);
        addButton("gp2x.x", 14);
        addButton("gp2x.y", 15);
        addButton("gp2x.start", 8);
        addButton("gp2x.select", 9);
        addButton("gp2x.volup", 16);
        addButton("gp2x.voldown", 17);
        addButton("gp2x.stickclick", 18);
        addTouch("gp2x.touch");
    #elif A_PLATFORM_WIZ
        addButton("wiz.up", 0);
        addButton("wiz.down", 4);
        addButton("wiz.left", 2);
        addButton("wiz.right", 6);
        addButton("wiz.upleft", 1);
        addButton("wiz.upright", 7);
        addButton("wiz.downleft", 3);
        addButton("wiz.downright", 5);
        addButton("wiz.l", 10);
        addButton("wiz.r", 11);
        addButton("wiz.a", 12);
        addButton("wiz.b", 13);
        addButton("wiz.x", 14);
        addButton("wiz.y", 15);
        addButton("wiz.menu", 8);
        addButton("wiz.select", 9);
        addButton("wiz.volup", 16);
        addButton("wiz.voldown", 17);
        addTouch("wiz.touch");
    #elif A_PLATFORM_CAANOO
        addButton("caanoo.up", -1);
        addButton("caanoo.down", -1);
        addButton("caanoo.left", -1);
        addButton("caanoo.right", -1);
        addButton("caanoo.l", 4);
        addButton("caanoo.r", 5);
        addButton("caanoo.a", 0);
        addButton("caanoo.b", 2);
        addButton("caanoo.x", 1);
        addButton("caanoo.y", 3);
        addButton("caanoo.home", 6);
        addButton("caanoo.hold", 7);
        addButton("caanoo.1", 8);
        addButton("caanoo.2", 9);
        addAnalog("caanoo.stick", 0, NULL, 0, 1);
        addTouch("caanoo.touch");
    #elif A_PLATFORM_PANDORA
        addButton("pandora.up", SDLK_UP);
        addButton("pandora.down", SDLK_DOWN);
        addButton("pandora.left", SDLK_LEFT);
        addButton("pandora.right", SDLK_RIGHT);
        addButton("pandora.l", SDLK_RSHIFT);
        addButton("pandora.r", SDLK_RCTRL);
        addButton("pandora.a", SDLK_HOME);
        addButton("pandora.b", SDLK_END);
        addButton("pandora.x", SDLK_PAGEDOWN);
        addButton("pandora.y", SDLK_PAGEUP);
        addButton("pandora.start", SDLK_LALT);
        addButton("pandora.select", SDLK_LCTRL);
        addTouch("pandora.touch");
        addAnalog("pandora.nub1", -1, "nub0", 0, 1);
        addAnalog("pandora.nub2", -1, "nub1", 0, 1);
        addButton("pandora.m", SDLK_m);
        addButton("pandora.s", SDLK_s);
    #elif A_PLATFORM_LINUXPC || A_PLATFORM_MINGW
        addButton("pc.up", SDLK_UP);
        addButton("pc.down", SDLK_DOWN);
        addButton("pc.left", SDLK_LEFT);
        addButton("pc.right", SDLK_RIGHT);
        addButton("pc.z", SDLK_z);
        addButton("pc.x", SDLK_x);
        addButton("pc.c", SDLK_c);
        addButton("pc.v", SDLK_v);
        addButton("pc.m", SDLK_m);
        addButton("pc.enter", SDLK_RETURN);
        addButton("pc.space", SDLK_SPACE);
        addButton("pc.f1", SDLK_F1);
        addButton("pc.f2", SDLK_F2);
        addButton("pc.f3", SDLK_F3);
        addButton("pc.f4", SDLK_F4);
        addButton("pc.f5", SDLK_F5);
        addButton("pc.f6", SDLK_F6);
        addButton("pc.f7", SDLK_F7);
        addButton("pc.f8", SDLK_F8);
        addButton("pc.f9", SDLK_F9);
        addButton("pc.f10", SDLK_F10);
        addButton("pc.f11", SDLK_F11);
        addButton("pc.f12", SDLK_F12);
        addButton("pc.1", SDLK_1);
        addButton("pc.0", SDLK_0);
        addTouch("pc.mouse");
    #endif
}

void a_sdl_input__uninit(void)
{
    A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
        freeHeader(&b->header);
        free(b);
    }

    A_STRHASH_ITERATE(g_analogs, ASdlInputAnalog*, a) {
        freeHeader(&a->header);
        free(a);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
        freeHeader(&t->header);
        free(t);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
            freeHeader(&b->header);
            free(b);
        }

        SDL_JoystickClose(c->joystick);
        a_strhash_free(c->buttons);
        free(c);
    }

    a_strhash_free(g_buttons);
    a_strhash_free(g_analogs);
    a_strhash_free(g_touchScreens);
    a_list_free(g_controllers);

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void a_sdl_input__bind(void)
{
    A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
        b->logicalButton = a_input__newButton(b->header.name);
    }

    A_STRHASH_ITERATE(g_analogs, ASdlInputAnalog*, a) {
        a->logicalAnalog = a_input__newAnalog(a->header.name);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
        t->logicalTouch = a_input__newTouch(t->header.name);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        a_input__newController();

        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
            b->logicalButton = a_input__newButton(b->header.name);
        }
    }
}

void a_sdl_input__get(void)
{
    for(SDL_Event event; SDL_PollEvent(&event); ) {
        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            case SDL_KEYDOWN: {
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    a_state_exit();
                    break;
                }

                A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
                    if(b->code.key == event.key.keysym.sym) {
                        a_input__button_setState(b->logicalButton, true);
                    }
                }
            } break;

            case SDL_KEYUP: {
                A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
                    if(b->code.key == event.key.keysym.sym) {
                        a_input__button_setState(b->logicalButton, false);
                    }
                }
            } break;

            case SDL_JOYBUTTONDOWN: {
                A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
                    if(b->code.button == event.jbutton.button) {
                        a_input__button_setState(b->logicalButton, true);
                    }
                }

                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jbutton.which) {
                        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                            if(b->code.button == event.jbutton.button) {
                                a_input__button_setState(b->logicalButton, true);
                            }
                        }

                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONUP: {
                A_STRHASH_ITERATE(g_buttons, ASdlInputButton*, b) {
                    if(b->code.button == event.jbutton.button) {
                        a_input__button_setState(b->logicalButton, false);
                    }
                }

                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jbutton.which) {
                        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                            if(b->code.button == event.jbutton.button) {
                                a_input__button_setState(b->logicalButton, false);
                            }
                        }

                        break;
                    }
                }
            } break;

            case SDL_JOYHATMOTION: {
                unsigned state = 0;
                #define UP_PRESSED    (1 << 0)
                #define DOWN_PRESSED  (1 << 1)
                #define LEFT_PRESSED  (1 << 2)
                #define RIGHT_PRESSED (1 << 3)

                switch(event.jhat.value) {
                    case SDL_HAT_UP: {
                        state = UP_PRESSED;
                    } break;

                    case SDL_HAT_DOWN: {
                        state = DOWN_PRESSED;
                    } break;

                    case SDL_HAT_LEFT: {
                        state = LEFT_PRESSED;
                    } break;

                    case SDL_HAT_RIGHT: {
                        state = RIGHT_PRESSED;
                    } break;

                    case SDL_HAT_LEFTUP: {
                        state = LEFT_PRESSED | UP_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTUP: {
                        state = RIGHT_PRESSED | UP_PRESSED;
                    } break;

                    case SDL_HAT_LEFTDOWN: {
                        state = LEFT_PRESSED | DOWN_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTDOWN: {
                        state = RIGHT_PRESSED | DOWN_PRESSED;
                    } break;
                }

                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jhat.which) {
                        ASdlInputButton* buttons[4] = {
                            a_strhash_get(c->buttons, "controller.up"),
                            a_strhash_get(c->buttons, "controller.down"),
                            a_strhash_get(c->buttons, "controller.left"),
                            a_strhash_get(c->buttons, "controller.right")
                        };

                        for(int i = 0; i < 4; i++, state >>= 1) {
                            ASdlInputButton* b = buttons[i];

                            if(state & 1) {
                                if(!b->lastStatePressed) {
                                    b->lastStatePressed = true;
                                    a_input__button_setState(b->logicalButton, true);
                                }
                            } else {
                                if(b->lastStatePressed) {
                                    b->lastStatePressed = false;
                                    a_input__button_setState(b->logicalButton, false);
                                }
                            }
                        }

                        break;
                    }
                }
            } break;

            case SDL_JOYAXISMOTION: {
                A_STRHASH_ITERATE(g_analogs, ASdlInputAnalog*, a) {
                    if(a->header.device_index == event.jaxis.which) {
                        if(event.jaxis.axis == a->xaxis_index) {
                            a_input__analog_setXAxis(a->logicalAnalog,
                                                     event.jaxis.value);
                        } else if(event.jaxis.axis == a->yaxis_index) {
                            a_input__analog_setYAxis(a->logicalAnalog,
                                                     event.jaxis.value);
                        }
                    }
                }
            } break;

            case SDL_MOUSEMOTION: {
                A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
                    a_input__touch_addMotion(t->logicalTouch,
                                             event.button.x,
                                             event.button.y);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
                            a_input__touch_setCoords(t->logicalTouch,
                                                     event.button.x,
                                                     event.button.y,
                                                     true);
                        }
                    } break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
                            a_input__touch_setCoords(t->logicalTouch,
                                                     event.button.x,
                                                     event.button.y,
                                                     false);
                        }
                    } break;
                }
            } break;

            default:break;
        }
    }
}
