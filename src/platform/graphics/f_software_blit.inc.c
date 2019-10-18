/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#include <faur.v.h>

#if F__COMPILE_INC
// Spans format for each graphic line:
// [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]
static void F__FUNC_NAME(Keyed, NoClip)(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->w;
    APixel* startDst = f_screen__bufferGetFrom(X, Y);
    const APixel* src = f_pixels__bufferGetStart(Pixels, Frame);
    const unsigned* spans = Texture->spans;

    for(int i = Pixels->h; i--; startDst += screenW) {
        bool draw = *spans & 1;
        unsigned numSpans = *spans++ >> 1;
        APixel* dst = startDst;

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

static void F__FUNC_NAME(Keyed, DoClip)(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->w;
    const int spriteW = Pixels->w;
    const int spriteH = Pixels->h;

    const int yClipUp = f_math_max(0, f__screen.clipY - Y);
    const int yClipDown = f_math_max(0, Y + spriteH - f__screen.clipY2);
    const int xClipLeft = f_math_max(0, f__screen.clipX - X);
    const int xClipRight = f_math_max(0, X + spriteW - f__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = f_screen__bufferGetFrom(X + xClipLeft, Y + yClipUp);
    const APixel* startSrc = f_pixels__bufferGetFrom(
                                Pixels, Frame, xClipLeft, yClipUp);

    const unsigned* spans = Texture->spans;

    // skip clipped top rows
    for(int i = yClipUp; i--; ) {
        spans += 1 + (*spans >> 1);
    }

    // draw visible rows
    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        bool draw = *spans & 1;
        const unsigned* nextLine = spans + 1 + (*spans >> 1);
        APixel* dst = startDst;
        const APixel* src = startSrc;
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

static void F__FUNC_NAME(Block, NoClip)(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    F_UNUSED(Texture);

    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->w;
    APixel* startDst = f_screen__bufferGetFrom(X, Y);
    const APixel* src = f_pixels__bufferGetStart(Pixels, Frame);

    for(int i = Pixels->h; i--; startDst += screenW) {
        APixel* dst = startDst;

        for(int j = Pixels->w; j--; ) {
            F__PIXEL_SETUP;
            F__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

static void F__FUNC_NAME(Block, DoClip)(const APlatformTexture* Texture, const APixels* Pixels, unsigned Frame, int X, int Y)
{
    F_UNUSED(Texture);

    F__BLEND_SETUP;

    const int screenW = f__screen.pixels->w;
    const int spriteW = Pixels->w;
    const int spriteH = Pixels->h;

    const int yClipUp = f_math_max(0, f__screen.clipY - Y);
    const int yClipDown = f_math_max(0, Y + spriteH - f__screen.clipY2);
    const int xClipLeft = f_math_max(0, f__screen.clipX - X);
    const int xClipRight = f_math_max(0, X + spriteW - f__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = f_screen__bufferGetFrom(X + xClipLeft, Y + yClipUp);
    const APixel* startSrc = f_pixels__bufferGetFrom(
                                Pixels, Frame, xClipLeft, yClipUp);

    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        APixel* dst = startDst;
        const APixel* src = startSrc;

        for(int j = columns; j--; ) {
            F__PIXEL_SETUP;
            F__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

#define F__PIXEL_TRANSPARENCY 0
#include "platform/graphics/f_software_blitex.inc.c"

#define F__PIXEL_TRANSPARENCY 1
#include "platform/graphics/f_software_blitex.inc.c"

#undef F__BLEND
#undef F__FILL
#undef F__BLEND_SETUP
#undef F__PIXEL_SETUP
#undef F__PIXEL_PARAMS
#endif // F__COMPILE_INC
