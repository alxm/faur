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

    You should have received Alpha copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_GRAPHICS_SPRITE_P_H
#define A_INC_GRAPHICS_SPRITE_P_H

#include "general/f_system_includes.h"

typedef struct ASprite ASprite;

#include "graphics/f_color.p.h"
#include "math/f_fix.p.h"

extern ASprite* f_sprite_newFromPng(const char* Path, int X, int Y, int FrameWidth, int FrameHeight);
extern ASprite* f_sprite_newFromSprite(const ASprite* Sheet, int X, int Y, int FrameWidth, int FrameHeight);
extern ASprite* f_sprite_newBlank(int Width, int Height, unsigned Frames, bool ColorKeyed);
extern ASprite* f_sprite_dup(const ASprite* Sprite);
extern void f_sprite_free(ASprite* Sprite);

extern void f_sprite_blit(const ASprite* Sprite, unsigned Frame, int X, int Y);
extern void f_sprite_blitEx(const ASprite* Sprite, unsigned Frame, int X, int Y, AFix Scale, unsigned Angle, AFix CenterX, AFix CenterY);

extern void f_sprite_swapColor(ASprite* Sprite, APixel OldColor, APixel NewColor);
extern void f_sprite_swapColors(ASprite* Sprite, const APixel* OldColors, const APixel* NewColors, unsigned NumColors);

extern AVectorInt f_sprite_sizeGet(const ASprite* Sprite);
extern int f_sprite_sizeGetWidth(const ASprite* Sprite);
extern int f_sprite_sizeGetHeight(const ASprite* Sprite);

extern unsigned f_sprite_framesNumGet(const ASprite* Sprite);

extern const APixel* f_sprite_pixelsGetBuffer(const ASprite* Sprite, unsigned Frame);
extern APixel f_sprite_pixelsGetValue(const ASprite* Sprite, unsigned Frame, int X, int Y);

#endif // A_INC_GRAPHICS_SPRITE_P_H
