/*
    Copyright 2010, 2016-2018 Alex Margarit
    This file is part of a2x, a C video game framework.

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

extern ASprite* a_sprite_newFromPng(const char* Path);
extern ASprite* a_sprite_newFromSprite(const ASprite* Sheet, int X, int Y);
extern ASprite* a_sprite_newFromSpriteEx(const ASprite* Sheet, int X, int Y, int W, int H);
extern ASprite* a_sprite_newBlank(int Width, int Height, bool ColorKeyed);
extern ASprite* a_sprite_dup(const ASprite* Sprite);
extern void a_sprite_free(ASprite* Sprite);

extern void a_sprite_blit(const ASprite* Sprite, int X, int Y);
extern void a_sprite_blitEx(const ASprite* Sprite, int X, int Y, AFix Scale, unsigned Angle, int CenterX, int CenterY);

extern void a_sprite_swapColor(ASprite* Sprite, APixel OldColor, APixel NewColor);
extern void a_sprite_swapColors(ASprite* Sprite, const APixel* OldColors, const APixel* NewColors, unsigned NumColors);

extern int a_sprite_widthGet(const ASprite* Sprite);
extern int a_sprite_widthGetLog2(const ASprite* Sprite);
extern int a_sprite_widthGetOriginal(const ASprite* Sprite);
extern void a_sprite_widthSetPowerOf2(ASprite* Sprite);

extern int a_sprite_heightGet(const ASprite* Sprite);

extern const APixel* a_sprite_pixelsGetBuffer(const ASprite* Sprite);
extern APixel a_sprite_pixelsGetPixel(const ASprite* Sprite, int X, int Y);

extern APixel a_sprite_colorKeyGet(void);
