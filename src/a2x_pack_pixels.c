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

#include "a2x_pack_main.v.h"
#include "a2x_pack_mem.v.h"

APixels* a_pixels_new(int W, int H)
{
    size_t bufferSize = (unsigned)W * (unsigned)H * sizeof(APixel);

    APixels* p = a_mem_zalloc(sizeof(APixels) + bufferSize);

    p->w = W;
    p->h = H;
    p->buffer = p->bufferData;
    p->bufferSize = bufferSize;

    return p;
}

APixels* a_pixels_dup(const APixels* Pixels)
{
    APixels* p = a_mem_dup(Pixels, sizeof(APixels) + Pixels->bufferSize);

    p->buffer = p->bufferData;

    return p;
}

void a_pixels_free(APixels* Pixels)
{
    free(Pixels);
}

AVectorInt a_pixels_sizeGet(const APixels* Pixels)
{
    return (AVectorInt){Pixels->w, Pixels->h};
}

int a_pixels_sizeGetWidth(const APixels* Pixels)
{
    return Pixels->w;
}

int a_pixels_sizeGetHeight(const APixels* Pixels)
{
    return Pixels->h;
}

void a_pixels_bufferSet(APixels* Pixels, APixel* Buffer, int W, int H)
{
    Pixels->w = W;
    Pixels->h = H;

    Pixels->buffer = Buffer;
    Pixels->bufferSize = (size_t)(W * H * (int)sizeof(APixel));
}

void a_pixels_clear(APixels* Pixels)
{
    memset(Pixels->buffer, 0, Pixels->bufferSize);
}

void a_pixels_fill(APixels* Pixels, APixel Value)
{
    APixel* buffer = Pixels->buffer;

    for(int i = Pixels->w * Pixels->h; i--; ) {
        *buffer++ = Value;
    }
}

void a_pixels_copyToPixels(APixels* Pixels, APixels* Destination)
{
    #if A_CONFIG_BUILD_DEBUG
        if(Pixels->w != Destination->w || Pixels->h != Destination->h
            || Pixels->bufferSize > Destination->bufferSize) {

            A__FATAL("a_pixels_copyToPixels: "
                     "Cannot copy %dx%d (%lu) to %dx%d (%lu)",
                     Pixels->w,
                     Pixels->h,
                     Pixels->bufferSize,
                     Destination->w,
                     Destination->h,
                     Destination->bufferSize);
        }
    #endif

    memcpy(Destination->buffer, Pixels->buffer, Pixels->bufferSize);
}

void a_pixels_copyToBuffer(APixels* Pixels, APixel* Buffer)
{
    memcpy(Buffer, Pixels->buffer, Pixels->bufferSize);
}
