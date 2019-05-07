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

APixels* a_pixels__new(int W, int H, bool IsSprite, bool AllocBuffer)
{
    size_t bufferSize = (size_t)(AllocBuffer * W * H * (int)sizeof(APixel));
    APixels* p = a_mem_zalloc(sizeof(APixels) + bufferSize);

    p->w = W;
    p->h = H;
    p->buffer = p->bufferData;
    p->bufferSize = bufferSize;
    p->isSprite = IsSprite;

    return p;
}

APixels* a_pixels__dup(const APixels* Pixels)
{
    APixels* p = a_mem_dup(Pixels, sizeof(APixels) + Pixels->bufferSize);

    p->buffer = p->bufferData;
    p->texture = NULL;

    return p;
}

void a_pixels__free(APixels* Pixels)
{
    a_platform_api__textureFree(Pixels->texture);

    free(Pixels);
}

void a_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H)
{
    Pixels->w = W;
    Pixels->h = H;

    Pixels->buffer = Buffer;
    Pixels->bufferSize = (size_t)(W * H * (int)sizeof(APixel));
}

void a_pixels__commit(APixels* Pixels)
{
    // Function is responsible for freeing the old Pixels->texture
    Pixels->texture = a_platform_api__textureNew(Pixels);
}
