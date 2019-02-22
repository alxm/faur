/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#include "a2x_pack_platform_sdl_input.v.h"

#if A_CONFIG_LIB_SDL
#include <SDL.h>

#include "a2x_pack_fps.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_main.v.h"
#include "a2x_pack_math.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_state.v.h"
#include "a2x_pack_str.v.h"

#if A_CONFIG_LIB_SDL == 1
    typedef uint8_t ASdlJoystickId;
    typedef SDLKey ASdlKeyCode;
#elif A_CONFIG_LIB_SDL == 2
    typedef SDL_JoystickID ASdlJoystickId;
    typedef SDL_Scancode ASdlKeyCode;
#endif

struct APlatformButton {
    const char* name;
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
    const char* name;
    AList* forwardButtons; // list of APlatformButtonPair or NULL
    int axisIndex;
    int value;
};

struct APlatformTouch {
    const char* name;
    int x, y;
    int dx, dy;
    bool tap;
    #if A_CONFIG_INPUT_MOUSE_TRACK
        AList* motion; // list of ATouchPoint captured by motion events
    #endif
};

struct APlatformController {
    APlatformController* next;
    SDL_Joystick* joystick;
    #if A_CONFIG_LIB_SDL == 2
        SDL_GameController* controller;
    #endif
    ASdlJoystickId id;
    int numButtons;
    int numHats;
    int numAxes;
    APlatformButton* buttons[A_BUTTON_NUM];
    APlatformAnalog* axes[A_AXIS_NUM];
};

typedef struct {
    int x, y;
} ATouchPoint;

typedef struct {
    APlatformButton* negative;
    APlatformButton* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} APlatformButtonPair;

static APlatformTouch g_mouse;
static AList* g_controllers;
static APlatformController* g_setController;
static AList* g_forwardButtonsQueue[2]; // list of APlatformButton
static uint32_t g_sdlFlags;

#if A_CONFIG_TRAIT_KEYBOARD
static APlatformButton* g_keys[A__KEY_ID(A_KEY_NUM)];

static void keyAdd(AKeyId Id, int Code)
{
    if(g_keys[A__KEY_ID(Id)] != NULL) {
        return;
    }

    APlatformButton* k = a_mem_malloc(sizeof(APlatformButton));

    k->name = a__keyNames[A__KEY_ID(Id)];
    k->forwardButtons = NULL;
    k->code.code = Code;
    k->lastEventTick = a_fps_ticksGet() - 1;
    k->pressed = false;

    g_keys[A__KEY_ID(Id)] = k;
}
#endif

static void buttonAdd(APlatformController* Controller, AButtonId Id, const char* Name, int Code)
{
    if(Controller->buttons[Id] != NULL) {
        return;
    }

    APlatformButton* b = a_mem_malloc(sizeof(APlatformButton));

    b->name = Name;
    b->forwardButtons = NULL;
    b->code.code = Code;
    b->lastEventTick = a_fps_ticksGet() - 1;
    b->lastHatEventPressed = false;
    b->pressed = false;

    Controller->buttons[Id] = b;
}

static void buttonFree(APlatformButton* Button)
{
    a_list_free(Button->forwardButtons);

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

static void analogAdd(APlatformController* Controller, AAxisId Id, const char* Name, int AxisIndex)
{
    if(Controller->axes[Id] != NULL) {
        return;
    }

    APlatformAnalog* a = a_mem_malloc(sizeof(APlatformAnalog));

    a->name = Name;
    a->forwardButtons = NULL;
    a->axisIndex = AxisIndex;
    a->value = 0;

    Controller->axes[Id] = a;
}

static void analogFree(APlatformAnalog* Analog)
{
    a_list_freeEx(Analog->forwardButtons, free);

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

    A_LIST_ITERATE(Analog->forwardButtons, APlatformButtonPair*, b) {
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

static void touchAdd(APlatformTouch* Touch)
{
    Touch->name = "Mouse";
    Touch->x = 0;
    Touch->y = 0;
    Touch->dx = 0;
    Touch->dy = 0;
    Touch->tap = false;

    #if A_CONFIG_INPUT_MOUSE_TRACK
        Touch->motion = a_list_new();
    #endif
}

#if A_CONFIG_INPUT_MOUSE_TRACK
static void touchFree(APlatformTouch* Touch)
{
    a_list_freeEx(Touch->motion, free);
}
#endif

static APlatformController* controllerAdd(int Index)
{
    SDL_Joystick* joystick = NULL;

    #if A_CONFIG_LIB_SDL == 2
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

    #if A_CONFIG_LIB_SDL == 1
        ASdlJoystickId id = (uint8_t)Index;
    #elif A_CONFIG_LIB_SDL == 2
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
    #if A_CONFIG_LIB_SDL == 2
        c->controller = controller;
    #endif
    c->id = id;
    c->numButtons = SDL_JoystickNumButtons(c->joystick);
    c->numHats = SDL_JoystickNumHats(c->joystick);
    c->numAxes = SDL_JoystickNumAxes(c->joystick);

    return c;
}

static void controllerFree(APlatformController* Controller)
{
    for(int id = 0; id < A_BUTTON_NUM; id++) {
        if(Controller->buttons[id]) {
            buttonFree(Controller->buttons[id]);
        }
    }

    for(int id = 0; id < A_AXIS_NUM; id++) {
        if(Controller->axes[id]) {
            analogFree(Controller->axes[id]);
        }
    }

    #if A_CONFIG_LIB_SDL == 1
        if(SDL_JoystickOpened(Controller->id)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #elif A_CONFIG_LIB_SDL == 2
        if(Controller->controller) {
            SDL_GameControllerClose(Controller->controller);
        } else if(SDL_JoystickGetAttached(Controller->joystick)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #endif

    free(Controller);
}

static const char* joystickName(APlatformController* Controller)
{
    #if A_CONFIG_LIB_SDL == 1
        return SDL_JoystickName(Controller->id);
    #elif A_CONFIG_LIB_SDL == 2
        return SDL_JoystickName(Controller->joystick);
    #endif
}

void a_platform_sdl_input__init(void)
{
    #if A_CONFIG_LIB_SDL == 1
        g_sdlFlags = SDL_INIT_JOYSTICK;
    #elif A_CONFIG_LIB_SDL == 2
        g_sdlFlags = SDL_INIT_GAMECONTROLLER;
    #endif

    if(SDL_InitSubSystem(g_sdlFlags) != 0) {
        A__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_controllers = a_list_new();
    g_setController = NULL;
    g_forwardButtonsQueue[0] = a_list_new();
    g_forwardButtonsQueue[1] = a_list_new();

    const int joysticksNum = SDL_NumJoysticks();
    a_out__message("Found %d controllers", joysticksNum);

    #if A_CONFIG_LIB_SDL == 2
        if(joysticksNum > 0) {
            const char* mFile = A_CONFIG_LIB_SDL_GAMEPADMAP;
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

        #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
            if(i == 0) {
                // Joystick 0 is the built-in controls on these platforms
                #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ
                    buttonAdd(c, A_BUTTON_UP, "Up", 0);
                    buttonAdd(c, A_BUTTON_DOWN, "Down", 4);
                    buttonAdd(c, A_BUTTON_LEFT, "Left", 2);
                    buttonAdd(c, A_BUTTON_RIGHT, "Right", 6);
                    buttonAdd(c, A_BUTTON_UPLEFT, "Up-Left", 1);
                    buttonAdd(c, A_BUTTON_UPRIGHT, "Up-Right", 7);
                    buttonAdd(c, A_BUTTON_DOWNLEFT, "Down-Left", 3);
                    buttonAdd(c, A_BUTTON_DOWNRIGHT, "Down-Right", 5);
                    buttonAdd(c, A_BUTTON_L, "L", 10);
                    buttonAdd(c, A_BUTTON_R, "R", 11);
                    buttonAdd(c, A_BUTTON_X, "A", 12);
                    buttonAdd(c, A_BUTTON_B, "B", 13);
                    buttonAdd(c, A_BUTTON_A, "X", 14);
                    buttonAdd(c, A_BUTTON_Y, "Y", 15);
                    buttonAdd(c, A_BUTTON_SELECT, "Select", 9);
                    buttonAdd(c, A_BUTTON_VOLUP, "Vol-Up", 16);
                    buttonAdd(c, A_BUTTON_VOLDOWN, "Vol-Down", 17);
                    #if A_CONFIG_SYSTEM_GP2X
                        buttonAdd(c, A_BUTTON_START, "Start", 8);
                        buttonAdd(c, A_BUTTON_STICKCLICK, "Stick-Click", 18);
                    #elif A_CONFIG_SYSTEM_WIZ
                        buttonAdd(c, A_BUTTON_START, "Menu", 8);
                    #endif
                #elif A_CONFIG_SYSTEM_CAANOO
                    buttonAdd(c, A_BUTTON_UP, "Up", -1);
                    buttonAdd(c, A_BUTTON_DOWN, "Down", -1);
                    buttonAdd(c, A_BUTTON_LEFT, "Left", -1);
                    buttonAdd(c, A_BUTTON_RIGHT, "Right", -1);
                    buttonAdd(c, A_BUTTON_L, "L", 4);
                    buttonAdd(c, A_BUTTON_R, "R", 5);
                    buttonAdd(c, A_BUTTON_X, "A", 0);
                    buttonAdd(c, A_BUTTON_B, "B", 2);
                    buttonAdd(c, A_BUTTON_A, "X", 1);
                    buttonAdd(c, A_BUTTON_Y, "Y", 3);
                    buttonAdd(c, A_BUTTON_GUIDE, "Home", 6);
                    buttonAdd(c, A_BUTTON_HOLD, "Hold", 7);
                    buttonAdd(c, A_BUTTON_START, "I", 8);
                    buttonAdd(c, A_BUTTON_SELECT, "II", 9);
                    analogAdd(c, A_AXIS_LEFTX, "Stick", 0);
                    analogAdd(c, A_AXIS_LEFTY, "Stick", 1);
                #endif
                continue;
            }
        #elif A_CONFIG_SYSTEM_PANDORA
            const char* name = joystickName(c);

            // Check if this is one of the built-in nubs
            if(a_str_equal(name, "nub0")) {
                analogAdd(c, A_AXIS_LEFTX, "Left Stick", 0);
                analogAdd(c, A_AXIS_LEFTY, "Left Stick", 1);

                buttonAdd(c, A_BUTTON_UP, "Up", -1);
                buttonAdd(c, A_BUTTON_DOWN, "Down", -1);
                buttonAdd(c, A_BUTTON_LEFT, "Left", -1);
                buttonAdd(c, A_BUTTON_RIGHT, "Right", -1);
                buttonAdd(c, A_BUTTON_L, "L", -1);
                buttonAdd(c, A_BUTTON_R, "R", -1);
                buttonAdd(c, A_BUTTON_A, "A", -1);
                buttonAdd(c, A_BUTTON_B, "B", -1);
                buttonAdd(c, A_BUTTON_X, "X", -1);
                buttonAdd(c, A_BUTTON_Y, "Y", -1);
                buttonAdd(c, A_BUTTON_START, "Start", -1);
                buttonAdd(c, A_BUTTON_SELECT, "Select", -1);

                continue;
            } else if(a_str_equal(name, "nub1")) {
                analogAdd(c, A_AXIS_RIGHTX, "Right Stick",  0);
                analogAdd(c, A_AXIS_RIGHTY, "Right Stick", 1);

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

#if A_CONFIG_LIB_SDL == 2
        if(c->controller) {
            a_out__message("Mapped %s: %d buttons, %d axes, %d hats",
                           SDL_GameControllerName(c->controller),
                           c->numButtons,
                           c->numAxes,
                           c->numHats);

            static const struct {
                AButtonId id;
                const char* name;
            } buttons[SDL_CONTROLLER_BUTTON_MAX] = {
                [SDL_CONTROLLER_BUTTON_A] = {A_BUTTON_A, "A"},
                [SDL_CONTROLLER_BUTTON_B] = {A_BUTTON_B, "B"},
                [SDL_CONTROLLER_BUTTON_X] = {A_BUTTON_X, "X"},
                [SDL_CONTROLLER_BUTTON_Y] = {A_BUTTON_Y, "Y"},
                [SDL_CONTROLLER_BUTTON_BACK] = {A_BUTTON_SELECT, "Select"},
                [SDL_CONTROLLER_BUTTON_GUIDE] = {A_BUTTON_GUIDE, "Guide"},
                [SDL_CONTROLLER_BUTTON_START] = {A_BUTTON_START, "Start"},
                [SDL_CONTROLLER_BUTTON_LEFTSTICK] = {A_BUTTON_STICKCLICK, "Left Stick Click"},
                [SDL_CONTROLLER_BUTTON_RIGHTSTICK] = {A_BUTTON_INVALID, "Right Stick Click"},
                [SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = {A_BUTTON_L, "L"},
                [SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = {A_BUTTON_R, "R"},
                [SDL_CONTROLLER_BUTTON_DPAD_UP] = {A_BUTTON_UP, "Up"},
                [SDL_CONTROLLER_BUTTON_DPAD_DOWN] = {A_BUTTON_DOWN, "Down"},
                [SDL_CONTROLLER_BUTTON_DPAD_LEFT] = {A_BUTTON_LEFT, "Left"},
                [SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = {A_BUTTON_RIGHT, "Right"},
            };

            for(SDL_GameControllerButton b = SDL_CONTROLLER_BUTTON_A;
                b < SDL_CONTROLLER_BUTTON_MAX;
                b++) {

                if(buttons[b].name == NULL) {
                    continue;
                }

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForButton(c->controller, b);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    buttonAdd(c, buttons[b].id, buttons[b].name, b);
                }
            }

            static const struct {
                AAxisId id;
                const char* name;
            } axes[SDL_CONTROLLER_AXIS_MAX] = {
                [SDL_CONTROLLER_AXIS_LEFTX] = {A_AXIS_LEFTX, "Left Stick"},
                [SDL_CONTROLLER_AXIS_LEFTY] = {A_AXIS_LEFTY, "Left Stick"},
                [SDL_CONTROLLER_AXIS_RIGHTX] = {A_AXIS_RIGHTX, "Right Stick"},
                [SDL_CONTROLLER_AXIS_RIGHTY] = {A_AXIS_RIGHTY, "Right Stick"},
                [SDL_CONTROLLER_AXIS_TRIGGERLEFT] = {A_AXIS_LEFTTRIGGER, "Left Trigger"},
                [SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = {A_AXIS_RIGHTTRIGGER, "Right Trigger"},
            };

            for(SDL_GameControllerAxis a = SDL_CONTROLLER_AXIS_LEFTX;
                a < SDL_CONTROLLER_AXIS_MAX;
                a++) {

                if(axes[a].name == NULL) {
                    continue;
                }

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForAxis(c->controller, a);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    analogAdd(c, axes[a].id, axes[a].name, a);
                }
            }
        } else {
#endif
            a_out__message("Found %s: %d buttons, %d axes, %d hats",
                           joystickName(c),
                           c->numButtons,
                           c->numAxes,
                           c->numHats);

            static struct {
                AButtonId id;
                const char* name;
            } buttons[] = {
                {A_BUTTON_A, "A"},
                {A_BUTTON_B, "B"},
                {A_BUTTON_X, "X"},
                {A_BUTTON_Y, "Y"},
                {A_BUTTON_L, "L"},
                {A_BUTTON_R, "R"},
                {A_BUTTON_SELECT, "Select"},
                {A_BUTTON_START, "Start"},
                {A_BUTTON_GUIDE, "Guide"},
            };

            for(int j = a_math_min(c->numButtons, A_ARRAY_LEN(buttons)); j--; ) {
                buttonAdd(c, buttons[j].id, buttons[j].name, j);
            }

            static const char* axes[A_AXIS_NUM] = {
                [A_AXIS_LEFTX] = "Left Stick",
                [A_AXIS_LEFTY] = "Left Stick",
                [A_AXIS_RIGHTX] = "Right Stick",
                [A_AXIS_RIGHTY] = "Right Stick",
                [A_AXIS_LEFTTRIGGER] = "Left Trigger",
                [A_AXIS_RIGHTTRIGGER] = "Right Trigger",
            };

            for(int id = a_math_min(c->numAxes, A_AXIS_NUM); id--; ) {
                analogAdd(c, id, axes[id], id);
            }
#if A_CONFIG_LIB_SDL == 2
        }
#endif

        if(c->numHats > 0 || c->numAxes >= 2) {
            // These buttons will be controlled by hats and analog axes
            buttonAdd(c, A_BUTTON_UP, "Up", -1);
            buttonAdd(c, A_BUTTON_DOWN, "Down", -1);
            buttonAdd(c, A_BUTTON_LEFT, "Left", -1);
            buttonAdd(c, A_BUTTON_RIGHT, "Right", -1);
            buttonAdd(c, A_BUTTON_L, "L", -1);
            buttonAdd(c, A_BUTTON_R, "R", -1);
        }
    }

    #if A_CONFIG_LIB_SDL == 1
        #define keyAdd(Id, Scancode, Keycode) keyAdd(Id, Keycode)
    #elif A_CONFIG_LIB_SDL == 2
        #define keyAdd(Id, Scancode, Keycode) keyAdd(Id, Scancode)
    #endif

    #if A_CONFIG_TRAIT_KEYBOARD
        keyAdd(A_KEY_UP, SDL_SCANCODE_UP, SDLK_UP);
        keyAdd(A_KEY_DOWN, SDL_SCANCODE_DOWN, SDLK_DOWN);
        keyAdd(A_KEY_LEFT, SDL_SCANCODE_LEFT, SDLK_LEFT);
        keyAdd(A_KEY_RIGHT, SDL_SCANCODE_RIGHT, SDLK_RIGHT);
        keyAdd(A_KEY_Z, SDL_SCANCODE_Z, SDLK_z);
        keyAdd(A_KEY_X, SDL_SCANCODE_X, SDLK_x);
        keyAdd(A_KEY_C, SDL_SCANCODE_C, SDLK_c);
        keyAdd(A_KEY_V, SDL_SCANCODE_V, SDLK_v);
        keyAdd(A_KEY_M, SDL_SCANCODE_M, SDLK_m);
        keyAdd(A_KEY_ENTER, SDL_SCANCODE_RETURN, SDLK_RETURN);
        keyAdd(A_KEY_SPACE, SDL_SCANCODE_SPACE, SDLK_SPACE);
        keyAdd(A_KEY_HOME, SDL_SCANCODE_HOME, SDLK_HOME);
        keyAdd(A_KEY_END, SDL_SCANCODE_END, SDLK_END);
        keyAdd(A_KEY_PAGEUP, SDL_SCANCODE_PAGEUP, SDLK_PAGEUP);
        keyAdd(A_KEY_PAGEDOWN, SDL_SCANCODE_PAGEDOWN, SDLK_PAGEDOWN);
        keyAdd(A_KEY_LALT, SDL_SCANCODE_LALT, SDLK_LALT);
        keyAdd(A_KEY_LCTRL, SDL_SCANCODE_LCTRL, SDLK_LCTRL);
        keyAdd(A_KEY_LSHIFT, SDL_SCANCODE_LSHIFT, SDLK_LSHIFT);
        keyAdd(A_KEY_RALT, SDL_SCANCODE_RALT, SDLK_RALT);
        keyAdd(A_KEY_RCTRL, SDL_SCANCODE_RCTRL, SDLK_RCTRL);
        keyAdd(A_KEY_RSHIFT, SDL_SCANCODE_RSHIFT, SDLK_RSHIFT);
        keyAdd(A_KEY_F1, SDL_SCANCODE_F1, SDLK_F1);
        keyAdd(A_KEY_F2, SDL_SCANCODE_F2, SDLK_F2);
        keyAdd(A_KEY_F3, SDL_SCANCODE_F3, SDLK_F3);
        keyAdd(A_KEY_F4, SDL_SCANCODE_F4, SDLK_F4);
        keyAdd(A_KEY_F5, SDL_SCANCODE_F5, SDLK_F5);
        keyAdd(A_KEY_F6, SDL_SCANCODE_F6, SDLK_F6);
        keyAdd(A_KEY_F7, SDL_SCANCODE_F7, SDLK_F7);
        keyAdd(A_KEY_F8, SDL_SCANCODE_F8, SDLK_F8);
        keyAdd(A_KEY_F9, SDL_SCANCODE_F9, SDLK_F9);
        keyAdd(A_KEY_F10, SDL_SCANCODE_F10, SDLK_F10);
        keyAdd(A_KEY_F11, SDL_SCANCODE_F11, SDLK_F11);
        keyAdd(A_KEY_F12, SDL_SCANCODE_F12, SDLK_F12);
    #endif

    touchAdd(&g_mouse);
}

void a_platform_sdl_input__uninit(void)
{
    #if A_CONFIG_TRAIT_KEYBOARD
        for(int id = 0; id < A__KEY_ID(A_KEY_NUM); id++) {
            if(g_keys[id]) {
                buttonFree(g_keys[id]);
            }
        }
    #endif

    #if A_CONFIG_INPUT_MOUSE_TRACK
        touchFree(&g_mouse);
    #endif

    a_list_freeEx(g_controllers, (AFree*)controllerFree);
    a_list_free(g_forwardButtonsQueue[0]);
    a_list_free(g_forwardButtonsQueue[1]);

    SDL_QuitSubSystem(g_sdlFlags);
}

void a_platform__inputsPoll(void)
{
    g_mouse.tap = false;

    #if A_CONFIG_INPUT_MOUSE_TRACK
        a_list_clearEx(g_mouse.motion, free);
    #endif

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        switch(event.type) {
            case SDL_QUIT: {
                a_state_exit();
            } break;

#if A_CONFIG_TRAIT_KEYBOARD
            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                #if !A_CONFIG_SYSTEM_EMSCRIPTEN
                    if(event.key.keysym.sym == SDLK_ESCAPE) {
                        a_state_exit();
                        break;
                    }
                #endif

                for(int id = 0; id < A__KEY_ID(A_KEY_NUM); id++) {
#if A_CONFIG_LIB_SDL == 1
                    if(g_keys[id]->code.keyCode == event.key.keysym.sym) {
#elif A_CONFIG_LIB_SDL == 2
                    if(g_keys[id]->code.keyCode == event.key.keysym.scancode) {
#endif
                        buttonPress(g_keys[id], event.key.state == SDL_PRESSED);
                        break;
                    }
                }
            } break;
#endif

            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    #if A_CONFIG_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jbutton.which) {
                        continue;
                    }

                    for(int id = 0; id < A_BUTTON_NUM; id++) {
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
                    #if A_CONFIG_LIB_SDL == 2
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
                    #if A_CONFIG_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jaxis.which) {
                        continue;
                    }

                    for(int id = 0; id < A_AXIS_NUM; id++) {
                        APlatformAnalog* a = c->axes[id];

                        if(a && a->axisIndex == event.jaxis.axis) {
                            analogSet(a, event.jaxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;

#if A_CONFIG_LIB_SDL == 2
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                A_LIST_ITERATE(g_controllers, APlatformController*, c) {
                    if(c->controller == NULL || c->id != event.cbutton.which) {
                        continue;
                    }

                    for(int id = 0; id < A_BUTTON_NUM; id++) {
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

                    for(int id = 0; id < A_AXIS_NUM; id++) {
                        APlatformAnalog* a = c->axes[id];

                        if(a && a->axisIndex == event.caxis.axis) {
                            analogSet(a, event.caxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;
#endif

            case SDL_MOUSEMOTION: {
                g_mouse.x = event.button.x;
                g_mouse.y = event.button.y;

                #if A_CONFIG_INPUT_MOUSE_TRACK
                    ATouchPoint* p = a_mem_malloc(sizeof(ATouchPoint));

                    p->x = g_mouse.x;
                    p->y = g_mouse.y;

                    a_list_addLast(g_mouse.motion, p);
                #endif
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        g_mouse.x = event.button.x;
                        g_mouse.y = event.button.y;
                        g_mouse.tap = true;
                    } break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        g_mouse.x = event.button.x;
                        g_mouse.y = event.button.y;
                        g_mouse.tap = false;
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

    #if !A_CONFIG_SYSTEM_EMSCRIPTEN
        int mouseDx = 0, mouseDy = 0;
        SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

        g_mouse.dx = mouseDx;
        g_mouse.dy = mouseDy;
    #endif
}

APlatformButton* a_platform__buttonGet(int Id)
{
    if(Id != A_BUTTON_INVALID) {
        if(Id & A__KEY_FLAG) {
            #if A_CONFIG_TRAIT_KEYBOARD
                return g_keys[A__KEY_ID(Id)];
            #endif
        } else if(g_setController) {
            return g_setController->buttons[Id];
        }
    }

    return NULL;
}

const char* a_platform__buttonNameGet(const APlatformButton* Button)
{
    return Button->name;
}

bool a_platform__buttonPressGet(const APlatformButton* Button)
{
    return Button->pressed;
}

void a_platform__buttonForward(int Source, int Destination)
{
    APlatformButton* bSrc = a_platform__buttonGet(Source);
    APlatformButton* bDst = a_platform__buttonGet(Destination);

    if(bSrc == NULL || bDst == NULL) {
        return;
    }

    if(bSrc->forwardButtons == NULL) {
        bSrc->forwardButtons = a_list_new();
    }

    a_list_addLast(bSrc->forwardButtons, bDst);
}

APlatformAnalog* a_platform__analogGet(int Id)
{
    if(Id != A_AXIS_INVALID) {
        const APlatformController* c = g_setController;

        while(c) {
            APlatformAnalog* a = c->axes[Id];

            if(a != NULL) {
                return a;
            }

            c = c->next;
        }
    }

    return NULL;
}

const char* a_platform__analogNameGet(const APlatformAnalog* Analog)
{
    return Analog->name;
}

int a_platform__analogValueGet(const APlatformAnalog* Analog)
{
    return Analog->value;
}

void a_platform__analogForward(AAxisId Source, AButtonId Negative, AButtonId Positive)
{
    APlatformAnalog* aSrc = a_platform__analogGet(Source);

    if(aSrc == NULL) {
        return;
    }

    APlatformButtonPair* f = a_mem_malloc(sizeof(APlatformButtonPair));

    f->negative = a_platform__buttonGet(Negative);
    f->positive = a_platform__buttonGet(Positive);
    f->lastPressedNegative = false;
    f->lastPressedPositive = false;

    if(aSrc->forwardButtons == NULL) {
        aSrc->forwardButtons = a_list_new();
    }

    a_list_addLast(aSrc->forwardButtons, f);
}

APlatformTouch* a_platform__touchGet(void)
{
    return &g_mouse;
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
        A__FATAL("Cannot set controller %d, %d total",
                 Index,
                 a_list_sizeGet(g_controllers));
    }

    g_setController = a_list_getByIndex(g_controllers, Index);
}

bool a_platform__controllerIsMapped(void)
{
    #if A_CONFIG_LIB_SDL == 2
        return g_setController && g_setController->controller != NULL;
    #else
        return false;
    #endif
}
#endif // A_CONFIG_LIB_SDL
