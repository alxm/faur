/*
    Copyright 2010, 2014, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_MATH_FIX_P_H
#define A_INC_MATH_FIX_P_H

#include "general/f_system_includes.h"

typedef int32_t AFix;
typedef uint32_t AFixu;

typedef struct {
    AFix x, y;
} AVectorFix;

typedef struct {
    int x, y;
} AVectorInt;

#define A_FIX_BIT_PRECISION (16)
#define A_FIX_ONE           (1 << A_FIX_BIT_PRECISION)
#define A_FIX_FRACTION_MASK (A_FIX_ONE - 1)
#define A_FIX_MIN_INT       (INT32_MIN >> A_FIX_BIT_PRECISION)
#define A_FIX_MAX_INT       (INT32_MAX >> A_FIX_BIT_PRECISION)
#define A_FIX_MAX_INTU      (UINT32_MAX >> A_FIX_BIT_PRECISION)

#define A_FIX_ANGLES_NUM 4096u

typedef enum {
    A_DEG_001_INT = (A_FIX_ANGLES_NUM / 360),
    A_DEG_022_INT = (A_FIX_ANGLES_NUM / 16),
    A_DEG_045_INT = (A_FIX_ANGLES_NUM / 8),
    A_DEG_067_INT = (A_DEG_045_INT + A_DEG_022_INT),
    A_DEG_090_INT = (2 * A_DEG_045_INT),
    A_DEG_112_INT = (A_DEG_090_INT + A_DEG_022_INT),
    A_DEG_135_INT = (3 * A_DEG_045_INT),
    A_DEG_157_INT = (A_DEG_135_INT + A_DEG_022_INT),
    A_DEG_180_INT = (4 * A_DEG_045_INT),
    A_DEG_202_INT = (A_DEG_180_INT + A_DEG_022_INT),
    A_DEG_225_INT = (5 * A_DEG_045_INT),
    A_DEG_247_INT = (A_DEG_225_INT + A_DEG_022_INT),
    A_DEG_270_INT = (6 * A_DEG_045_INT),
    A_DEG_292_INT = (A_DEG_270_INT + A_DEG_022_INT),
    A_DEG_315_INT = (7 * A_DEG_045_INT),
    A_DEG_337_INT = (A_DEG_315_INT + A_DEG_022_INT),
    A_DEG_360_INT = (8 * A_DEG_045_INT),
} ADegInt;

typedef enum {
    A_DEG_001_FIX = (A_FIX_ANGLES_NUM * A_FIX_ONE / 360),
    A_DEG_022_FIX = (A_DEG_022_INT * A_FIX_ONE),
    A_DEG_045_FIX = (A_DEG_045_INT * A_FIX_ONE),
    A_DEG_067_FIX = (A_DEG_067_INT * A_FIX_ONE),
    A_DEG_090_FIX = (A_DEG_090_INT * A_FIX_ONE),
    A_DEG_112_FIX = (A_DEG_112_INT * A_FIX_ONE),
    A_DEG_135_FIX = (A_DEG_135_INT * A_FIX_ONE),
    A_DEG_157_FIX = (A_DEG_157_INT * A_FIX_ONE),
    A_DEG_180_FIX = (A_DEG_180_INT * A_FIX_ONE),
    A_DEG_202_FIX = (A_DEG_202_INT * A_FIX_ONE),
    A_DEG_225_FIX = (A_DEG_225_INT * A_FIX_ONE),
    A_DEG_247_FIX = (A_DEG_247_INT * A_FIX_ONE),
    A_DEG_270_FIX = (A_DEG_270_INT * A_FIX_ONE),
    A_DEG_292_FIX = (A_DEG_292_INT * A_FIX_ONE),
    A_DEG_315_FIX = (A_DEG_315_INT * A_FIX_ONE),
    A_DEG_337_FIX = (A_DEG_337_INT * A_FIX_ONE),
    A_DEG_360_FIX = (A_DEG_360_INT * A_FIX_ONE),
} ADegFix;

#if A_CONFIG_BUILD_FIX_LUT
    extern const AFix f__fix_sin[A_FIX_ANGLES_NUM];
    extern const AFix f__fix_csc[A_FIX_ANGLES_NUM];
#else
    extern AFix f__fix_sin[A_FIX_ANGLES_NUM];
    extern AFix f__fix_csc[A_FIX_ANGLES_NUM];
#endif

static inline AFix f_fix_fromInt(int X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFix f_fix_fromFloat(float X)
{
    return (AFix)(X * A_FIX_ONE);
}

static inline AFix f_fix_fromDouble(double X)
{
    return (AFix)(X * A_FIX_ONE);
}

static inline int f_fix_toInt(AFix X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float f_fix_toFloat(AFix X)
{
    return (float)X / A_FIX_ONE;
}

static inline double f_fix_toDouble(AFix X)
{
    return (double)X / A_FIX_ONE;
}

static inline AFix f_fix_mul(AFix X, AFix Y)
{
    return (AFix)(((int64_t)X * Y) >> A_FIX_BIT_PRECISION);
}

static inline AFix f_fix_div(AFix X, AFix Y)
{
    return (AFix)(((int64_t)X << A_FIX_BIT_PRECISION) / Y);
}

static inline AFix f_fix_sqrt(AFix X)
{
    return (AFix)(sqrtf((float)X) * (1 << (A_FIX_BIT_PRECISION / 2)));
}

static inline AFix f_fix_round(AFix X)
{
    return (X + A_FIX_ONE / 2) & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix f_fix_floor(AFix X)
{
    return X & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix f_fix_ceiling(AFix X)
{
    return (X + A_FIX_ONE - 1) & (AFix)~A_FIX_FRACTION_MASK;
}

static inline AFix f_fix_truncate(AFix X)
{
    if(X >= 0) {
        return X & (AFix)~A_FIX_FRACTION_MASK;
    } else {
        return -((-X) & (AFix)~A_FIX_FRACTION_MASK);
    }
}

static inline AFix f_fix_fraction(AFix X)
{
    if(X >= 0) {
        return X & A_FIX_FRACTION_MASK;
    } else {
        return -((-X) & A_FIX_FRACTION_MASK);
    }
}

static inline AFixu f_fixu_fromInt(unsigned X)
{
    return X << A_FIX_BIT_PRECISION;
}

static inline AFixu f_fixu_fromFloat(float X)
{
    return (AFixu)(X * A_FIX_ONE);
}

static inline AFixu f_fixu_fromDouble(double X)
{
    return (AFixu)(X * A_FIX_ONE);
}

static inline unsigned f_fixu_toInt(AFixu X)
{
    return X >> A_FIX_BIT_PRECISION;
}

static inline float f_fixu_toFloat(AFixu X)
{
    return (float)X / A_FIX_ONE;
}

static inline double f_fixu_toDouble(AFixu X)
{
    return (double)X / A_FIX_ONE;
}

static inline AFixu f_fixu_mul(AFixu X, AFixu Y)
{
    return (AFixu)(((uint64_t)X * Y) >> A_FIX_BIT_PRECISION);
}

static inline AFixu f_fixu_div(AFixu X, AFixu Y)
{
    return (AFixu)(((uint64_t)X << A_FIX_BIT_PRECISION) / Y);
}

static inline AFixu f_fixu_sqrt(AFixu X)
{
    return (AFixu)(sqrtf((float)X) * (1 << (A_FIX_BIT_PRECISION / 2)));
}

static inline AFixu f_fixu_round(AFixu X)
{
    return (X + A_FIX_ONE / 2) & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu f_fixu_floor(AFixu X)
{
    return X & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu f_fixu_ceiling(AFixu X)
{
    return (X + A_FIX_ONE - 1) & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu f_fixu_truncate(AFixu X)
{
    return X & (AFixu)~A_FIX_FRACTION_MASK;
}

static inline AFixu f_fixu_fraction(AFixu X)
{
    return X & A_FIX_FRACTION_MASK;
}

static inline unsigned f_fix_angleWrap(unsigned Angle)
{
    return Angle & (A_FIX_ANGLES_NUM - 1);
}

static inline AFixu f_fix_angleWrapf(AFixu Angle)
{
    return Angle & (A_FIX_ANGLES_NUM * A_FIX_ONE - 1);
}

static inline unsigned f_fix_angleFromDeg(unsigned Degrees)
{
    return A_FIX_ANGLES_NUM * Degrees / 360;
}

static inline AFixu f_fix_angleFromDegf(unsigned Degrees)
{
    return (AFixu)((uint64_t)(A_FIX_ONE * A_FIX_ANGLES_NUM) * Degrees / 360);
}

static inline AFix f_fix_sin(unsigned Angle)
{
    return f__fix_sin[f_fix_angleWrap(Angle)];
}

static inline AFix f_fix_cos(unsigned Angle)
{
    return f__fix_sin[f_fix_angleWrap(Angle + A_DEG_090_INT)];
}

static inline AFix f_fix_csc(unsigned Angle)
{
    return f__fix_csc[f_fix_angleWrap(Angle)];
}

static inline AFix f_fix_sec(unsigned Angle)
{
    return f__fix_csc[f_fix_angleWrap(Angle + A_DEG_090_INT)];
}

static inline AFix f_fix_sinf(AFixu Angle)
{
    return f__fix_sin[f_fix_angleWrap(f_fixu_toInt(Angle))];
}

static inline AFix f_fix_cosf(AFixu Angle)
{
    return f__fix_sin[f_fix_angleWrap(f_fixu_toInt(Angle + A_DEG_090_FIX))];
}

static inline AFix f_fix_cscf(AFixu Angle)
{
    return f__fix_csc[f_fix_angleWrap(f_fixu_toInt(Angle))];
}

static inline AFix f_fix_secf(AFixu Angle)
{
    return f__fix_csc[f_fix_angleWrap(f_fixu_toInt(Angle + A_DEG_090_FIX))];
}

extern unsigned f_fix_atan(AFix X1, AFix Y1, AFix X2, AFix Y2);

extern AVectorFix f_fix_rotateCounter(AFix X, AFix Y, unsigned Angle);
extern AVectorFix f_fix_rotateClockwise(AFix X, AFix Y, unsigned Angle);

static inline AVectorInt f_vectorfix_toInt(const AVectorFix Fix)
{
    AVectorInt v = {f_fix_toInt(Fix.x), f_fix_toInt(Fix.y)};

    return v;
}

static inline AVectorFix f_vectorint_toFix(const AVectorInt Int)
{
    AVectorFix v = {f_fix_fromInt(Int.x), f_fix_fromInt(Int.y)};

    return v;
}

static inline bool f_vectorfix_equal(AVectorFix A, AVectorFix B)
{
    return A.x == B.x && A.y == B.y;
}

static inline bool f_vectorint_equal(AVectorInt A, AVectorInt B)
{
    return A.x == B.x && A.y == B.y;
}

#endif // A_INC_MATH_FIX_P_H
