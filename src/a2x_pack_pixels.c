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

#include "a2x_pack_pixels.v.h"

#include "a2x_pack_mem.v.h"

APixels* a_pixels__new(int W, int H)
{
    return a_pixels__newEx(W, H, true);
}

APixels* a_pixels__newEx(int W, int H, bool AllocBuffer)
{
    size_t bufferSize = AllocBuffer
                            ? (unsigned)W * (unsigned)H * sizeof(APixel) : 0;

    APixels* p = a_mem_zalloc(sizeof(APixels) + bufferSize);

    p->w = W;
    p->h = H;
    p->buffer = p->bufferData;
    p->bufferSize = bufferSize;

    return p;
}

APixels* a_pixels__dup(const APixels* Pixels)
{
    APixels* p = a_mem_dup(Pixels, sizeof(APixels) + Pixels->bufferSize);

    p->buffer = p->bufferData;

    return p;
}

void a_pixels__free(APixels* Pixels)
{
    free(Pixels);
}

void a_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H)
{
    Pixels->w = W;
    Pixels->h = H;

    Pixels->buffer = Buffer;
    Pixels->bufferSize = (size_t)(W * H * (int)sizeof(APixel));
}
