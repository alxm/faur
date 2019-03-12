/*
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
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

#include "a2x_pack_input_controller.v.h"

#include "a2x_pack_main.v.h"
#include "a2x_pack_platform.v.h"

void a_input_controller__init(void)
{
    for(unsigned i = a_platform__inputControllerNumGet(); i--; ) {
        a_platform__inputControllerSet(i);

        // Split diagonals into individual cardinal directions (for GP2X/Wiz)
        a_platform__inputButtonForward(A_BUTTON_UPLEFT, A_BUTTON_UP);
        a_platform__inputButtonForward(A_BUTTON_UPLEFT, A_BUTTON_LEFT);

        a_platform__inputButtonForward(A_BUTTON_UPRIGHT, A_BUTTON_UP);
        a_platform__inputButtonForward(A_BUTTON_UPRIGHT, A_BUTTON_RIGHT);

        a_platform__inputButtonForward(A_BUTTON_DOWNLEFT, A_BUTTON_DOWN);
        a_platform__inputButtonForward(A_BUTTON_DOWNLEFT, A_BUTTON_LEFT);

        a_platform__inputButtonForward(A_BUTTON_DOWNRIGHT, A_BUTTON_DOWN);
        a_platform__inputButtonForward(A_BUTTON_DOWNRIGHT, A_BUTTON_RIGHT);

        AAxisId axisX = A_AXIS_LEFTX;
        AAxisId axisY = A_AXIS_LEFTY;
        AButtonId buttonUp = A_BUTTON_UP;
        AButtonId buttonDown = A_BUTTON_DOWN;
        AButtonId buttonLeft = A_BUTTON_LEFT;
        AButtonId buttonRight = A_BUTTON_RIGHT;

        if(!a_platform__inputControllerIsMapped()) {
            #if A_CONFIG_INPUT_ANALOG_AXES_SWITCH
                axisX = A_AXIS_LEFTY;
                axisY = A_AXIS_LEFTX;
            #endif

            #if A_CONFIG_INPUT_ANALOG_AXES_INVERT
                buttonUp = A_BUTTON_DOWN;
                buttonDown = A_BUTTON_UP;
                buttonLeft = A_BUTTON_RIGHT;
                buttonRight = A_BUTTON_LEFT;
            #endif
        }

        // Forward the left analog stick to the direction buttons
        a_platform__inputAnalogForward(axisX, buttonLeft, buttonRight);
        a_platform__inputAnalogForward(axisY, buttonUp, buttonDown);

        // Forward analog shoulder triggers to the shoulder buttons
        a_platform__inputAnalogForward(
            A_AXIS_LEFTTRIGGER, A_BUTTON_INVALID, A_BUTTON_L);
        a_platform__inputAnalogForward(
            A_AXIS_RIGHTTRIGGER, A_BUTTON_INVALID, A_BUTTON_R);
    }

    #if A_CONFIG_SYSTEM_PANDORA
        // Pandora's game buttons are actually keyboard keys
        a_platform__inputButtonForward(A_KEY_UP, A_BUTTON_UP);
        a_platform__inputButtonForward(A_KEY_DOWN, A_BUTTON_DOWN);
        a_platform__inputButtonForward(A_KEY_LEFT, A_BUTTON_LEFT);
        a_platform__inputButtonForward(A_KEY_RIGHT, A_BUTTON_RIGHT);
        a_platform__inputButtonForward(A_KEY_RSHIFT, A_BUTTON_L);
        a_platform__inputButtonForward(A_KEY_RCTRL, A_BUTTON_R);
        a_platform__inputButtonForward(A_KEY_HOME, A_BUTTON_X);
        a_platform__inputButtonForward(A_KEY_END, A_BUTTON_B);
        a_platform__inputButtonForward(A_KEY_PAGEDOWN, A_BUTTON_A);
        a_platform__inputButtonForward(A_KEY_PAGEUP, A_BUTTON_Y);
        a_platform__inputButtonForward(A_KEY_LALT, A_BUTTON_START);
        a_platform__inputButtonForward(A_KEY_LCTRL, A_BUTTON_SELECT);
    #endif
}

unsigned a_input_controllerNumGet(void)
{
    return a_platform__inputControllerNumGet();
}

void a_input_controllerSet(unsigned Index)
{
    if(Index >= a_platform__inputControllerNumGet()) {
        A__FATAL("a_input_controllerSet(%d): Invalid arg, %d total",
                 Index,
                 a_platform__inputControllerNumGet());
        return;
    }

    a_platform__inputControllerSet(Index);
}
