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

#pragma once

#include "a2x_system_includes.h"

#define a_math_min(X, Y)          \
({                                \
    const __typeof(X) a__x = (X); \
    const __typeof(Y) a__y = (Y); \
    (a__x < a__y) ? a__x : a__y;  \
})

#define a_math_max(X, Y)          \
({                                \
    const __typeof(X) a__x = (X); \
    const __typeof(Y) a__y = (Y); \
    (a__x > a__y) ? a__x : a__y;  \
})

#define a_math_abs(X)             \
({                                \
    const __typeof(X) a__x = (X); \
    (a__x >= 0) ? a__x : -a__x;   \
})

#define A_MATH_PI                3.14159265f
#define A_MATH_ANGLES_NUM        256

#define a_math_degToRad(Angle)  ((Angle) * A_MATH_PI / (A_MATH_ANGLES_NUM / 2))
#define a_math_radToDeg(Angle)  ((int)((Angle) * (A_MATH_ANGLES_NUM / 2) / A_MATH_PI))
#define a_math_sin(Angle)       (a_math_sin_val[(Angle)])
#define a_math_cos(Angle)       (a_math_cos_val[(Angle)])
#define a_math_wrap(X, Limit)   ((X) & ((Limit) - 1))
#define a_math_wrapAngle(Angle) (a_math_wrap((Angle), A_MATH_ANGLES_NUM))

#define a_math_constrain(X, Min, Max)                             \
({                                                                \
    const __typeof(X) a__x = (X);                                 \
    const __typeof(X) a__min = (Min);                             \
    const __typeof(X) a__max = (Max);                             \
    (a__x < a__min) ? a__min : ((a__x > a__max) ? a__max : a__x); \
})

extern double a_math_sin_val[A_MATH_ANGLES_NUM];
extern double a_math_cos_val[A_MATH_ANGLES_NUM];
