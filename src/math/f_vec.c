/*
    Copyright 2018-2020 Alex Margarit <alex@alxm.org>
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

#include "f_vec.v.h"
#include <faur.v.h>

FVecFix f_vecfix_rotateCounter(FVecFix Vec, unsigned Angle)
{
    const FFix sin = f_fix_sin(Angle);
    const FFix cos = f_fix_cos(Angle);

    return (FVecFix){f_fix_mul(Vec.x,  cos) + f_fix_mul(Vec.y, sin),
                     f_fix_mul(Vec.x, -sin) + f_fix_mul(Vec.y, cos)};
}

FVecFix f_vecfix_rotateClockwise(FVecFix Vec, unsigned Angle)
{
    const FFix sin = f_fix_sin(Angle);
    const FFix cos = f_fix_cos(Angle);

    return (FVecFix){f_fix_mul(Vec.x, cos) + f_fix_mul(Vec.y, -sin),
                     f_fix_mul(Vec.x, sin) + f_fix_mul(Vec.y,  cos)};
}
