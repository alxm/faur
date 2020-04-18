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

struct FButton {
    FListNode* listNode;
    const char* name; // friendly name
    FList* platformInputs; // FList<FPlatformButton*>
    FList* combos; // FList<FList<FPlatformButton*>>
    FTimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

#if F_CONFIG_TRAIT_KEYBOARD
static const char* g_keyNames[F_KEY_NUM] = {
    [F_KEY_UP] = "Up",
    [F_KEY_DOWN] = "Down",
    [F_KEY_LEFT] = "Left",
    [F_KEY_RIGHT] = "Right",
    [F_KEY_Z] = "Z",
    [F_KEY_X] = "X",
    [F_KEY_C] = "C",
    [F_KEY_V] = "V",
    [F_KEY_M] = "M",
    [F_KEY_ENTER] = "Enter",
    [F_KEY_SPACE] = "Space",
    [F_KEY_HOME] = "Home",
    [F_KEY_END] = "End",
    [F_KEY_PAGEUP] = "PageUp",
    [F_KEY_PAGEDOWN] = "PageDown",
    [F_KEY_LALT] = "L-Alt",
    [F_KEY_LCTRL] = "L-Ctrl",
    [F_KEY_LSHIFT] = "L-Shift",
    [F_KEY_RALT] = "R-Alt",
    [F_KEY_RCTRL] = "R-Ctrl",
    [F_KEY_RSHIFT] = "R-Shift",
    [F_KEY_F1] = "F1",
    [F_KEY_F2] = "F2",
    [F_KEY_F3] = "F3",
    [F_KEY_F4] = "F4",
    [F_KEY_F5] = "F5",
    [F_KEY_F6] = "F6",
    [F_KEY_F7] = "F7",
    [F_KEY_F8] = "F8",
    [F_KEY_F9] = "F9",
    [F_KEY_F10] = "F10",
    [F_KEY_F11] = "F11",
    [F_KEY_F12] = "F12",
};
#endif // F_CONFIG_TRAIT_KEYBOARD

static const char* g_buttonNames[F_BUTTON_NUM] = {
    [F_BUTTON_UP] = "Up",
    [F_BUTTON_DOWN] = "Down",
    [F_BUTTON_LEFT] = "Left",
    [F_BUTTON_RIGHT] = "Right",
    #if F_CONFIG_SYSTEM_GP2X || F_CONFIG_SYSTEM_WIZ || F_CONFIG_SYSTEM_CAANOO || F_CONFIG_SYSTEM_PANDORA
        [F_BUTTON_A] = "X",
        [F_BUTTON_B] = "B",
        [F_BUTTON_X] = "A",
        [F_BUTTON_Y] = "Y",
    #else
        [F_BUTTON_A] = "A",
        [F_BUTTON_B] = "B",
        [F_BUTTON_X] = "X",
        [F_BUTTON_Y] = "Y",
    #endif
    [F_BUTTON_L] = "L",
    [F_BUTTON_R] = "R",
    #if F_CONFIG_SYSTEM_WIZ
        [F_BUTTON_START] = "Menu",
        [F_BUTTON_SELECT] = "Select",
    #elif F_CONFIG_SYSTEM_CAANOO
        [F_BUTTON_START] = "I",
        [F_BUTTON_SELECT] = "II",
    #else
        [F_BUTTON_START] = "Start",
        [F_BUTTON_SELECT] = "Select",
    #endif
    [F_BUTTON_STICKCLICK] = "Stick-Click",
    [F_BUTTON_UPLEFT] = "Up-Left",
    [F_BUTTON_UPRIGHT] = "Up-Right",
    [F_BUTTON_DOWNLEFT] = "Down-Left",
    [F_BUTTON_DOWNRIGHT] = "Down-Right",
    [F_BUTTON_VOLUP] = "Volume-Up",
    [F_BUTTON_VOLDOWN] = "Volume-Down",
    [F_BUTTON_GUIDE] = "Guide",
    [F_BUTTON_HOLD] = "Hold",
};

static const char* g_defaultName = "FButton";

static FList* g_buttons; // FList<FButton*>

void f_input_button__init(void)
{
    g_buttons = f_list_new();
}

void f_input_button__uninit(void)
{
    f_list_free(g_buttons);
}

FButton* f_button_new(void)
{
    FButton* b = f_mem_mallocz(sizeof(FButton));

    b->listNode = f_list_addLast(g_buttons, b);
    b->name = g_defaultName;
    b->platformInputs = f_list_new();

    return b;
}

FButton* f_button_dup(const FButton* Button)
{
    FButton* b = f_mem_dup(Button, sizeof(FButton));

    b->listNode = f_list_addLast(g_buttons, b);
    b->autoRepeat = NULL;
    b->isClone = true;
    b->waitForRelease = false;
    b->pressed = false;

    return b;
}

void f_button_free(FButton* Button)
{
    if(Button == NULL) {
        return;
    }

    f_list_removeNode(Button->listNode);

    if(!Button->isClone) {
        f_list_freeEx(Button->combos, (FFree*)f_list_free);
        f_list_free(Button->platformInputs);
    }

    f_timer_free(Button->autoRepeat);

    f_mem_free(Button);
}

void f_button_bindKey(FButton* Button, FKeyId Id)
{
    #if F_CONFIG_TRAIT_KEYBOARD
        const FPlatformButton* k = f_platform_api__inputKeyGet(Id);

        if(k == NULL) {
            return;
        }

        if(Button->name == g_defaultName) {
            Button->name = g_keyNames[Id];
        }

        f_list_addLast(Button->platformInputs, (FPlatformButton*)k);
    #else
        F_UNUSED(Button);
        F_UNUSED(Id);
    #endif
}

void f_button_bindButton(FButton* Button, const FController* Controller, FButtonId Id)
{
    const FPlatformButton* b = f_platform_api__inputButtonGet(Controller, Id);

    if(b == NULL) {
        return;
    }

    if(Button->name == g_defaultName) {
        Button->name = g_buttonNames[Id];
    }

    f_list_addLast(Button->platformInputs, (FPlatformButton*)b);
}

void f_button_bindCombo(FButton* Button, const FController* Controller, FButtonId Id, ...)
{
    va_list args;
    va_start(args, Id);

    FList* combo = f_list_new();

    for(int i = Id; i != F_BUTTON_INVALID; i = va_arg(args, int)) {
        const FPlatformButton* b =
            f_platform_api__inputButtonGet(Controller, i);

        if(b) {
            f_list_addLast(combo, (FPlatformButton*)b);
        }
    }

    if(f_list_sizeIsEmpty(combo)) {
        f_list_free(combo);
    } else {
        if(Button->combos == NULL) {
            Button->combos = f_list_new();
        }

        f_list_push(Button->combos, combo);
    }

    va_end(args);
}

bool f_button_isWorking(const FButton* Button)
{
    return !f_list_sizeIsEmpty(Button->platformInputs)
        || (Button->combos && !f_list_sizeIsEmpty(Button->combos));
}

const char* f_button_nameGet(const FButton* Button)
{
    return Button->name;
}

bool f_button_pressGet(const FButton* Button)
{
    return Button->pressed;
}

bool f_button_pressGetOnce(FButton* Button)
{
    bool pressed = f_button_pressGet(Button);

    if(pressed) {
        f_button_pressClear(Button);
    }

    return pressed;
}

void f_button_pressSetRepeat(FButton* Button, unsigned RepeatMs)
{
    if(Button->autoRepeat == NULL) {
        Button->autoRepeat = f_timer_new(F_TIMER_MS, RepeatMs, true);
    } else {
        f_timer_runStop(Button->autoRepeat);
        f_timer_periodSet(Button->autoRepeat, RepeatMs);
    }
}

void f_button_pressClear(FButton* Button)
{
    Button->waitForRelease = true;
    Button->pressed = false;
}

void f_input_button__tick(void)
{
    F_LIST_ITERATE(g_buttons, FButton*, b) {
        bool pressed = false;

        F_LIST_ITERATE(b->platformInputs, const FPlatformButton*, pb) {
            if(f_platform_api__inputButtonPressGet(pb)) {
                pressed = true;
                goto done;
            }
        }

        if(b->combos) {
            F_LIST_ITERATE(b->combos, FList*, andList) {
                F_LIST_ITERATE(andList, const FPlatformButton*, pb) {
                    if(!f_platform_api__inputButtonPressGet(pb)) {
                        break;
                    } else if(F_LIST_IS_LAST()) {
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
                if(!f_timer_runGet(b->autoRepeat)) {
                    f_timer_runStart(b->autoRepeat);
                } else if(!f_timer_expiredGet(b->autoRepeat)) {
                    pressed = false;
                }
            } else {
                f_timer_runStop(b->autoRepeat);
            }
        }

        b->pressed = pressed && !b->waitForRelease;
    }
}
