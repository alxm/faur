/*
    Copyright 2016-2018 Alex Margarit

    This file is part of a2x-framework.

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

#include "a2x_pack_input_controller.v.h"

#include "a2x_pack_input_analog.v.h"
#include "a2x_pack_input_button.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_settings.v.h"

void a_input_controller__init(void)
{
    for(unsigned i = a_platform__controllerNumGet(); i--; ) {
        a_platform__controllerSet(i);

        APlatformAnalog* x = a_platform__analogGet(A_AXIS_LEFTX);
        APlatformAnalog* y = a_platform__analogGet(A_AXIS_LEFTY);
        APlatformAnalog* lt = a_platform__analogGet(A_AXIS_LEFTTRIGGER);
        APlatformAnalog* rt = a_platform__analogGet(A_AXIS_RIGHTTRIGGER);
        APlatformButton* u = a_platform__buttonGet(A_BUTTON_UP);
        APlatformButton* d = a_platform__buttonGet(A_BUTTON_DOWN);
        APlatformButton* l = a_platform__buttonGet(A_BUTTON_LEFT);
        APlatformButton* r = a_platform__buttonGet(A_BUTTON_RIGHT);
        APlatformButton* ul = a_platform__buttonGet(A_BUTTON_UPLEFT);
        APlatformButton* ur = a_platform__buttonGet(A_BUTTON_UPRIGHT);
        APlatformButton* dl = a_platform__buttonGet(A_BUTTON_DOWNLEFT);
        APlatformButton* dr = a_platform__buttonGet(A_BUTTON_DOWNRIGHT);
        APlatformButton* lb = a_platform__buttonGet(A_BUTTON_L);
        APlatformButton* rb = a_platform__buttonGet(A_BUTTON_R);

        // GP2X and Wiz dpad diagonals are dedicated buttons, split them into
        // their cardinal directions.
        if(u && d && l && r && ul && ur && dl && dr) {
            a_platform__buttonForward(ul, u);
            a_platform__buttonForward(ul, l);

            a_platform__buttonForward(ur, u);
            a_platform__buttonForward(ur, r);

            a_platform__buttonForward(dl, d);
            a_platform__buttonForward(dl, l);

            a_platform__buttonForward(dr, d);
            a_platform__buttonForward(dr, r);
        }

        // Forward the left analog stick to the direction buttons
        if(x && y && u && d && l && r) {
            if(!a_platform__controllerIsMapped()) {
                if(a_settings_boolGet(A_SETTING_INPUT_ANALOG_AXES_SWITCH)) {
                    APlatformAnalog* save = x;

                    x = y;
                    y = save;
                }

                if(a_settings_boolGet(A_SETTING_INPUT_ANALOG_AXES_INVERT)) {
                    APlatformButton* save;

                    save = u;
                    u = d;
                    d = save;

                    save = l;
                    l = r;
                    r = save;
                }
            }

            a_platform__analogForward(x, l, r);
            a_platform__analogForward(y, u, d);
        }

        // Forward analog shoulder triggers to the shoulder buttons
        if(lt && rt && lb && rb) {
            a_platform__analogForward(lt, NULL, lb);
            a_platform__analogForward(rt, NULL, rb);
        }
    }
}

unsigned a_input_controllerNumGet(void)
{
    return a_platform__controllerNumGet();
}

void a_input_controllerSet(unsigned Index)
{
    if(Index >= a_platform__controllerNumGet()) {
        a_out__fatal("a_input_controllerSet: Cannot set %d, only %d total",
                     Index,
                     a_platform__controllerNumGet());
        return;
    }

    a_platform__controllerSet(Index);
}
