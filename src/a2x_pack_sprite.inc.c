/*
    Copyright 2010, 2016 Alex Margarit

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

void A__FUNC_NAME(a_blit__noclip)(const ASprite* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a_screen__width;
    APixel* dst = a_screen__pixels + Y * screenW + X;
    const APixel* a__pass_src = Sprite->pixels;
    const uint16_t* spans = Sprite->spans;

    for(int i = Sprite->h; i--; dst += screenW) {
        bool draw = *spans & 1;
        int numSpans = *spans++ >> 1;
        APixel* a__pass_dst = dst;

        while(numSpans--) {
            int len = *spans++;

            if(draw) {
                while(len--) {
                    A__PIXEL_DRAW(a__pass_dst);
                    a__pass_dst++;
                    a__pass_src++;
                }
            } else {
                a__pass_dst += len;
                a__pass_src += len;
            }

            draw = !draw;
        }
    }
}

void A__FUNC_NAME(a_blit__clip)(const ASprite* Sprite, int X, int Y)
{
    A__BLEND_SETUP;

    const int screenW = a_screen__width;
    const int screenH = a_screen__height;
    const int spriteW = Sprite->w;
    const int spriteH = Sprite->h;

    const int yClipUp = a_math_max(0, -Y);
    const int yClipDown = a_math_max(0, Y + spriteH - screenH);
    const int xClipLeft = a_math_max(0, -X);
    const int xClipRight = a_math_max(0, X + spriteW - screenW);

    const int rows = spriteH - yClipUp - yClipDown;
    const int columns = spriteW - xClipLeft - xClipRight;

    APixel* startDst = a_screen__pixels + (Y + yClipUp) * screenW + (X + xClipLeft);
    const APixel* startSrc = Sprite->pixels + yClipUp * spriteW + xClipLeft;
    const uint16_t* spans = Sprite->spans;

    // skip clipped top rows
    for(int i = yClipUp; i--; ) {
        spans += 1 + (*spans >> 1);
    }

    // draw visible rows
    for(int i = rows; i--; startDst += screenW, startSrc += spriteW) {
        bool draw = *spans & 1;
        const uint16_t* nextLine = spans + 1 + (*spans >> 1);
        APixel* a__pass_dst = startDst;
        const APixel* a__pass_src = startSrc;
        int clippedLen = 0;
        int drawColumns = columns;

        // skip clipped left columns
        while(clippedLen < xClipLeft) {
            clippedLen += *++spans;
            draw = !draw;
        }

        // account for overclipping
        if(clippedLen > xClipLeft) {
            int len = clippedLen - xClipLeft;

            // Inverse logic because we're drawing from the previous span
            if(draw) {
                a__pass_dst += len;
                a__pass_src += len;
                drawColumns -= len;
            } else {
                while(len-- && drawColumns--) {
                    A__PIXEL_DRAW(a__pass_dst);
                    a__pass_dst++;
                    a__pass_src++;
                }
            }
        }

        // draw visible columns
        while(drawColumns > 0) {
            int len = *++spans;

            if(draw) {
                while(len-- && drawColumns--) {
                    A__PIXEL_DRAW(a__pass_dst);
                    a__pass_dst++;
                    a__pass_src++;
                }
            } else {
                a__pass_dst += len;
                a__pass_src += len;
                drawColumns -= len;
            }

            draw = !draw;
        }

        // skip clipped right columns
        spans = nextLine;
    }
}
