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

#include <a2x.v.h>

APixels* a_pixels__new(int W, int H, APixelsFlags Flags)
{
    APixels* p = a_mem_zalloc(sizeof(APixels));

    p->w = W;
    p->h = H;
    p->flags = Flags;

    if(A_FLAG_TEST_ANY(Flags, A_PIXELS__ALLOC)) {
        p->bufferSize = (unsigned)(W * H * (int)sizeof(APixel));
        p->buffer = a_mem_malloc(p->bufferSize);
    }

    return p;
}

APixels* a_pixels__sub(APixels* Source, int X, int Y, int Width, int Height)
{
    APixels* p = a_pixels__new(Width, Height, A_PIXELS__ALLOC);

    const APixel* src = a_pixels__bufferGetFrom(Source, X, Y);
    APixel* dst = p->buffer;

    for(int i = Height; i--; ) {
        memcpy(dst, src, (unsigned)Width * sizeof(APixel));

        src += Source->w;
        dst += Width;
    }

    return p;
}

APixels* a_pixels__dup(const APixels* Pixels)
{
    APixels* p = a_pixels__new(Pixels->w, Pixels->h, Pixels->flags);

    if(A_FLAG_TEST_ANY(Pixels->flags, A_PIXELS__ALLOC)) {
        memcpy(p->buffer, Pixels->buffer, p->bufferSize);
    }

    return p;
}

void a_pixels__free(APixels* Pixels)
{
    if(Pixels == NULL) {
        return;
    }

    a_platform_api__textureFree(Pixels->texture);

    if(A_FLAG_TEST_ANY(Pixels->flags, A_PIXELS__ALLOC)) {
        free(Pixels->buffer);
    }

    free(Pixels);
}

void a_pixels__bufferSet(APixels* Pixels, APixel* Buffer, int W, int H)
{
    #if A_CONFIG_BUILD_DEBUG
        if(A_FLAG_TEST_ANY(Pixels->flags, A_PIXELS__ALLOC)) {
            A__FATAL("a_pixels__bufferSet: Already allocated buffer");
        }
    #endif

    Pixels->w = W;
    Pixels->h = H;

    Pixels->buffer = Buffer;
    Pixels->bufferSize = (unsigned)(W * H * (int)sizeof(APixel));
}

void a_pixels__commit(APixels* Pixels)
{
    // Function is responsible for freeing the old Pixels->texture
    Pixels->texture = a_platform_api__textureNew(Pixels);
}

static int findNextVerticalEdge(const APixels* Pixels, int StartX, int StartY, int* EdgeX)
{
    for(int x = StartX + *EdgeX + 1; x < Pixels->w; x++) {
        APixel p = a_pixels__bufferGetAt(Pixels, x, StartY);

        if(p == a_color__limit) {
            *EdgeX = x - StartX;

            int len = 1;
            APixel* buffer = a_pixels__bufferGetFrom(Pixels, x, StartY + 1);

            for(int y = Pixels->h - (StartY + 1); y--; ) {
                if(*buffer != a_color__limit) {
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

static int findNextHorizontalEdge(const APixels* Pixels, int StartX, int StartY, int* EdgeY)
{
    for(int y = StartY + *EdgeY + 1; y < Pixels->h; y++) {
        APixel p = a_pixels__bufferGetAt(Pixels, StartX, y);

        if(p == a_color__limit) {
            *EdgeY = y - StartY;

            int len = 1;
            APixel* buffer = a_pixels__bufferGetFrom(Pixels, StartX + 1, y);

            for(int x = Pixels->w - (StartX + 1); x--; ) {
                if(*buffer != a_color__limit) {
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

AVectorInt a_pixels__boundsFind(const APixels* Pixels, int X, int Y)
{
    AVectorInt bounds;

    if(X < 0 || X >= Pixels->w || Y < 0 || Y >= Pixels->h) {
        A__FATAL("a_pixels__boundsFind(%d, %d): Invalid coords on %dx%d area",
                 X,
                 Y,
                 Pixels->w,
                 Pixels->h);
    }

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
        bounds.x = Pixels->w - X;
        bounds.y = Pixels->h - Y;
    } else {
        bounds.x = vEdgeX;
        bounds.y = hEdgeY;
    }

    return bounds;
}
