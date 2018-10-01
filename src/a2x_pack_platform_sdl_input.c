/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_system_includes.h"

#if A_BUILD_LIB_SDL
#include "a2x_pack_platform_sdl_input.v.h"

#include <SDL.h>

#include "a2x_pack_fps.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strhash.v.h"

#if A_BUILD_LIB_SDL == 1
    typedef uint8_t ASdlJoystickId;
    typedef SDLKey ASdlKeyCode;
#elif A_BUILD_LIB_SDL == 2
    typedef SDL_JoystickID ASdlJoystickId;
    typedef SDL_Scancode ASdlKeyCode;
#endif

typedef struct {
    char* name;
    AList* forwardButtons; // list of APlatformButton or NULL
    unsigned lastEventTick;
} ASdlInputHeader;

struct APlatformButton {
    ASdlInputHeader header;
    union {
        ASdlKeyCode keyCode;
        uint8_t buttonIndex;
        int code;
    } code;
    bool lastHatEventPressed;
    bool pressed;
};

struct APlatformAnalog {
    ASdlInputHeader header;
    int axisIndex;
    int value;
};

typedef struct {
    APlatformButton* negative;
    APlatformButton* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} APlatformAnalog2Buttons;

struct APlatformTouch {
    ASdlInputHeader header;
    int x, y;
    int dx, dy;
    bool tap;
    AList* motion; // list of APlatformTouchPoint captured by motion events
};

typedef struct {
    int x, y;
} APlatformTouchPoint;

typedef struct {
    SDL_Joystick* joystick;
    #if A_BUILD_LIB_SDL == 2
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
static ASdlInputController* g_setController;
static AList* g_forwardButtonsQueue[2]; // list of APlatformButton
static uint32_t g_sdlFlags;

static void initHeader(ASdlInputHeader* Header, char* Name)
{
    Header->name = Name;
    Header->forwardButtons = NULL;
    Header->lastEventTick = a_fps_ticksGet() - 1;
}

static void freeHeader(ASdlInputHeader* Header)
{
    a_list_free(Header->forwardButtons);

    free(Header->name);
    free(Header);
}

static void addKey(const char* Name, const char* Id, int Code)
{
    if(a_strhash_contains(g_keys, Id)) {
        a_out__error("Key '%s' already defined", Id);
        return;
    }

    APlatformButton* k = a_mem_malloc(sizeof(APlatformButton));

    initHeader(&k->header, a_str_merge("[", Name, "]", NULL));

    k->code.code = Code;
    k->pressed = false;

    a_strhash_add(g_keys, Id, k);
}

static void addButton(AStrHash* ButtonsCollection, const char* Name, const char* Id, int Code)
{
    if(a_strhash_contains(ButtonsCollection, Id)) {
        return;
    }

    APlatformButton* b = a_mem_malloc(sizeof(APlatformButton));

    initHeader(&b->header, a_str_merge("(", Name, ")", NULL));

    b->code.code = Code;
    b->lastHatEventPressed = false;
    b->pressed = false;

    a_strhash_add(ButtonsCollection, Id, b);
}

static void pressButton(APlatformButton* Button, bool Pressed)
{
    Button->pressed = Pressed;
    Button->header.lastEventTick = a_fps_ticksGet();

    if(Button->header.forwardButtons == NULL) {
        return;
    }

    A_LIST_ITERATE(Button->header.forwardButtons, APlatformButton*, b) {
        // Queue forwarded button presses and releases to be processed after
        // all input events were received, so they don't conflict with them.
        a_list_addLast(g_forwardButtonsQueue[Pressed], b);
    }
}

static void addAnalog(AStrHash* AxesCollection, const char* Id, int AxisIndex)
{
    if(a_strhash_contains(AxesCollection, Id)) {
        a_out__error("Analog '%s' is already defined", Id);
        return;
    }

    APlatformAnalog* a = a_mem_malloc(sizeof(APlatformAnalog));

    initHeader(&a->header, a_str_dup(Id));

    a->axisIndex = AxisIndex;
    a->value = 0;

    a_strhash_add(AxesCollection, Id, a);
}

static void setAnalog(APlatformAnalog* Analog, int Value)
{
    Analog->value = Value;

    if(Analog->header.forwardButtons == NULL) {
        return;
    }

    #define A__PRESS_THRESHOLD ((1 << 15) / 3)

    bool pressedNegative = Value < -A__PRESS_THRESHOLD;
    bool pressedPositive = Value > A__PRESS_THRESHOLD;

    A_LIST_ITERATE(Analog->header.forwardButtons, APlatformAnalog2Buttons*, b) {
        if(b->negative && pressedNegative != b->lastPressedNegative) {
            pressButton(b->negative, pressedNegative);
            b->lastPressedNegative = pressedNegative;
        }

        if(b->positive && pressedPositive != b->lastPressedPositive) {
            pressButton(b->positive, pressedPositive);
            b->lastPressedPositive = pressedPositive;
        }
    }
}

static void addTouch(const char* Id)
{
    if(a_strhash_contains(g_touchScreens, Id)) {
        a_out__error("Touchscreen '%s' is already defined", Id);
        return;
    }

    APlatformTouch* t = a_mem_malloc(sizeof(APlatformTouch));

    initHeader(&t->header, a_str_dup(Id));

    t->x = 0;
    t->y = 0;
    t->dx = 0;
    t->dy = 0;
    t->tap = false;
    t->motion = a_list_new();

    a_strhash_add(g_touchScreens, Id, t);
}

static const char* joystickName(ASdlInputController* Controller)
{
    #if A_BUILD_LIB_SDL == 1
        return SDL_JoystickName(Controller->id);
    #elif A_BUILD_LIB_SDL == 2
        return SDL_JoystickName(Controller->joystick);
    #endif
}

void a_platform_sdl_input__init(void)
{
    #if A_BUILD_LIB_SDL == 1
        g_sdlFlags = SDL_INIT_JOYSTICK;
    #elif A_BUILD_LIB_SDL == 2
        g_sdlFlags = SDL_INIT_GAMECONTROLLER;
    #endif

    if(SDL_InitSubSystem(g_sdlFlags) != 0) {
        a_out__fatal("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_keys = a_strhash_new();
    g_touchScreens = a_strhash_new();
    g_controllers = a_list_new();
    g_setController = NULL;
    g_forwardButtonsQueue[0] = a_list_new();
    g_forwardButtonsQueue[1] = a_list_new();

    const int joysticksNum = SDL_NumJoysticks();
    a_out__message("Found %d controllers", joysticksNum);

    #if A_BUILD_LIB_SDL == 2
        if(joysticksNum > 0) {
            const char* mFile = a_settings_getString("input.mapfile");
            int mNum = SDL_GameControllerAddMappingsFromFile(mFile);

            if(mNum < 0) {
                a_out__error("SDL_GameControllerAddMappingsFromFile(%s): %s",
                             mFile,
                             SDL_GetError());
            } else {
                a_out__message("Loaded %d mappings from %s", mNum, mFile);
            }
        }
    #endif

    for(int i = 0; i < joysticksNum; i++) {
        SDL_Joystick* joystick = NULL;

        #if A_BUILD_LIB_SDL == 2
            SDL_GameController* controller = NULL;

            if(SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);

                if(controller == NULL) {
                    a_out__error(
                        "SDL_GameControllerOpen(%d): %s", i, SDL_GetError());
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
                a_out__error(
                    "SDL_JoystickOpen(%d): %s", i, SDL_GetError());
                continue;
            }
        }

        #if A_BUILD_LIB_SDL == 1
            ASdlJoystickId id = (uint8_t)i;
        #elif A_BUILD_LIB_SDL == 2
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
        #if A_BUILD_LIB_SDL == 2
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

        #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
            if(i == 0) {
                // Joystick 0 is the built-in controls on these platforms
                #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
                    addButton(c->buttons, "Up", "gamepad.b.up", 0);
                    addButton(c->buttons, "Down", "gamepad.b.down", 4);
                    addButton(c->buttons, "Left", "gamepad.b.left", 2);
                    addButton(c->buttons, "Right", "gamepad.b.right", 6);
                    addButton(c->buttons, "Up-Left", "gamepad.b.upLeft", 1);
                    addButton(c->buttons, "Up-Right", "gamepad.b.upRight", 7);
                    addButton(c->buttons, "Down-Left", "gamepad.b.downLeft", 3);
                    addButton(c->buttons, "Down-Right", "gamepad.b.downRight", 5);
                    addButton(c->buttons, "L", "gamepad.b.l", 10);
                    addButton(c->buttons, "R", "gamepad.b.r", 11);
                    addButton(c->buttons, "A", "gamepad.b.x", 12);
                    addButton(c->buttons, "B", "gamepad.b.b", 13);
                    addButton(c->buttons, "X", "gamepad.b.a", 14);
                    addButton(c->buttons, "Y", "gamepad.b.y", 15);
                    addButton(c->buttons, "Select", "gamepad.b.select", 9);
                    addButton(c->buttons, "Vol-Up", "gamepad.b.volUp", 16);
                    addButton(c->buttons, "Vol-Down", "gamepad.b.volDown", 17);
                    #if A_BUILD_SYSTEM_GP2X
                        addButton(c->buttons, "Start", "gamepad.b.start", 8);
                        addButton(c->buttons, "Stick-Click", "gamepad.b.stickClick", 18);
                    #elif A_BUILD_SYSTEM_WIZ
                        addButton(c->buttons, "Menu", "gamepad.b.start", 8);
                    #endif
                #elif A_BUILD_SYSTEM_CAANOO
                    addButton(c->buttons, "Up", "gamepad.b.up", -1);
                    addButton(c->buttons, "Down", "gamepad.b.down", -1);
                    addButton(c->buttons, "Left", "gamepad.b.left", -1);
                    addButton(c->buttons, "Right", "gamepad.b.right", -1);
                    addButton(c->buttons, "L", "gamepad.b.l", 4);
                    addButton(c->buttons, "R", "gamepad.b.r", 5);
                    addButton(c->buttons, "A", "gamepad.b.x", 0);
                    addButton(c->buttons, "B", "gamepad.b.b", 2);
                    addButton(c->buttons, "X", "gamepad.b.a", 1);
                    addButton(c->buttons, "Y", "gamepad.b.y", 3);
                    addButton(c->buttons, "Home", "gamepad.b.guide", 6);
                    addButton(c->buttons, "Hold", "gamepad.b.hold", 7);
                    addButton(c->buttons, "I", "gamepad.b.start", 8);
                    addButton(c->buttons, "II", "gamepad.b.select", 9);
                    addAnalog(c->axes, "gamepad.a.leftX", 0);
                    addAnalog(c->axes, "gamepad.a.leftY", 1);
                #endif
                continue;
            }
        #elif A_BUILD_SYSTEM_PANDORA
            const char* name = joystickName(c);

            // Check if this is one of the built-in nubs
            if(a_str_equal(name, "nub0")) {
                addAnalog(c->axes, "gamepad.a.leftX", 0);
                addAnalog(c->axes, "gamepad.a.leftY", 1);
                continue;
            } else if(a_str_equal(name, "nub1")) {
                addAnalog(c->axes, "gamepad.a.rightX", 0);
                addAnalog(c->axes, "gamepad.a.rightY", 1);
                continue;
            }
        #endif

        c->generic = true;

        #if A_BUILD_LIB_SDL == 2
            if(c->controller) {
                a_out__message("Mapped %s: %d buttons, %d axes, %d hats",
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

                    addButton(
                        c->buttons, buttonNames[b][0], buttonNames[b][1], b);
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
                a_out__message("Found %s: %d buttons, %d axes, %d hats",
                               joystickName(c),
                               c->numButtons,
                               c->numAxes,
                               c->numHats);

                static const char* buttons[][2] = {
                    {"A", "gamepad.b.a"},
                    {"B", "gamepad.b.b"},
                    {"X", "gamepad.b.x"},
                    {"Y", "gamepad.b.y"},
                    {"L", "gamepad.b.l"},
                    {"R", "gamepad.b.r"},
                    {"Select", "gamepad.b.select"},
                    {"Start", "gamepad.b.start"},
                    {"Guide", "gamepad.b.guide"}
                };

                static const char* axes[] = {
                    "gamepad.a.leftX",
                    "gamepad.a.leftY",
                    "gamepad.a.rightX",
                    "gamepad.a.rightY",
                    "gamepad.a.leftTrigger",
                    "gamepad.a.rightTrigger"
                };

                for(int j = a_math_min(c->numButtons, A_ARRAY_LEN(buttons)); j--; ) {
                    addButton(c->buttons, buttons[j][0], buttons[j][1], j);
                }

                for(int j = a_math_min(c->numAxes, A_ARRAY_LEN(axes)); j--; ) {
                    addAnalog(c->axes, axes[j], j);
                }
        #if A_BUILD_LIB_SDL == 2
            }
        #endif

        if(c->numHats > 0 || c->numAxes >= 2) {
            // These buttons will be controlled by hats and analog axes
            addButton(c->buttons, "Up", "gamepad.b.up", -1);
            addButton(c->buttons, "Down", "gamepad.b.down", -1);
            addButton(c->buttons, "Left", "gamepad.b.left", -1);
            addButton(c->buttons, "Right", "gamepad.b.right", -1);
            addButton(c->buttons, "L", "gamepad.b.l", -1);
            addButton(c->buttons, "R", "gamepad.b.r", -1);
        }
    }

    #if A_BUILD_SYSTEM_PANDORA
        // Because these are defined before the generic keys, they
        // will take precedence in the a_platform__inputsPoll event loop.
        addKey("Up", "gamepad.b.up", SDLK_UP);
        addKey("Down", "gamepad.b.down", SDLK_DOWN);
        addKey("Left", "gamepad.b.left", SDLK_LEFT);
        addKey("Right", "gamepad.b.right", SDLK_RIGHT);
        addKey("L", "gamepad.b.l", SDLK_RSHIFT);
        addKey("R", "gamepad.b.r", SDLK_RCTRL);
        addKey("A", "gamepad.b.x", SDLK_HOME);
        addKey("B", "gamepad.b.b", SDLK_END);
        addKey("X", "gamepad.b.a", SDLK_PAGEDOWN);
        addKey("Y", "gamepad.b.y", SDLK_PAGEUP);
        addKey("Start", "gamepad.b.start", SDLK_LALT);
        addKey("Select", "gamepad.b.select", SDLK_LCTRL);
    #endif

    #if A_BUILD_LIB_SDL == 1
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
    #elif A_BUILD_LIB_SDL == 2
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

void a_platform_sdl_input__uninit(void)
{
    A_STRHASH_ITERATE(g_keys, APlatformButton*, k) {
        freeHeader(&k->header);
    }

    A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
        a_list_free(t->motion);
        freeHeader(&t->header);
    }

    A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, APlatformButton*, b) {
            freeHeader(&b->header);
        }

        A_STRHASH_ITERATE(c->axes, APlatformAnalog*, a) {
            freeHeader(&a->header);
        }

        #if A_BUILD_LIB_SDL == 1
            if(SDL_JoystickOpened(c->id)) {
                SDL_JoystickClose(c->joystick);
            }
        #elif A_BUILD_LIB_SDL == 2
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
    a_list_free(g_forwardButtonsQueue[0]);
    a_list_free(g_forwardButtonsQueue[1]);

    SDL_QuitSubSystem(g_sdlFlags);
}

void a_platform__inputsPoll(void)
{
    A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
        t->tap = false;
        a_list_clearEx(t->motion, free);
    }

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                #if !A_BUILD_SYSTEM_EMSCRIPTEN
                    if(event.key.keysym.sym == SDLK_ESCAPE) {
                        a_state_exit();
                        break;
                    }
                #endif

                A_STRHASH_ITERATE(g_keys, APlatformButton*, k) {
                    #if A_BUILD_LIB_SDL == 1
                        if(k->code.keyCode == event.key.keysym.sym) {
                    #elif A_BUILD_LIB_SDL == 2
                        if(k->code.keyCode == event.key.keysym.scancode) {
                    #endif

                        pressButton(k, event.key.state == SDL_PRESSED);
                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    #if A_BUILD_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jbutton.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->buttons, APlatformButton*, b) {
                        if(b->code.buttonIndex == event.jbutton.button) {
                            pressButton(b, event.jbutton.state == SDL_PRESSED);
                            break;
                        }
                    }

                    break;
                }
            } break;

            case SDL_JOYHATMOTION: {
                unsigned state = 0;
                #define A__UP_PRESSED    (1 << 0)
                #define A__DOWN_PRESSED  (1 << 1)
                #define A__LEFT_PRESSED  (1 << 2)
                #define A__RIGHT_PRESSED (1 << 3)

                switch(event.jhat.value) {
                    case SDL_HAT_UP: {
                        state = A__UP_PRESSED;
                    } break;

                    case SDL_HAT_DOWN: {
                        state = A__DOWN_PRESSED;
                    } break;

                    case SDL_HAT_LEFT: {
                        state = A__LEFT_PRESSED;
                    } break;

                    case SDL_HAT_RIGHT: {
                        state = A__RIGHT_PRESSED;
                    } break;

                    case SDL_HAT_LEFTUP: {
                        state = A__LEFT_PRESSED | A__UP_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTUP: {
                        state = A__RIGHT_PRESSED | A__UP_PRESSED;
                    } break;

                    case SDL_HAT_LEFTDOWN: {
                        state = A__LEFT_PRESSED | A__DOWN_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTDOWN: {
                        state = A__RIGHT_PRESSED | A__DOWN_PRESSED;
                    } break;
                }

                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    #if A_BUILD_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jhat.which) {
                        continue;
                    }

                    APlatformButton* buttons[4] = {
                        a_strhash_get(c->buttons, "gamepad.b.up"),
                        a_strhash_get(c->buttons, "gamepad.b.down"),
                        a_strhash_get(c->buttons, "gamepad.b.left"),
                        a_strhash_get(c->buttons, "gamepad.b.right")
                    };

                    for(int i = 0; i < 4; i++, state >>= 1) {
                        APlatformButton* b = buttons[i];

                        if(state & 1) {
                            if(!b->lastHatEventPressed) {
                                b->lastHatEventPressed = true;
                                pressButton(b, true);
                            }
                        } else {
                            if(b->lastHatEventPressed) {
                                b->lastHatEventPressed = false;
                                pressButton(b, false);
                            }
                        }
                    }

                    break;
                }
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    #if A_BUILD_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jaxis.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->axes, APlatformAnalog*, a) {
                        if(event.jaxis.axis == a->axisIndex) {
                            setAnalog(a, event.jaxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;

#if A_BUILD_LIB_SDL == 2
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, ASdlInputController*, c) {
                    if(c->controller == NULL || c->id != event.cbutton.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->buttons, APlatformButton*, b) {
                        if(b->code.buttonIndex == event.cbutton.button) {
                            pressButton(b, event.cbutton.state == SDL_PRESSED);
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

                    A_STRHASH_ITERATE(c->axes, APlatformAnalog*, a) {
                        if(event.caxis.axis == a->axisIndex) {
                            setAnalog(a, event.caxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;
#endif

            case SDL_MOUSEMOTION: {
                const bool track = a_settings_getBool("input.trackMouse");

                A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
                    t->x = event.button.x;
                    t->y = event.button.y;

                    if(track) {
                        APlatformTouchPoint* p =
                            a_mem_malloc(sizeof(APlatformTouchPoint));

                        p->x = t->x;
                        p->y = t->y;

                        a_list_addLast(t->motion, p);
                    }
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
                            t->x = event.button.x;
                            t->y = event.button.y;
                            t->tap = true;
                        }
                    } break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
                            t->x = event.button.x;
                            t->y = event.button.y;
                            t->tap = false;
                        }
                    } break;
                }
            } break;

            default:break;
        }
    }

    A_LIST_ITERATE(g_forwardButtonsQueue[1], APlatformButton*, b) {
        // Overwrite whatever current state with a press
        pressButton(b, true);
    }

    unsigned ticksNow = a_fps_ticksGet();

    A_LIST_ITERATE(g_forwardButtonsQueue[0], APlatformButton*, b) {
        // Only release if did not receive an event this frame
        if(b->header.lastEventTick != ticksNow) {
            pressButton(b, false);
        }
    }

    a_list_clear(g_forwardButtonsQueue[0]);
    a_list_clear(g_forwardButtonsQueue[1]);

    #if !A_BUILD_SYSTEM_EMSCRIPTEN
        int mouseDx = 0, mouseDy = 0;
        SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

        A_STRHASH_ITERATE(g_touchScreens, APlatformTouch*, t) {
            t->dx = mouseDx;
            t->dy = mouseDy;
        }
    #endif
}

APlatformButton* a_platform__buttonGet(const char* Id)
{
    APlatformButton* b = a_strhash_get(g_keys, Id);

    if(b != NULL) {
        return b;
    }

    if(g_setController) {
        b = a_strhash_get(g_setController->buttons, Id);

        if(b != NULL) {
            return b;
        }
    }

    return NULL;
}

bool a_platform__buttonPressGet(const APlatformButton* Button)
{
    return Button->pressed;
}

const char* a_platform__buttonNameGet(const APlatformButton* Button)
{
    return Button->header.name;
}

void a_platform__buttonForward(APlatformButton* Source, APlatformButton* Destination)
{
    if(Source->header.forwardButtons == NULL) {
        Source->header.forwardButtons = a_list_new();
    }

    a_list_addLast(Source->header.forwardButtons, Destination);
}

APlatformAnalog* a_platform__analogGet(const char* Id)
{
    if(g_setController) {
        APlatformAnalog* a = a_strhash_get(g_setController->axes, Id);

        if(a != NULL) {
            return a;
        }
    }

    return NULL;
}

int a_platform__analogValueGet(const APlatformAnalog* Analog)
{
    return Analog->value;
}

void a_platform__analogForward(APlatformAnalog* Source, APlatformButton* Negative, APlatformButton* Positive)
{
    APlatformAnalog2Buttons* f = a_mem_malloc(sizeof(APlatformAnalog2Buttons));

    f->negative = Negative;
    f->positive = Positive;
    f->lastPressedNegative = false;
    f->lastPressedPositive = false;

    if(Source->header.forwardButtons == NULL) {
        Source->header.forwardButtons = a_list_new();
    }

    a_list_addLast(Source->header.forwardButtons, f);
}

APlatformTouch* a_platform__touchGet(const char* Id)
{
    return a_strhash_get(g_touchScreens, Id);
}

void a_platform__touchCoordsGet(const APlatformTouch* Touch, int* X, int* Y)
{
    *X = Touch->x;
    *Y = Touch->y;
}

void a_platform__touchDeltaGet(const APlatformTouch* Touch, int* Dx, int* Dy)
{
    *Dx = Touch->dx;
    *Dy = Touch->dy;
}

bool a_platform__touchTapGet(const APlatformTouch* Touch)
{
    return Touch->tap;
}

unsigned a_platform__controllerNumGet(void)
{
    return a_list_sizeGet(g_controllers);
}

void a_platform__controllerSet(unsigned Index)
{
    if(Index >= a_list_sizeGet(g_controllers)) {
        a_out__fatal("Cannot set controller %d, %d total",
                     Index,
                     a_list_sizeGet(g_controllers));
    }

    g_setController = a_list_getIndex(g_controllers, Index);
}

bool a_platform__controllerIsMapped(void)
{
    #if A_BUILD_LIB_SDL == 2
        return g_setController && g_setController->controller != NULL;
    #else
        return false;
    #endif
}

bool a_platform__controllerIsGeneric(void)
{
    return g_setController && g_setController->generic;
}
#endif // A_BUILD_LIB_SDL
