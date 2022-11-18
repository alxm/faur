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

#include "../../general/f_system_includes.h"

#ifdef F__BLEND
#if F__PIXEL_TRANSPARENCY
    #define F__COLORKEY Keyed
#else
    #define F__COLORKEY Block
#endif

static void F__FUNC_NAME_EX(const FPixels* Pixels, unsigned Frame, int TopY, int BottomY)
{
    F__BLEND_SETUP;

    const FVecInt screenSize = f_screen_sizeGet();
    FColorPixel* const screenPixels = f_screen__bufferGetFrom(0, 0);
    const FColorPixel* const pixels =
        f_pixels__bufferGetFromConst(Pixels, Frame, 0, 0);
    const FVecInt size = Pixels->size;

    for(int scrY = TopY; scrY <= BottomY; scrY++) {
        int screenX0 = g_edges[0].screen[scrY];
        int screenX1 = g_edges[1].screen[scrY];

        if(screenX0 >= f__screen.clipEnd.x
            || screenX1 < f__screen.clipStart.x) {

            continue;
        }

        const FVecFix sprite0 = g_edges[0].sprite[scrY];
        const FVecFix sprite1 = g_edges[1].sprite[scrY];

        const FFix spriteDeltaX =
            sprite1.x - sprite0.x
                + (sprite1.x > sprite0.x) - (sprite1.x < sprite0.x);
        const FFix spriteDeltaY =
            sprite1.y - sprite0.y
                + (sprite1.y > sprite0.y) - (sprite1.y < sprite0.y);

        FVecFix sprite = sprite0;

        const int screenDeltaX = screenX1 - screenX0 + 1;
        const FFix spriteXInc = spriteDeltaX / screenDeltaX;
        const FFix spriteYInc = spriteDeltaY / screenDeltaX;

        if(screenX0 < f__screen.clipStart.x) {
            sprite.x += spriteXInc * (f__screen.clipStart.x - screenX0);
            sprite.y += spriteYInc * (f__screen.clipStart.x - screenX0);

            screenX0 = f__screen.clipStart.x;
        }

        if(screenX1 >= f__screen.clipEnd.x) {
            screenX1 = f__screen.clipEnd.x - 1;
        }

        FColorPixel* dst = screenPixels + scrY * screenSize.x + screenX0;

        for(int x = screenX0; x <= screenX1; x++) {
            const FColorPixel* src =
                pixels + f_fix_toInt(sprite.y) * size.x + f_fix_toInt(sprite.x);

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
#endif // F__BLEND
