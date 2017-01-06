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
typedef uint32_t AFixu;

#include "a2x_pack_math.p.h"

#define A_FIX_BIT_PRECISION (8)
#define A_FIX_ONE           (1 << A_FIX_BIT_PRECISION)

static inline AFix a_fix_itofix(int X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFix a_fix_ftofix(float X)
{
    return X * (1 << A_FIX_BIT_PRECISION);
}

static inline int a_fix_fixtoi(AFix X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fix_fixtof(AFix X)
{
    return X / (1 << A_FIX_BIT_PRECISION);
}

static inline AFix a_fix_mul(AFix X, AFix Y)
{
    return ((int64_t)X * Y) >> A_FIX_BIT_PRECISION;
}

static inline AFix a_fix_div(AFix X, AFix Y)
{
    return ((int64_t)X << A_FIX_BIT_PRECISION) / Y;
}

static inline AFixu a_fixu_itofix(unsigned int X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFixu a_fixu_ftofix(float X)
{
    return X * (1 << A_FIX_BIT_PRECISION);
}

static inline unsigned int a_fixu_fixtoi(AFixu X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fixu_fixtof(AFixu X)
{
    return X / (1 << A_FIX_BIT_PRECISION);
}

static inline AFixu a_fixu_mul(AFixu X, AFixu Y)
{
    return ((uint64_t)X * Y) >> A_FIX_BIT_PRECISION;
}

static inline AFixu a_fixu_div(AFixu X, AFixu Y)
{
    return ((uint64_t)X << A_FIX_BIT_PRECISION) / Y;
}

extern AFix a_fix_sin_val[A_MATH_ANGLES_NUM];
extern AFix a_fix_cos_val[A_MATH_ANGLES_NUM];

static inline AFix a_fix_sin(int Angle)
{
    return a_fix_sin_val[Angle];
}

static inline AFix a_fix_cos(int Angle)
{
    return a_fix_cos_val[Angle];
}

static inline AFix a_fix_wrapAngle(AFix Angle)
{
    return Angle & ((A_MATH_ANGLES_NUM << A_FIX_BIT_PRECISION) - 1);
}
