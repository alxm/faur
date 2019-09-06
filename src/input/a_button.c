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

#include "a_button.v.h"
#include <a2x.v.h>

struct AButton {
    AInputUserHeader header;
    AListNode* listNode;
    AList* combos; // List of lists of APlatformInputButton, each a button combo
    AList* currentCombo;
    ATimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

static const char* g_keyNames[A__KEY_ID(A_KEY_NUM)] = {
    [A__KEY_ID(A_KEY_UP)] = "Up",
    [A__KEY_ID(A_KEY_DOWN)] = "Down",
    [A__KEY_ID(A_KEY_LEFT)] = "Left",
    [A__KEY_ID(A_KEY_RIGHT)] = "Right",
    [A__KEY_ID(A_KEY_Z)] = "Z",
    [A__KEY_ID(A_KEY_X)] = "X",
    [A__KEY_ID(A_KEY_C)] = "C",
    [A__KEY_ID(A_KEY_V)] = "V",
    [A__KEY_ID(A_KEY_M)] = "M",
    [A__KEY_ID(A_KEY_ENTER)] = "Enter",
    [A__KEY_ID(A_KEY_SPACE)] = "Space",
    [A__KEY_ID(A_KEY_HOME)] = "Home",
    [A__KEY_ID(A_KEY_END)] = "End",
    [A__KEY_ID(A_KEY_PAGEUP)] = "PageUp",
    [A__KEY_ID(A_KEY_PAGEDOWN)] = "PageDown",
    [A__KEY_ID(A_KEY_LALT)] = "L-Alt",
    [A__KEY_ID(A_KEY_LCTRL)] = "L-Ctrl",
    [A__KEY_ID(A_KEY_LSHIFT)] = "L-Shift",
    [A__KEY_ID(A_KEY_RALT)] = "R-Alt",
    [A__KEY_ID(A_KEY_RCTRL)] = "R-Ctrl",
    [A__KEY_ID(A_KEY_RSHIFT)] = "R-Shift",
    [A__KEY_ID(A_KEY_F1)] = "F1",
    [A__KEY_ID(A_KEY_F2)] = "F2",
    [A__KEY_ID(A_KEY_F3)] = "F3",
    [A__KEY_ID(A_KEY_F4)] = "F4",
    [A__KEY_ID(A_KEY_F5)] = "F5",
    [A__KEY_ID(A_KEY_F6)] = "F6",
    [A__KEY_ID(A_KEY_F7)] = "F7",
    [A__KEY_ID(A_KEY_F8)] = "F8",
    [A__KEY_ID(A_KEY_F9)] = "F9",
    [A__KEY_ID(A_KEY_F10)] = "F10",
    [A__KEY_ID(A_KEY_F11)] = "F11",
    [A__KEY_ID(A_KEY_F12)] = "F12",
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

static AList* g_buttons; // list of AButton

void a_input_button__init(void)
{
    g_buttons = a_list_new();
}

void a_input_button__uninit(void)
{
    a_list_free(g_buttons);
}

bool a_button_read(AButtonId Button)
{
    APlatformInputButton* b = a_platform_api__inputButtonGet(Button);

    return b ? a_platform_api__inputButtonPressGet(b) : false;
}

AButton* a_button_new(void)
{
    AButton* b = a_mem_malloc(sizeof(AButton));

    a_input__userHeaderInit(&b->header);

    b->listNode = a_list_addLast(g_buttons, b);
    b->combos = a_list_new();
    b->currentCombo = NULL;
    b->autoRepeat = NULL;
    b->isClone = false;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

AButton* a_button_dup(const AButton* Button)
{
    AButton* b = a_mem_dup(Button, sizeof(AButton));

    b->listNode = a_list_addLast(g_buttons, b);
    b->autoRepeat = NULL;
    b->isClone = true;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

void a_button_free(AButton* Button)
{
    if(Button == NULL) {
        return;
    }

    a_list_removeNode(Button->listNode);

    if(!Button->isClone) {
        a_list_freeEx(Button->combos, (AFree*)a_list_free);
        a_input__userHeaderFree(&Button->header);
    }

    a_timer_free(Button->autoRepeat);

    a_mem_free(Button);
}

void a_button_bind(AButton* Button, int Id)
{
    APlatformInputButton* pb = a_platform_api__inputButtonGet(Id);

    if(pb == NULL) {
        return;
    }

    if(Button->header.name == a__inputNameDefault) {
        Button->header.name = (Id & A__KEY_FLAG)
                                ? g_keyNames[A__KEY_ID(Id)] : g_buttonNames[Id];
    }

    if(Button->currentCombo) {
        a_list_addLast(Button->currentCombo, pb);
    } else {
        a_list_addLast(Button->header.platformInputs, pb);
    }
}

void a_button_bindComboStart(AButton* Button)
{
    Button->currentCombo = a_list_new();

    a_list_push(Button->combos, Button->currentCombo);
}

void a_button_bindComboEnd(AButton* Button)
{
    Button->currentCombo = NULL;
}

bool a_button_isWorking(const AButton* Button)
{
    return !a_list_isEmpty(Button->header.platformInputs)
        || !a_list_isEmpty(Button->combos);
}

const char* a_button_nameGet(const AButton* Button)
{
    return Button->header.name;
}

bool a_button_pressGet(const AButton* Button)
{
    return Button->pressed;
}

bool a_button_pressGetOnce(AButton* Button)
{
    bool pressed = a_button_pressGet(Button);

    if(pressed) {
        a_button_pressClear(Button);
    }

    return pressed;
}

void a_button_pressSetRepeat(AButton* Button, unsigned RepeatMs)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = a_timer_new(A_TIMER_MS, RepeatMs, true);
    } else {
        a_timer_stop(Button->autoRepeat);
        a_timer_periodSet(Button->autoRepeat, RepeatMs);
    }
}

void a_button_pressClear(AButton* Button)
{
    Button->waitForRelease = true;
    Button->pressed = false;
}

void a_input_button__tick(void)
{
    A_LIST_ITERATE(g_buttons, AButton*, b) {
        bool pressed = false;

        A_LIST_ITERATE(b->header.platformInputs, APlatformInputButton*, pb) {
            if(a_platform_api__inputButtonPressGet(pb)) {
                pressed = true;
                goto done;
            }
        }

        A_LIST_ITERATE(b->combos, AList*, andList) {
            A_LIST_ITERATE(andList, APlatformInputButton*, pb) {
                if(!a_platform_api__inputButtonPressGet(pb)) {
                    break;
                } else if(A_LIST_IS_LAST()) {
                    pressed = true;
                    goto done;
                }
            }
        }

done:
        if(b->waitForRelease && !pressed) {
            b->waitForRelease = false;
        }

        if(b->autoRepeat) {
            if(pressed) {
                if(!a_timer_isRunning(b->autoRepeat)) {
                    a_timer_start(b->autoRepeat);
                } else if(!a_timer_expiredGet(b->autoRepeat)) {
                    pressed = false;
                }
            } else {
                a_timer_stop(b->autoRepeat);
            }
        }

        b->pressed = pressed && !b->waitForRelease;
    }
}
