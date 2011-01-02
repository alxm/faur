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

typedef int fix12;
typedef int fix8;

#include "a2x_pack_math.p.h"

#define FONE12            (1 << 12)
#define a_fix12_to8(x)    ((x) >> 4)
#define a_fix12_itofix(x) ((x) << 12)
#define a_fix12_ftofix(x) ((x) * (1 << 12))
#define a_fix12_fixtoi(x) ((x) >> 12)
#define a_fix12_fixtof(x) ((float)(x) / (1 << 12))
#define a_fix12_mul(x, y) (((x) * (y)) >> 12)
#define a_fix12_div(x, y) (((x) << 12) / (y))
extern fix12 a_fix12_Sin[A_ANGLES_NUM];
extern fix12 a_fix12_Cos[A_ANGLES_NUM];
#define a_fix12_sin(a)       (a_fix12_Sin[(a)])
#define a_fix12_cos(a)       (a_fix12_Cos[(a)])
#define a_fix12_wrapAngle(a) ((a) & ((A_ANGLES_NUM << 12) - 1))

#define FONE8            (1 << 8)
#define a_fix8_to12(x)   ((x) << 4)
#define a_fix8_itofix(x) ((x) << 8)
#define a_fix8_ftofix(x) ((x) * (1 << 8))
#define a_fix8_fixtoi(x) ((x) >> 8)
#define a_fix8_fixtof(x) ((float)(x) / (1 << 8))
#define a_fix8_mul(x, y) (((x) * (y)) >> 8)
#define a_fix8_div(x, y) (((x) << 8) / (y))
extern fix8 a_fix8_Sin[A_ANGLES_NUM];
extern fix8 a_fix8_Cos[A_ANGLES_NUM];
#define a_fix8_sin(a)       (a_fix8_Sin[(a)])
#define a_fix8_cos(a)       (a_fix8_Cos[(a)])
#define a_fix8_wrapAngle(a) ((a) & ((A_ANGLES_NUM << 8) - 1))

#endif // A2X_PACK_FIX_PH
