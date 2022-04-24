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
    uint8_t analogPressVal;
    bool pressed;
};

static FPlatformButton g_buttons[F_BUTTON_NUM];

void f_platform_odroid_go_input__init(void)
{
    g_buttons[F_BUTTON_UP].odroidButton = &GO.JOY_Y;
    g_buttons[F_BUTTON_UP].analogPressVal = DPAD_V_FULL;

    g_buttons[F_BUTTON_DOWN].odroidButton = &GO.JOY_Y;
    g_buttons[F_BUTTON_DOWN].analogPressVal = DPAD_V_HALF;

    g_buttons[F_BUTTON_LEFT].odroidButton = &GO.JOY_X;
    g_buttons[F_BUTTON_LEFT].analogPressVal = DPAD_V_FULL;

    g_buttons[F_BUTTON_RIGHT].odroidButton = &GO.JOY_X;
    g_buttons[F_BUTTON_RIGHT].analogPressVal = DPAD_V_HALF;

    g_buttons[F_BUTTON_A].odroidButton = &GO.BtnB;
    g_buttons[F_BUTTON_B].odroidButton = &GO.BtnA;
    g_buttons[F_BUTTON_START].odroidButton = &GO.BtnStart;
    g_buttons[F_BUTTON_SELECT].odroidButton = &GO.BtnSelect;
    g_buttons[F_BUTTON_GUIDE].odroidButton = &GO.BtnMenu;
    g_buttons[F_BUTTON_VOLUP].odroidButton = &GO.BtnVolume;
    g_buttons[F_BUTTON_VOLDOWN].odroidButton = &GO.BtnVolume;
}

void f_platform_api__inputPoll(void)
{
    for(int b = F_BUTTON_NUM; b--; ) {
        Button* button = g_buttons[b].odroidButton;

        if(button != NULL) {
            g_buttons[b].pressed =
                (g_buttons[b].analogPressVal != 0)
                    ? (button->isAxisPressed() == g_buttons[b].analogPressVal)
                    : (button->isPressed() == 1);
        }
    }
}

const FPlatformButton* f_platform_api__inputButtonGet(const FPlatformController* Controller, FButtonId Id)
{
    F_UNUSED(Controller);

    return g_buttons[Id].odroidButton == NULL ? NULL : &g_buttons[Id];
}

bool f_platform_api__inputButtonPressGet(const FPlatformButton* Button)
{
    return Button->pressed;
}
#endif // F_CONFIG_SYSTEM_ODROID_GO
