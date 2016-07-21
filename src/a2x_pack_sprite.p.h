/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "a2x_system_includes.h"

typedef struct ASprite ASprite;

#include "a2x_pack_pixel.p.h"

extern ASprite* a_sprite_fromFile(const char* path);
extern ASprite* a_sprite_fromData(const uint8_t* data);
extern ASprite* a_sprite_fromPixels(APixel* pixels, int w, int h);

extern ASprite* a_sprite_new(const ASprite* sheet, int x, int y);
extern ASprite* a_sprite_zoomed(const ASprite* sheet, int x, int y, int zoom);
extern ASprite* a_sprite_blank(int w, int h);

extern void a_sprite_free(ASprite* s);

extern int a_sprite_w(const ASprite* s);
extern int a_sprite_wLog2(const ASprite* s);
extern int a_sprite_h(const ASprite* s);
extern APixel* a_sprite_data(ASprite* s);

extern uint8_t a_sprite_getAlpha(const ASprite* s);
extern void a_sprite_setAlpha(ASprite* s, uint8_t a);

extern APixel a_sprite_getPixel(const ASprite* s, int x, int y);

extern void a_sprite_refresh(ASprite* s);
extern ASprite* a_sprite_clone(const ASprite* src);

#define A_SPRITE_TRANSPARENT a_pixel_make(255, 0, 255)
#define A_SPRITE_LIMIT       a_pixel_make(0, 255, 0)
#define A_SPRITE_END         a_pixel_make(0, 255, 255)
