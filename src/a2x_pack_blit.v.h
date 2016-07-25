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

#include "a2x_pack_blit.p.h"

#include "a2x_pack_pixel.v.h"
#include "a2x_pack_screen.v.h"
#include "a2x_pack_sprite.v.h"

extern void a_blit__init(void);

extern void a_blit__setBlend(APixelBlend Blend);
extern void a_blit__setClip(bool DoClip);

extern void a_blit__setAlpha(uint8_t Alpha);
extern void a_blit__setRGB(uint8_t Red, uint8_t Green, uint8_t Blue);
