/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_SCREEN_V_H
#define F_INC_GRAPHICS_SCREEN_V_H

#include "f_screen.p.h"

typedef struct FScreen FScreen;

#include "../general/f_init.v.h"
#include "../graphics/f_sprite.v.h"
#include "../math/f_fix.v.h"

struct FScreen {
    FListIntrNode listNode;
    FPixels* pixels;
    FSprite* sprite;
    unsigned frame;
    FVecInt clipStart, clipEnd, clipSize;
    int yOffset;
    FPlatformTextureScreen* texture;
};

extern const FPack f_pack__screen;

extern FScreen f__screen;

extern void f_screen__tick(void);
extern void f_screen__draw(void);

extern void f_screen__toSprite(FSprite* Sprite, unsigned Frame);

static inline FColorPixel* f_screen__bufferGetFrom(int X, int Y)
{
    return f_pixels__bufferGetFrom(f__screen.pixels, f__screen.frame, X, Y);
}

#endif // F_INC_GRAPHICS_SCREEN_V_H
