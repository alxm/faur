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

#define A_MATH_ANGLES_NUM 256u

#define A_MATH_DEG_045 (A_MATH_ANGLES_NUM / 8)
#define A_MATH_DEG_090 (2 * A_MATH_DEG_045)
#define A_MATH_DEG_135 (3 * A_MATH_DEG_045)
#define A_MATH_DEG_180 (4 * A_MATH_DEG_045)
#define A_MATH_DEG_225 (5 * A_MATH_DEG_045)
#define A_MATH_DEG_270 (6 * A_MATH_DEG_045)
#define A_MATH_DEG_315 (7 * A_MATH_DEG_045)
#define A_MATH_DEG_360 (8 * A_MATH_DEG_045)

extern float a_math__sin[A_MATH_ANGLES_NUM];
extern float a_math__cos[A_MATH_ANGLES_NUM];

static inline int a_math_min(int X, int Y)
{
    return X < Y ? X : Y;
}

static inline unsigned a_math_minu(unsigned X, unsigned Y)
{
    return X < Y ? X : Y;
}

static inline int a_math_max(int X, int Y)
{
    return X > Y ? X : Y;
}

static inline unsigned a_math_maxu(unsigned X, unsigned Y)
{
    return X > Y ? X : Y;
}

static inline int a_math_abs(int X)
{
    return X >= 0 ? X : -X;
}

static inline int a_math_clamp(int X, int Min, int Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline unsigned a_math_clampu(unsigned X, unsigned Min, unsigned Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline unsigned a_math_wrapAngle(unsigned Angle)
{
    return Angle & (A_MATH_ANGLES_NUM - 1);
}

static inline float a_math_sin(unsigned Angle)
{
    return a_math__sin[a_math_wrapAngle(Angle)];
}

static inline float a_math_cos(unsigned Angle)
{
    return a_math__cos[a_math_wrapAngle(Angle)];
}
