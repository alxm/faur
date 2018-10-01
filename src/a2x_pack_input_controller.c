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

#include "a2x_pack_out.v.h"
#include "a2x_pack_platform.v.h"
#include "a2x_pack_settings.v.h"

void a_input_controller__init(void)
{
    for(unsigned i = a_platform__controllerNumGet(); i--; ) {
        a_platform__controllerSet(i);

        APlatformAnalog* x = a_platform__analogGet("gamepad.a.leftX");
        APlatformAnalog* y = a_platform__analogGet("gamepad.a.leftY");
        APlatformAnalog* lt = a_platform__analogGet("gamepad.a.leftTrigger");
        APlatformAnalog* rt = a_platform__analogGet("gamepad.a.rightTrigger");
        APlatformButton* u = a_platform__buttonGet("gamepad.b.up");
        APlatformButton* d = a_platform__buttonGet("gamepad.b.down");
        APlatformButton* l = a_platform__buttonGet("gamepad.b.left");
        APlatformButton* r = a_platform__buttonGet("gamepad.b.right");
        APlatformButton* ul = a_platform__buttonGet("gamepad.b.upLeft");
        APlatformButton* ur = a_platform__buttonGet("gamepad.b.upRight");
        APlatformButton* dl = a_platform__buttonGet("gamepad.b.downLeft");
        APlatformButton* dr = a_platform__buttonGet("gamepad.b.downRight");
        APlatformButton* lb = a_platform__buttonGet("gamepad.b.l");
        APlatformButton* rb = a_platform__buttonGet("gamepad.b.r");

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
                if(a_settings_getBool("input.switchAxes")) {
                    APlatformAnalog* save = x;

                    x = y;
                    y = save;
                }

                if(a_settings_getBool("input.invertAxes")) {
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

        #if A_BUILD_SYSTEM_PANDORA
            if(!a_platform__controllerIsGeneric() && x && y) {
                u = a_platform__buttonGet("gamepad.b.up");
                d = a_platform__buttonGet("gamepad.b.down");
                l = a_platform__buttonGet("gamepad.b.left");
                r = a_platform__buttonGet("gamepad.b.right");

                // Forward the left analog nub to the direction buttons
                a_platform__analogForward(x, l, r);
                a_platform__analogForward(y, u, d);
            }
        #endif
    }

    // Set the built-in controller as default if one exists, else first one
    if(a_platform__controllerNumGet() > 0) {
        for(unsigned i = a_platform__controllerNumGet(); i--; ) {
            a_platform__controllerSet(i);

            if(!a_platform__controllerIsGeneric()) {
                a_platform__controllerSet(i);
                return;
            }
        }

        a_platform__controllerSet(0);
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
