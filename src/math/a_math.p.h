/*
    Copyright 2010, 2018 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_MATH_MATH_P_H
#define A_INC_MATH_MATH_P_H

#include "general/a_system_includes.h"

static inline int a_math_min(int X, int Y)
{
    return X < Y ? X : Y;
}

static inline unsigned a_math_minu(unsigned X, unsigned Y)
{
    return X < Y ? X : Y;
}

static inline size_t a_math_minz(size_t X, size_t Y)
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

static inline size_t a_math_maxz(size_t X, size_t Y)
{
    return X > Y ? X : Y;
}

static inline int a_math_clamp(int X, int Min, int Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline unsigned a_math_clampu(unsigned X, unsigned Min, unsigned Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline size_t a_math_clampz(size_t X, size_t Min, size_t Max)
{
    return X < Min ? Min : (X > Max ? Max : X);
}

static inline int a_math_abs(int X)
{
    return X >= 0 ? X : -X;
}

#endif // A_INC_MATH_MATH_P_H
