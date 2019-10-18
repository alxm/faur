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
    #include "a_gamebuino_input.v.h"
}

#include <faur.v.h>

#if A_CONFIG_SYSTEM_GAMEBUINO
#include <Arduino.h>
#include <Gamebuino-Meta.h>

struct APlatformButton {
    Button code;
};

static const Button* const g_keys[A_KEY_NUM] = {
    [A_KEY_UP] = &BUTTON_UP,
    [A_KEY_DOWN] = &BUTTON_DOWN,
    [A_KEY_LEFT] = &BUTTON_LEFT,
    [A_KEY_RIGHT] = &BUTTON_RIGHT,
    [A_KEY_Z] = &BUTTON_A,
    [A_KEY_X] = &BUTTON_B,
    [A_KEY_C] = &BUTTON_MENU,
};

static const Button* const g_buttons[A_BUTTON_NUM] = {
    [A_BUTTON_UP] = &BUTTON_UP,
    [A_BUTTON_DOWN] = &BUTTON_DOWN,
    [A_BUTTON_LEFT] = &BUTTON_LEFT,
    [A_BUTTON_RIGHT] = &BUTTON_RIGHT,
    [A_BUTTON_A] = &BUTTON_A,
    [A_BUTTON_B] = &BUTTON_B,
    [A_BUTTON_X] = &BUTTON_A,
    [A_BUTTON_Y] = &BUTTON_B,
    [A_BUTTON_L] = &BUTTON_LEFT,
    [A_BUTTON_R] = &BUTTON_RIGHT,
    [A_BUTTON_START] = &BUTTON_MENU,
    [A_BUTTON_SELECT] = &BUTTON_MENU,
};

void a_platform_api__inputPoll(void)
{
}

const APlatformButton* a_platform_api__inputKeyGet(AKeyId Id)
{
    return (APlatformButton*)g_keys[Id];
}

const APlatformButton* a_platform_api__inputButtonGet(const APlatformController* Controller, AButtonId Id)
{
    A_UNUSED(Controller);

    return (APlatformButton*)g_buttons[Id];
}

bool a_platform_api__inputButtonPressGet(const APlatformButton* Button)
{
    return gb.buttons.repeat(Button->code, 0);
}

#endif // A_CONFIG_SYSTEM_GAMEBUINO
