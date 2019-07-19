/*
    Copyright 2010, 2018 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_GRAPHICS_FONT_V_H
#define A_INC_GRAPHICS_FONT_V_H

#include "graphics/a_font.p.h"

typedef enum {
    A_FONT__ID_INVALID = -1,
    A_FONT__ID_DEFAULT,
    A_FONT__ID_WHITE,
    A_FONT__ID_LIGHT_GRAY,
    A_FONT__ID_GREEN,
    A_FONT__ID_YELLOW,
    A_FONT__ID_RED,
    A_FONT__ID_BLUE,
    A_FONT__ID_NUM
} AFontId;

#include "general/a_main.v.h"

extern const APack a_pack__font;

extern void a_font__fontSet(AFontId Font);

#endif // A_INC_GRAPHICS_FONT_V_H
