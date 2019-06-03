/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

// Spans format for each graphic line:
// [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]
static void A__FUNC_NAME(keyed, noclip)(const APlatformTexture* Texture, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.pixels->w;
    APixel* startDst = a_pixels__bufferGetFrom(a__screen.pixels, X, Y);
    const APixel* src = Texture->pixels->buffer;
    const unsigned* spans = Texture->spans;

    for(int i = Texture->pixels->h; i--; startDst += screenW) {
        bool draw = *spans & 1;
        unsigned numSpans = *spans++ >> 1;
        APixel* dst = startDst;

        while(numSpans--) {
            int len = (int)*spans++;

            if(draw) {
                while(len--) {
                    A__PIXEL_SETUP;
                    A__PIXEL_DRAW(dst);
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

static void A__FUNC_NAME(keyed, doclip)(const APlatformTexture* Texture, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.pixels->w;
    const int spriteW = Texture->pixels->w;
    const int spriteH = Texture->pixels->h;

    const int yClipUp = a_math_max(0, a__screen.clipY - Y);
    const int yClipDown = a_math_max(0, Y + spriteH - a__screen.clipY2);
    const int xClipLeft = a_math_max(0, a__screen.clipX - X);
    const int xClipRight = a_math_max(0, X + spriteW - a__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = a_pixels__bufferGetFrom(
                        a__screen.pixels, X + xClipLeft, Y + yClipUp);
    const APixel* startSrc = a_pixels__bufferGetFrom(
                                Texture->pixels, xClipLeft, yClipUp);

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
                    A__PIXEL_SETUP;
                    A__PIXEL_DRAW(dst);
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
                    A__PIXEL_SETUP;
                    A__PIXEL_DRAW(dst);
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

static void A__FUNC_NAME(block, noclip)(const APlatformTexture* Texture, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.pixels->w;
    APixel* startDst = a_pixels__bufferGetFrom(a__screen.pixels, X, Y);
    const APixel* src = Texture->pixels->buffer;

    for(int i = Texture->pixels->h; i--; startDst += screenW) {
        APixel* dst = startDst;

        for(int j = Texture->pixels->w; j--; ) {
            A__PIXEL_SETUP;
            A__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

static void A__FUNC_NAME(block, doclip)(const APlatformTexture* Texture, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.pixels->w;
    const int spriteW = Texture->pixels->w;
    const int spriteH = Texture->pixels->h;

    const int yClipUp = a_math_max(0, a__screen.clipY - Y);
    const int yClipDown = a_math_max(0, Y + spriteH - a__screen.clipY2);
    const int xClipLeft = a_math_max(0, a__screen.clipX - X);
    const int xClipRight = a_math_max(0, X + spriteW - a__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = a_pixels__bufferGetFrom(
                        a__screen.pixels, X + xClipLeft, Y + yClipUp);
    const APixel* startSrc = a_pixels__bufferGetFrom(
                                Texture->pixels, xClipLeft, yClipUp);

    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        APixel* dst = startDst;
        const APixel* src = startSrc;

        for(int j = columns; j--; ) {
            A__PIXEL_SETUP;
            A__PIXEL_DRAW(dst);
            dst++;
            src++;
        }
    }
}

#define A__PIXEL_TRANSPARENCY 0
#include "a2x_pack_platform_software_blitex.inc.c"

#define A__PIXEL_TRANSPARENCY 1
#include "a2x_pack_platform_software_blitex.inc.c"

#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_SETUP
#undef A__PIXEL_PARAMS
