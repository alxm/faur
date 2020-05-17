/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_sdl_input.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_SDL
#if F_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
#elif F_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
#endif

struct FPlatformButton {
    FList* forwardButtons; // FList<FPlatformButton*/NULL>
    union {
        #if F_CONFIG_LIB_SDL == 1
            SDLKey keyCode;
        #elif F_CONFIG_LIB_SDL == 2
            SDL_Scancode keyCode;
        #endif
        uint8_t buttonIndex;
        int code;
    } code;
    unsigned lastEventTick;
    bool lastHatEventPressed;
    bool pressed;
};

struct FPlatformAnalog {
    FList* forwardButtons; // FList<FPlatformButtonPair*/NULL>
    int axisIndex;
    int value;
};

struct FPlatformController {
    #if F_CONFIG_SYSTEM_PANDORA
        FPlatformController* next;
    #endif
    SDL_Joystick* joystick;
    #if F_CONFIG_LIB_SDL == 1
        uint8_t id;
    #elif F_CONFIG_LIB_SDL == 2
        SDL_JoystickID id;
        SDL_GameController* controller;
        SDL_JoystickGUID guid;
        FControllerBind* bindCallback;
        bool removed;
    #endif
    bool claimed;
    int numButtons;
    int numHats;
    int numAxes;
    FPlatformButton* buttons[F_BUTTON_NUM];
    FPlatformAnalog* axes[F_AXIS_NUM];
};

typedef struct {
    FPlatformButton* negative;
    FPlatformButton* positive;
    bool lastPressedNegative;
    bool lastPressedPositive;
} FPlatformButtonPair;

static struct {
    FVecInt coords;
    FVecInt delta;
    bool tap;
} g_mouse;

static FPlatformController* g_defaultController; // first to be attached
static FList* g_controllers; // FList<FPlatformController*>
static FList* g_forwardButtonsQueue[2]; // FList<FPlatformButton*>
static uint32_t g_sdlFlags;

static const FButtonId g_defaultOrder[] = {
    F_BUTTON_A,
    F_BUTTON_B,
    F_BUTTON_X,
    F_BUTTON_Y,
    F_BUTTON_L,
    F_BUTTON_R,
    F_BUTTON_SELECT,
    F_BUTTON_START,
    F_BUTTON_GUIDE,
};

#if F_CONFIG_LIB_SDL == 2
static const FButtonId g_buttonsMap[SDL_CONTROLLER_BUTTON_MAX] = {
    [SDL_CONTROLLER_BUTTON_A] = F_BUTTON_A,
    [SDL_CONTROLLER_BUTTON_B] = F_BUTTON_B,
    [SDL_CONTROLLER_BUTTON_X] = F_BUTTON_X,
    [SDL_CONTROLLER_BUTTON_Y] = F_BUTTON_Y,
    [SDL_CONTROLLER_BUTTON_BACK] = F_BUTTON_SELECT,
    [SDL_CONTROLLER_BUTTON_GUIDE] = F_BUTTON_GUIDE,
    [SDL_CONTROLLER_BUTTON_START] = F_BUTTON_START,
    [SDL_CONTROLLER_BUTTON_LEFTSTICK] = F_BUTTON_STICKCLICK,
    [SDL_CONTROLLER_BUTTON_RIGHTSTICK] = F_BUTTON_INVALID,
    [SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = F_BUTTON_L,
    [SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = F_BUTTON_R,
    [SDL_CONTROLLER_BUTTON_DPAD_UP] = F_BUTTON_UP,
    [SDL_CONTROLLER_BUTTON_DPAD_DOWN] = F_BUTTON_DOWN,
    [SDL_CONTROLLER_BUTTON_DPAD_LEFT] = F_BUTTON_LEFT,
    [SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = F_BUTTON_RIGHT,
};

static const FAnalogId g_axesMap[SDL_CONTROLLER_AXIS_MAX] = {
    [SDL_CONTROLLER_AXIS_LEFTX] = F_AXIS_LEFTX,
    [SDL_CONTROLLER_AXIS_LEFTY] = F_AXIS_LEFTY,
    [SDL_CONTROLLER_AXIS_RIGHTX] = F_AXIS_RIGHTX,
    [SDL_CONTROLLER_AXIS_RIGHTY] = F_AXIS_RIGHTY,
    [SDL_CONTROLLER_AXIS_TRIGGERLEFT] = F_AXIS_LEFTTRIGGER,
    [SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = F_AXIS_RIGHTTRIGGER,
};

static FList* g_futureControllers; // FList<FPlatformController*>
#endif

#if F_CONFIG_TRAIT_KEYBOARD
static FPlatformButton* g_keys[F_KEY_NUM];

#if F_CONFIG_LIB_SDL == 1
    #define F__KEY_CODE(Scancode, Keycode) (Keycode)
#elif F_CONFIG_LIB_SDL == 2
    #define F__KEY_CODE(Scancode, Keycode) (Scancode)
#endif

static const int g_keysMap[F_KEY_NUM] = {
    [F_KEY_UP] = F__KEY_CODE(SDL_SCANCODE_UP, SDLK_UP),
    [F_KEY_DOWN] = F__KEY_CODE(SDL_SCANCODE_DOWN, SDLK_DOWN),
    [F_KEY_LEFT] = F__KEY_CODE(SDL_SCANCODE_LEFT, SDLK_LEFT),
    [F_KEY_RIGHT] = F__KEY_CODE(SDL_SCANCODE_RIGHT, SDLK_RIGHT),
    [F_KEY_Z] = F__KEY_CODE(SDL_SCANCODE_Z, SDLK_z),
    [F_KEY_X] = F__KEY_CODE(SDL_SCANCODE_X, SDLK_x),
    [F_KEY_C] = F__KEY_CODE(SDL_SCANCODE_C, SDLK_c),
    [F_KEY_V] = F__KEY_CODE(SDL_SCANCODE_V, SDLK_v),
    [F_KEY_M] = F__KEY_CODE(SDL_SCANCODE_M, SDLK_m),
    [F_KEY_ENTER] = F__KEY_CODE(SDL_SCANCODE_RETURN, SDLK_RETURN),
    [F_KEY_SPACE] = F__KEY_CODE(SDL_SCANCODE_SPACE, SDLK_SPACE),
    [F_KEY_HOME] = F__KEY_CODE(SDL_SCANCODE_HOME, SDLK_HOME),
    [F_KEY_END] = F__KEY_CODE(SDL_SCANCODE_END, SDLK_END),
    [F_KEY_PAGEUP] = F__KEY_CODE(SDL_SCANCODE_PAGEUP, SDLK_PAGEUP),
    [F_KEY_PAGEDOWN] = F__KEY_CODE(SDL_SCANCODE_PAGEDOWN, SDLK_PAGEDOWN),
    [F_KEY_LALT] = F__KEY_CODE(SDL_SCANCODE_LALT, SDLK_LALT),
    [F_KEY_LCTRL] = F__KEY_CODE(SDL_SCANCODE_LCTRL, SDLK_LCTRL),
    [F_KEY_LSHIFT] = F__KEY_CODE(SDL_SCANCODE_LSHIFT, SDLK_LSHIFT),
    [F_KEY_RALT] = F__KEY_CODE(SDL_SCANCODE_RALT, SDLK_RALT),
    [F_KEY_RCTRL] = F__KEY_CODE(SDL_SCANCODE_RCTRL, SDLK_RCTRL),
    [F_KEY_RSHIFT] = F__KEY_CODE(SDL_SCANCODE_RSHIFT, SDLK_RSHIFT),
    [F_KEY_F1] = F__KEY_CODE(SDL_SCANCODE_F1, SDLK_F1),
    [F_KEY_F2] = F__KEY_CODE(SDL_SCANCODE_F2, SDLK_F2),
    [F_KEY_F3] = F__KEY_CODE(SDL_SCANCODE_F3, SDLK_F3),
    [F_KEY_F4] = F__KEY_CODE(SDL_SCANCODE_F4, SDLK_F4),
    [F_KEY_F5] = F__KEY_CODE(SDL_SCANCODE_F5, SDLK_F5),
    [F_KEY_F6] = F__KEY_CODE(SDL_SCANCODE_F6, SDLK_F6),
    [F_KEY_F7] = F__KEY_CODE(SDL_SCANCODE_F7, SDLK_F7),
    [F_KEY_F8] = F__KEY_CODE(SDL_SCANCODE_F8, SDLK_F8),
    [F_KEY_F9] = F__KEY_CODE(SDL_SCANCODE_F9, SDLK_F9),
    [F_KEY_F10] = F__KEY_CODE(SDL_SCANCODE_F10, SDLK_F10),
    [F_KEY_F11] = F__KEY_CODE(SDL_SCANCODE_F11, SDLK_F11),
    [F_KEY_F12] = F__KEY_CODE(SDL_SCANCODE_F12, SDLK_F12),
};

static void keyAdd(FKeyId Id, int Code)
{
    if(g_keys[Id] != NULL) {
        return;
    }

    FPlatformButton* k = f_mem_malloc(sizeof(FPlatformButton));

    k->forwardButtons = NULL;
    k->code.code = Code;
    k->lastEventTick = f_fps_ticksGet() - 1;
    k->pressed = false;

    g_keys[Id] = k;
}
#endif

static void buttonAdd(FPlatformController* Controller, FButtonId Id, int Code)
{
    if(Controller->buttons[Id] != NULL) {
        return;
    }

    FPlatformButton* b = f_mem_malloc(sizeof(FPlatformButton));

    b->forwardButtons = NULL;
    b->code.code = Code;
    b->lastEventTick = f_fps_ticksGet() - 1;
    b->lastHatEventPressed = false;
    b->pressed = false;

    Controller->buttons[Id] = b;
}

static void buttonFree(FPlatformButton* Button)
{
    if(Button == NULL) {
        return;
    }

    f_list_free(Button->forwardButtons);

    f_mem_free(Button);
}

static void buttonPress(FPlatformButton* Button, bool Pressed)
{
    Button->pressed = Pressed;
    Button->lastEventTick = f_fps_ticksGet();

    if(Button->forwardButtons == NULL) {
        return;
    }

    F_LIST_ITERATE(Button->forwardButtons, FPlatformButton*, b) {
        // Queue forwarded button presses and releases to be processed after
        // all input events were received, so they don't conflict with them.
        f_list_addLast(g_forwardButtonsQueue[Pressed], b);
    }
}

#if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ
static void buttonForward(const FPlatformController* Controller, FButtonId Source, FButtonId Destination)
{
    FPlatformButton* bSrc = Controller->buttons[Source];
    FPlatformButton* bDst = Controller->buttons[Destination];

    if(bSrc == NULL || bDst == NULL) {
        return;
    }

    if(bSrc->forwardButtons == NULL) {
        bSrc->forwardButtons = f_list_new();
    }

    f_list_addLast(bSrc->forwardButtons, bDst);
}
#elif F_CONFIG_SYSTEM_PANDORA
static void keyForward(FKeyId Source, const FPlatformController* Controller, FButtonId Destination)
{
    FPlatformButton* bSrc = g_keys[Source];
    FPlatformButton* bDst = Controller->buttons[Destination];

    if(bSrc == NULL || bDst == NULL) {
        return;
    }

    if(bSrc->forwardButtons == NULL) {
        bSrc->forwardButtons = f_list_new();
    }

    f_list_addLast(bSrc->forwardButtons, bDst);
}
#endif

static void analogAdd(FPlatformController* Controller, FAnalogId Id, int AxisIndex)
{
    if(Controller->axes[Id] != NULL) {
        return;
    }

    FPlatformAnalog* a = f_mem_malloc(sizeof(FPlatformAnalog));

    a->forwardButtons = NULL;
    a->axisIndex = AxisIndex;
    a->value = 0;

    Controller->axes[Id] = a;
}

static void analogFree(FPlatformAnalog* Analog)
{
    if(Analog == NULL) {
        return;
    }

    f_list_freeEx(Analog->forwardButtons, f_mem_free);

    f_mem_free(Analog);
}

static void analogSet(FPlatformAnalog* Analog, int Value)
{
    Analog->value = Value;

    if(Analog->forwardButtons == NULL) {
        return;
    }

    #define F__PRESS_THRESHOLD ((1 << 15) / 3)

    bool pressedNegative = Value < -F__PRESS_THRESHOLD;
    bool pressedPositive = Value > F__PRESS_THRESHOLD;

    F_LIST_ITERATE(Analog->forwardButtons, FPlatformButtonPair*, b) {
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

static void analogForward(const FPlatformController* Controller, FAnalogId Source, FButtonId Negative, FButtonId Positive)
{
    FPlatformAnalog* aSrc = Controller->axes[Source];

    if(aSrc == NULL) {
        return;
    }

    FPlatformButtonPair* f = f_mem_malloc(sizeof(FPlatformButtonPair));

    f->negative = Negative < 0 ? NULL : Controller->buttons[Negative];
    f->positive = Positive < 0 ? NULL : Controller->buttons[Positive];
    f->lastPressedNegative = false;
    f->lastPressedPositive = false;

    if(aSrc->forwardButtons == NULL) {
        aSrc->forwardButtons = f_list_new();
    }

    f_list_addLast(aSrc->forwardButtons, f);
}

static inline const char* joystickName(const FPlatformController* Controller)
{
    #if F_CONFIG_LIB_SDL == 1
        return SDL_JoystickName(Controller->id);
    #elif F_CONFIG_LIB_SDL == 2
        return SDL_JoystickName(Controller->joystick);
    #endif
}

static bool controllerInit(FPlatformController* Controller, int Index)
{
    SDL_Joystick* joystick = NULL;

    #if F_CONFIG_LIB_SDL == 2
        char guidStrBuffer[64];
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(Index),
                                  guidStrBuffer,
                                  sizeof(guidStrBuffer) - 1);

        f_out__info("New controller %s", guidStrBuffer);

        SDL_GameController* controller = NULL;

        if(SDL_IsGameController(Index)) {
            controller = SDL_GameControllerOpen(Index);

            if(controller == NULL) {
                f_out__error(
                    "SDL_GameControllerOpen(%d): %s", Index, SDL_GetError());
            } else {
                joystick = SDL_GameControllerGetJoystick(controller);

                if(joystick == NULL) {
                    f_out__error(
                        "SDL_GameControllerGetJoystick: %s", SDL_GetError());

                    SDL_GameControllerClose(controller);

                    return false;
                }
            }
        }
    #endif

    if(joystick == NULL) {
        joystick = SDL_JoystickOpen(Index);

        if(joystick == NULL) {
            f_out__error("SDL_JoystickOpen(%d): %s", Index, SDL_GetError());

            return false;
        }
    }

    #if F_CONFIG_LIB_SDL == 1
        uint8_t id = (uint8_t)Index;
    #elif F_CONFIG_LIB_SDL == 2
        SDL_JoystickID id = SDL_JoystickInstanceID(joystick);

        if(id < 0) {
            f_out__error("SDL_JoystickInstanceID: %s", SDL_GetError());

            if(controller) {
                SDL_GameControllerClose(controller);
            } else {
                SDL_JoystickClose(joystick);
            }

            return false;
        }
    #endif

    Controller->joystick = joystick;
    #if F_CONFIG_LIB_SDL == 2
        Controller->controller = controller;
        Controller->guid = SDL_JoystickGetGUID(joystick);
    #endif
    Controller->id = id;
    Controller->numButtons = SDL_JoystickNumButtons(Controller->joystick);
    Controller->numHats = SDL_JoystickNumHats(Controller->joystick);
    Controller->numAxes = SDL_JoystickNumAxes(Controller->joystick);

    f_out__info("Controller '%s': %d buttons, %d axes, %d hats",
                joystickName(Controller),
                Controller->numButtons,
                Controller->numAxes,
                Controller->numHats);

    bool mappedBuiltIn = false;

    #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO
        if(Index == 0) {
            mappedBuiltIn = true;

            // Joystick 0 is the built-in controls on these platforms
            #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ
                buttonAdd(Controller, F_BUTTON_UP, 0);
                buttonAdd(Controller, F_BUTTON_DOWN, 4);
                buttonAdd(Controller, F_BUTTON_LEFT, 2);
                buttonAdd(Controller, F_BUTTON_RIGHT, 6);
                buttonAdd(Controller, F_BUTTON_UPLEFT, 1);
                buttonAdd(Controller, F_BUTTON_UPRIGHT, 7);
                buttonAdd(Controller, F_BUTTON_DOWNLEFT, 3);
                buttonAdd(Controller, F_BUTTON_DOWNRIGHT, 5);
                buttonAdd(Controller, F_BUTTON_L, 10);
                buttonAdd(Controller, F_BUTTON_R, 11);
                buttonAdd(Controller, F_BUTTON_X, 12);
                buttonAdd(Controller, F_BUTTON_B, 13);
                buttonAdd(Controller, F_BUTTON_A, 14);
                buttonAdd(Controller, F_BUTTON_Y, 15);
                buttonAdd(Controller, F_BUTTON_SELECT, 9);
                buttonAdd(Controller, F_BUTTON_VOLUP, 16);
                buttonAdd(Controller, F_BUTTON_VOLDOWN, 17);
                buttonAdd(Controller, F_BUTTON_START, 8);
                #if F_CONFIG_SYSTEM_GP2X
                    buttonAdd(Controller, F_BUTTON_STICKCLICK, 18);
                #endif

                // Split diagonals into individual cardinal directions
                buttonForward(Controller, F_BUTTON_UPLEFT, F_BUTTON_UP);
                buttonForward(Controller, F_BUTTON_UPLEFT, F_BUTTON_LEFT);
                buttonForward(Controller, F_BUTTON_UPRIGHT, F_BUTTON_UP);
                buttonForward(Controller, F_BUTTON_UPRIGHT, F_BUTTON_RIGHT);
                buttonForward(Controller, F_BUTTON_DOWNLEFT, F_BUTTON_DOWN);
                buttonForward(Controller, F_BUTTON_DOWNLEFT, F_BUTTON_LEFT);
                buttonForward(Controller, F_BUTTON_DOWNRIGHT, F_BUTTON_DOWN);
                buttonForward(Controller, F_BUTTON_DOWNRIGHT, F_BUTTON_RIGHT);
            #elif F_CONFIG_SYSTEM_CAANOO
                buttonAdd(Controller, F_BUTTON_UP, -1);
                buttonAdd(Controller, F_BUTTON_DOWN, -1);
                buttonAdd(Controller, F_BUTTON_LEFT, -1);
                buttonAdd(Controller, F_BUTTON_RIGHT, -1);
                buttonAdd(Controller, F_BUTTON_L, 4);
                buttonAdd(Controller, F_BUTTON_R, 5);
                buttonAdd(Controller, F_BUTTON_X, 0);
                buttonAdd(Controller, F_BUTTON_B, 2);
                buttonAdd(Controller, F_BUTTON_A, 1);
                buttonAdd(Controller, F_BUTTON_Y, 3);
                buttonAdd(Controller, F_BUTTON_GUIDE, 6);
                buttonAdd(Controller, F_BUTTON_HOLD, 7);
                buttonAdd(Controller, F_BUTTON_START, 8);
                buttonAdd(Controller, F_BUTTON_SELECT, 9);

                analogAdd(Controller, F_AXIS_LEFTX, 0);
                analogAdd(Controller, F_AXIS_LEFTY, 1);
            #endif
        }
    #elif F_CONFIG_SYSTEM_PANDORA
        const char* name = joystickName(Controller);

        // Check if this is one of the built-in nubs
        if(f_str_equal(name, "nub0")) {
            mappedBuiltIn = true;

            analogAdd(Controller, F_AXIS_LEFTX, 0);
            analogAdd(Controller, F_AXIS_LEFTY, 1);

            buttonAdd(Controller, F_BUTTON_UP, -1);
            buttonAdd(Controller, F_BUTTON_DOWN, -1);
            buttonAdd(Controller, F_BUTTON_LEFT, -1);
            buttonAdd(Controller, F_BUTTON_RIGHT, -1);
            buttonAdd(Controller, F_BUTTON_L, -1);
            buttonAdd(Controller, F_BUTTON_R, -1);
            buttonAdd(Controller, F_BUTTON_A, -1);
            buttonAdd(Controller, F_BUTTON_B, -1);
            buttonAdd(Controller, F_BUTTON_X, -1);
            buttonAdd(Controller, F_BUTTON_Y, -1);
            buttonAdd(Controller, F_BUTTON_START, -1);
            buttonAdd(Controller, F_BUTTON_SELECT, -1);

            // Pandora's game buttons are actually keyboard keys
            keyForward(F_KEY_UP, Controller, F_BUTTON_UP);
            keyForward(F_KEY_DOWN, Controller, F_BUTTON_DOWN);
            keyForward(F_KEY_LEFT, Controller, F_BUTTON_LEFT);
            keyForward(F_KEY_RIGHT, Controller, F_BUTTON_RIGHT);
            keyForward(F_KEY_RSHIFT, Controller, F_BUTTON_L);
            keyForward(F_KEY_RCTRL, Controller, F_BUTTON_R);
            keyForward(F_KEY_PAGEDOWN, Controller, F_BUTTON_A);
            keyForward(F_KEY_END, Controller, F_BUTTON_B);
            keyForward(F_KEY_HOME, Controller, F_BUTTON_X);
            keyForward(F_KEY_PAGEUP, Controller, F_BUTTON_Y);
            keyForward(F_KEY_LALT, Controller, F_BUTTON_START);
            keyForward(F_KEY_LCTRL, Controller, F_BUTTON_SELECT);
        } else if(f_str_equal(name, "nub1")) {
            mappedBuiltIn = true;
            Controller->claimed = true;

            analogAdd(Controller, F_AXIS_RIGHTX, 0);
            analogAdd(Controller, F_AXIS_RIGHTY, 1);

            // Attach to nub0 to compose a single dual-analog controller
            F_LIST_ITERATE(g_controllers, FPlatformController*, nub0) {
                if(f_str_equal(joystickName(nub0), "nub0")) {
                    nub0->next = Controller;
                    break;
                }
            }
        }
    #endif

    if(!mappedBuiltIn) {
#if F_CONFIG_LIB_SDL == 2
        if(Controller->controller) {
            f_out__info("Mapped as '%s'",
                        SDL_GameControllerName(Controller->controller));

            for(SDL_GameControllerButton b = SDL_CONTROLLER_BUTTON_A;
                b < SDL_CONTROLLER_BUTTON_MAX;
                b++) {

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForButton(
                        Controller->controller, b);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    buttonAdd(Controller, g_buttonsMap[b], b);
                }
            }

            for(SDL_GameControllerAxis a = SDL_CONTROLLER_AXIS_LEFTX;
                a < SDL_CONTROLLER_AXIS_MAX;
                a++) {

                SDL_GameControllerButtonBind bind =
                    SDL_GameControllerGetBindForAxis(Controller->controller, a);

                if(bind.bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
                    analogAdd(Controller, g_axesMap[a], a);
                }
            }
        } else {
#endif
            for(int b = f_math_min(Controller->numButtons,
                                   F_ARRAY_LEN(g_defaultOrder));
                b--; ) {

                buttonAdd(Controller, g_defaultOrder[b], b);
            }

            for(int id = f_math_min(Controller->numAxes, F_AXIS_NUM); id--; ) {
                analogAdd(Controller, id, id);
            }
#if F_CONFIG_LIB_SDL == 2
        }
#endif
    }

    if(Controller->numHats > 0 || Controller->numAxes >= 2) {
        // These buttons will be controlled by hats and analog axes
        buttonAdd(Controller, F_BUTTON_UP, -1);
        buttonAdd(Controller, F_BUTTON_DOWN, -1);
        buttonAdd(Controller, F_BUTTON_LEFT, -1);
        buttonAdd(Controller, F_BUTTON_RIGHT, -1);
        buttonAdd(Controller, F_BUTTON_L, -1);
        buttonAdd(Controller, F_BUTTON_R, -1);

        // Forward the left analog stick to the direction buttons
        analogForward(Controller, F_AXIS_LEFTX, F_BUTTON_LEFT, F_BUTTON_RIGHT);
        analogForward(Controller, F_AXIS_LEFTY, F_BUTTON_UP, F_BUTTON_DOWN);

        // Forward analog shoulder triggers to the shoulder buttons
        analogForward(
            Controller, F_AXIS_LEFTTRIGGER, F_BUTTON_INVALID, F_BUTTON_L);
        analogForward(
            Controller, F_AXIS_RIGHTTRIGGER, F_BUTTON_INVALID, F_BUTTON_R);
    }

    if(g_defaultController == NULL) {
        g_defaultController = Controller;
    }

    f_list_addLast(g_controllers, Controller);

    return true;
}

static void controllerFree(FPlatformController* Controller)
{
    for(int id = 0; id < F_BUTTON_NUM; id++) {
        buttonFree(Controller->buttons[id]);
    }

    for(int id = 0; id < F_AXIS_NUM; id++) {
        analogFree(Controller->axes[id]);
    }

    #if F_CONFIG_LIB_SDL == 1
        if(SDL_JoystickOpened(Controller->id)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #elif F_CONFIG_LIB_SDL == 2
        if(Controller->controller) {
            SDL_GameControllerClose(Controller->controller);
        } else if(SDL_JoystickGetAttached(Controller->joystick)) {
            SDL_JoystickClose(Controller->joystick);
        }
    #endif

    f_mem_free(Controller);
}

#if F_CONFIG_LIB_SDL == 2
static void controllerRemove(FPlatformController* Controller)
{
    char guidStrBuffer[64];
    SDL_JoystickGetGUIDString(
        Controller->guid, guidStrBuffer, sizeof(guidStrBuffer) - 1);

    f_out__info("Removed controller %s", guidStrBuffer);

    Controller->removed = true;

    for(int id = 0; id < F_BUTTON_NUM; id++) {
        if(Controller->buttons[id]) {
            buttonPress(Controller->buttons[id], false);
        }
    }

    for(int id = 0; id < F_AXIS_NUM; id++) {
        if(Controller->axes[id]) {
            analogSet(Controller->axes[id], 0);
        }
    }
}

static bool controllerRestore(FPlatformController* Controller, int Index)
{
    char guidStrBuffer[64];
    SDL_JoystickGetGUIDString(
        Controller->guid, guidStrBuffer, sizeof(guidStrBuffer) - 1);

    f_out__info("Restoring controller %s", guidStrBuffer);

    f_out__info("Controller '%s': %d buttons, %d axes, %d hats",
                joystickName(Controller),
                Controller->numButtons,
                Controller->numAxes,
                Controller->numHats);

    if(Controller->controller) {
        SDL_GameControllerClose(Controller->controller);

        Controller->controller = SDL_GameControllerOpen(Index);

        if(Controller->controller == NULL) {
            f_out__error(
                "SDL_GameControllerOpen(%d): %s", Index, SDL_GetError());

            return false;
        }

        Controller->joystick =
            SDL_GameControllerGetJoystick(Controller->controller);

        if(Controller->joystick == NULL) {
            f_out__error(
                "SDL_GameControllerGetJoystick: %s", SDL_GetError());

            return false;
        }

        f_out__info(
            "Mapped as '%s'", SDL_GameControllerName(Controller->controller));
    } else {
        SDL_JoystickClose(Controller->joystick);
        Controller->joystick = SDL_JoystickOpen(Index);

        if(Controller->joystick == NULL) {
            f_out__error("SDL_JoystickOpen(%d): %s", Index, SDL_GetError());

            return false;
        }
    }

    SDL_JoystickID id = SDL_JoystickInstanceID(Controller->joystick);

    if(id < 0) {
        f_out__error("SDL_JoystickInstanceID: %s", SDL_GetError());

        if(Controller->controller) {
            SDL_GameControllerClose(Controller->controller);
        } else {
            SDL_JoystickClose(Controller->joystick);
        }

        return false;
    }

    Controller->id = id;
    Controller->removed = false;

    return true;
}
#endif

void f_platform_sdl_input__init(void)
{
    #if F_CONFIG_LIB_SDL == 1
        g_sdlFlags = SDL_INIT_JOYSTICK;
    #elif F_CONFIG_LIB_SDL == 2
        g_sdlFlags = SDL_INIT_GAMECONTROLLER;
    #endif

    if(SDL_InitSubSystem(g_sdlFlags) != 0) {
        F__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }

    g_controllers = f_list_new();

    #if F_CONFIG_LIB_SDL == 2
        g_futureControllers = f_list_new();
    #endif

    g_forwardButtonsQueue[0] = f_list_new();
    g_forwardButtonsQueue[1] = f_list_new();

    #if F_CONFIG_TRAIT_KEYBOARD
        for(int k = 0; k < F_KEY_NUM; k++) {
            keyAdd(k, g_keysMap[k]);
        }
    #endif

    #if F_CONFIG_LIB_SDL == 1
        int joysticksNum = SDL_NumJoysticks();
        f_out__info("%d controllers attached", joysticksNum);

        for(int j = 0; j < joysticksNum; j++) {
            FPlatformController* c = f_mem_mallocz(sizeof(FPlatformController));

            if(!controllerInit(c, j)) {
                f_mem_free(c);
            }
        }
    #elif F_CONFIG_LIB_SDL == 2
        if(f_path_exists(F_CONFIG_LIB_SDL_GAMEPADMAP, F_PATH_FILE)) {
            int mappingsNum = SDL_GameControllerAddMappingsFromFile(
                                F_CONFIG_LIB_SDL_GAMEPADMAP);

            if(mappingsNum < 0) {
                f_out__error("SDL_GameControllerAddMappingsFromFile(%s): %s",
                             F_CONFIG_LIB_SDL_GAMEPADMAP,
                             SDL_GetError());
            } else {
                f_out__info("Loaded %d controller mappings from '%s'",
                            mappingsNum,
                            F_CONFIG_LIB_SDL_GAMEPADMAP);
            }
        }

        f_platform_api__inputPoll();
    #endif
}

void f_platform_sdl_input__uninit(void)
{
    #if F_CONFIG_TRAIT_KEYBOARD
        for(int id = 0; id < F_KEY_NUM; id++) {
            buttonFree(g_keys[id]);
        }
    #endif

    f_list_free(g_forwardButtonsQueue[0]);
    f_list_free(g_forwardButtonsQueue[1]);

    f_list_freeEx(g_controllers, (FFree*)controllerFree);

    #if F_CONFIG_LIB_SDL == 2
        f_list_freeEx(g_futureControllers, f_mem_free);
    #endif

    SDL_QuitSubSystem(g_sdlFlags);
}

void f_platform_api__inputPoll(void)
{
    g_mouse.tap = false;

    for(SDL_Event event; SDL_PollEvent(&event); ) {
        switch(event.type) {
            case SDL_QUIT: {
                f_state_exit();
            } break;

#if F_CONFIG_LIB_SDL == 2
            case SDL_JOYDEVICEADDED: {
                bool found = false;
                SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(
                                            event.jdevice.which);

                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    if(memcmp(&guid, &c->guid, sizeof(SDL_JoystickGUID)) == 0) {
                        if(!controllerRestore(c, event.jdevice.which)) {
                            controllerFree(c);
                            F_LIST_REMOVE_CURRENT();
                        }

                        found = true;
                        break;
                    }
                }

                if(!found) {
                    FPlatformController* c = f_list_pop(g_futureControllers);

                    if(c) {
                        if(controllerInit(c, event.jdevice.which)) {
                            if(c->bindCallback) {
                                c->bindCallback(c);
                            }
                        } else {
                            f_list_push(g_futureControllers, c);
                        }
                    } else {
                        c = f_mem_mallocz(sizeof(FPlatformController));

                        if(!controllerInit(c, event.jdevice.which)) {
                            f_mem_free(c);
                        }
                    }
                }
            } break;

            case SDL_JOYDEVICEREMOVED: {
                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    if(c->id == event.jdevice.which) {
                        controllerRemove(c);
                        break;
                    }
                }
            } break;
#endif

#if F_CONFIG_TRAIT_KEYBOARD
            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                #if !F_CONFIG_SYSTEM_EMSCRIPTEN
                    if(event.key.keysym.sym == SDLK_ESCAPE) {
                        f_state_exit();
                        break;
                    }
                #endif

                for(int id = 0; id < F_KEY_NUM; id++) {
#if F_CONFIG_LIB_SDL == 1
                    if(g_keys[id]->code.keyCode == event.key.keysym.sym) {
#elif F_CONFIG_LIB_SDL == 2
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
                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    #if F_CONFIG_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jbutton.which) {
                        continue;
                    }

                    for(int id = 0; id < F_BUTTON_NUM; id++) {
                        FPlatformButton* b = c->buttons[id];

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
                #define F__UP_PRESSED    (1 << 0)
                #define F__DOWN_PRESSED  (1 << 1)
                #define F__LEFT_PRESSED  (1 << 2)
                #define F__RIGHT_PRESSED (1 << 3)

                switch(event.jhat.value) {
                    case SDL_HAT_UP: {
                        state = F__UP_PRESSED;
                    } break;

                    case SDL_HAT_DOWN: {
                        state = F__DOWN_PRESSED;
                    } break;

                    case SDL_HAT_LEFT: {
                        state = F__LEFT_PRESSED;
                    } break;

                    case SDL_HAT_RIGHT: {
                        state = F__RIGHT_PRESSED;
                    } break;

                    case SDL_HAT_LEFTUP: {
                        state = F__LEFT_PRESSED | F__UP_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTUP: {
                        state = F__RIGHT_PRESSED | F__UP_PRESSED;
                    } break;

                    case SDL_HAT_LEFTDOWN: {
                        state = F__LEFT_PRESSED | F__DOWN_PRESSED;
                    } break;

                    case SDL_HAT_RIGHTDOWN: {
                        state = F__RIGHT_PRESSED | F__DOWN_PRESSED;
                    } break;
                }

                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    #if F_CONFIG_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jhat.which) {
                        continue;
                    }

                    FPlatformButton* buttons[4] = {
                        c->buttons[F_BUTTON_UP],
                        c->buttons[F_BUTTON_DOWN],
                        c->buttons[F_BUTTON_LEFT],
                        c->buttons[F_BUTTON_RIGHT]
                    };

                    for(int i = 0; i < 4; i++, state >>= 1) {
                        FPlatformButton* b = buttons[i];

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
                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    #if F_CONFIG_LIB_SDL == 2
                        if(c->controller) {
                            continue;
                        }
                    #endif

                    if(c->id != event.jaxis.which) {
                        continue;
                    }

                    for(int id = 0; id < F_AXIS_NUM; id++) {
                        FPlatformAnalog* a = c->axes[id];

                        if(a && a->axisIndex == event.jaxis.axis) {
                            analogSet(a, event.jaxis.value);
                            break;
                        }
                    }

                    break;
                }
            } break;

#if F_CONFIG_LIB_SDL == 2
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    if(c->controller == NULL || c->id != event.cbutton.which) {
                        continue;
                    }

                    for(int id = 0; id < F_BUTTON_NUM; id++) {
                        FPlatformButton* b = c->buttons[id];

                        if(b && b->code.buttonIndex == event.cbutton.button) {
                            buttonPress(b, event.cbutton.state == SDL_PRESSED);
                            break;
                        }
                    }

                    break;
                }
            } break;

            case SDL_CONTROLLERAXISMOTION: {
                F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
                    if(c->controller == NULL || c->id != event.caxis.which) {
                        continue;
                    }

                    for(int id = 0; id < F_AXIS_NUM; id++) {
                        FPlatformAnalog* a = c->axes[id];

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

    unsigned ticksNow = f_fps_ticksGet();

    F_LIST_ITERATE(g_forwardButtonsQueue[1], FPlatformButton*, b) {
        // Overwrite whatever current state with a press
        buttonPress(b, true);
    }

    F_LIST_ITERATE(g_forwardButtonsQueue[0], FPlatformButton*, b) {
        // Only release if did not receive an event this frame
        if(b->lastEventTick != ticksNow) {
            buttonPress(b, false);
        }
    }

    f_list_clear(g_forwardButtonsQueue[0]);
    f_list_clear(g_forwardButtonsQueue[1]);

    #if !F_CONFIG_SYSTEM_EMSCRIPTEN
        FVecInt mouseDelta = {0, 0};
        SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);

        g_mouse.delta = mouseDelta;
    #endif
}

#if F_CONFIG_TRAIT_KEYBOARD
const FPlatformButton* f_platform_api__inputKeyGet(FKeyId Id)
{
    return Id != F_KEY_INVALID ? g_keys[Id] : NULL;
}
#endif

const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id)
{
    if(Controller == NULL) {
        Controller = g_defaultController;
    }

    if(Controller && Id != F_BUTTON_INVALID) {
        return Controller->buttons[Id];
    }

    return NULL;
}

bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button)
{
    return Button->pressed;
}

const FPlatformAnalog* f_platform_api__inputAnalogGet(const FPlatformController* Controller, FAnalogId Id)
{
    if(Controller == NULL) {
        Controller = g_defaultController;
    }

    if(Controller && Id != F_AXIS_INVALID) {
        #if F_CONFIG_SYSTEM_PANDORA
            for(const FPlatformController* c = Controller; c; c = c->next) {
                if(c->axes[Id]) {
                    return c->axes[Id];
                }
            }
        #else
            return Controller->axes[Id];
        #endif
    }

    return NULL;
}

int f_platform_api__inputAnalogValueGet(const FPlatformAnalog* Analog)
{
    return Analog->value;
}

FVecInt f_platform_api__inputTouchCoordsGet(void)
{
    return g_mouse.coords;
}

FVecInt f_platform_api__inputTouchDeltaGet(void)
{
    return g_mouse.delta;
}

bool f_platform_api__inputTouchTapGet(void)
{
    return g_mouse.tap;
}

FPlatformController* f_platform_api__inputControllerClaim(FControllerBind* Callback)
{
    FPlatformController* controller = NULL;

    #if F_CONFIG_LIB_SDL == 2
        F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
            if(!c->removed && !c->claimed) {
                controller = c;
                break;
            }
        }
    #endif

    if(controller == NULL) {
        F_LIST_ITERATE(g_controllers, FPlatformController*, c) {
            if(!c->claimed) {
                controller = c;
                break;
            }
        }
    }

    if(controller) {
        controller->claimed = true;
        Callback(controller);
    }

    #if F_CONFIG_LIB_SDL == 2
        if(controller == NULL) {
            controller = f_mem_mallocz(sizeof(FPlatformController));

            controller->claimed = true;
            controller->bindCallback = Callback;

            f_list_addLast(g_futureControllers, controller);
        }
    #endif

    return controller;
}

void f_platform_api__inputControllerRelease(FPlatformController* Controller)
{
    if(Controller == NULL) {
        return;
    }

    Controller->claimed = false;

    #if F_CONFIG_LIB_SDL == 2
        if(f_list_removeItem(g_futureControllers, Controller)) {
            f_mem_free(Controller);
        }
    #endif
}
#endif // F_CONFIG_LIB_SDL
