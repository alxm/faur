/*
    Copyright 2010, 2014, 2018 Alex Margarit

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

#include "a2x_pack_fix.v.h"

AFix a_fix__sin[A_FIX_ANGLES_NUM];
AFix a_fix__cos[A_FIX_ANGLES_NUM];

static unsigned g_atan_angles[A_FIX_ONE];

void a_fix__init(void)
{
    for(unsigned a = 0; a < A_FIX_ANGLES_NUM; a++) {
        double rad = M_PI * (double)a / (A_FIX_ANGLES_NUM / 2);

        a_fix__sin[a] = a_fix_fromDouble(sin(rad));
        a_fix__cos[a] = a_fix_fromDouble(cos(rad));
	}

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

unsigned a_fix_atan(AFix X1, AFix Y1, AFix X2, AFix Y2)
{
    const AFix dx = a_math_abs(X2 - X1);
    const AFix dy = a_math_abs(Y2 - Y1);

    if(dx == dy) {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return A_MATH_DEG_045;
            } else {
                return A_MATH_DEG_315;
            }
        } else {
            if(Y2 <= Y1) {
                return A_MATH_DEG_135;
            } else {
                return A_MATH_DEG_225;
            }
        }
    } else if(dx == 0) {
        if(Y2 <= Y1) {
            return A_MATH_DEG_090;
        } else {
            return A_MATH_DEG_270;
        }
    } else if(dy == 0) {
        if(X2 >= X1) {
            return 0;
        } else {
            return A_MATH_DEG_180;
        }
    }

    const AFix ratio = a_fix_div(a_math_min(dx, dy), a_math_max(dx, dy));
    const unsigned cachedAngle = g_atan_angles[ratio];

    if(dy < dx) {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return cachedAngle;
            } else {
                return a_fix_wrapAngleInt(-cachedAngle);
            }
        } else {
            if(Y2 <= Y1) {
                return A_MATH_DEG_180 - cachedAngle;
            } else {
                return A_MATH_DEG_180 + cachedAngle;
            }
        }
    } else {
        if(X2 >= X1) {
            if(Y2 <= Y1) {
                return A_MATH_DEG_090 - cachedAngle;
            } else {
                return A_MATH_DEG_270 + cachedAngle;
            }
        } else {
            if(Y2 <= Y1) {
                return A_MATH_DEG_090 + cachedAngle;
            } else {
                return A_MATH_DEG_270 - cachedAngle;
            }
        }
    }
}

void a_fix_rotateCounter(AFix X, AFix Y, unsigned Angle, AFix* NewX, AFix* NewY)
{
    const AFix sin = a_fix_sin(Angle);
    const AFix cos = a_fix_cos(Angle);

    *NewX = a_fix_mul(X,  cos) + a_fix_mul(Y, sin);
    *NewY = a_fix_mul(X, -sin) + a_fix_mul(Y, cos);
}

void a_fix_rotateClockwise(AFix X, AFix Y, unsigned Angle, AFix* NewX, AFix* NewY)
{
    const AFix sin = a_fix_sin(Angle);
    const AFix cos = a_fix_cos(Angle);

    *NewX = a_fix_mul(X, cos) + a_fix_mul(Y, -sin);
    *NewY = a_fix_mul(X, sin) + a_fix_mul(Y,  cos);
}
