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

#ifndef A2X_PACK_FIX_PH
#define A2X_PACK_FIX_PH

#include "a2x_app_includes.h"

typedef int fix;
typedef unsigned int ufix;

#include "a2x_pack_math.p.h"

#define A_FIX_BIT_PRECISION (8)
#define A_FIX_ONE           (1 << A_FIX_BIT_PRECISION)

#define a_fix_itofix(x) ((x) << A_FIX_BIT_PRECISION)
#define a_fix_ftofix(x) ((x) * (1 << A_FIX_BIT_PRECISION))
#define a_fix_fixtoi(x) ((x) >> A_FIX_BIT_PRECISION)
#define a_fix_fixtof(x) ((float)(x) / (1 << A_FIX_BIT_PRECISION))
#define a_fix_mul(x, y) (((x) * (y)) >> A_FIX_BIT_PRECISION)
#define a_fix_div(x, y) (((x) << A_FIX_BIT_PRECISION) / (y))

extern fix a_fix_sin_val[A_MATH_ANGLES_NUM];
extern fix a_fix_cos_val[A_MATH_ANGLES_NUM];

#define a_fix_sin(a)       (a_fix_sin_val[(a)])
#define a_fix_cos(a)       (a_fix_cos_val[(a)])
#define a_fix_wrapAngle(a) ((a) & ((A_MATH_ANGLES_NUM << A_FIX_BIT_PRECISION) - 1))

#endif // A2X_PACK_FIX_PH
