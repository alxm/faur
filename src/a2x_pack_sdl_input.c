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

static void addKey(const char* Name, const char* Id, int Code)
{
    if(a_strhash_contains(g_keys, Id)) {
        a_out__error("Key '%s' already defined", Id);
        return;
    }

    ASdlInputButton* k = a_mem_malloc(sizeof(ASdlInputButton));

    k->header.name = a_str_merge("[", Name, "]", NULL);
    k->code.code = Code;
    k->lastStatePressed = false;

    a_strhash_add(g_keys, Id, k);
}

static void addButton(AStrHash* ButtonsCollection, const char* Name, const char* Id, int Code)
{
    if(a_strhash_contains(ButtonsCollection, Id)) {
        a_out__error("Button '%s' already defined", Id);
        return;
    }

    ASdlInputButton* b = a_mem_malloc(sizeof(ASdlInputButton));

    b->header.name = a_str_merge("(", Name, ")", NULL);
    b->code.code = Code;
    b->lastStatePressed = false;

    a_strhash_add(ButtonsCollection, Id, b);
}

static void addAnalog(AStrHash* AxesCollection, const char* Id, int AxisIndex)
{
    if(a_strhash_contains(AxesCollection, Id)) {
        a_out__error("Analog '%s' is already defined", Id);
        return;
    }

    ASdlInputAnalog* a = a_mem_malloc(sizeof(ASdlInputAnalog));

    a->header.name = a_str_dup(Id);
    a->axisIndex = AxisIndex;

    a_strhash_add(AxesCollection, Id, a);
}

static void addTouch(const char* Id)
{
    if(a_strhash_contains(g_touchScreens, Id)) {
        a_out__error("Touchscreen '%s' is already defined", Id);
        return;
    }

    ASdlInputTouch* t = a_mem_malloc(sizeof(ASdlInputTouch));

    t->header.name = a_str_dup(Id);

    a_strhash_add(g_touchScreens, Id, t);
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
        if(joysticksNum > 0) {
            const char* mapFile = a_settings_getString("input.mapfile");
            int mapsNum = SDL_GameControllerAddMappingsFromFile(mapFile);

            if(mapsNum < 0) {
                a_out__error("Cannot load mappings from %s: %s",
                             mapFile,
                             SDL_GetError());
            } else {
                a_out__message("Loaded %d mappings from %s", mapsNum, mapFile);
            }
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
                    addButton(c->buttons, "Up", "gp2x.up", 0);
                    addButton(c->buttons, "Down", "gp2x.down", 4);
                    addButton(c->buttons, "Left", "gp2x.left", 2);
                    addButton(c->buttons, "Right", "gp2x.right", 6);
                    addButton(c->buttons, "Up-Left", "gp2x.upLeft", 1);
                    addButton(c->buttons, "Up-Right", "gp2x.upRight", 7);
                    addButton(c->buttons, "Down-Left", "gp2x.downLeft", 3);
                    addButton(c->buttons, "Down-Right", "gp2x.downRight", 5);
                    addButton(c->buttons, "L", "gp2x.l", 10);
                    addButton(c->buttons, "R", "gp2x.r", 11);
                    addButton(c->buttons, "A", "gp2x.a", 12);
                    addButton(c->buttons, "B", "gp2x.b", 13);
                    addButton(c->buttons, "X", "gp2x.x", 14);
                    addButton(c->buttons, "Y", "gp2x.y", 15);
                    addButton(c->buttons, "Start", "gp2x.start", 8);
                    addButton(c->buttons, "Select", "gp2x.select", 9);
                    addButton(c->buttons, "Vol-Up", "gp2x.volUp", 16);
                    addButton(c->buttons, "Vol-Down", "gp2x.volDown", 17);
                    addButton(c->buttons, "Stick-Click", "gp2x.stickClick", 18);
                #elif A_PLATFORM_WIZ
                    addButton(c->buttons, "Up", "wiz.up", 0);
                    addButton(c->buttons, "Down", "wiz.down", 4);
                    addButton(c->buttons, "Left", "wiz.left", 2);
                    addButton(c->buttons, "Right", "wiz.right", 6);
                    addButton(c->buttons, "Up-Left", "wiz.upLeft", 1);
                    addButton(c->buttons, "Up-Right", "wiz.upRight", 7);
                    addButton(c->buttons, "Down-Left", "wiz.downLeft", 3);
                    addButton(c->buttons, "Down-Right", "wiz.downRight", 5);
                    addButton(c->buttons, "L", "wiz.l", 10);
                    addButton(c->buttons, "R", "wiz.r", 11);
                    addButton(c->buttons, "A", "wiz.a", 12);
                    addButton(c->buttons, "B", "wiz.b", 13);
                    addButton(c->buttons, "X", "wiz.x", 14);
                    addButton(c->buttons, "Y", "wiz.y", 15);
                    addButton(c->buttons, "Menu", "wiz.menu", 8);
                    addButton(c->buttons, "Select", "wiz.select", 9);
                    addButton(c->buttons, "Vol-Up", "wiz.volUp", 16);
                    addButton(c->buttons, "Vol-Down", "wiz.volDown", 17);
                #elif A_PLATFORM_CAANOO
                    addButton(c->buttons, "Up", "caanoo.up", -1);
                    addButton(c->buttons, "Down", "caanoo.down", -1);
                    addButton(c->buttons, "Left", "caanoo.left", -1);
                    addButton(c->buttons, "Right", "caanoo.right", -1);
                    addButton(c->buttons, "L", "caanoo.l", 4);
                    addButton(c->buttons, "R", "caanoo.r", 5);
                    addButton(c->buttons, "A", "caanoo.a", 0);
                    addButton(c->buttons, "B", "caanoo.b", 2);
                    addButton(c->buttons, "X", "caanoo.x", 1);
                    addButton(c->buttons, "Y", "caanoo.y", 3);
                    addButton(c->buttons, "Home", "caanoo.home", 6);
                    addButton(c->buttons, "Hold", "caanoo.hold", 7);
                    addButton(c->buttons, "I", "caanoo.1", 8);
                    addButton(c->buttons, "II", "caanoo.2", 9);
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

                static const char* buttonNames[SDL_CONTROLLER_BUTTON_MAX][2] = {
                    {"A", "gamepad.b.a"},
                    {"B", "gamepad.b.b"},
                    {"X", "gamepad.b.x"},
                    {"Y", "gamepad.b.y"},
                    {"Select", "gamepad.b.select"},
                    {"Guide", "gamepad.b.guide"},
                    {"Start", "gamepad.b.start"},
                    {"Left Stick", "gamepad.b.lStick"},
                    {"Right Stick", "gamepad.b.rStick"},
                    {"L", "gamepad.b.l"},
                    {"R", "gamepad.b.r"},
                    {"Up", "gamepad.b.up"},
                    {"Down", "gamepad.b.down"},
                    {"Left", "gamepad.b.left"},
                    {"Right", "gamepad.b.right"}
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

                    addButton(c->buttons,
                              buttonNames[b][0],
                              buttonNames[b][1],
                              b);
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
            char name[16], id[16];
            snprintf(name, sizeof(name), "B%d", j);
            snprintf(id, sizeof(id), "gamepad.b.%d", j);

            addButton(c->buttons, name, id, j);
        }

        for(int j = 0; j < c->numAxes; j++) {
            char id[16];
            snprintf(id, sizeof(id), "gamepad.a.%d", j);

            addAnalog(c->axes, id, j);
        }

        #if A_USE_LIB_SDL == 2
            }
        #endif

        if((c->numHats > 0 || c->numAxes >= 2)
            && !a_strhash_contains(c->buttons, "gamepad.b.up")) {

            // Declare virtual direction buttons
            addButton(c->buttons, "Up", "gamepad.b.up", -1);
            addButton(c->buttons, "Down", "gamepad.b.down", -1);
            addButton(c->buttons, "Left", "gamepad.b.left", -1);
            addButton(c->buttons, "Right", "gamepad.b.right", -1);
        }
    }

    #if A_PLATFORM_PANDORA
        // Because these are defined before the generic keys, they
        // will take precedence in the a_sdl_input__get event loop.
        addKey("Up", "pandora.up", SDLK_UP);
        addKey("Down", "pandora.down", SDLK_DOWN);
        addKey("Left", "pandora.left", SDLK_LEFT);
        addKey("Right", "pandora.right", SDLK_RIGHT);
        addKey("L", "pandora.l", SDLK_RSHIFT);
        addKey("R", "pandora.r", SDLK_RCTRL);
        addKey("A", "pandora.a", SDLK_HOME);
        addKey("B", "pandora.b", SDLK_END);
        addKey("X", "pandora.x", SDLK_PAGEDOWN);
        addKey("Y", "pandora.y", SDLK_PAGEUP);
        addKey("Start", "pandora.start", SDLK_LALT);
        addKey("Select", "pandora.select", SDLK_LCTRL);
    #endif

    #if A_USE_LIB_SDL == 1
        addKey("Up", "key.up", SDLK_UP);
        addKey("Down", "key.down", SDLK_DOWN);
        addKey("Left", "key.left", SDLK_LEFT);
        addKey("Right", "key.right", SDLK_RIGHT);
        addKey("z", "key.z", SDLK_z);
        addKey("x", "key.x", SDLK_x);
        addKey("c", "key.c", SDLK_c);
        addKey("v", "key.v", SDLK_v);
        addKey("m", "key.m", SDLK_m);
        addKey("Enter", "key.enter", SDLK_RETURN);
        addKey("Space", "key.space", SDLK_SPACE);
        addKey("F1", "key.f1", SDLK_F1);
        addKey("F2", "key.f2", SDLK_F2);
        addKey("F3", "key.f3", SDLK_F3);
        addKey("F4", "key.f4", SDLK_F4);
        addKey("F5", "key.f5", SDLK_F5);
        addKey("F6", "key.f6", SDLK_F6);
        addKey("F7", "key.f7", SDLK_F7);
        addKey("F8", "key.f8", SDLK_F8);
        addKey("F9", "key.f9", SDLK_F9);
        addKey("F10", "key.f10", SDLK_F10);
        addKey("F11", "key.f11", SDLK_F11);
        addKey("F12", "key.f12", SDLK_F12);
    #elif A_USE_LIB_SDL == 2
        addKey("Up", "key.up", SDL_SCANCODE_UP);
        addKey("Down", "key.down", SDL_SCANCODE_DOWN);
        addKey("Left", "key.left", SDL_SCANCODE_LEFT);
        addKey("Right", "key.right", SDL_SCANCODE_RIGHT);
        addKey("z", "key.z", SDL_SCANCODE_Z);
        addKey("x", "key.x", SDL_SCANCODE_X);
        addKey("c", "key.c", SDL_SCANCODE_C);
        addKey("v", "key.v", SDL_SCANCODE_V);
        addKey("m", "key.m", SDL_SCANCODE_M);
        addKey("Enter", "key.enter", SDL_SCANCODE_RETURN);
        addKey("Space", "key.space", SDL_SCANCODE_SPACE);
        addKey("F1", "key.f1", SDL_SCANCODE_F1);
        addKey("F2", "key.f2", SDL_SCANCODE_F2);
        addKey("F3", "key.f3", SDL_SCANCODE_F3);
        addKey("F4", "key.f4", SDL_SCANCODE_F4);
        addKey("F5", "key.f5", SDL_SCANCODE_F5);
        addKey("F6", "key.f6", SDL_SCANCODE_F6);
        addKey("F7", "key.f7", SDL_SCANCODE_F7);
        addKey("F8", "key.f8", SDL_SCANCODE_F8);
        addKey("F9", "key.f9", SDL_SCANCODE_F9);
        addKey("F10", "key.f10", SDL_SCANCODE_F10);
        addKey("F11", "key.f11", SDL_SCANCODE_F11);
        addKey("F12", "key.f12", SDL_SCANCODE_F12);
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

        #if A_USE_LIB_SDL == 1
            if(SDL_JoystickOpened(c->id)) {
                SDL_JoystickClose(c->joystick);
            }
        #elif A_USE_LIB_SDL == 2
            if(c->controller) {
                SDL_GameControllerClose(c->controller);
            } else if(SDL_JoystickGetAttached(c->joystick)) {
                SDL_JoystickClose(c->joystick);
            }
        #endif

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
        k->logicalButton = a_input_button__newSource(k->header.name,
                                                     A_STRHASH_KEY());
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
            b->logicalButton = a_input_button__newSource(b->header.name,
                                                         A_STRHASH_KEY());
            a_controller__addButton(b->logicalButton, A_STRHASH_KEY());
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
