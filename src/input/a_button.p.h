/*
    Copyright 2010, 2017-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_INPUTS_BUTTON_P_H
#define A_INC_INPUTS_BUTTON_P_H

#include "general/a_system_includes.h"

typedef struct AButton AButton;

typedef enum {
    A_BUTTON_INVALID = -1,
    A_BUTTON_UP,
    A_BUTTON_DOWN,
    A_BUTTON_LEFT,
    A_BUTTON_RIGHT,
    A_BUTTON_A,
    A_BUTTON_B,
    A_BUTTON_X,
    A_BUTTON_Y,
    A_BUTTON_L,
    A_BUTTON_R,
    A_BUTTON_START,
    A_BUTTON_SELECT,
    A_BUTTON_STICKCLICK,
    A_BUTTON_UPLEFT,
    A_BUTTON_UPRIGHT,
    A_BUTTON_DOWNLEFT,
    A_BUTTON_DOWNRIGHT,
    A_BUTTON_VOLUP,
    A_BUTTON_VOLDOWN,
    A_BUTTON_GUIDE,
    A_BUTTON_HOLD,
    A_BUTTON_NUM
} AButtonId;

typedef enum {
    A_KEY_INVALID = -1,
    A_KEY_UP,
    A_KEY_DOWN,
    A_KEY_LEFT,
    A_KEY_RIGHT,
    A_KEY_Z,
    A_KEY_X,
    A_KEY_C,
    A_KEY_V,
    A_KEY_M,
    A_KEY_ENTER,
    A_KEY_SPACE,
    A_KEY_HOME,
    A_KEY_END,
    A_KEY_PAGEUP,
    A_KEY_PAGEDOWN,
    A_KEY_LALT,
    A_KEY_LCTRL,
    A_KEY_LSHIFT,
    A_KEY_RALT,
    A_KEY_RCTRL,
    A_KEY_RSHIFT,
    A_KEY_F1,
    A_KEY_F2,
    A_KEY_F3,
    A_KEY_F4,
    A_KEY_F5,
    A_KEY_F6,
    A_KEY_F7,
    A_KEY_F8,
    A_KEY_F9,
    A_KEY_F10,
    A_KEY_F11,
    A_KEY_F12,
    A_KEY_NUM,
} AKeyId;

#include "input/a_controller.v.h"

extern AButton* a_button_new(void);
extern AButton* a_button_dup(const AButton* Button);
extern void a_button_free(AButton* Button);

extern void a_button_bindKey(AButton* Button, AKeyId Id);
extern void a_button_bindButton(AButton* Button, const AController* Controller, AButtonId Id);
extern void a_button_bindCombo(AButton* Button, const AController* Controller, AButtonId Id, ...);

extern bool a_button_isWorking(const AButton* Button);
extern const char* a_button_nameGet(const AButton* Button);

extern bool a_button_pressGet(const AButton* Button);
extern bool a_button_pressGetOnce(AButton* Button);
extern void a_button_pressSetRepeat(AButton* Button, unsigned RepeatMs);
extern void a_button_pressClear(AButton* Button);

#endif // A_INC_INPUTS_BUTTON_P_H
