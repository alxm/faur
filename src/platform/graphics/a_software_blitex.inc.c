/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#if A__PIXEL_TRANSPARENCY
    #define A__COLORKEY keyed
#else
    #define A__COLORKEY block
#endif

static void A__FUNC_NAME_EX(const APixels* Pixels, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, AFix CenterX, AFix CenterY)
{
    A__BLEND_SETUP;

    const AVectorInt size = {Pixels->w, Pixels->h};
    const AVectorFix sizeScaled = {size.x * Scale, size.y * Scale};
    const AVectorFix sizeScaledHalf = {sizeScaled.x / 2, sizeScaled.y / 2};
    const APixel* const pixels = a_pixels__bufferGetFrom(Pixels, Frame, 0, 0);

    const AVectorInt screenSize = a_screen_sizeGet();
    APixel* const screenPixels = a_screen__bufferGetFrom(0, 0);

    /*
         Counter-clockwise rotations:

                  p0 --- p1           p1 --- p2
                  |       |           |       |
                  p3 --- p2           p0 --- p3

                    0 deg               90 deg
    */

    Angle = a_fix_angleWrap(Angle);

    const AFix sin = a_fix_sin(Angle);
    const AFix cos = a_fix_cos(Angle);

    const AFix wLeft = sizeScaledHalf.x + a_fix_mul(CenterX, sizeScaledHalf.x);
    const AFix wRight = sizeScaled.x - wLeft;
    const AFix hTop = sizeScaledHalf.y + a_fix_mul(CenterY, sizeScaledHalf.y);
    const AFix hDown = sizeScaled.y - hTop;

    const AFix xMns = -wLeft;
    const AFix xPls = wRight - 1;
    const AFix yMns = -hTop;
    const AFix yPls = hDown - 1;

    #define ROTATE_X(x, y) a_fix_toInt(a_fix_mul(x,  cos) + a_fix_mul(y, sin))
    #define ROTATE_Y(x, y) a_fix_toInt(a_fix_mul(x, -sin) + a_fix_mul(y, cos))

    const AVectorInt
        p0 = {X + ROTATE_X(xMns, yMns), Y + ROTATE_Y(xMns, yMns)},
        p1 = {X + ROTATE_X(xPls, yMns), Y + ROTATE_Y(xPls, yMns)},
        p2 = {X + ROTATE_X(xPls, yPls), Y + ROTATE_Y(xPls, yPls)},
        p3 = {X + ROTATE_X(xMns, yPls), Y + ROTATE_Y(xMns, yPls)};

    #undef ROTATE_X
    #undef ROTATE_Y

    AVectorInt screenTop, screenBottom, screenLeft, screenRight;
    AVectorFix spriteTop, spriteBottom, spriteMidleft, spriteMidright;

    const AVectorFix
        sprite0 = {0, 0},
        sprite1 = {a_fix_fromInt(size.x) - 1, 0},
        sprite2 = {a_fix_fromInt(size.x) - 1, a_fix_fromInt(size.y) - 1},
        sprite3 = {0, a_fix_fromInt(size.y) - 1};

    // Based on Angle ranges, determine the top and bottom y coords
    // of the rotated sprite and the sides to interpolate.
    if(Angle < A_DEG_090_INT) {
        screenTop = p1;
        screenBottom = p3;
        screenLeft = p0;
        screenRight = p2;

        spriteTop = sprite1;
        spriteBottom = sprite3;
        spriteMidleft = sprite0;
        spriteMidright = sprite2;
    } else if(Angle >= A_DEG_090_INT && Angle < A_DEG_180_INT) {
        screenTop = p2;
        screenBottom = p0;
        screenLeft = p1;
        screenRight = p3;

        spriteTop = sprite2;
        spriteBottom = sprite0;
        spriteMidleft = sprite1;
        spriteMidright = sprite3;
    } else if(Angle >= A_DEG_180_INT && Angle < A_DEG_270_INT) {
        screenTop = p3;
        screenBottom = p1;
        screenLeft = p2;
        screenRight = p0;

        spriteTop = sprite3;
        spriteBottom = sprite1;
        spriteMidleft = sprite2;
        spriteMidright = sprite0;
    } else { // if(Angle >= A_DEG_270_INT) {
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

    const int scrTopYClipped = a_math_max(screenTop.y, 0);
    const int scrBottomYClipped = a_math_min(screenBottom.y, screenSize.y - 1);

    for(int scrY = scrTopYClipped; scrY <= scrBottomYClipped; scrY++) {
        int screenX0 = g_scanlines[scrY].screenX[LEFT];
        int screenX1 = g_scanlines[scrY].screenX[RIGHT];

        if(screenX0 >= screenSize.x || screenX1 < 0) {
            continue;
        }

        const AVectorFix sprite0 = g_scanlines[scrY].sprite[LEFT];
        const AVectorFix sprite1 = g_scanlines[scrY].sprite[RIGHT];

        const AFix spriteDeltaX =
            sprite1.x - sprite0.x
                + (sprite1.x > sprite0.x) - (sprite1.x < sprite0.x);
        const AFix spriteDeltaY =
            sprite1.y - sprite0.y
                + (sprite1.y > sprite0.y) - (sprite1.y < sprite0.y);

        AVectorFix sprite = sprite0;

        const int screenDeltaX = screenX1 - screenX0 + 1;
        const AFix spriteXInc = spriteDeltaX / screenDeltaX;
        const AFix spriteYInc = spriteDeltaY / screenDeltaX;

        if(screenX0 < 0) {
            sprite.x += spriteXInc * -screenX0;
            sprite.y += spriteYInc * -screenX0;

            screenX0 = 0;
        }

        if(screenX1 >= screenSize.x) {
            screenX1 = screenSize.x - 1;
        }

        APixel* dst = screenPixels + scrY * screenSize.x + screenX0;

        for(int x = screenX0; x <= screenX1; x++) {
            const APixel* src = pixels
                                    + a_fix_toInt(sprite.y) * size.x
                                    + a_fix_toInt(sprite.x);

            #if A__PIXEL_TRANSPARENCY
                if(*src != a_color__key) {
                    A__PIXEL_SETUP;
                    A__PIXEL_DRAW(dst);
                }
            #else
                A_UNUSED(src);

                A__PIXEL_SETUP;
                A__PIXEL_DRAW(dst);
            #endif

            dst++;

            sprite.x += spriteXInc;
            sprite.y += spriteYInc;
        }
    }
}

#undef A__PIXEL_TRANSPARENCY
#undef A__COLORKEY
