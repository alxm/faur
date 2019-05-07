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

#include "a2x_pack_pixels.p.h"

typedef struct APixels APixels;

#include "a2x_pack_color.v.h"
#include "a2x_pack_fix.v.h"

struct APixels {
    int w, h;
    APixel* buffer;
    size_t bufferSize;
    APixel bufferData[];
};

extern APixels* a_pixels__newEx(int W, int H, bool AllocBuffer);

extern APixels* a_pixels__new(int W, int H);
extern APixels* a_pixels__dup(const APixels* Pixels);
extern void a_pixels__free(APixels* Pixels);

extern AVectorInt a_pixels__sizeGet(const APixels* Pixels);
extern int a_pixels__sizeGetWidth(const APixels* Pixels);
extern int a_pixels__sizeGetHeight(const APixels* Pixels);

extern APixel* a_pixels__bufferGet(const APixels* Pixels);
extern APixel* a_pixels__bufferGetFrom(const APixels* Pixels, int X, int Y);
extern APixel a_pixels__bufferGetAt(const APixels* Pixels, int X, int Y);
extern void a_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H);

extern void a_pixels__clear(const APixels* Pixels);
extern void a_pixels__fill(const APixels* Pixels, APixel Value);

extern void a_pixels__copy(const APixels* Dst, const APixels* Src);
extern void a_pixels__copyToBuffer(const APixels* Pixels, APixel* Buffer);
