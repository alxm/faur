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
    typedef SDL_Scancode ASdlKeyCode;
#endif

typedef struct ASdlInputHeader {
    char* name;
} ASdlInputHeader;

typedef struct ASdlInputButton {
    ASdlInputHeader header;
    AInputButtonSource* logicalButton;
    union {
        ASdlKeyCode keyCode;
        uint8_t buttonIndex;
        int code;
    } code;
    bool lastStatePressed;
} ASdlInputButton;

typedef struct ASdlInputAnalog {
    ASdlInputHeader header;
    AInputAnalogSource* logicalAnalog;
    int axisIndex;
} ASdlInputAnalog;

typedef struct ASdlInputTouch {
    ASdlInputHeader header;
    AInputTouchSource* logicalTouch;
} ASdlInputTouch;

typedef struct ASdlInputController {
    SDL_Joystick* joystick;
    #if A_USE_LIB_SDL == 2
        SDL_GameController* controller;
    #endif
    ASdlJoystickId id;
    int numButtons;
    int numHats;
    int numAxes;
    AStrHash* buttons;
    AStrHash* axes;
    bool generic;
} ASdlInputController;

static AStrHash* g_keys;
static AStrHash* g_touchScreens;
static AList* g_controllers;
static uint32_t g_sdlFlags;

static void freeHeader(ASdlInputHeader* Header)
{
    free(Header->name);
    free(Header);
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

static const char* joystickName(ASdlInputController* Controller)
{
    #if A_USE_LIB_SDL == 1
        return SDL_JoystickName(Controller->id);
    #elif A_USE_LIB_SDL == 2
        return SDL_JoystickName(Controller->joystick);
    #endif
}

void a_sdl_input__init(void)
{
    #if A_USE_LIB_SDL == 1
        g_sdlFlags = SDL_INIT_JOYSTICK;
    #elif A_USE_LIB_SDL == 2
        g_sdlFlags = SDL_INIT_GAMECONTROLLER;
    #endif

    if(SDL_InitSubSystem(g_sdlFlags) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_keys = a_strhash_new();
    g_touchScreens = a_strhash_new();
    g_controllers = a_list_new();

    const int joysticksNum = SDL_NumJoysticks();
    a_out__message("Found %d controllers", joysticksNum);

    #if A_USE_LIB_SDL == 2
        int m = SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

        if(m < 0) {
            a_out__error("Cannot load gamepad mappings: %s", SDL_GetError());
        } else {
            a_out__message("Loaded %d gamepad mappings", m);
        }
    #endif

    for(int i = 0; i < joysticksNum; i++) {
        SDL_Joystick* joystick = NULL;

        #if A_USE_LIB_SDL == 2
            SDL_GameController* controller = NULL;

            if(SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);

                if(controller == NULL) {
                    a_out__error("SDL_GameControllerOpen(%d): %s",
                                 i,
                                 SDL_GetError());
                } else {
                    joystick = SDL_GameControllerGetJoystick(controller);

                    if(joystick == NULL) {
                        a_out__error("SDL_GameControllerGetJoystick: %s",
                                     SDL_GetError());

                        SDL_GameControllerClose(controller);
                        continue;
                    }
                }
            }
        #endif

        if(joystick == NULL) {
            joystick = SDL_JoystickOpen(i);

            if(joystick == NULL) {
                a_out__error("SDL_JoystickOpen(%d) failed: %s",
                             i,
                             SDL_GetError());
                continue;
            }
        }

        #if A_USE_LIB_SDL == 1
            ASdlJoystickId id = (uint8_t)i;
        #elif A_USE_LIB_SDL == 2
            ASdlJoystickId id = SDL_JoystickInstanceID(joystick);

            if(id < 0) {
                a_out__error("SDL_JoystickInstanceID: %s", SDL_GetError());

                if(controller) {
                    SDL_GameControllerClose(controller);
                } else {
                    SDL_JoystickClose(joystick);
                }

                continue;
            }
        #endif

        ASdlInputController* c = a_mem_malloc(sizeof(ASdlInputController));

        c->joystick = joystick;
        #if A_USE_LIB_SDL == 2
            c->controller = controller;
        #endif
        c->id = id;
        c->numButtons = SDL_JoystickNumButtons(c->joystick);
        c->numHats = SDL_JoystickNumHats(c->joystick);
        c->numAxes = SDL_JoystickNumAxes(c->joystick);
        c->buttons = a_strhash_new();
        c->axes = a_strhash_new();
        c->generic = false;

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
            const char* name = joystickName(c);

            // Check if this is one of the built-in nubs
            if(a_str_equal(name, "nub0")) {
                addAnalog(c->axes, "pandora.leftNubX", 0);
                addAnalog(c->axes, "pandora.leftNubY", 1);
                continue;
            } else if(a_str_equal(name, "nub1")) {
                addAnalog(c->axes, "pandora.rightNubX", 0);
                addAnalog(c->axes, "pandora.rightNubY", 1);
                continue;
            }
        #endif

        c->generic = true;

        #if A_USE_LIB_SDL == 2
            if(c->controller) {
                a_out__message("Found gamepad %s: %d buttons, %d axes, %d hats",
                               SDL_GameControllerName(controller),
                               c->numButtons,
                               c->numAxes,
                               c->numHats);

                static const char* buttonNames[SDL_CONTROLLER_BUTTON_MAX] = {
                    "gamepad.b.a",
                    "gamepad.b.b",
                    "gamepad.b.x",
                    "gamepad.b.y",
                    "gamepad.b.select",
                    "gamepad.b.guide",
                    "gamepad.b.start",
                    "gamepad.b.leftStick",
                    "gamepad.b.rightStick",
                    "gamepad.b.leftShoulder",
                    "gamepad.b.rightShoulder",
                    "gamepad.b.up",
                    "gamepad.b.down",
                    "gamepad.b.left",
                    "gamepad.b.right"
                };

                static const char* axisNames[SDL_CONTROLLER_AXIS_MAX] = {
                    "gamepad.a.leftX",
                    "gamepad.a.leftY",
                    "gamepad.a.rightX",
                    "gamepad.a.rightY",
                    "gamepad.a.leftTrigger",
                    "gamepad.a.rightTrigger"
                };

                for(SDL_GameControllerButton b = SDL_CONTROLLER_BUTTON_A;
                    b < SDL_CONTROLLER_BUTTON_MAX;
                    b++) {

                    SDL_GameControllerButtonBind bind =
                        SDL_GameControllerGetBindForButton(controller, b);

                    if(bind.bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
                        continue;
                    }

                    addButton(c->buttons, buttonNames[b], b);
                }

                for(SDL_GameControllerAxis a = SDL_CONTROLLER_AXIS_LEFTX;
                    a < SDL_CONTROLLER_AXIS_MAX;
                    a++) {

                    SDL_GameControllerButtonBind bind =
                        SDL_GameControllerGetBindForAxis(controller, a);

                    if(bind.bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
                        continue;
                    }

                    addAnalog(c->axes, axisNames[a], a);
                }
            } else {
        #endif

        a_out__message("Found gamepad %s: %d buttons, %d axes, %d hats",
                       joystickName(c),
                       c->numButtons,
                       c->numAxes,
                       c->numHats);

        for(int j = 0; j < c->numButtons; j++) {
            char name[16];
            snprintf(name, sizeof(name), "gamepad.b.%d", j);
            addButton(c->buttons, name, j);
        }

        for(int j = 0; j < c->numAxes; j++) {
            char name[16];
            snprintf(name, sizeof(name), "gamepad.a.%d", j);
            addAnalog(c->axes, name, j);
        }

        #if A_USE_LIB_SDL == 2
            }
        #endif

        if(c->numHats > 0 || c->numAxes >= 2) {
            // Declare virtual direction buttons
            addButton(c->buttons, "gamepad.b.up", -1);
            addButton(c->buttons, "gamepad.b.down", -1);
            addButton(c->buttons, "gamepad.b.left", -1);
            addButton(c->buttons, "gamepad.b.right", -1);
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

    #if A_USE_LIB_SDL == 1
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
    #elif A_USE_LIB_SDL == 2
        addKey("key.up", SDL_SCANCODE_UP);
        addKey("key.down", SDL_SCANCODE_DOWN);
        addKey("key.left", SDL_SCANCODE_LEFT);
        addKey("key.right", SDL_SCANCODE_RIGHT);
        addKey("key.z", SDL_SCANCODE_Z);
        addKey("key.x", SDL_SCANCODE_X);
        addKey("key.c", SDL_SCANCODE_C);
        addKey("key.v", SDL_SCANCODE_V);
        addKey("key.m", SDL_SCANCODE_M);
        addKey("key.enter", SDL_SCANCODE_RETURN);
        addKey("key.space", SDL_SCANCODE_SPACE);
        addKey("key.f1", SDL_SCANCODE_F1);
        addKey("key.f2", SDL_SCANCODE_F2);
        addKey("key.f3", SDL_SCANCODE_F3);
        addKey("key.f4", SDL_SCANCODE_F4);
        addKey("key.f5", SDL_SCANCODE_F5);
        addKey("key.f6", SDL_SCANCODE_F6);
        addKey("key.f7", SDL_SCANCODE_F7);
        addKey("key.f8", SDL_SCANCODE_F8);
        addKey("key.f9", SDL_SCANCODE_F9);
        addKey("key.f10", SDL_SCANCODE_F10);
        addKey("key.f11", SDL_SCANCODE_F11);
        addKey("key.f12", SDL_SCANCODE_F12);
        addKey("key.1", SDL_SCANCODE_1);
        addKey("key.0", SDL_SCANCODE_0);
    #endif

    addTouch("touchScreen");
}

void a_sdl_input__uninit(void)
{
    A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
        freeHeader(&k->header);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
        freeHeader(&t->header);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
            freeHeader(&b->header);
        }

        A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
            freeHeader(&a->header);
        }

        #if A_USE_LIB_SDL == 2
            if(c->controller) {
                SDL_GameControllerClose(c->controller);
                c->joystick = NULL;
            }
        #endif

        if(c->joystick) {
            SDL_JoystickClose(c->joystick);
        }

        a_strhash_free(c->buttons);
        a_strhash_free(c->axes);
        free(c);
    }

    a_strhash_free(g_keys);
    a_strhash_free(g_touchScreens);
    a_list_free(g_controllers);

    SDL_QuitSubSystem(g_sdlFlags);
}

void a_sdl_input__bind(void)
{
    A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
        k->logicalButton = a_input_button__newSource(k->header.name);
    }

    A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
        t->logicalTouch = a_input_touch__newSource(t->header.name);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        #if A_USE_LIB_SDL == 1
            a_controller__new(c->generic, false);
        #elif A_USE_LIB_SDL == 2
            a_controller__new(c->generic, c->controller != NULL);
        #endif

        A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
            b->logicalButton = a_input_button__newSource(b->header.name);
            a_controller__addButton(b->logicalButton, b->header.name);
        }

        A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
            a->logicalAnalog = a_input_analog__newSource(a->header.name);
            a_controller__addAnalog(a->logicalAnalog, a->header.name);
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

            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    a_state_exit();
                    break;
                }

                A_STRHASH_ITERATE(g_keys, ASdlInputButton*, k) {
                    #if A_USE_LIB_SDL == 1
                        if(k->code.keyCode == event.key.keysym.sym) {
                    #elif A_USE_LIB_SDL == 2
                        if(k->code.keyCode == event.key.keysym.scancode) {
                    #endif

                        a_input_button__setState(
                            k->logicalButton,
                            event.key.state == SDL_PRESSED);
                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    #if A_USE_LIB_SDL == 2
                        if(c->controller != NULL) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jbutton.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                        if(b->code.buttonIndex == event.jbutton.button) {
                            a_input_button__setState(
                                b->logicalButton,
                                event.jbutton.state == SDL_PRESSED);
                            break;
                        }
                    }

                    break;
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
                    #if A_USE_LIB_SDL == 2
                        if(c->controller != NULL) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jhat.which) {
                        continue;
                    }

                    ASdlInputButton* buttons[4] = {
                        a_strhash_get(c->buttons, "gamepad.b.up"),
                        a_strhash_get(c->buttons, "gamepad.b.down"),
                        a_strhash_get(c->buttons, "gamepad.b.left"),
                        a_strhash_get(c->buttons, "gamepad.b.right")
                    };

                    for(int i = 0; i < 4; i++, state >>= 1) {
                        ASdlInputButton* b = buttons[i];

                        if(state & 1) {
                            if(!b->lastStatePressed) {
                                b->lastStatePressed = true;
                                a_input_button__setState(b->logicalButton,
                                                         true);
                            }
                        } else {
                            if(b->lastStatePressed) {
                                b->lastStatePressed = false;
                                a_input_button__setState(b->logicalButton,
                                                         false);
                            }
                        }
                    }

                    break;
                }
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    #if A_USE_LIB_SDL == 2
                        if(c->controller != NULL) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jaxis.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
                        if(event.jaxis.axis == a->axisIndex) {
                            a_input_analog__setAxisValue(a->logicalAnalog,
                                                         event.jaxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;

#if A_USE_LIB_SDL == 2
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->controller == NULL || c->id != event.cbutton.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->buttons, ASdlInputButton*, b) {
                        if(b->code.buttonIndex == event.cbutton.button) {
                            a_input_button__setState(
                                b->logicalButton,
                                event.cbutton.state == SDL_PRESSED);
                            break;
                        }
                    }

                    break;
                }
            } break;

            case SDL_CONTROLLERAXISMOTION: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->controller == NULL || c->id != event.caxis.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->axes, ASdlInputAnalog*, a) {
                        if(event.caxis.axis == a->axisIndex) {
                            a_input_analog__setAxisValue(a->logicalAnalog,
                                                         event.caxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;
#endif

            case SDL_MOUSEMOTION: {
                A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
                    a_input_touch__addMotion(t->logicalTouch,
                                             event.button.x,
                                             event.button.y);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, ASdlInputTouch*, t) {
                            a_input_touch__setCoords(t->logicalTouch,
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
                            a_input_touch__setCoords(t->logicalTouch,
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
