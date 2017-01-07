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

static inline int a_math_constrain(int X, int Min, int Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline int a_math_constrainu(unsigned X, unsigned Min, unsigned Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

#define A_MATH_PI         3.14159265359
#define A_MATH_ANGLES_NUM 256

extern double a_math_sin_val[A_MATH_ANGLES_NUM];
extern double a_math_cos_val[A_MATH_ANGLES_NUM];

static inline double a_math_sin(int Angle)
{
    return a_math_sin_val[(Angle)];
}

static inline double a_math_cos(int Angle)
{
    return a_math_cos_val[(Angle)];
}

static inline int a_math_wrapAngle(int Angle)
{
    return Angle & (A_MATH_ANGLES_NUM - 1);
}

static inline double a_math_degToRad(int Angle)
{
    return Angle * A_MATH_PI / (A_MATH_ANGLES_NUM / 2);
}

static inline int a_math_radToDeg(double Angle)
{
    return Angle * (A_MATH_ANGLES_NUM / 2) / A_MATH_PI;
}
