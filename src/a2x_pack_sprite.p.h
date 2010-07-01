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

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

typedef struct Sprite Sprite;

#include "a2x_pack_list.p.h"
#include "a2x_pack_screen.p.h"
#include "a2x_pack_sheet.p.h"
#include "a2x_pack_types.p.h"

#define a_sprite_make(g, x, y, w, h) a_sprite_makeZoomed((g), (x), (y), (w), (h), 1)
extern Sprite* a_sprite_makeZoomed(const Sheet* const graphic, const int x, const int y, const int w, const int h, const int zoom);
extern Sprite* a_sprite_makeBlank(const int w, const int h);
extern void a_sprite_makeTransparent(Sprite* const s, const Sheet* const graphic);
extern void a_sprite_free(Sprite* const s);
extern Sprite* a_sprite_clone(const Sprite* const src);
extern int a_sprite_w(const Sprite* const s);
extern int a_sprite_h(const Sprite* const s);
extern Pixel* a_sprite_data(const Sprite* const s);
extern fix8 a_sprite_getAlpha(const Sprite* const s);
extern void a_sprite_setAlpha(Sprite* const s, const fix8 a);
extern Pixel a_sprite_t(const Sprite* const s);
extern Pixel a_sprite_getPixel(const Sprite* const s, const int x, const int y);

#endif // A2X_PACK_SPRITE_PH
