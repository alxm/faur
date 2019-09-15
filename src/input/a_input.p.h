/*
    Copyright 2010, 2017, 2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_INPUTS_INPUT_P_H
#define A_INC_INPUTS_INPUT_P_H

#include "general/a_system_includes.h"

#include "input/a_analog.p.h"
#include "input/a_button.p.h"
#include "math/a_fix.p.h"

extern bool a_input_readKey(AKeyId Key);
extern bool a_input_readButton(AButtonId Button);
extern AFix a_input_readAnalog(AAnalogId Axis);

#endif // A_INC_INPUTS_INPUT_P_H
