/*
    Copyright 2017-2020 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_MATH_VEC_P_H
#define F_INC_MATH_VEC_P_H

#include "../general/f_system_includes.h"

#include "../math/f_fix.p.h"

typedef struct {
    FFix x, y;
} FVectorFix;

typedef struct {
    int x, y;
} FVectorInt;

extern FVectorFix f_fix_rotateCounter(FVectorFix Vec, unsigned Angle);
extern FVectorFix f_fix_rotateClockwise(FVectorFix Vec, unsigned Angle);

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

#endif // F_INC_MATH_VEC_P_H
