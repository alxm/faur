/*
    Copyright 2010, 2019-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_STATE_V_H
#define F_INC_GENERAL_STATE_V_H

#include "f_state.p.h"

typedef struct FStateEntry FStateEntry;

#include "../data/f_listintr.v.h"
#include "../general/f_init.v.h"

struct FStateEntry {
    FListIntrNode listNode;
    const char* name;
    FCallState* handler;
    F__StateStage stage;
};

extern const FPack f_pack__state;

extern bool f_state__runStep(void);

#endif // F_INC_GENERAL_STATE_V_H
