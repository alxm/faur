/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

extern "C" {
    #include "f_gamebuino_input.v.h"
}

#include <faur.v.h>

#if F_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

struct APlatformButton {
    Button code;
};

static const Button* const g_keys[F_KEY_NUM] = {
    [F_KEY_UP] = &BUTTON_UP,
    [F_KEY_DOWN] = &BUTTON_DOWN,
    [F_KEY_LEFT] = &BUTTON_LEFT,
    [F_KEY_RIGHT] = &BUTTON_RIGHT,
    [F_KEY_Z] = &BUTTON_A,
    [F_KEY_X] = &BUTTON_B,
    [F_KEY_C] = &BUTTON_MENU,
};

static const Button* const g_buttons[F_BUTTON_NUM] = {
    [F_BUTTON_UP] = &BUTTON_UP,
    [F_BUTTON_DOWN] = &BUTTON_DOWN,
    [F_BUTTON_LEFT] = &BUTTON_LEFT,
    [F_BUTTON_RIGHT] = &BUTTON_RIGHT,
    [F_BUTTON_A] = &BUTTON_A,
    [F_BUTTON_B] = &BUTTON_B,
    [F_BUTTON_X] = &BUTTON_A,
    [F_BUTTON_Y] = &BUTTON_B,
    [F_BUTTON_L] = &BUTTON_LEFT,
    [F_BUTTON_R] = &BUTTON_RIGHT,
    [F_BUTTON_START] = &BUTTON_MENU,
    [F_BUTTON_SELECT] = &BUTTON_MENU,
};

void f_platform_api__inputPoll(void)
{
}

const APlatformButton* f_platform_api__inputKeyGet(AKeyId Id)
{
    return (APlatformButton*)g_keys[Id];
}

const APlatformButton* f_platform_api__inputButtonGet(const APlatformController* Controller, AButtonId Id)
{
    F_UNUSED(Controller);

    return (APlatformButton*)g_buttons[Id];
}

bool f_platform_api__inputButtonPressGet(const APlatformButton* Button)
{
    return gb.buttons.repeat(Button->code, 0);
}

#endif // F_CONFIG_SYSTEM_GAMEBUINO
