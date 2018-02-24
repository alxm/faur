/*
    Copyright 2010, 2014, 2016-2018 Alex Margarit

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

typedef int32_t AFix;
typedef uint32_t AFixu;

#define A_FIX_BIT_PRECISION (16)
#define A_FIX_ONE           (1 << A_FIX_BIT_PRECISION)
#define A_FIX_FRACTION_MASK (A_FIX_ONE - 1)
#define A_FIX_MIN_INT       (INT32_MIN >> A_FIX_BIT_PRECISION)
#define A_FIX_MAX_INT       (INT32_MAX >> A_FIX_BIT_PRECISION)
#define A_FIXU_MAX_INT      (UINT32_MAX >> A_FIX_BIT_PRECISION)

#define A_FIX_ANGLES_NUM 4096u

typedef enum {
    A_INT_DEG_022 = (A_FIX_ANGLES_NUM / 16),
    A_INT_DEG_045 = (A_FIX_ANGLES_NUM / 8),
    A_INT_DEG_067 = (A_INT_DEG_045 + A_INT_DEG_022),
    A_INT_DEG_090 = (2 * A_INT_DEG_045),
    A_INT_DEG_112 = (A_INT_DEG_090 + A_INT_DEG_022),
    A_INT_DEG_135 = (3 * A_INT_DEG_045),
    A_INT_DEG_157 = (A_INT_DEG_135 + A_INT_DEG_022),
    A_INT_DEG_180 = (4 * A_INT_DEG_045),
    A_INT_DEG_202 = (A_INT_DEG_180 + A_INT_DEG_022),
    A_INT_DEG_225 = (5 * A_INT_DEG_045),
    A_INT_DEG_247 = (A_INT_DEG_225 + A_INT_DEG_022),
    A_INT_DEG_270 = (6 * A_INT_DEG_045),
    A_INT_DEG_292 = (A_INT_DEG_270 + A_INT_DEG_022),
    A_INT_DEG_315 = (7 * A_INT_DEG_045),
    A_INT_DEG_337 = (A_INT_DEG_315 + A_INT_DEG_022),
    A_INT_DEG_360 = (8 * A_INT_DEG_045),
} AIntAngles;

typedef enum {
    A_FIX_DEG_022 = (A_INT_DEG_022 * A_FIX_ONE),
    A_FIX_DEG_045 = (A_INT_DEG_045 * A_FIX_ONE),
    A_FIX_DEG_067 = (A_INT_DEG_067 * A_FIX_ONE),
    A_FIX_DEG_090 = (A_INT_DEG_090 * A_FIX_ONE),
    A_FIX_DEG_112 = (A_INT_DEG_112 * A_FIX_ONE),
    A_FIX_DEG_135 = (A_INT_DEG_135 * A_FIX_ONE),
    A_FIX_DEG_157 = (A_INT_DEG_157 * A_FIX_ONE),
    A_FIX_DEG_180 = (A_INT_DEG_180 * A_FIX_ONE),
    A_FIX_DEG_202 = (A_INT_DEG_202 * A_FIX_ONE),
    A_FIX_DEG_225 = (A_INT_DEG_225 * A_FIX_ONE),
    A_FIX_DEG_247 = (A_INT_DEG_247 * A_FIX_ONE),
    A_FIX_DEG_270 = (A_INT_DEG_270 * A_FIX_ONE),
    A_FIX_DEG_292 = (A_INT_DEG_292 * A_FIX_ONE),
    A_FIX_DEG_315 = (A_INT_DEG_315 * A_FIX_ONE),
    A_FIX_DEG_337 = (A_INT_DEG_337 * A_FIX_ONE),
    A_FIX_DEG_360 = (A_INT_DEG_360 * A_FIX_ONE),
    A_FIX_DEG_001 = (A_FIX_DEG_360 / 360),
} AFixAngles;

extern AFix a_fix__sin[A_FIX_ANGLES_NUM];
extern AFix a_fix__csc[A_FIX_ANGLES_NUM];

static inline AFix a_fix_fromInt(int X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFix a_fix_fromFloat(float X)
{
    return (AFix)(X * A_FIX_ONE);
}

static inline AFix a_fix_fromDouble(double X)
{
    return (AFix)(X * A_FIX_ONE);
}

static inline int a_fix_toInt(AFix X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fix_toFloat(AFix X)
{
    return (float)X / A_FIX_ONE;
}

static inline double a_fix_toDouble(AFix X)
{
    return (double)X / A_FIX_ONE;
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

static inline AFixu a_fixu_fromInt(unsigned X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFixu a_fixu_fromFloat(float X)
{
    return (AFixu)(X * A_FIX_ONE);
}

static inline AFixu a_fixu_fromDouble(double X)
{
    return (AFixu)(X * A_FIX_ONE);
}

static inline unsigned a_fixu_toInt(AFixu X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float a_fixu_toFloat(AFixu X)
{
    return (float)X / A_FIX_ONE;
}

static inline double a_fixu_toDouble(AFixu X)
{
    return (double)X / A_FIX_ONE;
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

static inline unsigned a_fix_wrapAngleInt(unsigned Angle)
{
    return Angle & (A_FIX_ANGLES_NUM - 1);
}

static inline AFixu a_fix_wrapAngleFix(AFixu Angle)
{
    return Angle & (A_FIX_ANGLES_NUM * A_FIX_ONE - 1);
}

static inline AFix a_fix_sin(unsigned Angle)
{
    return a_fix__sin[a_fix_wrapAngleInt(Angle)];
}

static inline AFix a_fix_cos(unsigned Angle)
{
    return a_fix__sin[a_fix_wrapAngleInt(Angle + A_INT_DEG_090)];
}

static inline AFix a_fix_csc(unsigned Angle)
{
    return a_fix__csc[a_fix_wrapAngleInt(Angle)];
}

static inline AFix a_fix_sec(unsigned Angle)
{
    return a_fix__csc[a_fix_wrapAngleInt(Angle + A_INT_DEG_090)];
}

static inline AFix a_fix_sinf(AFixu Angle)
{
    return a_fix__sin[a_fix_wrapAngleInt(a_fixu_toInt(Angle))];
}

static inline AFix a_fix_cosf(AFixu Angle)
{
    return a_fix__sin[a_fix_wrapAngleInt(a_fixu_toInt(Angle + A_FIX_DEG_090))];
}

static inline AFix a_fix_cscf(AFixu Angle)
{
    return a_fix__csc[a_fix_wrapAngleInt(a_fixu_toInt(Angle))];
}

static inline AFix a_fix_secf(AFixu Angle)
{
    return a_fix__csc[a_fix_wrapAngleInt(a_fixu_toInt(Angle + A_FIX_DEG_090))];
}

extern unsigned a_fix_atan(AFix X1, AFix Y1, AFix X2, AFix Y2);

extern void a_fix_rotateCounter(AFix X, AFix Y, unsigned Angle, AFix* NewX, AFix* NewY);
extern void a_fix_rotateClockwise(AFix X, AFix Y, unsigned Angle, AFix* NewX, AFix* NewY);
