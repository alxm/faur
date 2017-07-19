/*
    Copyright 2010, 2016, 2017 Alex Margarit

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
#define A_FIX_FRACTION_MASK (A_FIX_ONE - 1)

extern AFix a_fix__sin[A_MATH_ANGLES_NUM];
extern AFix a_fix__cos[A_MATH_ANGLES_NUM];

static inline AFix a_fix_itofix(int X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFix a_fix_ftofix(float X)
{
    return (AFix)(X * (1 << A_FIX_BIT_PRECISION));
}

static inline int a_fix_fixtoi(AFix X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fix_fixtof(AFix X)
{
    return (float)X / (1 << A_FIX_BIT_PRECISION);
}

static inline AFix a_fix_mul(AFix X, AFix Y)
{
    return (AFix)(((int64_t)X * Y) >> A_FIX_BIT_PRECISION);
}

static inline AFix a_fix_div(AFix X, AFix Y)
{
    return (AFix)(((int64_t)X << A_FIX_BIT_PRECISION) / Y);
}

static inline AFix a_fix_sqrt(AFix X)
{
    return (AFix)(sqrtf((float)X) * (1 << (A_FIX_BIT_PRECISION / 2)));
}

static inline AFix a_fix_round(AFix X)
{
    return (X + A_FIX_ONE / 2) & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix a_fix_floor(AFix X)
{
    return X & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix a_fix_ceiling(AFix X)
{
    return (X + A_FIX_ONE - 1) & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix a_fix_truncate(AFix X)
{
    if(X >= 0) {
        return X & (AFix)~A_FIX_FRACTION_MASK;
    } else {
        return -((-X) & (AFix)~A_FIX_FRACTION_MASK);
    }
}

static inline AFixu a_fixu_itofix(unsigned X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFixu a_fixu_ftofix(float X)
{
    return (AFixu)(X * (1 << A_FIX_BIT_PRECISION));
}

static inline unsigned a_fixu_fixtoi(AFixu X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fixu_fixtof(AFixu X)
{
    return (float)X / (1 << A_FIX_BIT_PRECISION);
}

static inline AFixu a_fixu_mul(AFixu X, AFixu Y)
{
    return (AFixu)(((uint64_t)X * Y) >> A_FIX_BIT_PRECISION);
}

static inline AFixu a_fixu_div(AFixu X, AFixu Y)
{
    return (AFixu)(((uint64_t)X << A_FIX_BIT_PRECISION) / Y);
}

static inline AFixu a_fixu_sqrt(AFixu X)
{
    return (AFixu)(sqrtf((float)X) * (1 << (A_FIX_BIT_PRECISION / 2)));
}

static inline AFixu a_fixu_round(AFixu X)
{
    return (X + A_FIX_ONE / 2) & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu a_fixu_floor(AFixu X)
{
    return X & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu a_fixu_ceiling(AFixu X)
{
    return (X + A_FIX_ONE - 1) & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu a_fixu_truncate(AFixu X)
{
    return X & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFix a_fix_sin(unsigned Angle)
{
    return a_fix__sin[a_math_wrapAngle(Angle)];
}

static inline AFix a_fix_cos(unsigned Angle)
{
    return a_fix__cos[a_math_wrapAngle(Angle)];
}

static inline AFix a_fix_sinf(AFixu Angle)
{
    return a_fix__sin[a_math_wrapAngle(a_fixu_fixtoi(Angle))];
}

static inline AFix a_fix_cosf(AFixu Angle)
{
    return a_fix__cos[a_math_wrapAngle(a_fixu_fixtoi(Angle))];
}

extern unsigned a_fix_atan(AFix X1, AFix Y1, AFix X2, AFix Y2);
extern void a_fix_rotate(AFix X, AFix Y, unsigned Angle, AFix* NewX, AFix* NewY);
