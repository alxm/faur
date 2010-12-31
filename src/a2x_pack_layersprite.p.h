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

#ifndef A2X_PACK_LAYERSPRITE_PH
#define A2X_PACK_LAYERSPRITE_PH

typedef struct LayerSprite LayerSprite;

#include "a2x_pack_blit.p.h"
#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern LayerSprite* a_layersprite_make(void);
extern void a_layersprite_free(LayerSprite* const ls);

extern void a_layersprite_add(LayerSprite* const ls, Sprite* const sprite, const PixelBlend_t blend, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_layersprite_blit(LayerSprite* const ls, const int x, const int y);

#endif // A2X_PACK_LAYERSPRITE_PH
