/*
    Copyright 2010, 2016-2020 Alex Margarit <alex@alxm.org>
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

#include "../../general/f_system_includes.h"

#ifdef F__BLEND
// Spans format for each graphic line:
// [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]
static void F__FUNC_NAME(Keyed, NoClip)(const FTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->size.x;
    FColorPixel* startDst = f_screen__bufferGetFrom(X, Y);
    const FColorPixel* src = f_pixels__bufferGetStart(Pixels, Frame);
    const FSpriteWord* spans = Texture->spans[Frame];

    for(int i = Pixels->size.y; i--; startDst += screenW) {
        bool draw = *spans & 1;
        FSpriteWord numSpans = *spans++ >> 1;
        FColorPixel* dst = startDst;

        while(numSpans--) {
            int len = (int)*spans++;

            if(draw) {
                while(len--) {
                    F__PIXEL_SETUP;
                    F__PIXEL_DRAW(dst);
                    dst++;
                    src++;
                }
            } else {
                dst += len;
                src += len;
            }

            draw = !draw;
        }
    }
}

static void F__FUNC_NAME(Keyed, DoClip)(const FTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->size.x;
    const int spriteW = Pixels->size.x;
    const int spriteH = Pixels->size.y;

    const int yClipUp = f_math_max(0, f__screen.clipStart.y - Y);
    const int yClipDown = f_math_max(0, Y + spriteH - f__screen.clipEnd.y);
    const int xClipLeft = f_math_max(0, f__screen.clipStart.x - X);
    const int xClipRight = f_math_max(0, X + spriteW - f__screen.clipEnd.x);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    FColorPixel* startDst = f_screen__bufferGetFrom(X + xClipLeft, Y + yClipUp);
    const FColorPixel* startSrc = f_pixels__bufferGetFrom(
                                    Pixels, Frame, xClipLeft, yClipUp);

    const FSpriteWord* spans = Texture->spans[Frame];

    // skip clipped top rows
    for(int i = yClipUp; i--; ) {
        spans += 1 + (*spans >> 1);
    }

    // draw visible rows
    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        bool draw = *spans & 1;
        const FSpriteWord* nextLine = spans + 1 + (*spans >> 1);
        FColorPixel* dst = startDst;
        const FColorPixel* src = startSrc;
        int clippedLen = 0;
        int drawColumns = columns;

        // skip clipped left columns
        while(clippedLen < xClipLeft) {
            clippedLen += (int)*++spans;
            draw = !draw;
        }

        // account for overclipping
        if(clippedLen > xClipLeft) {
            int len = clippedLen - xClipLeft;

            // Inverse logic because we're drawing from the previous span
            if(draw) {
                dst += len;
                src += len;
                drawColumns -= len;
            } else {
                while(len-- && drawColumns--) {
                    F__PIXEL_SETUP;
                    F__PIXEL_DRAW(dst);
                    dst++;
                    src++;
                }
            }
        }

        // draw visible columns
        while(drawColumns > 0) {
            int len = (int)*++spans;

            if(draw) {
                while(len-- && drawColumns--) {
                    F__PIXEL_SETUP;
                    F__PIXEL_DRAW(dst);
                    dst++;
                    src++;
                }
            } else {
                dst += len;
                src += len;
                drawColumns -= len;
            }

            draw = !draw;
        }

        // skip clipped right columns
        spans = nextLine;
    }
}

static void F__FUNC_NAME(Block, NoClip)(const FTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    F_UNUSED(Texture);

    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->size.x;
    FColorPixel* startDst = f_screen__bufferGetFrom(X, Y);
    const FColorPixel* src = f_pixels__bufferGetStart(Pixels, Frame);

    for(int i = Pixels->size.y; i--; startDst += screenW) {
        FColorPixel* dst = startDst;

        for(int j = Pixels->size.x; j--; ) {
            F__PIXEL_SETUP;
            F__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

static void F__FUNC_NAME(Block, DoClip)(const FTexture* Texture, const FPixels* Pixels, unsigned Frame, int X, int Y)
{
    F_UNUSED(Texture);

    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->size.x;
    const int spriteW = Pixels->size.x;
    const int spriteH = Pixels->size.y;

    const int yClipUp = f_math_max(0, f__screen.clipStart.y - Y);
    const int yClipDown = f_math_max(0, Y + spriteH - f__screen.clipEnd.y);
    const int xClipLeft = f_math_max(0, f__screen.clipStart.x - X);
    const int xClipRight = f_math_max(0, X + spriteW - f__screen.clipEnd.x);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    FColorPixel* startDst = f_screen__bufferGetFrom(X + xClipLeft, Y + yClipUp);
    const FColorPixel* startSrc = f_pixels__bufferGetFrom(
                                    Pixels, Frame, xClipLeft, yClipUp);

    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        FColorPixel* dst = startDst;
        const FColorPixel* src = startSrc;

        for(int j = columns; j--; ) {
            F__PIXEL_SETUP;
            F__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

#define F__PIXEL_TRANSPARENCY 0
#include "f_software_blitex.inc.c"

#define F__PIXEL_TRANSPARENCY 1
#include "f_software_blitex.inc.c"

#undef F__BLEND
#undef F__FILL
#undef F__BLEND_SETUP
#undef F__PIXEL_SETUP
#undef F__PIXEL_PARAMS
#endif // F__BLEND
