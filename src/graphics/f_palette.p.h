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

#ifndef F_INC_GRAPHICS_PALETTE_P_H
#define F_INC_GRAPHICS_PALETTE_P_H

#include "../general/f_system_includes.h"

typedef struct FPalette FPalette;

#include "../graphics/f_sprite.p.h"

extern FPalette* f_palette_newFromImage(const char* Path);
extern FPalette* f_palette_newFromSprite(const FSprite* Sprite);
extern void f_palette_free(FPalette* Palette);

extern FColorPixel f_palette_getPixel(const FPalette* Palette, unsigned Index);
extern FColorRgb f_palette_getRgb(const FPalette* Palette, unsigned Index);

extern unsigned f_palette_sizeGet(const FPalette* Palette);

#endif // F_INC_GRAPHICS_PALETTE_P_H
