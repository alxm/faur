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

#ifndef F_INC_GRAPHICS_FONT_V_H
#define F_INC_GRAPHICS_FONT_V_H

#include "f_font.p.h"

typedef enum {
    F_FONT__ID_INVALID = -1,
    F_FONT__ID_DEFAULT,
    F_FONT__ID_WHITE,
    F_FONT__ID_LIGHT_GRAY,
    F_FONT__ID_GREEN,
    F_FONT__ID_YELLOW,
    F_FONT__ID_RED,
    F_FONT__ID_BLUE,
    F_FONT__ID_NUM
} FFontId;

#include "../general/f_main.v.h"

extern const FPack f_pack__font;

extern void f_font__fontSet(FFontId Font);

#endif // F_INC_GRAPHICS_FONT_V_H
