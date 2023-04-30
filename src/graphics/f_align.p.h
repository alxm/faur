/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_ALIGN_P_H
#define F_INC_GRAPHICS_ALIGN_P_H

#include "../general/f_system_includes.h"

typedef enum {
    F_ALIGN_X_NONE,
    F_ALIGN_X_LEFT,
    F_ALIGN_X_CENTER,
    F_ALIGN_X_RIGHT,
} FAlignX;

typedef enum {
    F_ALIGN_Y_NONE,
    F_ALIGN_Y_TOP,
    F_ALIGN_Y_CENTER,
    F_ALIGN_Y_BOTTOM,
} FAlignY;

extern void f_align_push(void);
extern void f_align_pop(void);
extern void f_align_reset(void);

extern void f_align_set(FAlignX X, FAlignY Y);

#endif // F_INC_GRAPHICS_ALIGN_P_H
