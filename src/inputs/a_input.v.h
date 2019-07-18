/*
    Copyright 2010, 2016, 2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_INPUTS_INPUT_V_H
#define A_INC_INPUTS_INPUT_V_H

#include "inputs/a_input.p.h"

typedef struct AInputUserHeader AInputUserHeader;

#include "data/a_list.v.h"
#include "general/a_main.v.h"

struct AInputUserHeader {
    const char* name;
    AList* platformInputs; // List of APlatformInputButton/Analog/Touch
};

extern const APack a_pack__input;

extern const char* a__inputNameDefault;

extern void a_input__userHeaderInit(AInputUserHeader* Header);
extern void a_input__userHeaderFree(AInputUserHeader* Header);

extern void a_input__tick(void);

#endif // A_INC_INPUTS_INPUT_V_H
