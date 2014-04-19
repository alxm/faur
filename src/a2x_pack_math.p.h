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

#ifndef A2X_PACK_MATH_PH
#define A2X_PACK_MATH_PH

#include "a2x_app_includes.h"

#define a_math_srand()     srand(time(NULL))
#define a_math_rand(range) (rand() % (range))

#define a_math_min(x, y)         \
({                               \
    const typeof(x) a__x = (x);  \
    const typeof(y) a__y = (y);  \
    (a__x < a__y) ? a__x : a__y; \
})

#define a_math_max(x, y)         \
({                               \
    const typeof(x) a__x = (x);  \
    const typeof(y) a__y = (y);  \
    (a__x > a__y) ? a__x : a__y; \
})

#define a_math_abs(x)           \
({                              \
    const typeof(x) a__x = (x); \
    (a__x >= 0) ? a__x : -a__x; \
})

#define A_MATH_PI                3.14159265f
#define A_MATH_ANGLES_NUM        256

#define a_math_degToRad(a)  ((a) * A_MATH_PI / (A_MATH_ANGLES_NUM / 2))
#define a_math_radToDeg(a)  ((int)((a) * (A_MATH_ANGLES_NUM / 2) / A_MATH_PI))
#define a_math_sin(a)       (a_math_sin_val[(a)])
#define a_math_cos(a)       (a_math_cos_val[(a)])
#define a_math_wrap(x, w)   ((x) & ((w) - 1))
#define a_math_wrapAngle(a) (a_math_wrap((a), A_MATH_ANGLES_NUM))

#define a_math_constrain(x, min, max) \
({                                    \
    int a__new = (x);                 \
    if(a__new < (min)) {              \
        a__new = (min);               \
    } else if(a__new > (max)) {       \
        a__new = (max);               \
    }                                 \
    a__new;                           \
})

extern double a_math_sin_val[A_MATH_ANGLES_NUM];
extern double a_math_cos_val[A_MATH_ANGLES_NUM];

extern int a_math_log2(int x);

#endif // A2X_PACK_MATH_PH
