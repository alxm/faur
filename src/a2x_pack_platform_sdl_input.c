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
#include "a2x_pack_input_button.v.h"
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

struct APlatformButton {
    char* name;
    AList* forwardButtons; // list of APlatformButton or NULL
    union {
        ASdlKeyCode keyCode;
        uint8_t buttonIndex;
        int code;
    } code;
    unsigned lastEventTick;
    bool lastHatEventPressed;
    bool pressed;
};

struct APlatformAnalog {
    char* name;
    AList* forwardButtons; // list of APlatformAnalog2Buttons or NULL
    int axisIndex;
    int value;
};

struct APlatformTouch {
    char* name;
    int x, y;
    int dx, dy;
    bool tap;
    AList* motion; // list of APlatformTouchPoint captured by motion events
};

struct APlatformController {
    APlatformController* next;
    SDL_Joystick* joystick;
    #if A_BUILD_LIB_SDL == 2
        SDL_GameController* controller;
    #endif
    ASdlJoystickId id;
    int numButtons;
    int numHats;
    int numAxes;
    APlatformButton* buttons[A_BUTTON_NUM];
    AStrHash* axes;
};

typedef struct {
    int x, y;
} APlatformTouchPoint;

typedef struct {
    APlatformButton* negative;
    APlatformButton* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} APlatformAnalog2Buttons;

static APlatformButton* g_keys[A_KEY_NUM - A__KEY_FLAG];
static AStrHash* g_touchScreens;
static AList* g_controllers;
static APlatformController* g_setController;
static AList* g_forwardButtonsQueue[2]; // list of APlatformButton
static uint32_t g_sdlFlags;

static void keyAdd(const char* Name, AKeyId Id, int Code)
{
    if(g_keys[Id - A__KEY_FLAG] != NULL) {
        return;
    }

    APlatformButton* k = a_mem_malloc(sizeof(APlatformButton));

    k->name = a_str_merge("[", Name, "]", NULL);
    k->forwardButtons = NULL;
    k->code.code = Code;
    k->lastEventTick = a_fps_ticksGet() - 1;
    k->pressed = false;

    g_keys[Id - A__KEY_FLAG] = k;
}

static void buttonAdd(APlatformButton** ButtonsCollection, const char* Name, AButtonId Id, int Code)
{
    if(ButtonsCollection[Id] != NULL) {
        return;
    }

    APlatformButton* b = a_mem_malloc(sizeof(APlatformButton));

    b->name = a_str_merge("(", Name, ")", NULL);
    b->forwardButtons = NULL;
    b->code.code = Code;
    b->lastEventTick = a_fps_ticksGet() - 1;
    b->lastHatEventPressed = false;
    b->pressed = false;

    ButtonsCollection[Id] = b;
}

static void buttonFree(APlatformButton* Button)
{
    a_list_free(Button->forwardButtons);

    free(Button->name);
    free(Button);
}

static void buttonPress(APlatformButton* Button, bool Pressed)
{
    Button->pressed = Pressed;
    Button->lastEventTick = a_fps_ticksGet();

    if(Button->forwardButtons == NULL) {
        return;
    }

    A_LIST_ITERATE(Button->forwardButtons, APlatformButton*, b) {
        // Queue forwarded button presses and releases to be processed after
        // all input events were received, so they don't conflict with them.
        a_list_addLast(g_forwardButtonsQueue[Pressed], b);
    }
}

static void analogAdd(AStrHash* AxesCollection, const char* Id, int AxisIndex)
{
    if(a_strhash_contains(AxesCollection, Id)) {
        a_out__error("Analog '%s' is already defined", Id);
        return;
    }

    APlatformAnalog* a = a_mem_malloc(sizeof(APlatformAnalog));

    a->name = a_str_dup(Id);
    a->forwardButtons = NULL;
    a->axisIndex = AxisIndex;
    a->value = 0;

    a_strhash_add(AxesCollection, Id, a);
}

static void analogFree(APlatformAnalog* Analog)
{
    a_list_freeEx(Analog->forwardButtons, free);

    free(Analog->name);
    free(Analog);
}

static void analogSet(APlatformAnalog* Analog, int Value)
{
    Analog->value = Value;

    if(Analog->forwardButtons == NULL) {
        return;
    }

    #define A__PRESS_THRESHOLD ((1 << 15) / 3)

    bool pressedNegative = Value < -A__PRESS_THRESHOLD;
    bool pressedPositive = Value > A__PRESS_THRESHOLD;

    A_LIST_ITERATE(Analog->forwardButtons, APlatformAnalog2Buttons*, b) {
        if(b->negative && pressedNegative != b->lastPressedNegative) {
            buttonPress(b->negative, pressedNegative);
            b->lastPressedNegative = pressedNegative;
        }

        if(b->positive && pressedPositive != b->lastPressedPositive) {
            buttonPress(b->positive, pressedPositive);
            b->lastPressedPositive = pressedPositive;
        }
    }
}

static void touchAdd(const char* Id)
{
    if(a_strhash_contains(g_touchScreens, Id)) {
        a_out__error("Touchscreen '%s' is already defined", Id);
        return;
    }

    APlatformTouch* t = a_mem_malloc(sizeof(APlatformTouch));

    t->name = a_str_dup(Id);
    t->x = 0;
    t->y = 0;
    t->dx = 0;
    t->dy = 0;
    t->tap = false;
    t->motion = a_list_new();

    a_strhash_add(g_touchScreens, Id, t);
}

static void touchFree(APlatformTouch* Touch)
{
    a_list_freeEx(Touch->motion, free);

    free(Touch->name);
    free(Touch);
}

static APlatformController* controllerAdd(int Index)
{
    SDL_Joystick* joystick = NULL;

    #if A_BUILD_LIB_SDL == 2
        SDL_GameController* controller = NULL;

        if(SDL_IsGameController(Index)) {
            controller = SDL_GameControllerOpen(Index);

            if(controller == NULL) {
                a_out__error(
                    "SDL_GameControllerOpen(%d): %s", Index, SDL_GetError());
            } else {
                joystick = SDL_GameControllerGetJoystick(controller);

                if(joystick == NULL) {
                    a_out__error(
                        "SDL_GameControllerGetJoystick: %s", SDL_GetError());

                    SDL_GameControllerClose(controller);

                    return NULL;
                }
            }
        }
    #endif

    if(joystick == NULL) {
        joystick = SDL_JoystickOpen(Index);

        if(joystick == NULL) {
            a_out__error("SDL_JoystickOpen(%d): %s", Index, SDL_GetError());
            return NULL;
        }
    }

    #if A_BUILD_LIB_SDL == 1
        ASdlJoystickId id = (uint8_t)Index;
    #elif A_BUILD_LIB_SDL == 2
        ASdlJoystickId id = SDL_JoystickInstanceID(joystick);

        if(id < 0) {
            a_out__error("SDL_JoystickInstanceID: %s", SDL_GetError());

            if(controller) {
                SDL_GameControllerClose(controller);
            } else {
                SDL_JoystickClose(joystick);
            }

            return NULL;
        }
    #endif

    APlatformController* c = a_mem_zalloc(sizeof(APlatformController));

    c->next = NULL;
    c->joystick = joystick;
    #if A_BUILD_LIB_SDL == 2
        c->controller = controller;
    #endif
    c->id = id;
    c->numButtons = SDL_JoystickNumButtons(c->joystick);
    c->numHats = SDL_JoystickNumHats(c->joystick);
    c->numAxes = SDL_JoystickNumAxes(c->joystick);
    c->axes = a_strhash_new();

    return c;
}

static void controllerFree(APlatformController* Controller)
{
    for(AButtonId id = 0; id < A_BUTTON_NUM; id++) {
        if(Controller->buttons[id]) {
            buttonFree(Controller->buttons[id]);
        }
    }

    A_STRHASH_ITERATE(Controller->axes, APlatformAnalog*, a) {
        analogFree(a);
    }

    #if A_BUILD_LIB_SDL == 1
        if(SDL_JoystickOpened(Controller->id)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #elif A_BUILD_LIB_SDL == 2
        if(Controller->controller) {
            SDL_GameControllerClose(Controller->controller);
        } else if(SDL_JoystickGetAttached(Controller->joystick)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #endif

    a_strhash_free(Controller->axes);

    free(Controller);
}

static const char* joystickName(APlatformController* Controller)
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

    g_touchScreens = a_strhash_new();
    g_controllers = a_list_new();
    g_setController = NULL;
    g_forwardButtonsQueue[0] = a_list_new();
    g_forwardButtonsQueue[1] = a_list_new();

    const int joysticksNum = SDL_NumJoysticks();
    a_out__message("Found %d controllers", joysticksNum);

    #if A_BUILD_LIB_SDL == 2
        if(joysticksNum > 0) {
            const char* mFile = a_settings_stringGet(A_SETTING_FILE_GAMEPADMAP);
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
        APlatformController* c = controllerAdd(i);

        if(c == NULL) {
            continue;
        }

        a_list_addLast(g_controllers, c);

        #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO
            if(i == 0) {
                // Joystick 0 is the built-in controls on these platforms
                #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ
                    buttonAdd(c->buttons, "Up", A_BUTTON_UP, 0);
                    buttonAdd(c->buttons, "Down", A_BUTTON_DOWN, 4);
                    buttonAdd(c->buttons, "Left", A_BUTTON_LEFT, 2);
                    buttonAdd(c->buttons, "Right", A_BUTTON_RIGHT, 6);
                    buttonAdd(c->buttons, "Up-Left", A_BUTTON_UPLEFT, 1);
                    buttonAdd(c->buttons, "Up-Right", A_BUTTON_UPRIGHT, 7);
                    buttonAdd(c->buttons, "Down-Left", A_BUTTON_DOWNLEFT, 3);
                    buttonAdd(c->buttons, "Down-Right", A_BUTTON_DOWNRIGHT, 5);
                    buttonAdd(c->buttons, "L", A_BUTTON_L, 10);
                    buttonAdd(c->buttons, "R", A_BUTTON_R, 11);
                    buttonAdd(c->buttons, "A", A_BUTTON_X, 12);
                    buttonAdd(c->buttons, "B", A_BUTTON_B, 13);
                    buttonAdd(c->buttons, "X", A_BUTTON_A, 14);
                    buttonAdd(c->buttons, "Y", A_BUTTON_Y, 15);
                    buttonAdd(c->buttons, "Select", A_BUTTON_SELECT, 9);
                    buttonAdd(c->buttons, "Vol-Up", A_BUTTON_VOLUP, 16);
                    buttonAdd(c->buttons, "Vol-Down", A_BUTTON_VOLDOWN, 17);
                    #if A_BUILD_SYSTEM_GP2X
                        buttonAdd(c->buttons, "Start", A_BUTTON_START, 8);
                        buttonAdd(c->buttons, "Stick-Click", A_BUTTON_STICKCLICK, 18);
                    #elif A_BUILD_SYSTEM_WIZ
                        buttonAdd(c->buttons, "Menu", A_BUTTON_START, 8);
                    #endif
                #elif A_BUILD_SYSTEM_CAANOO
                    buttonAdd(c->buttons, "Up", A_BUTTON_UP, -1);
                    buttonAdd(c->buttons, "Down", A_BUTTON_DOWN, -1);
                    buttonAdd(c->buttons, "Left", A_BUTTON_LEFT, -1);
                    buttonAdd(c->buttons, "Right", A_BUTTON_RIGHT, -1);
                    buttonAdd(c->buttons, "L", A_BUTTON_L, 4);
                    buttonAdd(c->buttons, "R", A_BUTTON_R, 5);
                    buttonAdd(c->buttons, "A", A_BUTTON_X, 0);
                    buttonAdd(c->buttons, "B", A_BUTTON_B, 2);
                    buttonAdd(c->buttons, "X", A_BUTTON_A, 1);
                    buttonAdd(c->buttons, "Y", A_BUTTON_Y, 3);
                    buttonAdd(c->buttons, "Home", A_BUTTON_GUIDE, 6);
                    buttonAdd(c->buttons, "Hold", A_BUTTON_HOLD, 7);
                    buttonAdd(c->buttons, "I", A_BUTTON_START, 8);
                    buttonAdd(c->buttons, "II", A_BUTTON_SELECT, 9);
                    analogAdd(c->axes, "gamepad.a.leftX", 0);
                    analogAdd(c->axes, "gamepad.a.leftY", 1);
                #endif
                continue;
            }
        #elif A_BUILD_SYSTEM_PANDORA
            const char* name = joystickName(c);

            // Check if this is one of the built-in nubs
            if(a_str_equal(name, "nub0")) {
                analogAdd(c->axes, "gamepad.a.leftX", 0);
                analogAdd(c->axes, "gamepad.a.leftY", 1);
                continue;
            } else if(a_str_equal(name, "nub1")) {
                analogAdd(c->axes, "gamepad.a.rightX", 0);
                analogAdd(c->axes, "gamepad.a.rightY", 1);

                // Attach to nub0 to compose a single dual-analog controller
                A_LIST_ITERATE(g_controllers, APlatformController*, nub0) {
                    if(a_str_equal(joystickName(nub0), "nub0")) {
                        nub0->next = c;
                        break;
                    }
                }

                continue;
            }
        #endif

#if A_BUILD_LIB_SDL == 2
        if(c->controller) {
            a_out__message("Mapped %s: %d buttons, %d axes, %d hats",
                           SDL_GameControllerName(c->controller),
                           c->numButtons,
                           c->numAxes,
                           c->numHats);

            static struct {
                const char* name;
                AButtonId id;
            } buttons[SDL_CONTROLLER_BUTTON_MAX] = {
                {"A", A_BUTTON_A},
                {"B", A_BUTTON_B},
                {"X", A_BUTTON_X},
                {"Y", A_BUTTON_Y},
                {"Select", A_BUTTON_SELECT},
                {"Guide", A_BUTTON_GUIDE},
                {"Start", A_BUTTON_START},
                {"Left Stick", A_BUTTON_INVALID},
                {"Right Stick", A_BUTTON_INVALID},
                {"L", A_BUTTON_L},
                {"R", A_BUTTON_R},
                {"Up", A_BUTTON_UP},
                {"Down", A_BUTTON_DOWN},
                {"Left", A_BUTTON_LEFT},
                {"Right", A_BUTTON_RIGHT},
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
                    SDL_GameControllerGetBindForButton(c->controller, b);

                if(bind.bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
                    continue;
                }

                buttonAdd(c->buttons, buttons[b].name, buttons[b].id, b);
            }

            for(SDL_GameControllerAxis a = SDL_CONTROLLER_AXIS_LEFTX;
                a < SDL_CONTROLLER_AXIS_MAX;
                a++) {

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForAxis(c->controller, a);

                if(bind.bindType == SDL_CONTROLLER_BINDTYPE_NONE) {
                    continue;
                }

                analogAdd(c->axes, axisNames[a], a);
            }
        } else {
#endif
            a_out__message("Found %s: %d buttons, %d axes, %d hats",
                           joystickName(c),
                           c->numButtons,
                           c->numAxes,
                           c->numHats);

            static struct {
                const char* name;
                AButtonId id;
            } buttons[] = {
                {"A", A_BUTTON_A},
                {"B", A_BUTTON_B},
                {"X", A_BUTTON_X},
                {"Y", A_BUTTON_Y},
                {"L", A_BUTTON_L},
                {"R", A_BUTTON_R},
                {"Select", A_BUTTON_SELECT},
                {"Start", A_BUTTON_START},
                {"Guide", A_BUTTON_GUIDE}
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
                buttonAdd(c->buttons, buttons[j].name, buttons[j].id, j);
            }

            for(int j = a_math_min(c->numAxes, A_ARRAY_LEN(axes)); j--; ) {
                analogAdd(c->axes, axes[j], j);
            }
#if A_BUILD_LIB_SDL == 2
        }
#endif

        if(c->numHats > 0 || c->numAxes >= 2) {
            // These buttons will be controlled by hats and analog axes
            buttonAdd(c->buttons, "Up", A_BUTTON_UP, -1);
            buttonAdd(c->buttons, "Down", A_BUTTON_DOWN, -1);
            buttonAdd(c->buttons, "Left", A_BUTTON_LEFT, -1);
            buttonAdd(c->buttons, "Right", A_BUTTON_RIGHT, -1);
            buttonAdd(c->buttons, "L", A_BUTTON_L, -1);
            buttonAdd(c->buttons, "R", A_BUTTON_R, -1);
        }
    }

    #if A_BUILD_SYSTEM_PANDORA
        // Because these are defined before the generic keys, they
        // will take precedence in the a_platform__inputsPoll event loop.
        keyAdd("Up", A_BUTTON_UP, SDLK_UP);
        keyAdd("Down", A_BUTTON_DOWN, SDLK_DOWN);
        keyAdd("Left", A_BUTTON_LEFT, SDLK_LEFT);
        keyAdd("Right", A_BUTTON_RIGHT, SDLK_RIGHT);
        keyAdd("L", A_BUTTON_L, SDLK_RSHIFT);
        keyAdd("R", A_BUTTON_R, SDLK_RCTRL);
        keyAdd("A", A_BUTTON_X, SDLK_HOME);
        keyAdd("B", A_BUTTON_B, SDLK_END);
        keyAdd("X", A_BUTTON_A, SDLK_PAGEDOWN);
        keyAdd("Y", A_BUTTON_Y, SDLK_PAGEUP);
        keyAdd("Start", A_BUTTON_START, SDLK_LALT);
        keyAdd("Select", A_BUTTON_SELECT, SDLK_LCTRL);
    #endif

    #if A_BUILD_LIB_SDL == 1
        keyAdd("Up", A_KEY_UP, SDLK_UP);
        keyAdd("Down", A_KEY_DOWN, SDLK_DOWN);
        keyAdd("Left", A_KEY_LEFT, SDLK_LEFT);
        keyAdd("Right", A_KEY_RIGHT, SDLK_RIGHT);
        keyAdd("z", A_KEY_Z, SDLK_z);
        keyAdd("x", A_KEY_X, SDLK_x);
        keyAdd("c", A_KEY_C, SDLK_c);
        keyAdd("v", A_KEY_V, SDLK_v);
        keyAdd("m", A_KEY_M, SDLK_m);
        keyAdd("Enter", A_KEY_ENTER, SDLK_RETURN);
        keyAdd("Space", A_KEY_SPACE, SDLK_SPACE);
        keyAdd("Home", A_KEY_HOME, SDLK_HOME);
        keyAdd("End", A_KEY_END, SDLK_END);
        keyAdd("PageUp", A_KEY_PAGEUP, SDLK_PAGEUP);
        keyAdd("PageDown", A_KEY_PAGEDOWN, SDLK_PAGEDOWN);
        keyAdd("L-Alt", A_KEY_LALT, SDLK_LALT);
        keyAdd("L-Ctrl", A_KEY_LCTRL, SDLK_LCTRL);
        keyAdd("L-Shift", A_KEY_LSHIFT, SDLK_LSHIFT);
        keyAdd("R-Alt", A_KEY_RALT, SDLK_RALT);
        keyAdd("R-Ctrl", A_KEY_RCTRL, SDLK_RCTRL);
        keyAdd("R-Shift", A_KEY_RSHIFT, SDLK_RSHIFT);
        keyAdd("F1", A_KEY_F1, SDLK_F1);
        keyAdd("F2", A_KEY_F2, SDLK_F2);
        keyAdd("F3", A_KEY_F3, SDLK_F3);
        keyAdd("F4", A_KEY_F4, SDLK_F4);
        keyAdd("F5", A_KEY_F5, SDLK_F5);
        keyAdd("F6", A_KEY_F6, SDLK_F6);
        keyAdd("F7", A_KEY_F7, SDLK_F7);
        keyAdd("F8", A_KEY_F8, SDLK_F8);
        keyAdd("F9", A_KEY_F9, SDLK_F9);
        keyAdd("F10", A_KEY_F10, SDLK_F10);
        keyAdd("F11", A_KEY_F11, SDLK_F11);
        keyAdd("F12", A_KEY_F12, SDLK_F12);
    #elif A_BUILD_LIB_SDL == 2
        keyAdd("Up", A_KEY_UP, SDL_SCANCODE_UP);
        keyAdd("Down", A_KEY_DOWN, SDL_SCANCODE_DOWN);
        keyAdd("Left", A_KEY_LEFT, SDL_SCANCODE_LEFT);
        keyAdd("Right", A_KEY_RIGHT, SDL_SCANCODE_RIGHT);
        keyAdd("z", A_KEY_Z, SDL_SCANCODE_Z);
        keyAdd("x", A_KEY_X, SDL_SCANCODE_X);
        keyAdd("c", A_KEY_C, SDL_SCANCODE_C);
        keyAdd("v", A_KEY_V, SDL_SCANCODE_V);
        keyAdd("m", A_KEY_M, SDL_SCANCODE_M);
        keyAdd("Enter", A_KEY_ENTER, SDL_SCANCODE_RETURN);
        keyAdd("Space", A_KEY_SPACE, SDL_SCANCODE_SPACE);
        keyAdd("Home", A_KEY_HOME, SDL_SCANCODE_HOME);
        keyAdd("End", A_KEY_END, SDL_SCANCODE_END);
        keyAdd("PageUp", A_KEY_PAGEUP, SDL_SCANCODE_PAGEUP);
        keyAdd("PageDown", A_KEY_PAGEDOWN, SDL_SCANCODE_PAGEDOWN);
        keyAdd("L-Alt", A_KEY_LALT, SDL_SCANCODE_LALT);
        keyAdd("L-Ctrl", A_KEY_LCTRL, SDL_SCANCODE_LCTRL);
        keyAdd("L-Shift", A_KEY_LSHIFT, SDL_SCANCODE_LSHIFT);
        keyAdd("R-Alt", A_KEY_RALT, SDL_SCANCODE_RALT);
        keyAdd("R-Ctrl", A_KEY_RCTRL, SDL_SCANCODE_RCTRL);
        keyAdd("R-Shift", A_KEY_RSHIFT, SDL_SCANCODE_RSHIFT);
        keyAdd("F1", A_KEY_F1, SDL_SCANCODE_F1);
        keyAdd("F2", A_KEY_F2, SDL_SCANCODE_F2);
        keyAdd("F3", A_KEY_F3, SDL_SCANCODE_F3);
        keyAdd("F4", A_KEY_F4, SDL_SCANCODE_F4);
        keyAdd("F5", A_KEY_F5, SDL_SCANCODE_F5);
        keyAdd("F6", A_KEY_F6, SDL_SCANCODE_F6);
        keyAdd("F7", A_KEY_F7, SDL_SCANCODE_F7);
        keyAdd("F8", A_KEY_F8, SDL_SCANCODE_F8);
        keyAdd("F9", A_KEY_F9, SDL_SCANCODE_F9);
        keyAdd("F10", A_KEY_F10, SDL_SCANCODE_F10);
        keyAdd("F11", A_KEY_F11, SDL_SCANCODE_F11);
        keyAdd("F12", A_KEY_F12, SDL_SCANCODE_F12);
    #endif

    touchAdd("touchScreen");
}

void a_platform_sdl_input__uninit(void)
{
    for(AKeyId id = 0; id < A_KEY_NUM - A__KEY_FLAG; id++) {
        if(g_keys[id]) {
            buttonFree(g_keys[id]);
        }
    }

    a_strhash_freeEx(g_touchScreens, (AFree*)touchFree);
    a_list_freeEx(g_controllers, (AFree*)controllerFree);
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

                for(AKeyId id = 0; id < A_KEY_NUM - A__KEY_FLAG; id++) {
#if A_BUILD_LIB_SDL == 1
                    if(g_keys[id]->code.keyCode == event.key.keysym.sym) {
#elif A_BUILD_LIB_SDL == 2
                    if(g_keys[id]->code.keyCode == event.key.keysym.scancode) {
#endif
                        buttonPress(g_keys[id], event.key.state == SDL_PRESSED);
                        break;
                    }
                }
            } break;

            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    #if A_BUILD_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jbutton.which) {
                        continue;
                    }

                    for(AButtonId id = 0; id < A_BUTTON_NUM; id++) {
                        APlatformButton* b = c->buttons[id];

                        if(b && b->code.buttonIndex == event.jbutton.button) {
                            buttonPress(b, event.jbutton.state == SDL_PRESSED);
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

                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    #if A_BUILD_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jhat.which) {
                        continue;
                    }

                    APlatformButton* buttons[4] = {
                        c->buttons[A_BUTTON_UP],
                        c->buttons[A_BUTTON_DOWN],
                        c->buttons[A_BUTTON_LEFT],
                        c->buttons[A_BUTTON_RIGHT]
                    };

                    for(int i = 0; i < 4; i++, state >>= 1) {
                        APlatformButton* b = buttons[i];

                        if(state & 1) {
                            if(!b->lastHatEventPressed) {
                                b->lastHatEventPressed = true;
                                buttonPress(b, true);
                            }
                        } else {
                            if(b->lastHatEventPressed) {
                                b->lastHatEventPressed = false;
                                buttonPress(b, false);
                            }
                        }
                    }

                    break;
                }
            } break;

            case SDL_JOYAXISMOTION: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
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
                            analogSet(a, event.jaxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;

#if A_BUILD_LIB_SDL == 2
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    if(c->controller == NULL || c->id != event.cbutton.which) {
                        continue;
                    }

                    for(AButtonId id = 0; id < A_BUTTON_NUM; id++) {
                        APlatformButton* b = c->buttons[id];

                        if(b && b->code.buttonIndex == event.cbutton.button) {
                            buttonPress(b, event.cbutton.state == SDL_PRESSED);
                            break;
                        }
                    }

                    break;
                }
            } break;

            case SDL_CONTROLLERAXISMOTION: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    if(c->controller == NULL || c->id != event.caxis.which) {
                        continue;
                    }

                    A_STRHASH_ITERATE(c->axes, APlatformAnalog*, a) {
                        if(event.caxis.axis == a->axisIndex) {
                            analogSet(a, event.caxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;
#endif

            case SDL_MOUSEMOTION: {
                const bool track =
                    a_settings_boolGet(A_SETTING_INPUT_MOUSE_TRACK);

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
        buttonPress(b, true);
    }

    unsigned ticksNow = a_fps_ticksGet();

    A_LIST_ITERATE(g_forwardButtonsQueue[0], APlatformButton*, b) {
        // Only release if did not receive an event this frame
        if(b->lastEventTick != ticksNow) {
            buttonPress(b, false);
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

APlatformButton* a_platform__buttonGet(int Id)
{
    if(Id & A__KEY_FLAG) {
        return g_keys[Id - A__KEY_FLAG];
    } else if(g_setController) {
        return g_setController->buttons[Id];
    }

    return NULL;
}

bool a_platform__buttonPressGet(const APlatformButton* Button)
{
    return Button->pressed;
}

const char* a_platform__buttonNameGet(const APlatformButton* Button)
{
    return Button->name;
}

void a_platform__buttonForward(APlatformButton* Source, APlatformButton* Destination)
{
    if(Source->forwardButtons == NULL) {
        Source->forwardButtons = a_list_new();
    }

    a_list_addLast(Source->forwardButtons, Destination);
}

APlatformAnalog* a_platform__analogGet(const char* Id)
{
    const APlatformController* c = g_setController;

    while(c) {
        APlatformAnalog* a = a_strhash_get(c->axes, Id);

        if(a != NULL) {
            return a;
        }

        c = c->next;
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

    if(Source->forwardButtons == NULL) {
        Source->forwardButtons = a_list_new();
    }

    a_list_addLast(Source->forwardButtons, f);
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
#endif // A_BUILD_LIB_SDL
