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

#include "a2x_pack_draw.p.h"

#include "a2x_pack_fix.v.h"
#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"

extern void a_draw__init(void);

extern void a_draw__setBlend(APixelBlend b);
extern void a_draw__setClip(bool c);

extern void a_draw__setAlpha(uint8_t a);
extern void a_draw__setRGB(uint8_t r, uint8_t g, uint8_t b);
