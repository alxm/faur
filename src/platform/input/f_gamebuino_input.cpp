/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

struct FPlatformButton {
    Button code;
};

static const Button* const g_buttons[F_BUTTON_NUM] = {
    [F_BUTTON_UP] = &BUTTON_UP,
    [F_BUTTON_DOWN] = &BUTTON_DOWN,
    [F_BUTTON_LEFT] = &BUTTON_LEFT,
    [F_BUTTON_RIGHT] = &BUTTON_RIGHT,
    [F_BUTTON_A] = &BUTTON_A,
    [F_BUTTON_B] = &BUTTON_B,
    [F_BUTTON_X] = NULL,
    [F_BUTTON_Y] = NULL,
    [F_BUTTON_L] = NULL,
    [F_BUTTON_R] = NULL,
    [F_BUTTON_START] = &BUTTON_MENU,
    [F_BUTTON_SELECT] = &BUTTON_MENU,
};

void f_platform_api__inputPoll(void)
{
}

const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id)
{
    F_UNUSED(Controller);

    return (FPlatformButton*)g_buttons[Id];
}

bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button)
{
    return gb.buttons.repeat(Button->code, 0);
}

#endif // F_CONFIG_SYSTEM_GAMEBUINO
