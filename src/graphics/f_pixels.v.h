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

#ifndef F_INC_GRAPHICS_PIXELS_V_H
#define F_INC_GRAPHICS_PIXELS_V_H

#include "f_pixels.p.h"

typedef struct FPixels FPixels;

#include "../general/f_init.v.h"
#include "../graphics/f_color.v.h"
#include "../math/f_fix.v.h"

typedef enum {
    F_PIXELS__ALLOC = F_FLAGS_BIT(0),
    F_PIXELS__DYNAMIC = F_FLAGS_BIT(1),
    F_PIXELS__CONST = F_FLAGS_BIT(2),
} FPixelsFlags;

struct FPixels {
    FVecInt size;
    unsigned framesNum;
    unsigned bufferLen;
    unsigned bufferSize;
    FPixelsFlags flags;
    FColorPixel* buffer; // [w * h * framesNum]
};

extern FPixels* f_pixels__new(int W, int H, unsigned Frames, FPixelsFlags Flags);
extern void f_pixels__init(FPixels* Pixels, int W, int H, unsigned Frames, FPixelsFlags Flags);
extern void f_pixels__free(FPixels* Pixels);

extern void f_pixels__copy(FPixels* Dst, const FPixels* Src);
extern void f_pixels__copyFrame(const FPixels* Dst, unsigned DstFrame, const FPixels* Src, unsigned SrcFrame);
extern void f_pixels__copyFrameEx(const FPixels* Dst, unsigned DstFrame, const FPixels* SrcPixels, unsigned SrcFrame, int SrcX, int SrcY);

extern void f_pixels__bufferSet(FPixels* Pixels, FColorPixel* Buffer, int W, int H);

extern void f_pixels__fill(const FPixels* Pixels, unsigned Frame, FColorPixel Value);

extern FVecInt f_pixels__boundsFind(const FPixels* Pixels, unsigned Frame, int X, int Y);

static inline FColorPixel* f_pixels__bufferGetStart(const FPixels* Pixels, unsigned Frame)
{
    return Pixels->buffer + Frame * Pixels->bufferLen;
}

static inline FColorPixel* f_pixels__bufferGetFrom(const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    return f_pixels__bufferGetStart(Pixels, Frame) + Y * Pixels->size.x + X;
}

static inline FColorPixel f_pixels__bufferGetValue(const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    return *f_pixels__bufferGetFrom(Pixels, Frame, X, Y);
}

#endif // F_INC_GRAPHICS_PIXELS_V_H
