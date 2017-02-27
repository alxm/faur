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

#include "a2x_pack_input.v.h"

#define A_ANALOG_MAX_DISTANCE (1 << 15)
#define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

struct AInput {
    const char* name;
    AList* buttons; // List of AInputButton
    AList* analogs; // List of AInputAnalog
    AList* touchScreens; // List of AInputTouch
    AList* combos; // List of AInputButtonCombo
    unsigned repeatFrames;
    unsigned lastPressedFrame;
};

typedef struct AInputHeader {
    char* name;
    char* shortName;
    unsigned lastEventFrame;
} AInputHeader;

struct AInputButton {
    AInputHeader header;
    bool pressed;
    bool ignorePressed;
};

struct AInputAnalog {
    AInputHeader header;
    int xaxis;
    int yaxis;
};

struct AInputTouch {
    AInputHeader header;
    int x;
    int y;
    bool tap;
    AList* motion; // APoints captured by motion event
};

typedef struct AInputButtonCombo {
    AInputHeader header;
    AList* buttons; // List of AInputButton
} AInputButtonCombo;

typedef struct AInputController {
    AStrHash* buttons;
} AInputController;

typedef struct APoint {
    int x;
    int y;
} APoint;

typedef struct AInputCallbackContainer {
    AInputCallback callback;
} AInputCallbackContainer;

static AStrHash* g_buttons;
static AStrHash* g_analogs;
static AStrHash* g_touchScreens;
static AStrHash* g_umbrellas;
static AList* g_controllers;
static AInputController* g_activeController;

static AList* g_userInputs;
static AList* g_callbacks;

static void initHeader(AInputHeader* Header, const char* Name)
{
    Header->name = a_str_dup(Name);
    Header->shortName = a_str_getSuffixLastFind(Name, '.');
    Header->lastEventFrame = 0;
}

static void freeHeader(AInputHeader* Header)
{
    free(Header->name);
    free(Header->shortName);
}

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    static inline bool isFreshEvent(const AInputHeader* Header)
    {
        return Header->lastEventFrame == a_fps_getCounter();
    }
#endif

static inline void setFreshEvent(AInputHeader* Header)
{
    Header->lastEventFrame = a_fps_getCounter();
}

static void addUmbrella(const char* Name, const char* Inputs)
{
    AInput* umbrella = a_input_new(Inputs);
    a_strhash_add(g_umbrellas, Name, umbrella);
}

static void findInput(const char* Name, const AStrHash* Collection, AInput* Input, AList* AddTo)
{
    AInputHeader* h = a_strhash_get(Collection, Name);

    if(h) {
        a_list_addLast(AddTo, h);

        if(Input->name == NULL) {
            Input->name = h->shortName;
        }
    }
}

void a_input__init(void)
{
    g_buttons = a_strhash_new();
    g_analogs = a_strhash_new();
    g_touchScreens = a_strhash_new();
    g_umbrellas = a_strhash_new();
    g_controllers = a_list_new();
    g_activeController = NULL;

    g_userInputs = a_list_new();
    g_callbacks = a_list_new();

    a_sdl_input__bind();

    if(a_input_numControllers() > 0) {
        a_input_setController(0);
    }

    addUmbrella("generic.up", "pc.up controller.up gp2x.up wiz.up caanoo.up pandora.up");
    addUmbrella("generic.down", "pc.down controller.down gp2x.down wiz.down caanoo.down pandora.down");
    addUmbrella("generic.left", "pc.left controller.left gp2x.left wiz.left caanoo.left pandora.left");
    addUmbrella("generic.right", "pc.right controller.right gp2x.right wiz.right caanoo.right pandora.right");

    addUmbrella("generic.b0", "pc.z controller.b0 gp2x.x wiz.x caanoo.x pandora.x");
    addUmbrella("generic.b1", "pc.x controller.b1 gp2x.b wiz.b caanoo.b pandora.b");
    addUmbrella("generic.b2", "pc.c controller.b2 gp2x.a wiz.a caanoo.a pandora.a");
    addUmbrella("generic.b3", "pc.v controller.b3 gp2x.y wiz.y caanoo.y pandora.y");
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(g_userInputs, AInput*, input) {
        a_input__free(input);
    }

    a_list_free(g_userInputs);

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        free(c);
    }

    a_list_free(g_callbacks);

    A_STRHASH_ITERATE(g_buttons, AInputButton*, b) {
        freeHeader(&b->header);
        free(b);
    }

    A_STRHASH_ITERATE(g_analogs, AInputAnalog*, a) {
        freeHeader(&a->header);
        free(a);
    }

    A_STRHASH_ITERATE(g_touchScreens, AInputTouch*, t) {
        freeHeader(&t->header);

        A_LIST_ITERATE(t->motion, APoint*, p) {
            free(p);
        }

        a_list_free(t->motion);
        free(t);
    }

    A_LIST_ITERATE(g_controllers, AInputController*, c) {
        A_STRHASH_ITERATE(c->buttons, AInputButton*, b) {
            freeHeader(&b->header);
            free(b);
        }

        a_strhash_free(c->buttons);
        free(c);
    }

    a_strhash_free(g_buttons);
    a_strhash_free(g_analogs);
    a_strhash_free(g_touchScreens);
    a_strhash_free(g_umbrellas);
    a_list_free(g_controllers);
}

void a_input__newController(void)
{
    AInputController* c = a_mem_malloc(sizeof(AInputController));

    c->buttons = a_strhash_new();

    a_list_addLast(g_controllers, c);
    g_activeController = c;
}

AInputButton* a_input__newButton(const char* Name)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    initHeader(&b->header, Name);

    b->pressed = false;
    b->ignorePressed = false;

    if(g_activeController == NULL) {
        a_strhash_add(g_buttons, Name, b);
    } else {
        a_strhash_add(g_activeController->buttons, Name, b);
    }

    return b;
}

AInputAnalog* a_input__newAnalog(const char* Name)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    initHeader(&a->header, Name);

    a->xaxis = 0;
    a->yaxis = 0;

    a_strhash_add(g_analogs, Name, a);

    return a;
}

AInputTouch* a_input__newTouch(const char* Name)
{
    AInputTouch* t = a_mem_malloc(sizeof(AInputTouch));

    initHeader(&t->header, Name);

    t->tap = false;
    t->x = 0;
    t->y = 0;
    t->motion = a_list_new();

    a_strhash_add(g_touchScreens, Name, t);

    return t;
}

void a_input__addCallback(AInputCallback Callback)
{
    AInputCallbackContainer* c = a_mem_malloc(sizeof(AInputCallbackContainer));
    c->callback = Callback;

    a_list_addLast(g_callbacks, c);
}

void a_input__get(void)
{
    A_STRHASH_ITERATE(g_touchScreens, AInputTouch*, touchScreen) {
        touchScreen->tap = false;

        A_LIST_ITERATE(touchScreen->motion, APoint*, p) {
            free(p);
        }

        a_list_clear(touchScreen->motion);
    }

    a_sdl_input__get();

    // GP2X and Wiz dpad diagonals show up as dedicated buttons instead of a
    // combination of two separate buttons. This code checks diagonal events
    // and sets the state of each actual button accordingly.
    #if A_PLATFORM_GP2X || A_PLATFORM_WIZ
        #if A_PLATFORM_GP2X
            AInputButton* upLeft = a_strhash_get(g_buttons, "gp2x.upleft");
            AInputButton* upRight = a_strhash_get(g_buttons, "gp2x.upright");
            AInputButton* downLeft = a_strhash_get(g_buttons, "gp2x.downleft");
            AInputButton* downRight = a_strhash_get(g_buttons, "gp2x.downright");
            AInputButton* up = a_strhash_get(g_buttons, "gp2x.up");
            AInputButton* down = a_strhash_get(g_buttons, "gp2x.down");
            AInputButton* left = a_strhash_get(g_buttons, "gp2x.left");
            AInputButton* right = a_strhash_get(g_buttons, "gp2x.right");
        #elif A_PLATFORM_WIZ
            AInputButton* upLeft = a_strhash_get(g_buttons, "wiz.upleft");
            AInputButton* upRight = a_strhash_get(g_buttons, "wiz.upright");
            AInputButton* downLeft = a_strhash_get(g_buttons, "wiz.downleft");
            AInputButton* downRight = a_strhash_get(g_buttons, "wiz.downright");
            AInputButton* up = a_strhash_get(g_buttons, "wiz.up");
            AInputButton* down = a_strhash_get(g_buttons, "wiz.down");
            AInputButton* left = a_strhash_get(g_buttons, "wiz.left");
            AInputButton* right = a_strhash_get(g_buttons, "wiz.right");
        #endif

        if(isFreshEvent(&upLeft->header)) {
            a_input__button_setState(up, upLeft->pressed);
            a_input__button_setState(left, upLeft->pressed);
        }

        if(isFreshEvent(&upRight->header)) {
            a_input__button_setState(up, upRight->pressed);
            a_input__button_setState(right, upRight->pressed);
        }

        if(isFreshEvent(&downLeft->header)) {
            a_input__button_setState(down, downLeft->pressed);
            a_input__button_setState(left, downLeft->pressed);
        }

        if(isFreshEvent(&downRight->header)) {
            a_input__button_setState(down, downRight->pressed);
            a_input__button_setState(right, downRight->pressed);
        }
    #endif

    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        AInputAnalog* stick = a_strhash_get(g_analogs, "caanoo.stick");

        if(isFreshEvent(&stick->header)) {
            AInputButton* up = a_strhash_get(g_buttons, "caanoo.up");
            AInputButton* down = a_strhash_get(g_buttons, "caanoo.down");
            AInputButton* left = a_strhash_get(g_buttons, "caanoo.left");
            AInputButton* right = a_strhash_get(g_buttons, "caanoo.right");

            // Pressed at least half-way
            #define ANALOG_TRESH ((1 << 15) / 2)

            a_input__button_setState(left, stick->xaxis < -ANALOG_TRESH);
            a_input__button_setState(right, stick->xaxis > ANALOG_TRESH);
            a_input__button_setState(up, stick->yaxis < -ANALOG_TRESH);
            a_input__button_setState(down, stick->yaxis > ANALOG_TRESH);
        }
    #endif

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        c->callback();
    }
}

unsigned a_input_numControllers(void)
{
    return a_list_size(g_controllers);
}

void a_input_setController(unsigned Index)
{
    if(Index >= a_list_size(g_controllers)) {
        a_out__error("Controller %u not present", Index);
        return;
    }

    g_activeController = a_list_get(g_controllers, Index);
}

AInput* a_input_new(const char* Names)
{
    AInput* i = a_mem_malloc(sizeof(AInput));
    AStrTok* tok = a_strtok_new(Names, ", ");

    i->name = NULL;
    i->buttons = a_list_new();
    i->analogs = a_list_new();
    i->touchScreens = a_list_new();
    i->combos = a_list_new();
    i->repeatFrames = 0;
    i->lastPressedFrame = 0;

    A_STRTOK_ITERATE(tok, name) {
        if(a_str_firstIndex(name, '+') > 0) {
            AList* buttons = a_list_new();
            AStrTok* tok = a_strtok_new(name, "+");
            bool missing = false;

            A_STRTOK_ITERATE(tok, part) {
                AInputButton* button = a_strhash_get(g_buttons, part);

                if(button == NULL) {
                    missing = true;
                    a_list_free(buttons);
                    break;
                }

                a_list_addLast(buttons, button);
            }

            if(!missing) {
                AInputButtonCombo* combo = a_mem_malloc(sizeof(AInputButtonCombo));
                AStrBuilder* sb = a_strbuilder_new(128);

                A_LIST_ITERATE(buttons, AInputButton*, button) {
                    a_strbuilder_addString(sb, button->header.shortName);

                    if(!A_LIST_IS_LAST()) {
                        a_strbuilder_addString(sb, "+");
                    }
                }

                combo->header.name = a_str_dup(a_strbuilder_string(sb));
                combo->header.shortName = NULL;
                combo->header.lastEventFrame = 0;
                combo->buttons = buttons;

                a_list_addLast(i->combos, combo);

                if(i->name == NULL) {
                    i->name = combo->header.name;
                }

                a_strbuilder_free(sb);
            }

            a_strtok_free(tok);
        } else if(a_strhash_contains(g_umbrellas, name)) {
            const AInput* umbrella = a_strhash_get(g_umbrellas, name);

            A_LIST_ITERATE(umbrella->buttons, AInputButton*, b) {
                a_list_addLast(i->buttons, b);
            }

            A_LIST_ITERATE(umbrella->analogs, AInputAnalog*, a) {
                a_list_addLast(i->analogs, a);
            }

            A_LIST_ITERATE(umbrella->touchScreens, AInputTouch*, t) {
                a_list_addLast(i->touchScreens, t);
            }

            A_LIST_ITERATE(umbrella->combos, AInputButtonCombo*, c) {
                a_list_addLast(i->combos, c);
            }
        } else if(g_activeController != NULL && a_str_startsWith(name, "controller.")) {
            findInput(name, g_activeController->buttons, i, i->buttons);
        } else {
            findInput(name, g_buttons, i, i->buttons);
            findInput(name, g_analogs, i, i->analogs);
            findInput(name, g_touchScreens, i, i->touchScreens);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(i->buttons)
        && a_list_empty(i->analogs)
        && a_list_empty(i->touchScreens)
        && a_list_empty(i->combos)) {

        a_out__error("No inputs found for '%s'", Names);
    }

    a_list_addLast(g_userInputs, i);

    return i;
}

void a_input__free(AInput* Input)
{
    a_list_free(Input->buttons);
    a_list_free(Input->analogs);
    a_list_free(Input->touchScreens);

    A_LIST_ITERATE(Input->combos, AInputButtonCombo*, c) {
        freeHeader(&c->header);
        a_list_free(c->buttons);
    }

    a_list_free(Input->combos);

    free(Input);
}

const char* a_input_name(const AInput* Input)
{
    return Input->name;
}

bool a_input_working(const AInput* Input)
{
    return !a_list_empty(Input->buttons)
        || !a_list_empty(Input->analogs)
        || !a_list_empty(Input->touchScreens)
        || !a_list_empty(Input->combos);
}

void a_input_setRepeat(AInput* Input, unsigned RepeatFrames)
{
    Input->repeatFrames = RepeatFrames;
    Input->lastPressedFrame = a_fps_getCounter() - RepeatFrames;
}

bool a_button_get(AInput* Button)
{
    const unsigned now = a_fps_getCounter();

    A_LIST_ITERATE(Button->buttons, AInputButton*, b) {
        if(b->pressed && !b->ignorePressed) {
            if(Button->repeatFrames > 0) {
                if(now - Button->lastPressedFrame >= Button->repeatFrames) {
                    Button->lastPressedFrame = now;
                    return true;
                }
            } else {
                return true;
            }
        }
    }

    A_LIST_ITERATE(Button->combos, AInputButtonCombo*, c) {
        A_LIST_ITERATE(c->buttons, AInputButton*, b) {
            if(!b->pressed || b->ignorePressed) {
                break;
            }

            if(Button->repeatFrames > 0
                && now - Button->lastPressedFrame < Button->repeatFrames) {

                break;
            }

            if(A_LIST_IS_LAST()) {
                if(Button->repeatFrames > 0) {
                    Button->lastPressedFrame = now;
                }

                return true;
            }
        }
    }

    return false;
}

void a_button_release(const AInput* Button)
{
    A_LIST_ITERATE(Button->buttons, AInputButton*, b) {
        if(b->pressed) {
            b->ignorePressed = true;
        }
    }

    A_LIST_ITERATE(Button->combos, AInputButtonCombo*, c) {
        A_LIST_ITERATE(c->buttons, AInputButton*, b) {
            if(b->pressed) {
                b->ignorePressed = true;
            }
        }
    }
}

bool a_button_getOnce(AInput* Button)
{
    bool pressed = a_button_get(Button);

    if(pressed) {
        a_button_release(Button);
    }

    return pressed;
}

int a_analog_xaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, AInputAnalog*, a) {
        if(a_math_abs(a->xaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->xaxis;
        }
    }

    return 0;
}

int a_analog_yaxis(const AInput* Analog)
{
    A_LIST_ITERATE(Analog->analogs, AInputAnalog*, a) {
        if(a_math_abs(a->yaxis) > A_ANALOG_ERROR_MARGIN) {
            return a->yaxis;
        }
    }

    return 0;
}

AFix a_analog_xaxis_fix(const AInput* Analog)
{
    return a_analog_xaxis(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

AFix a_analog_yaxis_fix(const AInput* Analog)
{
    return a_analog_yaxis(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

bool a_touch_tapped(const AInput* Touch)
{
    A_LIST_ITERATE(Touch->touchScreens, AInputTouch*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_point(const AInput* Touch, int X, int Y)
{
    return a_touch_box(Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_box(const AInput* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->touchScreens, AInputTouch*, t) {
        if(t->tap && a_collide_pointInBox(t->x, t->y, X, Y, W, H)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(AInputButton* Button, bool Pressed)
{
    if(!Pressed && Button->ignorePressed) {
        Button->ignorePressed = false;
    }

    Button->pressed = Pressed;

    setFreshEvent(&Button->header);
}

void a_input__analog_setXAxis(AInputAnalog* Analog, int Value)
{
    Analog->xaxis = Value;

    setFreshEvent(&Analog->header);
}

void a_input__analog_setYAxis(AInputAnalog* Analog, int Value)
{
    Analog->yaxis = Value;

    setFreshEvent(&Analog->header);
}

void a_input__touch_addMotion(AInputTouch* Touch, int X, int Y)
{
    Touch->x = X;
    Touch->y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        APoint* p = a_mem_malloc(sizeof(APoint));

        p->x = Touch->x;
        p->y = Touch->y;

        a_list_addLast(Touch->motion, p);
    }

    setFreshEvent(&Touch->header);
}

void a_input__touch_setCoords(AInputTouch* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;

    setFreshEvent(&Touch->header);
}
