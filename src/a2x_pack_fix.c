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

#include "a2x_pack_fix.v.h"

AFix a_fix_sin_val[A_MATH_ANGLES_NUM];
AFix a_fix_cos_val[A_MATH_ANGLES_NUM];

void a_fix__init(void)
{
    for(unsigned a = 0; a < A_MATH_ANGLES_NUM; a++) {
        a_fix_sin_val[a] = a_fix_ftofix(a_math_sin(a));
		a_fix_cos_val[a] = a_fix_ftofix(a_math_cos(a));
	}
}
