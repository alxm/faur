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
#include "a2x_pack_main.v.h"
#include "a2x_pack_platform.v.h"

struct APixels {
    int w, h;
    APixel* buffer;
    size_t bufferSize;
    APlatformTexture* texture;
    APixel bufferData[];
};

extern APixels* a_pixels__newEx(int W, int H, bool AllocBuffer);

extern APixels* a_pixels__new(int W, int H);
extern APixels* a_pixels__dup(const APixels* Pixels);
extern void a_pixels__free(APixels* Pixels);

extern void a_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H);
extern void a_pixels__commit(APixels* Pixels);

static inline APixel* a_pixels__bufferGetFrom(const APixels* Pixels, int X, int Y)
{
    return Pixels->buffer + Y * Pixels->w + X;
}

static inline APixel a_pixels__bufferGetAt(const APixels* Pixels, int X, int Y)
{
    return *(Pixels->buffer + Y * Pixels->w + X);
}

static inline void a_pixels__clear(const APixels* Pixels)
{
    memset(Pixels->buffer, 0, Pixels->bufferSize);
}

static inline void a_pixels__fill(const APixels* Pixels, APixel Value)
{
    APixel* buffer = Pixels->buffer;

    for(int i = Pixels->w * Pixels->h; i--; ) {
        *buffer++ = Value;
    }
}

static inline void a_pixels__copy(const APixels* Dst, const APixels* Src)
{
    #if A_CONFIG_BUILD_DEBUG
        if(Src->w != Dst->w || Src->h != Dst->h
            || Src->bufferSize > Dst->bufferSize) {

            A__FATAL("a_pixels__copy(%dx%d, %dx%d): Different sizes",
                     Dst->w,
                     Dst->h,
                     Src->w,
                     Src->h);
        }
    #endif

    memcpy(Dst->buffer, Src->buffer, Src->bufferSize);
}

static inline void a_pixels__copyToBuffer(const APixels* Pixels, APixel* Buffer)
{
    memcpy(Buffer, Pixels->buffer, Pixels->bufferSize);
}
