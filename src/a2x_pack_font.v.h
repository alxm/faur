/*
    Copyright 2010, 2018 Alex Margarit

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

#include "a2x_pack_font.p.h"

typedef enum {
    A_FONT_ID_INVALID = -1,
    A_FONT_ID_DEFAULT,
    A_FONT_ID_WHITE,
    A_FONT_ID_LIGHT_GRAY,
    A_FONT_ID_GREEN,
    A_FONT_ID_YELLOW,
    A_FONT_ID_RED,
    A_FONT_ID_BLUE,
    A_FONT_ID_NUM
} AFontId;

extern void a_font__init(void);
extern void a_font__uninit(void);

extern void a_font__fontSet(AFontId Font);
