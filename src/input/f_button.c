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

#include "f_button.v.h"
#include <faur.v.h>

struct AButton {
    AListNode* listNode;
    const char* name; // friendly name
    AList* platformInputs; // list of APlatformButton
    AList* combos; // List of lists of APlatformButton, each a button combo
    ATimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

static const char* g_keyNames[A_KEY_NUM] = {
    [A_KEY_UP] = "Up",
    [A_KEY_DOWN] = "Down",
    [A_KEY_LEFT] = "Left",
    [A_KEY_RIGHT] = "Right",
    [A_KEY_Z] = "Z",
    [A_KEY_X] = "X",
    [A_KEY_C] = "C",
    [A_KEY_V] = "V",
    [A_KEY_M] = "M",
    [A_KEY_ENTER] = "Enter",
    [A_KEY_SPACE] = "Space",
    [A_KEY_HOME] = "Home",
    [A_KEY_END] = "End",
    [A_KEY_PAGEUP] = "PageUp",
    [A_KEY_PAGEDOWN] = "PageDown",
    [A_KEY_LALT] = "L-Alt",
    [A_KEY_LCTRL] = "L-Ctrl",
    [A_KEY_LSHIFT] = "L-Shift",
    [A_KEY_RALT] = "R-Alt",
    [A_KEY_RCTRL] = "R-Ctrl",
    [A_KEY_RSHIFT] = "R-Shift",
    [A_KEY_F1] = "F1",
    [A_KEY_F2] = "F2",
    [A_KEY_F3] = "F3",
    [A_KEY_F4] = "F4",
    [A_KEY_F5] = "F5",
    [A_KEY_F6] = "F6",
    [A_KEY_F7] = "F7",
    [A_KEY_F8] = "F8",
    [A_KEY_F9] = "F9",
    [A_KEY_F10] = "F10",
    [A_KEY_F11] = "F11",
    [A_KEY_F12] = "F12",
};

static const char* g_buttonNames[A_BUTTON_NUM] = {
    [A_BUTTON_UP] = "Up",
    [A_BUTTON_DOWN] = "Down",
    [A_BUTTON_LEFT] = "Left",
    [A_BUTTON_RIGHT] = "Right",
    #if A_CONFIG_SYSTEM_GP2X || A_CONFIG_SYSTEM_WIZ || A_CONFIG_SYSTEM_CAANOO || A_CONFIG_SYSTEM_PANDORA
        [A_BUTTON_A] = "X",
        [A_BUTTON_B] = "B",
        [A_BUTTON_X] = "A",
        [A_BUTTON_Y] = "Y",
    #else
        [A_BUTTON_A] = "A",
        [A_BUTTON_B] = "B",
        [A_BUTTON_X] = "X",
        [A_BUTTON_Y] = "Y",
    #endif
    [A_BUTTON_L] = "L",
    [A_BUTTON_R] = "R",
    #if A_CONFIG_SYSTEM_WIZ
        [A_BUTTON_START] = "Menu",
        [A_BUTTON_SELECT] = "Select",
    #elif A_CONFIG_SYSTEM_CAANOO
        [A_BUTTON_START] = "I",
        [A_BUTTON_SELECT] = "II",
    #else
        [A_BUTTON_START] = "Start",
        [A_BUTTON_SELECT] = "Select",
    #endif
    [A_BUTTON_STICKCLICK] = "Stick-Click",
    [A_BUTTON_UPLEFT] = "Up-Left",
    [A_BUTTON_UPRIGHT] = "Up-Right",
    [A_BUTTON_DOWNLEFT] = "Down-Left",
    [A_BUTTON_DOWNRIGHT] = "Down-Right",
    [A_BUTTON_VOLUP] = "Volume-Up",
    [A_BUTTON_VOLDOWN] = "Volume-Down",
    [A_BUTTON_GUIDE] = "Guide",
    [A_BUTTON_HOLD] = "Hold",
};

static const char* g_defaultName = "AButton";

static AList* g_buttons; // list of AButton

void f_input_button__init(void)
{
    g_buttons = f_list_new();
}

void f_input_button__uninit(void)
{
    f_list_free(g_buttons);
}

AButton* f_button_new(void)
{
    AButton* b = f_mem_malloc(sizeof(AButton));

    b->listNode = f_list_addLast(g_buttons, b);
    b->name = g_defaultName;
    b->platformInputs = f_list_new();
    b->combos = NULL;
    b->autoRepeat = NULL;
    b->isClone = false;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

AButton* f_button_dup(const AButton* Button)
{
    AButton* b = f_mem_dup(Button, sizeof(AButton));

    b->listNode = f_list_addLast(g_buttons, b);
    b->autoRepeat = NULL;
    b->isClone = true;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

void f_button_free(AButton* Button)
{
    if(Button == NULL) {
        return;
    }

    f_list_removeNode(Button->listNode);

    if(!Button->isClone) {
        f_list_freeEx(Button->combos, (AFree*)f_list_free);
        f_list_free(Button->platformInputs);
    }

    f_timer_free(Button->autoRepeat);

    f_mem_free(Button);
}

void f_button_bindKey(AButton* Button, AKeyId Id)
{
    const APlatformButton* k = f_platform_api__inputKeyGet(Id);

    if(k == NULL) {
        return;
    }

    if(Button->name == g_defaultName) {
        Button->name = g_keyNames[Id];
    }

    f_list_addLast(Button->platformInputs, (APlatformButton*)k);
}

void f_button_bindButton(AButton* Button, const AController* Controller, AButtonId Id)
{
    const APlatformButton* b = f_platform_api__inputButtonGet(Controller, Id);

    if(b == NULL) {
        return;
    }

    if(Button->name == g_defaultName) {
        Button->name = g_buttonNames[Id];
    }

    f_list_addLast(Button->platformInputs, (APlatformButton*)b);
}

void f_button_bindCombo(AButton* Button, const AController* Controller, AButtonId Id, ...)
{
    va_list args;
    va_start(args, Id);

    AList* combo = f_list_new();

    for(int i = Id; i != A_BUTTON_INVALID; i = va_arg(args, int)) {
        const APlatformButton* b =
            f_platform_api__inputButtonGet(Controller, i);

        if(b) {
            f_list_addLast(combo, (APlatformButton*)b);
        }
    }

    if(f_list_isEmpty(combo)) {
        f_list_free(combo);
    } else {
        if(Button->combos == NULL) {
            Button->combos = f_list_new();
        }

        f_list_push(Button->combos, combo);
    }

    va_end(args);
}

bool f_button_isWorking(const AButton* Button)
{
    return !f_list_isEmpty(Button->platformInputs)
        || (Button->combos && !f_list_isEmpty(Button->combos));
}

const char* f_button_nameGet(const AButton* Button)
{
    return Button->name;
}

bool f_button_pressGet(const AButton* Button)
{
    return Button->pressed;
}

bool f_button_pressGetOnce(AButton* Button)
{
    bool pressed = f_button_pressGet(Button);

    if(pressed) {
        f_button_pressClear(Button);
    }

    return pressed;
}

void f_button_pressSetRepeat(AButton* Button, unsigned RepeatMs)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = f_timer_new(A_TIMER_MS, RepeatMs, true);
    } else {
        f_timer_stop(Button->autoRepeat);
        f_timer_periodSet(Button->autoRepeat, RepeatMs);
    }
}

void f_button_pressClear(AButton* Button)
{
    Button->waitForRelease = true;
    Button->pressed = false;
}

void f_input_button__tick(void)
{
    A_LIST_ITERATE(g_buttons, AButton*, b) {
        bool pressed = false;

        A_LIST_ITERATE(b->platformInputs, const APlatformButton*, pb) {
            if(f_platform_api__inputButtonPressGet(pb)) {
                pressed = true;
                goto done;
            }
        }

        if(b->combos) {
            A_LIST_ITERATE(b->combos, AList*, andList) {
                A_LIST_ITERATE(andList, const APlatformButton*, pb) {
                    if(!f_platform_api__inputButtonPressGet(pb)) {
                        break;
                    } else if(A_LIST_IS_LAST()) {
                        pressed = true;
                        goto done;
                    }
                }
            }
        }

done:
        if(b->waitForRelease && !pressed) {
            b->waitForRelease = false;
        }

        if(b->autoRepeat) {
            if(pressed) {
                if(!f_timer_isRunning(b->autoRepeat)) {
                    f_timer_start(b->autoRepeat);
                } else if(!f_timer_expiredGet(b->autoRepeat)) {
                    pressed = false;
                }
            } else {
                f_timer_stop(b->autoRepeat);
            }
        }

        b->pressed = pressed && !b->waitForRelease;
    }
}
