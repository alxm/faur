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

#ifndef F_INC_INPUTS_BUTTON_V_H
#define F_INC_INPUTS_BUTTON_V_H

#include "f_button.p.h"

#include "../data/f_list.v.h"
#include "../data/f_listintr.v.h"
#include "../time/f_timer.v.h"

struct FButton {
    FListIntrNode listNode;
    const char* name; // friendly name
    FList* platformInputs; // FList<FPlatformButton*>
    FListIntr combos; // FListIntr<FButtonCombo*>
    FTimer* autoRepeat;
    bool isClone;
    bool waitForRelease;
    bool pressed;
};

extern void f_input_button__tick(void);

#endif // F_INC_INPUTS_BUTTON_V_H
