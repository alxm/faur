/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#pragma once

#include "a2x_system_includes.h"

typedef struct APixels APixels;

#include "a2x_pack_color.p.h"
#include "a2x_pack_fix.p.h"

extern APixels* a_pixels_new(int W, int H);
extern APixels* a_pixels_dup(const APixels* Pixels);
extern void a_pixels_free(APixels* Pixels);

extern AVectorInt a_pixels_sizeGet(const APixels* Pixels);
extern int a_pixels_sizeGetWidth(const APixels* Pixels);
extern int a_pixels_sizeGetHeight(const APixels* Pixels);

extern void a_pixels_fill(APixels* Pixels, APixel Value);
