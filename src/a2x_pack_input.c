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

typedef struct AInputHeader {
    char* name;
    AList* sourceInputs; // List of AInputSourceButton/Analog/Touch
} AInputHeader;

typedef struct AInputSourceHeader {
    char* name;
    char* shortName;
    unsigned lastEventFrame;
} AInputSourceHeader;

struct AInputButton {
    AInputHeader header;
    AList* combos; // List of lists of AInputSourceButton; for combo buttons
    unsigned repeatFrames;
    unsigned lastPressedFrame;
};

struct AInputSourceButton {
    AInputSourceHeader header;
    bool isLeaf;
    union {
        struct {
            AList* orList; // List of AInputSourceButton; for alias buttons
        } node;
        struct {
            bool pressed;
            bool ignorePressed;
        } leaf;
    } u;
};

struct AInputAnalog {
    AInputHeader header;
};

struct AInputSourceAnalog {
    AInputSourceHeader header;
    int axisValue;
};

struct AInputTouch {
    AInputHeader header;
};

struct AInputSourceTouch {
    AInputSourceHeader header;
    int x;
    int y;
    bool tap;
    AList* motion; // AInputTouchPoints captured by motion event
};

typedef struct AInputTouchPoint {
    int x;
    int y;
} AInputTouchPoint;

typedef struct AInputSourceController {
    AStrHash* buttons;
    AStrHash* axes;
} AInputSourceController;

typedef struct AInputCallbackContainer {
    AInputCallback callback;
} AInputCallbackContainer;

static AList* g_buttons;
static AStrHash* g_sourceButtons;

static AList* g_analogs;
static AStrHash* g_sourceAnalogs;

static AList* g_touchScreens;
static AStrHash* g_sourceTouchScreens;

static AList* g_controllers;
static AInputSourceController* g_activeController;

static AList* g_callbacks;

static void a_input__newSourceButtonNode(const char* Name, const char* ButtonNames);

static void initHeader(AInputHeader* Header)
{
    Header->name = NULL;
    Header->sourceInputs = a_list_new();
}

static void freeHeader(AInputHeader* Header)
{
    free(Header->name);
    a_list_free(Header->sourceInputs);
}

static void initSourceHeader(AInputSourceHeader* Header, const char* Name)
{
    Header->name = a_str_dup(Name);
    Header->shortName = a_str_getSuffixLastFind(Name, '.');
    Header->lastEventFrame = 0;

    if(Header->shortName == NULL) {
        Header->shortName = a_str_dup(Name);
    }
}

static void freeSourceHeader(AInputSourceHeader* Header)
{
    free(Header->name);
    free(Header->shortName);
}

#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_CAANOO
    static inline bool isFreshEvent(const AInputSourceHeader* Header)
    {
        return Header->lastEventFrame == a_fps_getCounter();
    }
#endif

static inline void setFreshEvent(AInputSourceHeader* Header)
{
    Header->lastEventFrame = a_fps_getCounter();
}

static bool findSourceInput(const char* Name, const AStrHash* Collection, AInputHeader* UserInput)
{
    AInputSourceHeader* source = a_strhash_get(Collection, Name);

    if(source == NULL) {
        return false;
    }

    if(UserInput->name == NULL) {
        UserInput->name = a_str_dup(source->shortName);
    }

    a_list_addLast(UserInput->sourceInputs, source);

    return true;
}

void a_input__init(void)
{
    g_buttons = a_list_new();
    g_sourceButtons = a_strhash_new();

    g_analogs = a_list_new();
    g_sourceAnalogs = a_strhash_new();

    g_touchScreens = a_list_new();
    g_sourceTouchScreens = a_strhash_new();

    g_controllers = a_list_new();
    g_activeController = NULL;

    g_callbacks = a_list_new();

    a_sdl_input__bind();

    if(a_input_numControllers() > 0) {
        a_input_setController(0);
    }

    a_input__newSourceButtonNode("generic.up", "key.up controller.up gp2x.up wiz.up caanoo.up pandora.up");
    a_input__newSourceButtonNode("generic.down", "key.down controller.down gp2x.down wiz.down caanoo.down pandora.down");
    a_input__newSourceButtonNode("generic.left", "key.left controller.left gp2x.left wiz.left caanoo.left pandora.left");
    a_input__newSourceButtonNode("generic.right", "key.right controller.right gp2x.right wiz.right caanoo.right pandora.right");

    a_input__newSourceButtonNode("generic.b0", "key.z controller.b0 gp2x.x wiz.x caanoo.x pandora.x");
    a_input__newSourceButtonNode("generic.b1", "key.x controller.b1 gp2x.b wiz.b caanoo.b pandora.b");
    a_input__newSourceButtonNode("generic.b2", "key.c controller.b2 gp2x.a wiz.a caanoo.a pandora.a");
    a_input__newSourceButtonNode("generic.b3", "key.v controller.b3 gp2x.y wiz.y caanoo.y pandora.y");
}

void a_input__uninit(void)
{
    A_LIST_ITERATE(g_buttons, AInputButton*, b) {
        A_LIST_ITERATE(b->combos, AList*, andList) {
            a_list_free(andList);
        }

        a_list_free(b->combos);
        freeHeader(&b->header);
        free(b);
    }

    A_STRHASH_ITERATE(g_sourceButtons, AInputSourceButton*, b) {
        if(!b->isLeaf) {
            a_list_free(b->u.node.orList);
        }

        freeSourceHeader(&b->header);
        free(b);
    }

    A_LIST_ITERATE(g_analogs, AInputAnalog*, a) {
        freeHeader(&a->header);
        free(a);
    }

    A_STRHASH_ITERATE(g_sourceAnalogs, AInputSourceAnalog*, a) {
        freeSourceHeader(&a->header);
        free(a);
    }

    A_LIST_ITERATE(g_touchScreens, AInputTouch*, t) {
        freeHeader(&t->header);
        free(t);
    }

    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputSourceTouch*, t) {
        freeSourceHeader(&t->header);

        A_LIST_ITERATE(t->motion, AInputTouchPoint*, p) {
            free(p);
        }

        a_list_free(t->motion);
        free(t);
    }

    A_LIST_ITERATE(g_controllers, AInputSourceController*, c) {
        A_STRHASH_ITERATE(c->buttons, AInputSourceButton*, b) {
            freeSourceHeader(&b->header);
            free(b);
        }

        A_STRHASH_ITERATE(c->axes, AInputSourceAnalog*, a) {
            freeSourceHeader(&a->header);
            free(a);
        }

        a_strhash_free(c->buttons);
        a_strhash_free(c->axes);
        free(c);
    }

    A_LIST_ITERATE(g_callbacks, AInputCallbackContainer*, c) {
        free(c);
    }

    a_list_free(g_buttons);
    a_strhash_free(g_sourceButtons);

    a_list_free(g_analogs);
    a_strhash_free(g_sourceAnalogs);

    a_list_free(g_touchScreens);
    a_strhash_free(g_sourceTouchScreens);

    a_list_free(g_controllers);

    a_list_free(g_callbacks);
}

void a_input__newController(void)
{
    AInputSourceController* c = a_mem_malloc(sizeof(AInputSourceController));

    c->buttons = a_strhash_new();
    c->axes = a_strhash_new();

    a_list_addLast(g_controllers, c);
    g_activeController = c;
}

AInputSourceButton* a_input__newSourceButton(const char* Name)
{
    AInputSourceButton* b = a_mem_malloc(sizeof(AInputSourceButton));

    initSourceHeader(&b->header, Name);

    b->isLeaf = true;
    b->u.leaf.pressed = false;
    b->u.leaf.ignorePressed = false;

    if(g_activeController == NULL) {
        a_strhash_add(g_sourceButtons, Name, b);
    } else {
        a_strhash_add(g_activeController->buttons, Name, b);
    }

    return b;
}

static void a_input__newSourceButtonNode(const char* Name, const char* ButtonNames)
{
    AInputSourceButton* b = a_mem_malloc(sizeof(AInputSourceButton));

    initSourceHeader(&b->header, Name);

    b->isLeaf = false;
    b->u.node.orList = a_list_new();

    AStrTok* tok = a_strtok_new(ButtonNames, ", ");

    A_STRTOK_ITERATE(tok, name) {
        AInputSourceButton* btn = a_strhash_get(g_sourceButtons, name);

        if(btn == NULL && g_activeController != NULL) {
            btn = a_strhash_get(g_activeController->buttons, name);
        }

        if(btn != NULL) {
            a_list_addLast(b->u.node.orList, btn);
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(b->u.node.orList)) {
        a_out__fatal("'%s' found no buttons in '%s'", Name, ButtonNames);
    }

    a_strhash_add(g_sourceButtons, Name, b);
}

AInputSourceAnalog* a_input__newSourceAnalog(const char* Name)
{
    AInputSourceAnalog* a = a_mem_malloc(sizeof(AInputSourceAnalog));

    initSourceHeader(&a->header, Name);

    a->axisValue = 0;

    if(g_activeController == NULL) {
        a_strhash_add(g_sourceAnalogs, Name, a);
    } else {
        a_strhash_add(g_activeController->axes, Name, a);
    }

    return a;
}

AInputSourceTouch* a_input__newSourceTouch(const char* Name)
{
    AInputSourceTouch* t = a_mem_malloc(sizeof(AInputSourceTouch));

    initSourceHeader(&t->header, Name);

    t->tap = false;
    t->x = 0;
    t->y = 0;
    t->motion = a_list_new();

    a_strhash_add(g_sourceTouchScreens, Name, t);

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
    A_STRHASH_ITERATE(g_sourceTouchScreens, AInputSourceTouch*, touchScreen) {
        touchScreen->tap = false;

        A_LIST_ITERATE(touchScreen->motion, AInputTouchPoint*, p) {
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
            AInputSourceButton* upLeft = a_strhash_get(g_sourceButtons, "gp2x.upleft");
            AInputSourceButton* upRight = a_strhash_get(g_sourceButtons, "gp2x.upright");
            AInputSourceButton* downLeft = a_strhash_get(g_sourceButtons, "gp2x.downleft");
            AInputSourceButton* downRight = a_strhash_get(g_sourceButtons, "gp2x.downright");
            AInputSourceButton* up = a_strhash_get(g_sourceButtons, "gp2x.up");
            AInputSourceButton* down = a_strhash_get(g_sourceButtons, "gp2x.down");
            AInputSourceButton* left = a_strhash_get(g_sourceButtons, "gp2x.left");
            AInputSourceButton* right = a_strhash_get(g_sourceButtons, "gp2x.right");
        #elif A_PLATFORM_WIZ
            AInputSourceButton* upLeft = a_strhash_get(g_sourceButtons, "wiz.upleft");
            AInputSourceButton* upRight = a_strhash_get(g_sourceButtons, "wiz.upright");
            AInputSourceButton* downLeft = a_strhash_get(g_sourceButtons, "wiz.downleft");
            AInputSourceButton* downRight = a_strhash_get(g_sourceButtons, "wiz.downright");
            AInputSourceButton* up = a_strhash_get(g_sourceButtons, "wiz.up");
            AInputSourceButton* down = a_strhash_get(g_sourceButtons, "wiz.down");
            AInputSourceButton* left = a_strhash_get(g_sourceButtons, "wiz.left");
            AInputSourceButton* right = a_strhash_get(g_sourceButtons, "wiz.right");
        #endif

        if(isFreshEvent(&upLeft->header)) {
            a_input__button_setState(up, upLeft->u.leaf.pressed);
            a_input__button_setState(left, upLeft->u.leaf.pressed);
        }

        if(isFreshEvent(&upRight->header)) {
            a_input__button_setState(up, upRight->u.leaf.pressed);
            a_input__button_setState(right, upRight->u.leaf.pressed);
        }

        if(isFreshEvent(&downLeft->header)) {
            a_input__button_setState(down, downLeft->u.leaf.pressed);
            a_input__button_setState(left, downLeft->u.leaf.pressed);
        }

        if(isFreshEvent(&downRight->header)) {
            a_input__button_setState(down, downRight->u.leaf.pressed);
            a_input__button_setState(right, downRight->u.leaf.pressed);
        }
    #endif

    // Caanoo has an analog stick instead of a dpad, but in most cases it's
    // useful to be able to use it as a dpad like on the other platforms.
    #if A_PLATFORM_CAANOO
        // Pressed at least half-way
        #define ANALOG_TRESH ((1 << 15) / 2)

        AInputSourceAnalog* stickx = a_strhash_get(g_sourceAnalogs, "caanoo.stickX");
        AInputSourceAnalog* sticky = a_strhash_get(g_sourceAnalogs, "caanoo.stickY");

        if(isFreshEvent(&stickx->header)) {
            AInputSourceButton* left = a_strhash_get(g_sourceButtons, "caanoo.left");
            a_input__button_setState(left, stickx->axisValue < -ANALOG_TRESH);

            AInputSourceButton* right = a_strhash_get(g_sourceButtons, "caanoo.right");
            a_input__button_setState(right, stickx->axisValue > ANALOG_TRESH);
        }

        if(isFreshEvent(&sticky->header)) {
            AInputSourceButton* up = a_strhash_get(g_sourceButtons, "caanoo.up");
            a_input__button_setState(up, sticky->axisValue < -ANALOG_TRESH);

            AInputSourceButton* down = a_strhash_get(g_sourceButtons, "caanoo.down");
            a_input__button_setState(down, sticky->axisValue > ANALOG_TRESH);
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

AInputButton* a_button_new(const char* Names)
{
    AInputButton* b = a_mem_malloc(sizeof(AInputButton));

    initHeader(&b->header);

    b->combos = a_list_new();
    b->repeatFrames = 0;
    b->lastPressedFrame = 0;

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        if(a_str_firstIndex(name, '+') > 0) {
            AList* buttons = a_list_new();
            AStrTok* tok = a_strtok_new(name, "+");
            bool missing = false;

            A_STRTOK_ITERATE(tok, part) {
                AInputSourceButton* button = a_strhash_get(g_sourceButtons,
                                                           part);

                if(button == NULL) {
                    if(g_activeController != NULL) {
                        button = a_strhash_get(g_activeController->buttons,
                                               part);
                    }

                    if(button == NULL) {
                        missing = true;
                        a_list_free(buttons);
                        break;
                    }
                }

                a_list_addLast(buttons, button);
            }

            if(!missing) {
                a_list_addLast(b->combos, buttons);

                if(b->header.name == NULL) {
                    AStrBuilder* sb = a_strbuilder_new(128);

                    A_LIST_ITERATE(buttons, AInputSourceButton*, button) {
                        a_strbuilder_addString(sb, button->header.shortName);

                        if(!A_LIST_IS_LAST()) {
                            a_strbuilder_addString(sb, "+");
                        }
                    }

                    b->header.name = a_str_dup(a_strbuilder_string(sb));

                    a_strbuilder_free(sb);
                }
            }

            a_strtok_free(tok);
        } else if(!findSourceInput(name, g_sourceButtons, &b->header)) {
            if(g_activeController != NULL) {
                findSourceInput(name, g_activeController->buttons, &b->header);
            }
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(b->header.sourceInputs) && a_list_empty(b->combos)) {
        a_out__error("No buttons found for '%s'", Names);
    }

    a_list_addLast(g_buttons, b);

    return b;
}

bool a_button_working(const AInputButton* Button)
{
    return !a_list_empty(Button->header.sourceInputs)
        || !a_list_empty(Button->combos);
}

const char* a_button_name(const AInputButton* Button)
{
    return Button->header.name;
}

static bool isSourceButtonPressed(const AInputSourceButton* Button)
{
    if(Button->isLeaf) {
        return Button->u.leaf.pressed && !Button->u.leaf.ignorePressed;
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputSourceButton*, b) {
            if(isSourceButtonPressed(b)) {
                return true;
            }
        }

        return false;
    }
}

static void releaseSourceButton(AInputSourceButton* Button)
{
    if(Button->isLeaf) {
        if(Button->u.leaf.pressed) {
            Button->u.leaf.ignorePressed = true;
        }
    } else {
        A_LIST_ITERATE(Button->u.node.orList, AInputSourceButton*, b) {
            releaseSourceButton(b);
        }
    }
}

bool a_button_get(AInputButton* Button)
{
    const unsigned now = a_fps_getCounter();

    A_LIST_ITERATE(Button->header.sourceInputs, AInputSourceButton*, b) {
        if(isSourceButtonPressed(b)) {
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

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputSourceButton*, b) {
            if(!isSourceButtonPressed(b)) {
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

void a_button_release(const AInputButton* Button)
{
    A_LIST_ITERATE(Button->header.sourceInputs, AInputSourceButton*, b) {
        releaseSourceButton(b);
    }

    A_LIST_ITERATE(Button->combos, AList*, andList) {
        A_LIST_ITERATE(andList, AInputSourceButton*, b) {
            releaseSourceButton(b);
        }
    }
}

bool a_button_getOnce(AInputButton* Button)
{
    bool pressed = a_button_get(Button);

    if(pressed) {
        a_button_release(Button);
    }

    return pressed;
}

void a_button_setRepeat(AInputButton* Button, unsigned RepeatFrames)
{
    Button->repeatFrames = RepeatFrames;
    Button->lastPressedFrame = a_fps_getCounter() - RepeatFrames;
}

AInputAnalog* a_analog_new(const char* Names)
{
    AInputAnalog* a = a_mem_malloc(sizeof(AInputAnalog));

    initHeader(&a->header);

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        if(!findSourceInput(name, g_sourceAnalogs, &a->header)) {
            if(g_activeController != NULL) {
                findSourceInput(name, g_activeController->axes, &a->header);
            }
        }
    }

    a_strtok_free(tok);

    if(a_list_empty(a->header.sourceInputs)) {
        a_out__error("No analog axes found for '%s'", Names);
    }

    a_list_addLast(g_analogs, a);

    return a;
}

bool a_analog_working(const AInputAnalog* Analog)
{
    return !a_list_empty(Analog->header.sourceInputs);
}

int a_analog_axisRaw(const AInputAnalog* Analog)
{
    #define A_ANALOG_MAX_DISTANCE (1 << 15)
    #define A_ANALOG_ERROR_MARGIN (A_ANALOG_MAX_DISTANCE / 20)

    A_LIST_ITERATE(Analog->header.sourceInputs, AInputSourceAnalog*, a) {
        if(a_math_abs(a->axisValue) > A_ANALOG_ERROR_MARGIN) {
            return a->axisValue;
        }
    }

    return 0;
}

AFix a_analog_axisFix(const AInputAnalog* Analog)
{
    return a_analog_axisRaw(Analog) >> (15 - A_FIX_BIT_PRECISION);
}

AInputTouch* a_touch_new(const char* Names)
{
    AInputTouch* t = a_mem_malloc(sizeof(AInputTouch));

    initHeader(&t->header);

    AStrTok* tok = a_strtok_new(Names, ", ");

    A_STRTOK_ITERATE(tok, name) {
        findSourceInput(name, g_sourceTouchScreens, &t->header);
    }

    a_strtok_free(tok);

    if(a_list_empty(t->header.sourceInputs)) {
        a_out__error("No touch screen found for '%s'", Names);
    }

    a_list_addLast(g_touchScreens, t);

    return t;
}

bool a_touch_working(const AInputTouch* Touch)
{
    return !a_list_empty(Touch->header.sourceInputs);
}

bool a_touch_tapped(const AInputTouch* Touch)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputSourceTouch*, t) {
        if(t->tap) {
            return true;
        }
    }

    return false;
}

bool a_touch_point(const AInputTouch* Touch, int X, int Y)
{
    return a_touch_box(Touch, X - 1, Y - 1, 3, 3);
}

bool a_touch_box(const AInputTouch* Touch, int X, int Y, int W, int H)
{
    A_LIST_ITERATE(Touch->header.sourceInputs, AInputSourceTouch*, t) {
        if(t->tap && a_collide_pointInBox(t->x, t->y, X, Y, W, H)) {
            return true;
        }
    }

    return false;
}

void a_input__button_setState(AInputSourceButton* Button, bool Pressed)
{
    if(!Pressed && Button->u.leaf.ignorePressed) {
        Button->u.leaf.ignorePressed = false;
    }

    Button->u.leaf.pressed = Pressed;

    setFreshEvent(&Button->header);
}

void a_input__analog_setAxisValue(AInputSourceAnalog* Analog, int Value)
{
    Analog->axisValue = Value;

    setFreshEvent(&Analog->header);
}

void a_input__touch_addMotion(AInputSourceTouch* Touch, int X, int Y)
{
    Touch->x = X;
    Touch->y = Y;

    if(a_settings_getBool("input.trackMouse")) {
        AInputTouchPoint* p = a_mem_malloc(sizeof(AInputTouchPoint));

        p->x = Touch->x;
        p->y = Touch->y;

        a_list_addLast(Touch->motion, p);
    }

    setFreshEvent(&Touch->header);
}

void a_input__touch_setCoords(AInputSourceTouch* Touch, int X, int Y, bool Tapped)
{
    Touch->x = X;
    Touch->y = Y;
    Touch->tap = Tapped;

    setFreshEvent(&Touch->header);
}
