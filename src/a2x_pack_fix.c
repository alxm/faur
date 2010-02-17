/*
    Copyright 2010 Alex Margarit

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

#include "a2x_pack_fix.h"

fix12 a_fix12_Sin[A_ANGLES_NUM];
fix12 a_fix12_Cos[A_ANGLES_NUM];

fix8 a_fix8_Sin[A_ANGLES_NUM];
fix8 a_fix8_Cos[A_ANGLES_NUM];

void a__fix_set(void)
{
    for(int a = 0; a < A_ANGLES_NUM; a++) {
		a_fix12_Sin[a] = a_fix12_ftofix(a_math_sin(a));
		a_fix12_Cos[a] = a_fix12_ftofix(a_math_cos(a));

        a_fix8_Sin[a] = a_fix12_to8(a_fix12_Sin[a]);
        a_fix8_Cos[a] = a_fix12_to8(a_fix12_Cos[a]);
	}
}
