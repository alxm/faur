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

    You should have received Alpha copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_GRAPHICS_SPRITE_P_H
#define F_INC_GRAPHICS_SPRITE_P_H

#include "../general/f_system_includes.h"

typedef struct FSprite FSprite;

#include "../graphics/f_color.p.h"
#include "../math/f_vec.p.h"

extern FSprite* f_sprite_newFromPng(const char* Path, int X, int Y, int FrameWidth, int FrameHeight);
extern FSprite* f_sprite_newFromSprite(const FSprite* Sheet, int X, int Y, int FrameWidth, int FrameHeight);
extern FSprite* f_sprite_newBlank(int Width, int Height, unsigned Frames, bool ColorKeyed);
extern FSprite* f_sprite_dup(const FSprite* Sprite);
extern void f_sprite_free(FSprite* Sprite);

extern void f_sprite_blit(const FSprite* Sprite, unsigned Frame, int X, int Y);
extern void f_sprite_blitEx(const FSprite* Sprite, unsigned Frame, int X, int Y, FFix Scale, unsigned Angle, FFix CenterX, FFix CenterY);

extern void f_sprite_swapColor(FSprite* Sprite, FColorPixel OldColor, FColorPixel NewColor);
extern void f_sprite_swapColors(FSprite* Sprite, const FColorPixel* OldColors, const FColorPixel* NewColors, unsigned NumColors);

extern FVecInt f_sprite_sizeGet(const FSprite* Sprite);
extern int f_sprite_sizeGetWidth(const FSprite* Sprite);
extern int f_sprite_sizeGetHeight(const FSprite* Sprite);

extern unsigned f_sprite_framesNumGet(const FSprite* Sprite);

extern const FColorPixel* f_sprite_pixelsGetBuffer(const FSprite* Sprite, unsigned Frame);
extern FColorPixel f_sprite_pixelsGetValue(const FSprite* Sprite, unsigned Frame, int X, int Y);

#endif // F_INC_GRAPHICS_SPRITE_P_H
