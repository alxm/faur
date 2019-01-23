/*
    Copyright 2010, 2016-2018 Alex Margarit
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

#include "a2x_pack_input_button.v.h"

#include "a2x_pack_input.v.h"
#include "a2x_pack_listit.v.h"
#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_timer.v.h"

struct AButton {
    AInputUserHeader header;
    AListNode* listNode;
    AList* combos; // List of lists of APlatformButton, each a buttons combo
    AList* currentCombo;
    ATimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

const char* a__keyNames[A__KEY_ID(A_KEY_NUM)] = {
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

static AList* g_buttons; // list of AButton

void a_input_button__init(void)
{
    g_buttons = a_list_new();
}

void a_input_button__uninit(void)
{
    a_list_free(g_buttons);
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

    free(Button);
}

void a_button_bind(AButton* Button, int Id)
{
    APlatformButton* pb = a_platform__buttonGet(Id);

    if(pb == NULL) {
        return;
    }

    if(Button->header.name == a__inputNameDefault) {
        Button->header.name = a_platform__buttonNameGet(pb);
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

        A_LIST_ITERATE(b->header.platformInputs, APlatformButton*, pb) {
            if(a_platform__buttonPressGet(pb)) {
                pressed = true;
                goto done;
            }
        }

        A_LIST_ITERATE(b->combos, AList*, andList) {
            A_LIST_ITERATE(andList, APlatformButton*, pb) {
                if(!a_platform__buttonPressGet(pb)) {
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
