/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_sdl_input.v.h"
#include <a2x.v.h>

#if A_CONFIG_LIB_SDL
#if A_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
#elif A_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
#endif

struct APlatformButton {
    AList* forwardButtons; // list of APlatformButton or NULL
    union {
        #if A_CONFIG_LIB_SDL == 1
            SDLKey keyCode;
        #elif A_CONFIG_LIB_SDL == 2
            SDL_Scancode keyCode;
        #endif
        uint8_t buttonIndex;
        int code;
    } code;
    unsigned lastEventTick;
    bool lastHatEventPressed;
    bool pressed;
};

struct APlatformAnalog {
    AList* forwardButtons; // list of APlatformButtonPair or NULL
    int axisIndex;
    int value;
};

struct APlatformController {
    #if A_CONFIG_SYSTEM_PANDORA
        APlatformController* next;
    #endif
    SDL_Joystick* joystick;
    #if A_CONFIG_LIB_SDL == 1
        uint8_t id;
    #elif A_CONFIG_LIB_SDL == 2
        SDL_JoystickID id;
        SDL_GameController* controller;
    #endif
    int numButtons;
    int numHats;
    int numAxes;
    APlatformButton* buttons[A_BUTTON_NUM];
    APlatformAnalog* axes[A_AXIS_NUM];
};

typedef struct {
    APlatformButton* negative;
    APlatformButton* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} APlatformButtonPair;

static struct {
    AVectorInt coords;
    AVectorInt delta;
    bool tap;
} g_mouse;

static AList* g_controllers;
static APlatformController* g_setController;
static AList* g_forwardButtonsQueue[2]; // list of APlatformButton
static uint32_t g_sdlFlags;

static const AButtonId g_defaultOrder[] = {
    A_BUTTON_A,
    A_BUTTON_B,
    A_BUTTON_X,
    A_BUTTON_Y,
    A_BUTTON_L,
    A_BUTTON_R,
    A_BUTTON_SELECT,
    A_BUTTON_START,
    A_BUTTON_GUIDE,
};

#if A_CONFIG_LIB_SDL == 2
static const AButtonId g_buttonsMap[SDL_CONTROLLER_BUTTON_MAX] = {
    [SDL_CONTROLLER_BUTTON_A] = A_BUTTON_A,
    [SDL_CONTROLLER_BUTTON_B] = A_BUTTON_B,
    [SDL_CONTROLLER_BUTTON_X] = A_BUTTON_X,
    [SDL_CONTROLLER_BUTTON_Y] = A_BUTTON_Y,
    [SDL_CONTROLLER_BUTTON_BACK] = A_BUTTON_SELECT,
    [SDL_CONTROLLER_BUTTON_GUIDE] = A_BUTTON_GUIDE,
    [SDL_CONTROLLER_BUTTON_START] = A_BUTTON_START,
    [SDL_CONTROLLER_BUTTON_LEFTSTICK] = A_BUTTON_STICKCLICK,
    [SDL_CONTROLLER_BUTTON_RIGHTSTICK] = A_BUTTON_INVALID,
    [SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = A_BUTTON_L,
    [SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = A_BUTTON_R,
    [SDL_CONTROLLER_BUTTON_DPAD_UP] = A_BUTTON_UP,
    [SDL_CONTROLLER_BUTTON_DPAD_DOWN] = A_BUTTON_DOWN,
    [SDL_CONTROLLER_BUTTON_DPAD_LEFT] = A_BUTTON_LEFT,
    [SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = A_BUTTON_RIGHT,
};

static const AAnalogId g_axesMap[SDL_CONTROLLER_AXIS_MAX] = {
    [SDL_CONTROLLER_AXIS_LEFTX] = A_AXIS_LEFTX,
    [SDL_CONTROLLER_AXIS_LEFTY] = A_AXIS_LEFTY,
    [SDL_CONTROLLER_AXIS_RIGHTX] = A_AXIS_RIGHTX,
    [SDL_CONTROLLER_AXIS_RIGHTY] = A_AXIS_RIGHTY,
    [SDL_CONTROLLER_AXIS_TRIGGERLEFT] = A_AXIS_LEFTTRIGGER,
    [SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = A_AXIS_RIGHTTRIGGER,
};
#endif

#if A_CONFIG_TRAIT_KEYBOARD
static APlatformButton* g_keys[A__KEY_ID(A_KEY_NUM)];

static void keyAdd(AKeyId Id, int Code)
{
    if(g_keys[A__KEY_ID(Id)] != NULL) {
        return;
    }

    APlatformButton* k = a_mem_malloc(sizeof(APlatformButton));

    k->forwardButtons = NULL;
    k->code.code = Code;
    k->lastEventTick = a_fps_ticksGet() - 1;
    k->pressed = false;

    g_keys[A__KEY_ID(Id)] = k;
}
#endif

static void buttonAdd(APlatformController* Controller, AButtonId Id, int Code)
{
    if(Controller->buttons[Id] != NULL) {
        return;
    }

    APlatformButton* b = a_mem_malloc(sizeof(APlatformButton));

    b->forwardButtons = NULL;
    b->code.code = Code;
    b->lastEventTick = a_fps_ticksGet() - 1;
    b->lastHatEventPressed = false;
    b->pressed = false;

    Controller->buttons[Id] = b;
}

static void buttonFree(APlatformButton* Button)
{
    if(Button == NULL) {
        return;
    }

    a_list_free(Button->forwardButtons);

    a_mem_free(Button);
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

#if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_PANDORA
static void buttonForward(int Source, int Destination)
{
    APlatformButton* bSrc = a_platform_api__inputButtonGet(Source);
    APlatformButton* bDst = a_platform_api__inputButtonGet(Destination);

    if(bSrc == NULL || bDst == NULL) {
        return;
    }

    if(bSrc->forwardButtons == NULL) {
        bSrc->forwardButtons = a_list_new();
    }

    a_list_addLast(bSrc->forwardButtons, bDst);
}
#endif

static void analogAdd(APlatformController* Controller, AAnalogId Id, int AxisIndex)
{
    if(Controller->axes[Id] != NULL) {
        return;
    }

    APlatformAnalog* a = a_mem_malloc(sizeof(APlatformAnalog));

    a->forwardButtons = NULL;
    a->axisIndex = AxisIndex;
    a->value = 0;

    Controller->axes[Id] = a;
}

static void analogFree(APlatformAnalog* Analog)
{
    if(Analog == NULL) {
        return;
    }

    a_list_freeEx(Analog->forwardButtons, a_mem_free);

    a_mem_free(Analog);
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

static void analogForward(AAnalogId Source, AButtonId Negative, AButtonId Positive)
{
    APlatformAnalog* aSrc = a_platform_api__inputAnalogGet(Source);

    if(aSrc == NULL) {
        return;
    }

    APlatformButtonPair* f = a_mem_malloc(sizeof(APlatformButtonPair));

    f->negative = a_platform_api__inputButtonGet(Negative);
    f->positive = a_platform_api__inputButtonGet(Positive);
    f->lastPressedNegative = false;
    f->lastPressedPositive = false;

    if(aSrc->forwardButtons == NULL) {
        aSrc->forwardButtons = a_list_new();
    }

    a_list_addLast(aSrc->forwardButtons, f);
}

static inline const char* joystickName(APlatformController* Controller)
{
    #if A_CONFIG_LIB_SDL == 1
        return SDL_JoystickName(Controller->id);
    #elif A_CONFIG_LIB_SDL == 2
        return SDL_JoystickName(Controller->joystick);
    #endif
}

static APlatformController* controllerAdd(int Index)
{
    SDL_Joystick* joystick = NULL;

    #if A_CONFIG_LIB_SDL == 2
        char guidStrBuffer[64];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(Index),
                                  guidStrBuffer,
                                  sizeof(guidStrBuffer) - 1);
        a_out__info("New controller %s", guidStrBuffer);

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
        uint8_t id = (uint8_t)Index;
    #elif A_CONFIG_LIB_SDL == 2
        SDL_JoystickID id = SDL_JoystickInstanceID(joystick);

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

    g_setController = c;

    c->joystick = joystick;
    #if A_CONFIG_LIB_SDL == 2
        c->controller = controller;
    #endif
    c->id = id;
    c->numButtons = SDL_JoystickNumButtons(c->joystick);
    c->numHats = SDL_JoystickNumHats(c->joystick);
    c->numAxes = SDL_JoystickNumAxes(c->joystick);

    a_out__info("Controller '%s': %d buttons, %d axes, %d hats",
                joystickName(c),
                c->numButtons,
                c->numAxes,
                c->numHats);

    bool mappedBuiltIn = false;

    #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO
        if(Index == 0) {
            mappedBuiltIn = true;

            // Joystick 0 is the built-in controls on these platforms
            #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ
                buttonAdd(c, A_BUTTON_UP, 0);
                buttonAdd(c, A_BUTTON_DOWN, 4);
                buttonAdd(c, A_BUTTON_LEFT, 2);
                buttonAdd(c, A_BUTTON_RIGHT, 6);
                buttonAdd(c, A_BUTTON_UPLEFT, 1);
                buttonAdd(c, A_BUTTON_UPRIGHT, 7);
                buttonAdd(c, A_BUTTON_DOWNLEFT, 3);
                buttonAdd(c, A_BUTTON_DOWNRIGHT, 5);
                buttonAdd(c, A_BUTTON_L, 10);
                buttonAdd(c, A_BUTTON_R, 11);
                buttonAdd(c, A_BUTTON_X, 12);
                buttonAdd(c, A_BUTTON_B, 13);
                buttonAdd(c, A_BUTTON_A, 14);
                buttonAdd(c, A_BUTTON_Y, 15);
                buttonAdd(c, A_BUTTON_SELECT, 9);
                buttonAdd(c, A_BUTTON_VOLUP, 16);
                buttonAdd(c, A_BUTTON_VOLDOWN, 17);
                buttonAdd(c, A_BUTTON_START, 8);
                #if A_CONFIG_SYSTEM_GP2X
                    buttonAdd(c, A_BUTTON_STICKCLICK, 18);
                #elif A_CONFIG_SYSTEM_WIZ
                    buttonAdd(c, A_BUTTON_START, 8);
                #endif

                // Split diagonals into individual cardinal directions
                buttonForward(A_BUTTON_UPLEFT, A_BUTTON_UP);
                buttonForward(A_BUTTON_UPLEFT, A_BUTTON_LEFT);
                buttonForward(A_BUTTON_UPRIGHT, A_BUTTON_UP);
                buttonForward(A_BUTTON_UPRIGHT, A_BUTTON_RIGHT);
                buttonForward(A_BUTTON_DOWNLEFT, A_BUTTON_DOWN);
                buttonForward(A_BUTTON_DOWNLEFT, A_BUTTON_LEFT);
                buttonForward(A_BUTTON_DOWNRIGHT, A_BUTTON_DOWN);
                buttonForward(A_BUTTON_DOWNRIGHT, A_BUTTON_RIGHT);
            #elif A_CONFIG_SYSTEM_CAANOO
                buttonAdd(c, A_BUTTON_UP, -1);
                buttonAdd(c, A_BUTTON_DOWN, -1);
                buttonAdd(c, A_BUTTON_LEFT, -1);
                buttonAdd(c, A_BUTTON_RIGHT, -1);
                buttonAdd(c, A_BUTTON_L, 4);
                buttonAdd(c, A_BUTTON_R, 5);
                buttonAdd(c, A_BUTTON_X, 0);
                buttonAdd(c, A_BUTTON_B, 2);
                buttonAdd(c, A_BUTTON_A, 1);
                buttonAdd(c, A_BUTTON_Y, 3);
                buttonAdd(c, A_BUTTON_GUIDE, 6);
                buttonAdd(c, A_BUTTON_HOLD, 7);
                buttonAdd(c, A_BUTTON_START, 8);
                buttonAdd(c, A_BUTTON_SELECT, 9);

                analogAdd(c, A_AXIS_LEFTX, 0);
                analogAdd(c, A_AXIS_LEFTY, 1);
            #endif
        }
    #elif A_CONFIG_SYSTEM_PANDORA
        const char* name = joystickName(c);

        // Check if this is one of the built-in nubs
        if(a_str_equal(name, "nub0")) {
            mappedBuiltIn = true;

            analogAdd(c, A_AXIS_LEFTX, 0);
            analogAdd(c, A_AXIS_LEFTY, 1);

            buttonAdd(c, A_BUTTON_UP, -1);
            buttonAdd(c, A_BUTTON_DOWN, -1);
            buttonAdd(c, A_BUTTON_LEFT, -1);
            buttonAdd(c, A_BUTTON_RIGHT, -1);
            buttonAdd(c, A_BUTTON_L, -1);
            buttonAdd(c, A_BUTTON_R, -1);
            buttonAdd(c, A_BUTTON_A, -1);
            buttonAdd(c, A_BUTTON_B, -1);
            buttonAdd(c, A_BUTTON_X, -1);
            buttonAdd(c, A_BUTTON_Y, -1);
            buttonAdd(c, A_BUTTON_START, -1);
            buttonAdd(c, A_BUTTON_SELECT, -1);

            // Pandora's game buttons are actually keyboard keys
            buttonForward(A_KEY_UP, A_BUTTON_UP);
            buttonForward(A_KEY_DOWN, A_BUTTON_DOWN);
            buttonForward(A_KEY_LEFT, A_BUTTON_LEFT);
            buttonForward(A_KEY_RIGHT, A_BUTTON_RIGHT);
            buttonForward(A_KEY_RSHIFT, A_BUTTON_L);
            buttonForward(A_KEY_RCTRL, A_BUTTON_R);
            buttonForward(A_KEY_PAGEDOWN, A_BUTTON_A);
            buttonForward(A_KEY_END, A_BUTTON_B);
            buttonForward(A_KEY_HOME, A_BUTTON_X);
            buttonForward(A_KEY_PAGEUP, A_BUTTON_Y);
            buttonForward(A_KEY_LALT, A_BUTTON_START);
            buttonForward(A_KEY_LCTRL, A_BUTTON_SELECT);
        } else if(a_str_equal(name, "nub1")) {
            mappedBuiltIn = true;

            analogAdd(c, A_AXIS_RIGHTX, 0);
            analogAdd(c, A_AXIS_RIGHTY, 1);

            // Attach to nub0 to compose a single dual-analog controller
            A_LIST_ITERATE(g_controllers, APlatformController*, nub0) {
                if(a_str_equal(joystickName(nub0), "nub0")) {
                    nub0->next = c;
                    break;
                }
            }
        }
    #endif

    if(!mappedBuiltIn) {
#if A_CONFIG_LIB_SDL == 2
        if(c->controller) {
            a_out__info(
                "Mapped as '%s'", SDL_GameControllerName(c->controller));

            for(SDL_GameControllerButton b = SDL_CONTROLLER_BUTTON_A;
                b < SDL_CONTROLLER_BUTTON_MAX;
                b++) {

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForButton(c->controller, b);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    buttonAdd(c, g_buttonsMap[b], b);
                }
            }

            for(SDL_GameControllerAxis a = SDL_CONTROLLER_AXIS_LEFTX;
                a < SDL_CONTROLLER_AXIS_MAX;
                a++) {

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForAxis(c->controller, a);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    analogAdd(c, g_axesMap[a], a);
                }
            }
        } else {
#endif
            for(int b = a_math_min(c->numButtons, A_ARRAY_LEN(g_defaultOrder));
                b--; ) {

                buttonAdd(c, g_defaultOrder[b], b);
            }

            for(int id = a_math_min(c->numAxes, A_AXIS_NUM); id--; ) {
                analogAdd(c, id, id);
            }

            if(c->numHats > 0 || c->numAxes >= 2) {
                // These buttons will be controlled by hats and analog axes
                buttonAdd(c, A_BUTTON_UP, -1);
                buttonAdd(c, A_BUTTON_DOWN, -1);
                buttonAdd(c, A_BUTTON_LEFT, -1);
                buttonAdd(c, A_BUTTON_RIGHT, -1);
                buttonAdd(c, A_BUTTON_L, -1);
                buttonAdd(c, A_BUTTON_R, -1);
            }
#if A_CONFIG_LIB_SDL == 2
        }
#endif
    }

    // Forward the left analog stick to the direction buttons
    analogForward(A_AXIS_LEFTX, A_BUTTON_LEFT, A_BUTTON_RIGHT);
    analogForward(A_AXIS_LEFTY, A_BUTTON_UP, A_BUTTON_DOWN);

    // Forward analog shoulder triggers to the shoulder buttons
    analogForward(A_AXIS_LEFTTRIGGER, A_BUTTON_INVALID, A_BUTTON_L);
    analogForward(A_AXIS_RIGHTTRIGGER, A_BUTTON_INVALID, A_BUTTON_R);

    return c;
}

static void controllerFree(APlatformController* Controller)
{
    for(int id = 0; id < A_BUTTON_NUM; id++) {
        buttonFree(Controller->buttons[id]);
    }

    for(int id = 0; id < A_AXIS_NUM; id++) {
        analogFree(Controller->axes[id]);
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

    a_mem_free(Controller);
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
    g_forwardButtonsQueue[0] = a_list_new();
    g_forwardButtonsQueue[1] = a_list_new();

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

    const int joysticksNum = SDL_NumJoysticks();
    a_out__info("Found %d controllers", joysticksNum);

    #if A_CONFIG_LIB_SDL == 2
        if(a_path_exists(A_CONFIG_LIB_SDL_GAMEPADMAP, A_PATH_FILE)) {
            int mappingsNum = SDL_GameControllerAddMappingsFromFile(
                                A_CONFIG_LIB_SDL_GAMEPADMAP);

            if(mappingsNum < 0) {
                a_out__error("SDL_GameControllerAddMappingsFromFile(%s): %s",
                             A_CONFIG_LIB_SDL_GAMEPADMAP,
                             SDL_GetError());
            } else {
                a_out__info("Loaded %d controller mappings from '%s'",
                            mappingsNum,
                            A_CONFIG_LIB_SDL_GAMEPADMAP);
            }
        }
    #endif

    for(int j = 0; j < joysticksNum; j++) {
        APlatformController* c = controllerAdd(j);

        if(c) {
            a_list_addLast(g_controllers, c);
        }
    }

    a_platform_api__inputControllerSet(0);
}

void a_platform_sdl_input__uninit(void)
{
    #if A_CONFIG_TRAIT_KEYBOARD
        for(int id = 0; id < A__KEY_ID(A_KEY_NUM); id++) {
            buttonFree(g_keys[id]);
        }
    #endif

    a_list_freeEx(g_controllers, (AFree*)controllerFree);
    a_list_free(g_forwardButtonsQueue[0]);
    a_list_free(g_forwardButtonsQueue[1]);

    SDL_QuitSubSystem(g_sdlFlags);
}

void a_platform_api__inputPoll(void)
{
    g_mouse.tap = false;

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
                g_mouse.coords.x = event.button.x;
                g_mouse.coords.y = event.button.y;
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        g_mouse.coords.x = event.button.x;
                        g_mouse.coords.y = event.button.y;
                        g_mouse.tap = true;
                    } break;
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        g_mouse.coords.x = event.button.x;
                        g_mouse.coords.y = event.button.y;
                        g_mouse.tap = false;
                    } break;
                }
            } break;

            default:break;
        }
    }

    unsigned ticksNow = a_fps_ticksGet();

    A_LIST_ITERATE(g_forwardButtonsQueue[1], APlatformButton*, b) {
        // Overwrite whatever current state with a press
        buttonPress(b, true);
    }

    A_LIST_ITERATE(g_forwardButtonsQueue[0], APlatformButton*, b) {
        // Only release if did not receive an event this frame
        if(b->lastEventTick != ticksNow) {
            buttonPress(b, false);
        }
    }

    a_list_clear(g_forwardButtonsQueue[0]);
    a_list_clear(g_forwardButtonsQueue[1]);

    #if !A_CONFIG_SYSTEM_EMSCRIPTEN
        AVectorInt mouseDelta = {0, 0};
        SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);

        g_mouse.delta = mouseDelta;
    #endif
}

APlatformButton* a_platform_api__inputButtonGet(int Id)
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

bool a_platform_api__inputButtonPressGet(const APlatformButton* Button)
{
    return Button->pressed;
}

APlatformAnalog* a_platform_api__inputAnalogGet(AAnalogId Id)
{
    if(g_setController && Id != A_AXIS_INVALID) {
        #if A_CONFIG_SYSTEM_PANDORA
            for(APlatformController* c = g_setController; c; c = c->next) {
                if(c->axes[Id]) {
                    return c->axes[Id];
                }
            }
        #else
            return g_setController->axes[Id];
        #endif
    }

    return NULL;
}

int a_platform_api__inputAnalogValueGet(const APlatformAnalog* Analog)
{
    return Analog->value;
}

AVectorInt a_platform_api__inputTouchCoordsGet(void)
{
    return g_mouse.coords;
}

AVectorInt a_platform_api__inputTouchDeltaGet(void)
{
    return g_mouse.delta;
}

bool a_platform_api__inputTouchTapGet(void)
{
    return g_mouse.tap;
}

unsigned a_platform_api__inputControllerNumGet(void)
{
    return a_list_sizeGet(g_controllers);
}

void a_platform_api__inputControllerSet(unsigned Index)
{
    g_setController = a_list_getByIndex(g_controllers, Index);
}
#endif // A_CONFIG_LIB_SDL
