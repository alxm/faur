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

#include <faur.v.h>

#if F__COMPILE_INC
#if F__PIXEL_TRANSPARENCY
    #define F__COLORKEY Keyed
#else
    #define F__COLORKEY Block
#endif

static void F__FUNC_NAME_EX(const FPixels* Pixels, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY)
{
    F__BLEND_SETUP;

    const FVectorInt size = {Pixels->w, Pixels->h};
    const FVectorFix sizeScaled = {size.x * Scale, size.y * Scale};
    const FVectorFix sizeScaledHalf = {sizeScaled.x / 2, sizeScaled.y / 2};
    const FPixel* const pixels = f_pixels__bufferGetFrom(Pixels, Frame, 0, 0);

    const FVectorInt screenSize = f_screen_sizeGet();
    FPixel* const screenPixels = f_screen__bufferGetFrom(0, 0);

    /*
         Counter-clockwise rotations:

                  p0 --- p1           p1 --- p2
                  |       |           |       |
                  p3 --- p2           p0 --- p3

                    0 deg               90 deg
    */

    Angle = f_fix_angleWrap(Angle);

    const FFix sin = f_fix_sin(Angle);
    const FFix cos = f_fix_cos(Angle);

    const FFix wLeft = sizeScaledHalf.x + f_fix_mul(CenterX, sizeScaledHalf.x);
    const FFix wRight = sizeScaled.x - wLeft;
    const FFix hTop = sizeScaledHalf.y + f_fix_mul(CenterY, sizeScaledHalf.y);
    const FFix hDown = sizeScaled.y - hTop;

    const FFix xMns = -wLeft;
    const FFix xPls = wRight - 1;
    const FFix yMns = -hTop;
    const FFix yPls = hDown - 1;

    #define ROTATE_X(x, y) f_fix_toInt(f_fix_mul(x,  cos) + f_fix_mul(y, sin))
    #define ROTATE_Y(x, y) f_fix_toInt(f_fix_mul(x, -sin) + f_fix_mul(y, cos))

    const FVectorInt
        p0 = {X + ROTATE_X(xMns, yMns), Y + ROTATE_Y(xMns, yMns)},
        p1 = {X + ROTATE_X(xPls, yMns), Y + ROTATE_Y(xPls, yMns)},
        p2 = {X + ROTATE_X(xPls, yPls), Y + ROTATE_Y(xPls, yPls)},
        p3 = {X + ROTATE_X(xMns, yPls), Y + ROTATE_Y(xMns, yPls)};

    #undef ROTATE_X
    #undef ROTATE_Y

    FVectorInt screenTop, screenBottom, screenLeft, screenRight;
    FVectorFix spriteTop, spriteBottom, spriteMidleft, spriteMidright;

    const FVectorFix
        sprite0 = {0, 0},
        sprite1 = {f_fix_fromInt(size.x) - 1, 0},
        sprite2 = {f_fix_fromInt(size.x) - 1, f_fix_fromInt(size.y) - 1},
        sprite3 = {0, f_fix_fromInt(size.y) - 1};

    // Based on Angle ranges, determine the top and bottom y coords
    // of the rotated sprite and the sides to interpolate.
    if(Angle < F_DEG_090_INT) {
        screenTop = p1;
        screenBottom = p3;
        screenLeft = p0;
        screenRight = p2;

        spriteTop = sprite1;
        spriteBottom = sprite3;
        spriteMidleft = sprite0;
        spriteMidright = sprite2;
    } else if(Angle >= F_DEG_090_INT && Angle < F_DEG_180_INT) {
        screenTop = p2;
        screenBottom = p0;
        screenLeft = p1;
        screenRight = p3;

        spriteTop = sprite2;
        spriteBottom = sprite0;
        spriteMidleft = sprite1;
        spriteMidright = sprite3;
    } else if(Angle >= F_DEG_180_INT && Angle < F_DEG_270_INT) {
        screenTop = p3;
        screenBottom = p1;
        screenLeft = p2;
        screenRight = p0;

        spriteTop = sprite3;
        spriteBottom = sprite1;
        spriteMidleft = sprite2;
        spriteMidright = sprite0;
    } else { // if(Angle >= F_DEG_270_INT) {
        screenTop = p0;
        screenBottom = p2;
        screenLeft = p3;
        screenRight = p1;

        spriteTop = sprite0;
        spriteBottom = sprite2;
        spriteMidleft = sprite3;
        spriteMidright = sprite1;
    }

    if(screenBottom.y < 0 || screenTop.y >= screenSize.y
        || screenRight.x < 0 || screenLeft.x >= screenSize.x) {

        return;
    }

    scan_line(LEFT, screenTop, screenLeft, spriteTop, spriteMidleft);
    scan_line(LEFT, screenLeft, screenBottom, spriteMidleft, spriteBottom);
    scan_line(RIGHT, screenTop, screenRight, spriteTop, spriteMidright);
    scan_line(RIGHT, screenRight, screenBottom, spriteMidright, spriteBottom);

    const int scrTopYClipped = f_math_max(screenTop.y, 0);
    const int scrBottomYClipped = f_math_min(screenBottom.y, screenSize.y - 1);

    for(int scrY = scrTopYClipped; scrY <= scrBottomYClipped; scrY++) {
        int screenX0 = g_scanlines[scrY].screenX[LEFT];
        int screenX1 = g_scanlines[scrY].screenX[RIGHT];

        if(screenX0 >= screenSize.x || screenX1 < 0) {
            continue;
        }

        const FVectorFix sprite0 = g_scanlines[scrY].sprite[LEFT];
        const FVectorFix sprite1 = g_scanlines[scrY].sprite[RIGHT];

        const FFix spriteDeltaX =
            sprite1.x - sprite0.x
                + (sprite1.x > sprite0.x) - (sprite1.x < sprite0.x);
        const FFix spriteDeltaY =
            sprite1.y - sprite0.y
                + (sprite1.y > sprite0.y) - (sprite1.y < sprite0.y);

        FVectorFix sprite = sprite0;

        const int screenDeltaX = screenX1 - screenX0 + 1;
        const FFix spriteXInc = spriteDeltaX / screenDeltaX;
        const FFix spriteYInc = spriteDeltaY / screenDeltaX;

        if(screenX0 < 0) {
            sprite.x += spriteXInc * -screenX0;
            sprite.y += spriteYInc * -screenX0;

            screenX0 = 0;
        }

        if(screenX1 >= screenSize.x) {
            screenX1 = screenSize.x - 1;
        }

        FPixel* dst = screenPixels + scrY * screenSize.x + screenX0;

        for(int x = screenX0; x <= screenX1; x++) {
            const FPixel* src = pixels
                                    + f_fix_toInt(sprite.y) * size.x
                                    + f_fix_toInt(sprite.x);

            #if F__PIXEL_TRANSPARENCY
                if(*src != f_color__key) {
                    F__PIXEL_SETUP;
                    F__PIXEL_DRAW(dst);
                }
            #else
                F_UNUSED(src);

                F__PIXEL_SETUP;
                F__PIXEL_DRAW(dst);
            #endif

            dst++;

            sprite.x += spriteXInc;
            sprite.y += spriteYInc;
        }
    }
}

#undef F__PIXEL_TRANSPARENCY
#undef F__COLORKEY
#endif // F__COMPILE_INC
