/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
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

#include "f_pixels.v.h"
#include <faur.v.h>

FPixels* f_pixels__new(int W, int H, unsigned Frames, unsigned Flags)
{
    if(W <= 0 || H <= 0 || Frames == 0) {
        F__FATAL("f_pixels__init(%d, %d, %u): Invalid args", W, H, Frames);
    }

    FPixels* p = f_mem_malloc(sizeof(FPixels));

    f_pixels__init(p, W, H, Frames, Flags | F_PIXELS__DYNAMIC);

    return p;
}

void f_pixels__init(FPixels* Pixels, int W, int H, unsigned Frames, unsigned Flags)
{
    if(W <= 0 || H <= 0 || Frames == 0) {
        F__FATAL("f_pixels__init(%d, %d, %u): Invalid args", W, H, Frames);
    }

    Pixels->size.x = W;
    Pixels->size.y = H;
    Pixels->framesNum = Frames;
    Pixels->flags = Flags;

    if(F_FLAGS_TEST_ANY(Flags, F_PIXELS__ALLOC)) {
        Pixels->bufferLen = (unsigned)(W * H);
        Pixels->bufferSize = Pixels->bufferLen * (unsigned)sizeof(FColorPixel);
        Pixels->buffer = f_mem_mallocz(Pixels->bufferSize * Frames);
    } else {
        Pixels->bufferLen = 0;
        Pixels->bufferSize = 0;
        Pixels->buffer = NULL;
    }
}

void f_pixels__free(FPixels* Pixels)
{
    if(Pixels == NULL) {
        return;
    }

    if(F_FLAGS_TEST_ANY(Pixels->flags, F_PIXELS__ALLOC)) {
        f_mem_free(Pixels->buffer);
    }

    if(F_FLAGS_TEST_ANY(Pixels->flags, F_PIXELS__DYNAMIC)) {
        f_mem_free(Pixels);
    }
}

void f_pixels__copy(FPixels* Dst, const FPixels* Src)
{
    memcpy(Dst, Src, sizeof(FPixels));

    if(F_FLAGS_TEST_ANY(Dst->flags, F_PIXELS__CONST)) {
        F_FLAGS_CLEAR(Dst->flags, F_PIXELS__CONST);
        F_FLAGS_SET(Dst->flags, F_PIXELS__ALLOC);
    }

    if(F_FLAGS_TEST_ANY(Dst->flags, F_PIXELS__ALLOC)) {
        Dst->buffer = f_mem_dup(Dst->buffer, Dst->bufferSize * Dst->framesNum);
    }
}

void f_pixels__copyFrame(const FPixels* Dst, unsigned DstFrame, const FPixels* Src, unsigned SrcFrame)
{
    memcpy(f_pixels__bufferGetFrom(Dst, DstFrame, 0, 0),
           f_pixels__bufferGetFrom(Src, SrcFrame, 0, 0),
           Src->bufferSize);
}

void f_pixels__copyFrameEx(const FPixels* Dst, unsigned DstFrame, const FPixels* SrcPixels, int SrcX, int SrcY)
{
    FColorPixel* dst = f_pixels__bufferGetStart(Dst, DstFrame);
    const FColorPixel* src = f_pixels__bufferGetFrom(SrcPixels, 0, SrcX, SrcY);

    for(int i = Dst->size.y; i--; ) {
        memcpy(dst, src, (unsigned)Dst->size.x * sizeof(FColorPixel));

        src += SrcPixels->size.x;
        dst += Dst->size.x;
    }
}

void f_pixels__bufferSet(FPixels* Pixels, FColorPixel* Buffer, int W, int H)
{
    Pixels->size.x = W;
    Pixels->size.y = H;
    Pixels->buffer = Buffer;
    Pixels->bufferLen = (unsigned)(W * H);
    Pixels->bufferSize = Pixels->bufferLen * (unsigned)sizeof(FColorPixel);
    Pixels->framesNum = 1;
}

void f_pixels__fill(const FPixels* Pixels, unsigned Frame, FColorPixel Value)
{
    FColorPixel* buffer = f_pixels__bufferGetStart(Pixels, Frame);

    for(unsigned i = Pixels->bufferLen; i--; ) {
        *buffer++ = Value;
    }
}

static int findNextVerticalEdge(const FPixels* Pixels, int StartX, int StartY, int* EdgeX)
{
    for(int x = StartX + *EdgeX + 1; x < Pixels->size.x; x++) {
        FColorPixel p = f_pixels__bufferGetValue(Pixels, 0, x, StartY);

        if(p == f_color__limit) {
            *EdgeX = x - StartX;

            int len = 1;
            FColorPixel* buffer = f_pixels__bufferGetFrom(
                                    Pixels, 0, x, StartY + 1);

            for(int y = Pixels->size.y - (StartY + 1); y--; ) {
                if(*buffer != f_color__limit) {
                    break;
                }

                buffer += Pixels->size.x;
                len++;
            }

            return len;
        }
    }

    return -1;
}

static int findNextHorizontalEdge(const FPixels* Pixels, int StartX, int StartY, int* EdgeY)
{
    for(int y = StartY + *EdgeY + 1; y < Pixels->size.y; y++) {
        FColorPixel p = f_pixels__bufferGetValue(Pixels, 0, StartX, y);

        if(p == f_color__limit) {
            *EdgeY = y - StartY;

            int len = 1;
            FColorPixel* buffer = f_pixels__bufferGetFrom(
                                    Pixels, 0, StartX + 1, y);

            for(int x = Pixels->size.x - (StartX + 1); x--; ) {
                if(*buffer != f_color__limit) {
                    break;
                }

                buffer++;
                len++;
            }

            return len;
        }
    }

    return -1;
}

FVecInt f_pixels__boundsFind(const FPixels* Pixels, int X, int Y)
{
    if(X < 0 || X >= Pixels->size.x || Y < 0 || Y >= Pixels->size.y) {
        F__FATAL("f_pixels__boundsFind(%d, %d): Invalid coords on %dx%d area",
                 X,
                 Y,
                 Pixels->size.x,
                 Pixels->size.y);
    }

    FVecInt bounds;

    int vEdgeX = 0;
    int vEdgeLen = findNextVerticalEdge(Pixels, X, Y, &vEdgeX);
    int hEdgeY = 0;
    int hEdgeLen = findNextHorizontalEdge(Pixels, X, Y, &hEdgeY);

    while(vEdgeLen != -1 && hEdgeLen != -1) {
        if(vEdgeLen < hEdgeY) {
            vEdgeLen = findNextVerticalEdge(Pixels, X, Y, &vEdgeX);
        } else if(hEdgeLen < vEdgeX) {
            hEdgeLen = findNextHorizontalEdge(Pixels, X, Y, &hEdgeY);
        } else {
            break;
        }
    }

    if(vEdgeLen == -1 || hEdgeLen == -1) {
        bounds.x = Pixels->size.x - X;
        bounds.y = Pixels->size.y - Y;
    } else {
        bounds.x = vEdgeX;
        bounds.y = hEdgeY;
    }

    return bounds;
}
