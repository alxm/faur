/*
    Copyright 2010, 2014, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#include "a_fix.v.h"
#include <a2x.v.h>

AFix a__fix_sin[A_FIX_ANGLES_NUM];
AFix a__fix_csc[A_FIX_ANGLES_NUM];
static unsigned g_atan_angles[A_FIX_ONE];

static void initSin(void)
{
    for(unsigned a = 0; a < A_FIX_ANGLES_NUM; a++) {
        double rad = M_PI * (double)a / (A_FIX_ANGLES_NUM / 2);
        AFix sine = a_fix_fromDouble(sin(rad));

        a__fix_sin[a] = sine;

        if(sine == 0) {
            a__fix_csc[a] = A_FIX_MAX_INT * A_FIX_ONE;
        } else {
            a__fix_csc[a] = a_fix_div(A_FIX_ONE, sine);
        }
    }
}

static void initAtan(void)
{
    unsigned angle = 0;
    AFix lastRatio = 0;

    for(AFix referenceRatio = 0; referenceRatio < A_FIX_ONE; referenceRatio++) {
        AFix currentRatio = a_fix_div(a_fix_sin(angle), a_fix_cos(angle));

        while(currentRatio < referenceRatio) {
            angle++;
            lastRatio = currentRatio;
            currentRatio = a_fix_div(a_fix_sin(angle), a_fix_cos(angle));
        }

        AFix diff1 = referenceRatio - lastRatio;
        AFix diff2 = currentRatio - referenceRatio;

        if(diff2 <= diff1) {
            g_atan_angles[referenceRatio] = angle;
        } else {
            g_atan_angles[referenceRatio] = angle - 1;
        }
    }
}

static void a_fix__init(void)
{
    initSin();
    initAtan();
}

const APack a_pack__fix = {
    "Fix",
    {
        [0] = a_fix__init,
    },
    {
        NULL,
    },
};

unsigned a_fix_atan(AFix X1, AFix Y1, AFix X2, AFix Y2)
{
    const AFix dx = a_math_abs(X2 - X1);
    const AFix dy = a_math_abs(Y2 - Y1);

    if(dx == dy) {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return A_DEG_045_INT;
            } else {
                return A_DEG_315_INT;
            }
        } else {
            if(Y2 <= Y1) {
                return A_DEG_135_INT;
            } else {
                return A_DEG_225_INT;
            }
        }
    } else if(dx == 0) {
        if(Y2 <= Y1) {
            return A_DEG_090_INT;
        } else {
            return A_DEG_270_INT;
        }
    } else if(dy == 0) {
        if(X2 >= X1) {
            return 0;
        } else {
            return A_DEG_180_INT;
        }
    }

    const AFix ratio = a_fix_div(a_math_min(dx, dy), a_math_max(dx, dy));
    const unsigned cachedAngle = g_atan_angles[ratio];

    if(dy < dx) {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return cachedAngle;
            } else {
                return a_fix_angleWrap(-cachedAngle);
            }
        } else {
            if(Y2 <= Y1) {
                return A_DEG_180_INT - cachedAngle;
            } else {
                return A_DEG_180_INT + cachedAngle;
            }
        }
    } else {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return A_DEG_090_INT - cachedAngle;
            } else {
                return A_DEG_270_INT + cachedAngle;
            }
        } else {
            if(Y2 <= Y1) {
                return A_DEG_090_INT + cachedAngle;
            } else {
                return A_DEG_270_INT - cachedAngle;
            }
        }
    }
}

AVectorFix a_fix_rotateCounter(AFix X, AFix Y, unsigned Angle)
{
    const AFix sin = a_fix_sin(Angle);
    const AFix cos = a_fix_cos(Angle);

    return (AVectorFix){a_fix_mul(X,  cos) + a_fix_mul(Y, sin),
                        a_fix_mul(X, -sin) + a_fix_mul(Y, cos)};
}

AVectorFix a_fix_rotateClockwise(AFix X, AFix Y, unsigned Angle)
{
    const AFix sin = a_fix_sin(Angle);
    const AFix cos = a_fix_cos(Angle);

    return (AVectorFix){a_fix_mul(X, cos) + a_fix_mul(Y, -sin),
                        a_fix_mul(X, sin) + a_fix_mul(Y,  cos)};
}
