/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_GRAPHICS_PIXELS_V_H
#define A_INC_GRAPHICS_PIXELS_V_H

#include "graphics/f_pixels.p.h"

typedef struct APixels APixels;

#include "general/f_main.v.h"
#include "graphics/f_color.v.h"
#include "math/f_fix.v.h"

typedef enum {
    A_PIXELS__ALLOC = A_FLAGS_BIT(0),
    A_PIXELS__DIRTY = A_FLAGS_BIT(1),
    A_PIXELS__DYNAMIC = A_FLAGS_BIT(2),
    A_PIXELS__CONST = A_FLAGS_BIT(3),
} APixelsFlags;

struct APixels {
    int w, h;
    unsigned framesNum;
    unsigned bufferLen;
    unsigned bufferSize;
    APixelsFlags flags;
    APixel* buffer; // [w * h * framesNum]
};

extern APixels* f_pixels__new(int W, int H, unsigned Frames, APixelsFlags Flags);
extern void f_pixels__init(APixels* Pixels, int W, int H, unsigned Frames, APixelsFlags Flags);
extern void f_pixels__free(APixels* Pixels);

extern void f_pixels__copy(APixels* Dst, const APixels* Src);
extern void f_pixels__copyFrame(const APixels* Dst, unsigned DstFrame, const APixels* Src, unsigned SrcFrame);
extern void f_pixels__copyFrameEx(const APixels* Dst, unsigned DstFrame, const APixels* SrcPixels, unsigned SrcFrame, int SrcX, int SrcY);

extern void f_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H);

extern void f_pixels__clear(const APixels* Pixels, unsigned Frame);
extern void f_pixels__fill(const APixels* Pixels, unsigned Frame, APixel Value);

extern AVectorInt f_pixels__boundsFind(const APixels* Pixels, unsigned Frame, int X, int Y);

static inline APixel* f_pixels__bufferGetStart(const APixels* Pixels, unsigned Frame)
{
    return Pixels->buffer + Frame * Pixels->bufferLen;
}

static inline APixel* f_pixels__bufferGetFrom(const APixels* Pixels, unsigned Frame, int X, int Y)
{
    return f_pixels__bufferGetStart(Pixels, Frame) + Y * Pixels->w + X;
}

static inline APixel f_pixels__bufferGetValue(const APixels* Pixels, unsigned Frame, int X, int Y)
{
    return *f_pixels__bufferGetFrom(Pixels, Frame, X, Y);
}

#endif // A_INC_GRAPHICS_PIXELS_V_H
