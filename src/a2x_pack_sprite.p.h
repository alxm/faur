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

    You should have received Alpha copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_system_includes.h"

typedef struct ASprite ASprite;

#include "a2x_pack_fix.p.h"
#include "a2x_pack_pixel.p.h"

extern ASprite* a_sprite_newFromFile(const char* Path);
extern ASprite* a_sprite_newFromSprite(const ASprite* Sheet, int X, int Y);
extern ASprite* a_sprite_newBlank(int Width, int Height, bool ColorKeyed);
extern ASprite* a_sprite_dup(const ASprite* Sprite);
extern void a_sprite_free(ASprite* Sprite);

extern void a_sprite_blit(const ASprite* Sprite, int X, int Y);
extern void a_sprite_blitCenter(const ASprite* Sprite);
extern void a_sprite_blitCenterX(const ASprite* Sprite, int Y);
extern void a_sprite_blitCenterY(const ASprite* Sprite, int X);

#if A_CONFIG_RENDER_SDL2
extern void a_sprite_blitEx(const ASprite* Sprite, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY);
#endif

extern void a_sprite_pow2Width(ASprite* Sprite);
extern void a_sprite_swapColor(ASprite* Sprite, APixel OldColor, APixel NewColor);

extern int a_sprite_getWidth(const ASprite* Sprite);
extern int a_sprite_getWidthLog2(const ASprite* Sprite);
extern int a_sprite_getWidthOriginal(const ASprite* Sprite);
extern int a_sprite_getHeight(const ASprite* Sprite);

extern APixel* a_sprite_getPixels(ASprite* Sprite);
extern APixel a_sprite_getPixel(const ASprite* Sprite, int X, int Y);

extern APixel a_sprite_getColorKey(void);
