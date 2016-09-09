/*
    Copyright 2010, 2016 Alex Margarit

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

#pragma once

#include "a2x_system_includes.h"

typedef int32_t AFix;

#include "a2x_pack_math.p.h"

#define A_FIX_BIT_PRECISION (8)
#define A_FIX_ONE           (1 << A_FIX_BIT_PRECISION)

#define a_fix_itofix(X) ((X) << A_FIX_BIT_PRECISION)
#define a_fix_ftofix(X) ((X) * (1 << A_FIX_BIT_PRECISION))
#define a_fix_fixtoi(X) ((X) >> A_FIX_BIT_PRECISION)
#define a_fix_fixtof(X) ((float)(X) / (1 << A_FIX_BIT_PRECISION))
#define a_fix_mul(X, Y) (int32_t)(((int64_t)(X) * (Y)) >> A_FIX_BIT_PRECISION)
#define a_fix_div(X, Y) (int32_t)(((int64_t)(X) << A_FIX_BIT_PRECISION) / (Y))

extern AFix a_fix_sin_val[A_MATH_ANGLES_NUM];
extern AFix a_fix_cos_val[A_MATH_ANGLES_NUM];

#define a_fix_sin(Angle)       (a_fix_sin_val[(Angle)])
#define a_fix_cos(Angle)       (a_fix_cos_val[(Angle)])
#define a_fix_wrapAngle(Angle) ((Angle) & ((A_MATH_ANGLES_NUM << A_FIX_BIT_PRECISION) - 1))
