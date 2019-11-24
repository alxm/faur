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

#if F_CONFIG_SYSTEM_ODROID_GO
#include <odroid_go.h>

struct FPlatformButton {
    Button* odroidButton;
    bool pressed;
};

static FPlatformButton g_buttons[F_BUTTON_NUM] = {
    [F_BUTTON_UP] = {NULL, false},
    [F_BUTTON_DOWN] = {NULL, false},
    [F_BUTTON_LEFT] = {NULL, false},
    [F_BUTTON_RIGHT] = {NULL, false},
    [F_BUTTON_A] = {&GO.BtnB, false},
    [F_BUTTON_B] = {&GO.BtnA, false},
    [F_BUTTON_X] = {&GO.BtnB, false},
    [F_BUTTON_Y] = {&GO.BtnA, false},
    [F_BUTTON_L] = {&GO.BtnB, false},
    [F_BUTTON_R] = {&GO.BtnA, false},
    [F_BUTTON_START] = {&GO.BtnStart, false},
    [F_BUTTON_SELECT] = {&GO.BtnSelect, false},
};

void f_platform_api__inputPoll(void)
{
    g_buttons[F_BUTTON_UP].pressed = GO.JOY_Y.isAxisPressed() == 2;
    g_buttons[F_BUTTON_DOWN].pressed = GO.JOY_Y.isAxisPressed() == 1;
    g_buttons[F_BUTTON_LEFT].pressed = GO.JOY_X.isAxisPressed() == 2;
    g_buttons[F_BUTTON_RIGHT].pressed = GO.JOY_X.isAxisPressed() == 1;

    for(int b = F_BUTTON_A; b <= F_BUTTON_SELECT; b++) {
        g_buttons[b].pressed = g_buttons[b].odroidButton->isPressed() == 1;
    }
}

const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id)
{
    F_UNUSED(Controller);

    return &g_buttons[Id];
}

bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button)
{
    return Button->pressed;
}
#endif // F_CONFIG_SYSTEM_ODROID_GO
