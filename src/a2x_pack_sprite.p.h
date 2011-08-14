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

#ifndef A2X_PACK_SPRITE_PH
#define A2X_PACK_SPRITE_PH

#include "a2x_app_includes.h"

typedef struct Sprite Sprite;

#include "a2x_pack_pixel.p.h"
#include "a2x_pack_sheet.p.h"

#define a_sprite_new(g, x, y, w, h) a_sprite_newZoomed((g), (x), (y), (w), (h), 1)
extern Sprite* a_sprite_newZoomed(const Sheet* const graphic, const int x, const int y, const int w, const int h, const int zoom);
extern Sprite* a_sprite_newBlank(const int w, const int h);
extern void a_sprite_newTransparent(Sprite* const s);
extern void a_sprite_free(Sprite* const s);

extern Sprite* a_sprite_clone(const Sprite* const src);

extern int a_sprite_w(const Sprite* const s);
extern int a_sprite_h(const Sprite* const s);
extern Pixel* a_sprite_data(const Sprite* const s);

extern uint8_t a_sprite_getAlpha(const Sprite* const s);
extern void a_sprite_setAlpha(Sprite* const s, const uint8_t a);

extern Pixel a_sprite_getTransparent(const Sprite* const s);
extern void a_sprite_setTransparent(Sprite* const s, const Pixel c);

extern Pixel a_sprite_getPixel(const Sprite* const s, const int x, const int y);

#endif // A2X_PACK_SPRITE_PH
