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

#ifndef F_INC_GRAPHICS_SCREEN_V_H
#define F_INC_GRAPHICS_SCREEN_V_H

#include "graphics/f_screen.p.h"

typedef struct AScreen AScreen;

#include "general/f_main.v.h"
#include "graphics/f_sprite.v.h"

struct AScreen {
    APixels* pixels;
    #if !F_CONFIG_LIB_RENDER_SOFTWARE
        APlatformTexture* texture;
    #endif
    ASprite* sprite;
    unsigned frame;
    int clipX, clipY;
    int clipX2, clipY2;
    int clipWidth, clipHeight;
};

extern const APack f_pack__screen;

extern AScreen f__screen;

extern void f_screen__tick(void);
extern void f_screen__draw(void);

extern void f_screen__toSprite(ASprite* Sprite, unsigned Frame);

static inline APixel* f_screen__bufferGetFrom(int X, int Y)
{
    return f_pixels__bufferGetFrom(f__screen.pixels, f__screen.frame, X, Y);
}

#endif // F_INC_GRAPHICS_SCREEN_V_H
