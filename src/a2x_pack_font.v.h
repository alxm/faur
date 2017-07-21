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

#include "a2x_pack_font.p.h"

typedef enum AFontDefaults {
    A_FONT_FACE_WHITE,
    A_FONT_FACE_LIGHT_GRAY,
    A_FONT_FACE_GREEN,
    A_FONT_FACE_YELLOW,
    A_FONT_FACE_RED,
    A_FONT_FACE_BLUE,
    A_FONT_FACE_DEFAULT_NUM
} AFontDefaults;

#include "a2x_pack_list.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_sprite.v.h"
#include "a2x_pack_spriteframes.v.h"

extern void a_font__init(void);
extern void a_font__uninit(void);

extern void a_font__setFace(AFontDefaults Font);
