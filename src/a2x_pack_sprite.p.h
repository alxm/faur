/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_pack_pixel.p.h"

extern ASprite* a_sprite_fromFile(const char* Path);
extern ASprite* a_sprite_fromData(const uint8_t* Data);
extern ASprite* a_sprite_fromPixels(const APixel* Pixels, int Width, int Height);
extern ASprite* a_sprite_fromSprite(const ASprite* Sheet, int X, int Y);
extern ASprite* a_sprite_blank(int Width, int Height, bool ColorKeyed);

extern void a_sprite_free(ASprite* Sprite);

extern void a_sprite_blit(const ASprite* Sprite, int X, int Y);
extern void a_sprite_blitCenter(const ASprite* Sprite);
extern void a_sprite_blitCenterX(const ASprite* Sprite, int Y);
extern void a_sprite_blitCenterY(const ASprite* Sprite, int X);
extern void a_sprite_fillFlat(bool FillFlatColor);

extern int a_sprite_w(const ASprite* Sprite);
extern int a_sprite_wLog2(const ASprite* Sprite);
extern int a_sprite_h(const ASprite* Sprite);
extern APixel* a_sprite_pixels(ASprite* Sprite);

extern APixel a_sprite_getPixel(const ASprite* Sprite, int X, int Y);

extern void a_sprite_refresh(ASprite* Sprite);
extern ASprite* a_sprite_clone(const ASprite* Sprite);

#define A_SPRITE_COLORKEY a_pixel_make(255, 0,   255)
#define A_SPRITE_LIMIT    a_pixel_make(0,   255, 0)
#define A_SPRITE_END      a_pixel_make(0,   255, 255)
