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

#ifndef F_INC_MATH_FIX_P_H
#define F_INC_MATH_FIX_P_H

#include "../general/f_system_includes.h"

typedef int32_t FFix;
typedef uint32_t FFixu;

typedef struct {
    FFix x, y;
} FVectorFix;

typedef struct {
    int x, y;
} FVectorInt;

#define F_FIX_BIT_PRECISION (16)
#define F_FIX_ONE           (1 << F_FIX_BIT_PRECISION)
#define F_FIX_FRACTION_MASK (F_FIX_ONE - 1)
#define F_FIX_MIN_INT       (INT32_MIN >> F_FIX_BIT_PRECISION)
#define F_FIX_MAX_INT       (INT32_MAX >> F_FIX_BIT_PRECISION)
#define F_FIX_MAX_INTU      (UINT32_MAX >> F_FIX_BIT_PRECISION)

#define F_FIX_ANGLES_NUM 4096u

typedef enum {
    F_DEG_001_INT = (F_FIX_ANGLES_NUM / 360),
    F_DEG_022_INT = (F_FIX_ANGLES_NUM / 16),
    F_DEG_045_INT = (F_FIX_ANGLES_NUM / 8),
    F_DEG_067_INT = (F_DEG_045_INT + F_DEG_022_INT),
    F_DEG_090_INT = (2 * F_DEG_045_INT),
    F_DEG_112_INT = (F_DEG_090_INT + F_DEG_022_INT),
    F_DEG_135_INT = (3 * F_DEG_045_INT),
    F_DEG_157_INT = (F_DEG_135_INT + F_DEG_022_INT),
    F_DEG_180_INT = (4 * F_DEG_045_INT),
    F_DEG_202_INT = (F_DEG_180_INT + F_DEG_022_INT),
    F_DEG_225_INT = (5 * F_DEG_045_INT),
    F_DEG_247_INT = (F_DEG_225_INT + F_DEG_022_INT),
    F_DEG_270_INT = (6 * F_DEG_045_INT),
    F_DEG_292_INT = (F_DEG_270_INT + F_DEG_022_INT),
    F_DEG_315_INT = (7 * F_DEG_045_INT),
    F_DEG_337_INT = (F_DEG_315_INT + F_DEG_022_INT),
    F_DEG_360_INT = (8 * F_DEG_045_INT),
} FDegInt;

typedef enum {
    F_DEG_001_FIX = (F_FIX_ANGLES_NUM * F_FIX_ONE / 360),
    F_DEG_022_FIX = (F_DEG_022_INT * F_FIX_ONE),
    F_DEG_045_FIX = (F_DEG_045_INT * F_FIX_ONE),
    F_DEG_067_FIX = (F_DEG_067_INT * F_FIX_ONE),
    F_DEG_090_FIX = (F_DEG_090_INT * F_FIX_ONE),
    F_DEG_112_FIX = (F_DEG_112_INT * F_FIX_ONE),
    F_DEG_135_FIX = (F_DEG_135_INT * F_FIX_ONE),
    F_DEG_157_FIX = (F_DEG_157_INT * F_FIX_ONE),
    F_DEG_180_FIX = (F_DEG_180_INT * F_FIX_ONE),
    F_DEG_202_FIX = (F_DEG_202_INT * F_FIX_ONE),
    F_DEG_225_FIX = (F_DEG_225_INT * F_FIX_ONE),
    F_DEG_247_FIX = (F_DEG_247_INT * F_FIX_ONE),
    F_DEG_270_FIX = (F_DEG_270_INT * F_FIX_ONE),
    F_DEG_292_FIX = (F_DEG_292_INT * F_FIX_ONE),
    F_DEG_315_FIX = (F_DEG_315_INT * F_FIX_ONE),
    F_DEG_337_FIX = (F_DEG_337_INT * F_FIX_ONE),
    F_DEG_360_FIX = (F_DEG_360_INT * F_FIX_ONE),
} FDegFix;

#if F_CONFIG_BUILD_LUT_GENERATE
    extern FFix f__fix_sin[F_FIX_ANGLES_NUM];
    extern FFix f__fix_csc[F_FIX_ANGLES_NUM];
#else
    extern const FFix f__fix_sin[F_FIX_ANGLES_NUM];
    extern const FFix f__fix_csc[F_FIX_ANGLES_NUM];
#endif

static inline FFix f_fix_fromInt(int X)
{
    return X << F_FIX_BIT_PRECISION;
}

static inline FFix f_fix_fromFloat(float X)
{
    return (FFix)(X * F_FIX_ONE);
}

static inline FFix f_fix_fromDouble(double X)
{
    return (FFix)(X * F_FIX_ONE);
}

static inline int f_fix_toInt(FFix X)
{
    return X >> F_FIX_BIT_PRECISION;
}

static inline float f_fix_toFloat(FFix X)
{
    return (float)X / F_FIX_ONE;
}

static inline double f_fix_toDouble(FFix X)
{
    return (double)X / F_FIX_ONE;
}

static inline FFix f_fix_mul(FFix X, FFix Y)
{
    return (FFix)(((int64_t)X * Y) >> F_FIX_BIT_PRECISION);
}

static inline FFix f_fix_div(FFix X, FFix Y)
{
    return (FFix)(((int64_t)X << F_FIX_BIT_PRECISION) / Y);
}

static inline FFix f_fix_inverse(FFix X)
{
    return (FFix)((int64_t)F_FIX_ONE * F_FIX_ONE / X);
}

static inline FFix f_fix_sqrt(FFix X)
{
    return (FFix)(sqrtf((float)X) * (1 << (F_FIX_BIT_PRECISION / 2)));
}

static inline FFix f_fix_round(FFix X)
{
    return (X + F_FIX_ONE / 2) & (FFix)~F_FIX_FRACTION_MASK;
}

static inline FFix f_fix_floor(FFix X)
{
    return X & (FFix)~F_FIX_FRACTION_MASK;
}

static inline FFix f_fix_ceiling(FFix X)
{
    return (X + F_FIX_ONE - 1) & (FFix)~F_FIX_FRACTION_MASK;
}

static inline FFix f_fix_truncate(FFix X)
{
    if(X >= 0) {
        return X & (FFix)~F_FIX_FRACTION_MASK;
    } else {
        return -((-X) & (FFix)~F_FIX_FRACTION_MASK);
    }
}

static inline FFix f_fix_fraction(FFix X)
{
    if(X >= 0) {
        return X & F_FIX_FRACTION_MASK;
    } else {
        return -((-X) & F_FIX_FRACTION_MASK);
    }
}

static inline FFixu f_fixu_fromInt(unsigned X)
{
    return X << F_FIX_BIT_PRECISION;
}

static inline FFixu f_fixu_fromFloat(float X)
{
    return (FFixu)(X * F_FIX_ONE);
}

static inline FFixu f_fixu_fromDouble(double X)
{
    return (FFixu)(X * F_FIX_ONE);
}

static inline unsigned f_fixu_toInt(FFixu X)
{
    return X >> F_FIX_BIT_PRECISION;
}

static inline float f_fixu_toFloat(FFixu X)
{
    return (float)X / F_FIX_ONE;
}

static inline double f_fixu_toDouble(FFixu X)
{
    return (double)X / F_FIX_ONE;
}

static inline FFixu f_fixu_mul(FFixu X, FFixu Y)
{
    return (FFixu)(((uint64_t)X * Y) >> F_FIX_BIT_PRECISION);
}

static inline FFixu f_fixu_div(FFixu X, FFixu Y)
{
    return (FFixu)(((uint64_t)X << F_FIX_BIT_PRECISION) / Y);
}

static inline FFixu f_fixu_inverse(FFixu X)
{
    return (FFixu)((uint64_t)F_FIX_ONE * F_FIX_ONE / X);
}

static inline FFixu f_fixu_sqrt(FFixu X)
{
    return (FFixu)(sqrtf((float)X) * (1 << (F_FIX_BIT_PRECISION / 2)));
}

static inline FFixu f_fixu_round(FFixu X)
{
    return (X + F_FIX_ONE / 2) & (FFixu)~F_FIX_FRACTION_MASK;
}

static inline FFixu f_fixu_floor(FFixu X)
{
    return X & (FFixu)~F_FIX_FRACTION_MASK;
}

static inline FFixu f_fixu_ceiling(FFixu X)
{
    return (X + F_FIX_ONE - 1) & (FFixu)~F_FIX_FRACTION_MASK;
}

static inline FFixu f_fixu_truncate(FFixu X)
{
    return X & (FFixu)~F_FIX_FRACTION_MASK;
}

static inline FFixu f_fixu_fraction(FFixu X)
{
    return X & F_FIX_FRACTION_MASK;
}

static inline unsigned f_fix_angleWrap(unsigned Angle)
{
    return Angle & (F_FIX_ANGLES_NUM - 1);
}

static inline FFixu f_fix_angleWrapf(FFixu Angle)
{
    return Angle & (F_FIX_ANGLES_NUM * F_FIX_ONE - 1);
}

static inline unsigned f_fix_angleFromDeg(unsigned Degrees)
{
    return F_FIX_ANGLES_NUM * Degrees / 360;
}

static inline FFixu f_fix_angleFromDegf(unsigned Degrees)
{
    return (FFixu)((uint64_t)(F_FIX_ONE * F_FIX_ANGLES_NUM) * Degrees / 360);
}

static inline FFix f_fix_sin(unsigned Angle)
{
    return f__fix_sin[f_fix_angleWrap(Angle)];
}

static inline FFix f_fix_cos(unsigned Angle)
{
    return f__fix_sin[f_fix_angleWrap(Angle + F_DEG_090_INT)];
}

static inline FFix f_fix_csc(unsigned Angle)
{
    return f__fix_csc[f_fix_angleWrap(Angle)];
}

static inline FFix f_fix_sec(unsigned Angle)
{
    return f__fix_csc[f_fix_angleWrap(Angle + F_DEG_090_INT)];
}

static inline FFix f_fix_sinf(FFixu Angle)
{
    return f__fix_sin[f_fix_angleWrap(f_fixu_toInt(Angle))];
}

static inline FFix f_fix_cosf(FFixu Angle)
{
    return f__fix_sin[f_fix_angleWrap(f_fixu_toInt(Angle + F_DEG_090_FIX))];
}

static inline FFix f_fix_cscf(FFixu Angle)
{
    return f__fix_csc[f_fix_angleWrap(f_fixu_toInt(Angle))];
}

static inline FFix f_fix_secf(FFixu Angle)
{
    return f__fix_csc[f_fix_angleWrap(f_fixu_toInt(Angle + F_DEG_090_FIX))];
}

extern unsigned f_fix_atan(FFix X1, FFix Y1, FFix X2, FFix Y2);

extern FVectorFix f_fix_rotateCounter(FFix X, FFix Y, unsigned Angle);
extern FVectorFix f_fix_rotateClockwise(FFix X, FFix Y, unsigned Angle);

static inline FVectorInt f_vectorfix_toInt(const FVectorFix Fix)
{
    FVectorInt v = {f_fix_toInt(Fix.x), f_fix_toInt(Fix.y)};

    return v;
}

static inline FVectorFix f_vectorint_toFix(const FVectorInt Int)
{
    FVectorFix v = {f_fix_fromInt(Int.x), f_fix_fromInt(Int.y)};

    return v;
}

static inline bool f_vectorfix_equal(FVectorFix A, FVectorFix B)
{
    return A.x == B.x && A.y == B.y;
}

static inline bool f_vectorint_equal(FVectorInt A, FVectorInt B)
{
    return A.x == B.x && A.y == B.y;
}

#endif // F_INC_MATH_FIX_P_H
