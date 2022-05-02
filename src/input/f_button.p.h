/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_INPUTS_BUTTON_P_H
#define F_INC_INPUTS_BUTTON_P_H

#include "../general/f_system_includes.h"

typedef struct FButton FButton;

typedef enum {
    F_BUTTON_INVALID = -1,
    F_BUTTON_UP,
    F_BUTTON_DOWN,
    F_BUTTON_LEFT,
    F_BUTTON_RIGHT,
    F_BUTTON_A,
    F_BUTTON_B,
    F_BUTTON_X,
    F_BUTTON_Y,
    F_BUTTON_L,
    F_BUTTON_R,
    F_BUTTON_START,
    F_BUTTON_SELECT,
    F_BUTTON_STICKCLICK,
    F_BUTTON_UPLEFT,
    F_BUTTON_UPRIGHT,
    F_BUTTON_DOWNLEFT,
    F_BUTTON_DOWNRIGHT,
    F_BUTTON_VOLUP,
    F_BUTTON_VOLDOWN,
    F_BUTTON_GUIDE,
    F_BUTTON_HOLD,
    F_BUTTON_NUM
} FButtonId;

typedef enum {
    F_KEY_INVALID = -1,
    F_KEY_UP,
    F_KEY_DOWN,
    F_KEY_LEFT,
    F_KEY_RIGHT,
    F_KEY_Z,
    F_KEY_X,
    F_KEY_C,
    F_KEY_V,
    F_KEY_M,
    F_KEY_ENTER,
    F_KEY_SPACE,
    F_KEY_HOME,
    F_KEY_END,
    F_KEY_PAGEUP,
    F_KEY_PAGEDOWN,
    F_KEY_LALT,
    F_KEY_LCTRL,
    F_KEY_LSHIFT,
    F_KEY_RALT,
    F_KEY_RCTRL,
    F_KEY_RSHIFT,
    F_KEY_F1,
    F_KEY_F2,
    F_KEY_F3,
    F_KEY_F4,
    F_KEY_F5,
    F_KEY_F6,
    F_KEY_F7,
    F_KEY_F8,
    F_KEY_F9,
    F_KEY_F10,
    F_KEY_F11,
    F_KEY_F12,
    F_KEY_NUM,
} FKeyId;

#include "../input/f_controller.p.h"

extern FButton* f_button_new(void);
extern FButton* f_button_dup(const FButton* Button);
extern void f_button_free(FButton* Button);

extern void f_button_bindKey(FButton* Button, FKeyId Id);
extern void f_button_bindButton(FButton* Button, const FController* Controller, FButtonId Id);
extern void f_button_bindCombo(FButton* Button, const FController* Controller, FButtonId Id, ...);

extern bool f_button_isWorking(const FButton* Button);
extern const char* f_button_nameGet(const FButton* Button);

extern bool f_button_pressGet(const FButton* Button);
extern bool f_button_pressGetOnce(FButton* Button);
extern void f_button_pressSetRepeat(FButton* Button, unsigned RepeatMs);
extern void f_button_pressClear(FButton* Button);

#endif // F_INC_INPUTS_BUTTON_P_H
