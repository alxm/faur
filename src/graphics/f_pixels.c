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

#include "f_pixels.v.h"
#include <faur.v.h>

APixels* f_pixels__new(int W, int H, unsigned Frames, APixelsFlags Flags)
{
    APixels* p = f_mem_malloc(sizeof(APixels));

    f_pixels__init(p, W, H, Frames, Flags | A_PIXELS__DYNAMIC);

    return p;
}

void f_pixels__init(APixels* Pixels, int W, int H, unsigned Frames, APixelsFlags Flags)
{
    Pixels->w = W;
    Pixels->h = H;
    Pixels->framesNum = Frames;
    Pixels->flags = Flags;

    if(A_FLAGS_TEST_ANY(Flags, A_PIXELS__ALLOC)) {
        Pixels->bufferLen = (unsigned)(W * H);
        Pixels->bufferSize = Pixels->bufferLen * (unsigned)sizeof(APixel);
        Pixels->buffer = f_mem_zalloc(Pixels->bufferSize * Frames);
    } else {
        Pixels->bufferLen = 0;
        Pixels->bufferSize = 0;
        Pixels->buffer = NULL;
    }
}

void f_pixels__free(APixels* Pixels)
{
    if(Pixels == NULL) {
        return;
    }

    if(A_FLAGS_TEST_ANY(Pixels->flags, A_PIXELS__ALLOC)) {
        f_mem_free(Pixels->buffer);
    }

    if(A_FLAGS_TEST_ANY(Pixels->flags, A_PIXELS__DYNAMIC)) {
        f_mem_free(Pixels);
    }
}

void f_pixels__copy(APixels* Dst, const APixels* Src)
{
    memcpy(Dst, Src, sizeof(APixels));

    if(A_FLAGS_TEST_ANY(Dst->flags, A_PIXELS__ALLOC)) {
        Dst->buffer = f_mem_dup(Dst->buffer, Dst->bufferSize * Dst->framesNum);
    }
}

void f_pixels__copyFrame(const APixels* Dst, unsigned DstFrame, const APixels* Src, unsigned SrcFrame)
{
    memcpy(f_pixels__bufferGetFrom(Dst, DstFrame, 0, 0),
           f_pixels__bufferGetFrom(Src, SrcFrame, 0, 0),
           Src->bufferSize);
}

void f_pixels__copyFrameEx(const APixels* Dst, unsigned DstFrame, const APixels* SrcPixels, unsigned SrcFrame, int SrcX, int SrcY)
{
    APixel* dst = f_pixels__bufferGetStart(Dst, DstFrame);
    const APixel* src = f_pixels__bufferGetFrom(
                            SrcPixels, SrcFrame, SrcX, SrcY);

    for(int i = Dst->h; i--; ) {
        memcpy(dst, src, (unsigned)Dst->w * sizeof(APixel));

        src += SrcPixels->w;
        dst += Dst->w;
    }
}

void f_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H)
{
    Pixels->w = W;
    Pixels->h = H;
    Pixels->buffer = Buffer;
    Pixels->bufferLen = (unsigned)(W * H);
    Pixels->bufferSize = Pixels->bufferLen * (unsigned)sizeof(APixel);
    Pixels->framesNum = 1;
}

void f_pixels__clear(const APixels* Pixels, unsigned Frame)
{
    memset(f_pixels__bufferGetFrom(Pixels, Frame, 0, 0), 0, Pixels->bufferSize);
}

void f_pixels__fill(const APixels* Pixels, unsigned Frame, APixel Value)
{
    APixel* buffer = f_pixels__bufferGetStart(Pixels, Frame);

    for(int i = Pixels->w * Pixels->h; i--; ) {
        *buffer++ = Value;
    }
}

static int findNextVerticalEdge(const APixels* Pixels, unsigned Frame, int StartX, int StartY, int* EdgeX)
{
    for(int x = StartX + *EdgeX + 1; x < Pixels->w; x++) {
        APixel p = f_pixels__bufferGetValue(Pixels, Frame, x, StartY);

        if(p == f_color__limit) {
            *EdgeX = x - StartX;

            int len = 1;
            APixel* buffer = f_pixels__bufferGetFrom(
                                Pixels, Frame, x, StartY + 1);

            for(int y = Pixels->h - (StartY + 1); y--; ) {
                if(*buffer != f_color__limit) {
                    break;
                }

                buffer += Pixels->w;
                len++;
            }

            return len;
        }
    }

    return -1;
}

static int findNextHorizontalEdge(const APixels* Pixels, unsigned Frame, int StartX, int StartY, int* EdgeY)
{
    for(int y = StartY + *EdgeY + 1; y < Pixels->h; y++) {
        APixel p = f_pixels__bufferGetValue(Pixels, Frame, StartX, y);

        if(p == f_color__limit) {
            *EdgeY = y - StartY;

            int len = 1;
            APixel* buffer = f_pixels__bufferGetFrom(
                                Pixels, Frame, StartX + 1, y);

            for(int x = Pixels->w - (StartX + 1); x--; ) {
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

AVectorInt f_pixels__boundsFind(const APixels* Pixels, unsigned Frame, int X, int Y)
{
    AVectorInt bounds;

    if(X < 0 || X >= Pixels->w || Y < 0 || Y >= Pixels->h) {
        A__FATAL("f_pixels__boundsFind(%d, %d): Invalid coords on %dx%d area",
                 X,
                 Y,
                 Pixels->w,
                 Pixels->h);
    }

    int vEdgeX = 0;
    int vEdgeLen = findNextVerticalEdge(Pixels, Frame, X, Y, &vEdgeX);
    int hEdgeY = 0;
    int hEdgeLen = findNextHorizontalEdge(Pixels, Frame, X, Y, &hEdgeY);

    while(vEdgeLen != -1 && hEdgeLen != -1) {
        if(vEdgeLen < hEdgeY) {
            vEdgeLen = findNextVerticalEdge(Pixels, Frame, X, Y, &vEdgeX);
        } else if(hEdgeLen < vEdgeX) {
            hEdgeLen = findNextHorizontalEdge(Pixels, Frame, X, Y, &hEdgeY);
        } else {
            break;
        }
    }

    if(vEdgeLen == -1 || hEdgeLen == -1) {
        bounds.x = Pixels->w - X;
        bounds.y = Pixels->h - Y;
    } else {
        bounds.x = vEdgeX;
        bounds.y = hEdgeY;
    }

    return bounds;
}
