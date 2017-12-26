/*
    Copyright 2010, 2016, 2017 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

// Spans format for each graphic line:
// [NumSpans << 1 | 1 (draw) / 0 (transparent)][[len]...]
static void A__FUNC_NAME(keyed, noclip)(const APlatformTexture* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.width;
    APixel* startDst = a__screen.pixels + Y * screenW + X;
    const APixel* a__pass_src = Sprite->pixels;
    const unsigned* spans = Sprite->spans;

    for(int i = Sprite->height; i--; startDst += screenW) {
        bool draw = *spans & 1;
        unsigned numSpans = *spans++ >> 1;
        APixel* dst = startDst;

        while(numSpans--) {
            int len = (int)*spans++;

            if(draw) {
                while(len--) {
                    A__PIXEL_DRAW(dst);
                    dst++;
                    a__pass_src++;
                }
            } else {
                dst += len;
                a__pass_src += len;
            }

            draw = !draw;
        }
    }
}

static void A__FUNC_NAME(keyed, doclip)(const APlatformTexture* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.width;
    const int spriteW = Sprite->width;
    const int spriteH = Sprite->height;

    const int yClipUp = a_math_max(0, a__screen.clipY - Y);
    const int yClipDown = a_math_max(0, Y + spriteH - a__screen.clipY2);
    const int xClipLeft = a_math_max(0, a__screen.clipX - X);
    const int xClipRight = a_math_max(0, X + spriteW - a__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = a__screen.pixels + (Y + yClipUp) * screenW + X + xClipLeft;
    const APixel* startSrc = Sprite->pixels + yClipUp * spriteW + xClipLeft;
    const unsigned* spans = Sprite->spans;

    // skip clipped top rows
    for(int i = yClipUp; i--; ) {
        spans += 1 + (*spans >> 1);
    }

    // draw visible rows
    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        bool draw = *spans & 1;
        const unsigned* nextLine = spans + 1 + (*spans >> 1);
        APixel* dst = startDst;
        const APixel* a__pass_src = startSrc;
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
                a__pass_src += len;
                drawColumns -= len;
            } else {
                while(len-- && drawColumns--) {
                    A__PIXEL_DRAW(dst);
                    dst++;
                    a__pass_src++;
                }
            }
        }

        // draw visible columns
        while(drawColumns > 0) {
            int len = (int)*++spans;

            if(draw) {
                while(len-- && drawColumns--) {
                    A__PIXEL_DRAW(dst);
                    dst++;
                    a__pass_src++;
                }
            } else {
                dst += len;
                a__pass_src += len;
                drawColumns -= len;
            }

            draw = !draw;
        }

        // skip clipped right columns
        spans = nextLine;
    }
}

static void A__FUNC_NAME(block, noclip)(const APlatformTexture* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.width;
    APixel* startDst = a__screen.pixels + Y * screenW + X;
    const APixel* a__pass_src = Sprite->pixels;

    for(int i = Sprite->height; i--; startDst += screenW) {
        APixel* dst = startDst;

        for(int j = Sprite->width; j--; ) {
            A__PIXEL_DRAW(dst);
            dst++;
            a__pass_src++;
        }
    }
}

static void A__FUNC_NAME(block, doclip)(const APlatformTexture* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a__screen.width;
    const int spriteW = Sprite->width;
    const int spriteH = Sprite->height;

    const int yClipUp = a_math_max(0, a__screen.clipY - Y);
    const int yClipDown = a_math_max(0, Y + spriteH - a__screen.clipY2);
    const int xClipLeft = a_math_max(0, a__screen.clipX - X);
    const int xClipRight = a_math_max(0, X + spriteW - a__screen.clipX2);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = a__screen.pixels + (Y + yClipUp) * screenW + X + xClipLeft;
    const APixel* startSrc = Sprite->pixels + yClipUp * spriteW + xClipLeft;

    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        APixel* dst = startDst;
        const APixel* a__pass_src = startSrc;

        for(int j = columns; j--; ) {
            A__PIXEL_DRAW(dst);
            dst++;
            a__pass_src++;
        }
    }
}

#undef A__BLEND
#undef A__FILL
#undef A__BLEND_SETUP
#undef A__PIXEL_PARAMS
