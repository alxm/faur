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

#if A_USE_LIB_SDL == 1
    typedef uint8_t ASdlJoystickId;
    typedef SDLKey ASdlKeyCode;
#elif A_USE_LIB_SDL == 2
    typedef SDL_JoystickID ASdlJoystickId;
    typedef SDL_Keycode ASdlKeyCode;
#endif

typedef struct ASdlInputHeader {
    char* name;
} ASdlInputHeader;

typedef struct ASdlInputButton {
    ASdlInputHeader header;
    AInputSourceButton* logicalButton;
    union {
        ASdlKeyCode keyCode;
        uint8_t buttonIndex;
        int code;
    } code;
    bool lastStatePressed;
} ASdlInputButton;

typedef struct ASdlInputAnalog {
    ASdlInputHeader header;
    AInputSourceAnalog* logicalAnalog;
    int axisIndex;
} ASdlInputAnalog;

typedef struct ASdlInputTouch {
    ASdlInputHeader header;
    AInputSourceTouch* logicalTouch;
} ASdlInputTouch;

typedef struct ASdlInputController {
    SDL_Joystick* joystick;
    ASdlJoystickId id;
    int numButtons;
    int numHats;
    int numAxes;
    AStrHash* buttons;
    AStrHash* axes;
} ASdlInputController;

static AStrHash* g_keys;
static AStrHash* g_touchScreens;
static AList* g_controllers;

static void freeHeader(ASdlInputHeader* Header)
{
    free(Header->name);
}

static void addKey(const char* Name, int Code)
{
    if(a_strhash_contains(g_keys, Name)) {
        a_out__error("Key '%s' already defined", Name);
        return;
    }

    ASdlInputButton* k = a_mem_malloc(sizeof(ASdlInputButton));

    k->header.name = a_str_dup(Name);
    k->code.code = Code;
    k->lastStatePressed = false;

    a_strhash_add(g_keys, Name, k);
}

static void addButton(AStrHash* ButtonsCollection, const char* Name, int Code)
{
    if(a_strhash_contains(ButtonsCollection, Name)) {
        a_out__error("Button '%s' already defined", Name);
        return;
    }

    ASdlInputButton* b = a_mem_malloc(sizeof(ASdlInputButton));

    b->header.name = a_str_dup(Name);
    b->code.code = Code;
    b->lastStatePressed = false;

    a_strhash_add(ButtonsCollection, Name, b);
}

static void addAnalog(AStrHash* AxesCollection, const char* Name, int AxisIndex)
{
    if(a_strhash_contains(AxesCollection, Name)) {
        a_out__error("Analog '%s' is already defined", Name);
        return;
    }

    ASdlInputAnalog* a = a_mem_malloc(sizeof(ASdlInputAnalog));

    a->header.name = a_str_dup(Name);
    a->axisIndex = AxisIndex;

    a_strhash_add(AxesCollection, Name, a);
}

static void addTouch(const char* Name)
{
    if(a_strhash_contains(g_touchScreens, Name)) {
        a_out__error("Touchscreen '%s' is already defined", Name);
        return;
    }

    ASdlInputTouch* t = a_mem_malloc(sizeof(ASdlInputTouch));

    t->header.name = a_str_dup(Name);

    a_strhash_add(g_touchScreens, Name, t);
}

void a_sdl_input__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_keys = a_strhash_new();
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

        #if A_USE_LIB_SDL == 1
            ASdlJoystickId id = (uint8_t)i;
        #elif A_USE_LIB_SDL == 2
            ASdlJoystickId id = SDL_JoystickInstanceID(joystick);

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
        c->id = id;
        c->numButtons = SDL_JoystickNumButtons(c->joystick);
        c->numHats = SDL_JoystickNumHats(c->joystick);
        c->numAxes = SDL_JoystickNumAxes(c->joystick);
        c->buttons = a_strhash_new();
        c->axes = a_strhash_new();

        a_list_addLast(g_controllers, c);

        #if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
            if(i == 0) {
                // Joystick 0 is the built-in controls on these platforms
                #if A_PLATFORM_GP2X
                    addButton(c->buttons, "gp2x.up", 0);
                    addButton(c->buttons, "gp2x.down", 4);
                    addButton(c->buttons, "gp2x.left", 2);
                    addButton(c->buttons, "gp2x.right", 6);
                    addButton(c->buttons, "gp2x.upleft", 1);
                    addButton(c->buttons, "gp2x.upright", 7);
                    addButton(c->buttons, "gp2x.downleft", 3);
                    addButton(c->buttons, "gp2x.downright", 5);
                    addButton(c->buttons, "gp2x.l", 10);
                    addButton(c->buttons, "gp2x.r", 11);
                    addButton(c->buttons, "gp2x.a", 12);
                    addButton(c->buttons, "gp2x.b", 13);
                    addButton(c->buttons, "gp2x.x", 14);
                    addButton(c->buttons, "gp2x.y", 15);
                    addButton(c->buttons, "gp2x.start", 8);
                    addButton(c->buttons, "gp2x.select", 9);
                    addButton(c->buttons, "gp2x.volup", 16);
                    addButton(c->buttons, "gp2x.voldown", 17);
                    addButton(c->buttons, "gp2x.stickclick", 18);
                #elif A_PLATFORM_WIZ
                    addButton(c->buttons, "wiz.up", 0);
                    addButton(c->buttons, "wiz.down", 4);
                    addButton(c->buttons, "wiz.left", 2);
                    addButton(c->buttons, "wiz.right", 6);
                    addButton(c->buttons, "wiz.upleft", 1);
                    addButton(c->buttons, "wiz.upright", 7);
                    addButton(c->buttons, "wiz.downleft", 3);
                    addButton(c->buttons, "wiz.downright", 5);
                    addButton(c->buttons, "wiz.l", 10);
                    addButton(c->buttons, "wiz.r", 11);
                    addButton(c->buttons, "wiz.a", 12);
                    addButton(c->buttons, "wiz.b", 13);
                    addButton(c->buttons, "wiz.x", 14);
                    addButton(c->buttons, "wiz.y", 15);
                    addButton(c->buttons, "wiz.menu", 8);
                    addButton(c->buttons, "wiz.select", 9);
                    addButton(c->buttons, "wiz.volup", 16);
                    addButton(c->buttons, "wiz.voldown", 17);
                #elif A_PLATFORM_CAANOO
                    addButton(c->buttons, "caanoo.up", -1);
                    addButton(c->buttons, "caanoo.down", -1);
                    addButton(c->buttons, "caanoo.left", -1);
                    addButton(c->buttons, "caanoo.right", -1);
                    addButton(c->buttons, "caanoo.l", 4);
                    addButton(c->buttons, "caanoo.r", 5);
                    addButton(c->buttons, "caanoo.a", 0);
                    addButton(c->buttons, "caanoo.b", 2);
                    addButton(c->buttons, "caanoo.x", 1);
                    addButton(c->buttons, "caanoo.y", 3);
                    addButton(c->buttons, "caanoo.home", 6);
                    addButton(c->buttons, "caanoo.hold", 7);
                    addButton(c->buttons, "caanoo.1", 8);
                    addButton(c->buttons, "caanoo.2", 9);
                    addAnalog(c->axes, "caanoo.stickX", 0);
                    addAnalog(c->axes, "caanoo.stickY", 1);
                #endif

                continue;
            }
        #elif A_PLATFORM_PANDORA
            #if A_USE_LIB_SDL == 1
                const char* joystickName = SDL_JoystickName(c->id);
            #elif A_USE_LIB_SDL == 2
                const char* joystickName = SDL_JoystickName(c->joystick);
            #endif

            // Check if this is one of the built-in nubs
            if(a_str_equal(joystickName, "nub0")) {
                addAnalog(c->axes, "pandora.leftNubX", 0);
                addAnalog(c->axes, "pandora.leftNubY", 1);
                continue;
            } else if(a_str_equal(joystickName, "nub1")) {
                addAnalog(c->axes, "pandora.rightNubX", 0);
                addAnalog(c->axes, "pandora.rightNubY", 1);
                continue;
            }
        #endif

        for(int j = 0; j < c->numButtons; j++) {
            char name[32];
            snprintf(name, sizeof(name), "controller.b%d", j);
            addButton(c->buttons, name, j);
        }

        for(int j = 0; j < c->numAxes; j++) {
            char name[32];
            snprintf(name, sizeof(name), "controller.axis%d", j);
            addAnalog(c->axes, name, j);
        }

        if(c->numHats > 0) {
            addButton(c->buttons, "controller.up", -1);
            addButton(c->buttons, "controller.down", -1);
            addButton(c->buttons, "controller.left", -1);
            addButton(c->buttons, "controller.right", -1);
        }
    }

    #if A_PLATFORM_PANDORA
        // Because these are defined before the generic keys, they
        // will take precedence in the a_sdl_input__get event loop.
        addKey("pandora.up", SDLK_UP);
        addKey("pandora.down", SDLK_DOWN);
        addKey("pandora.left", SDLK_LEFT);
        addKey("pandora.right", SDLK_RIGHT);
        addKey("pandora.l", SDLK_RSHIFT);
        addKey("pandora.r", SDLK_RCTRL);
        addKey("pandora.a", SDLK_HOME);
        addKey("pandora.b", SDLK_END);
        addKey("pandora.x", SDLK_PAGEDOWN);
        addKey("pandora.y", SDLK_PAGEUP);
        addKey("pandora.start", SDLK_LALT);
        addKey("pandora.select", SDLK_LCTRL);
    #endif

    addKey("key.up", SDLK_UP);
    addKey("key.down", SDLK_DOWN);
    addKey("key.left", SDLK_LEFT);
    addKey("key.right", SDLK_RIGHT);
    addKey("key.z", SDLK_z);
    addKey("key.x", SDLK_x);
    addKey("key.c", SDLK_c);
    addKey("key.v", SDLK_v);
    addKey("key.m", SDLK_m);
    addKey("key.enter", SDLK_RETURN);
    addKey("key.space", SDLK_SPACE);
    addKey("key.f1", SDLK_F1);
    addKey("key.f2", SDLK_F2);
    addKey("key.f3", SDLK_F3);
    addKey("key.f4", SDLK_F4);
    addKey("key.f5", SDLK_F5);
    addKey("key.f6", SDLK_F6);
    addKey("key.f7", SDLK_F7);
    addKey("key.f8", SDLK_F8);
    addKey("key.f9", SDLK_F9);
    addKey("key.f10", SDLK_F10);
    addKey("key.f11", SDLK_F11);
    addKey("key.f12", SDLK_F12);
    addKey("key.1", SDLK_1);
    addKey("key.0", SDLK_0);

    addTouch("touchScreen");
}

void a_sdl_input__uninit(void)
{
    A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
        freeHeader(&k->header);
        free(k);
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

        A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
            freeHeader(&a->header);
            free(a);
        }

        SDL_JoystickClose(c->joystick);
        a_strhash_free(c->buttons);
        a_strhash_free(c->axes);
        free(c);
    }

    a_strhash_free(g_keys);
    a_strhash_free(g_touchScreens);
    a_list_free(g_controllers);

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void a_sdl_input__bind(void)
{
    A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
        k->logicalButton = a_input__newSourceButton(k->header.name);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
        t->logicalTouch = a_input__newSourceTouch(t->header.name);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        a_input__newController();

        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
            b->logicalButton = a_input__newSourceButton(b->header.name);
        }

        A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
            a->logicalAnalog = a_input__newSourceAnalog(a->header.name);
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

                A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
                    if(k->code.keyCode == event.key.keysym.sym) {
                        a_input__button_setState(k->logicalButton, true);
                        break;
                    }
                }
            } break;

            case SDL_KEYUP: {
                A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
                    if(k->code.keyCode == event.key.keysym.sym) {
                        a_input__button_setState(k->logicalButton, false);
                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jbutton.which) {
                        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                            if(b->code.buttonIndex == event.jbutton.button) {
                                a_input__button_setState(b->logicalButton,
                                                         true);
                                break;
                            }
                        }

                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONUP: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jbutton.which) {
                        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                            if(b->code.buttonIndex == event.jbutton.button) {
                                a_input__button_setState(b->logicalButton,
                                                         false);
                                break;
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
                                    a_input__button_setState(b->logicalButton,
                                                             true);
                                }
                            } else {
                                if(b->lastStatePressed) {
                                    b->lastStatePressed = false;
                                    a_input__button_setState(b->logicalButton,
                                                             false);
                                }
                            }
                        }

                        break;
                    }
                }
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->id == event.jaxis.which) {
                        A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
                            if(event.jaxis.axis == a->axisIndex) {
                                a_input__analog_setAxisValue(a->logicalAnalog,
                                                             event.jaxis.value);
                                break;
                            }
                        }

                        break;
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
