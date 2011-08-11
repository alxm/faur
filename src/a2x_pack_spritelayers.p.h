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

#ifndef A2X_PACK_SPRITELAYERS_PH
#define A2X_PACK_SPRITELAYERS_PH

#include "a2x_app_includes.h"

#include "a2x_pack_pixel.p.h"
#include "a2x_pack_sprite.p.h"

typedef struct SpriteLayers SpriteLayers;

extern SpriteLayers* a_spritelayers_new(void);
extern void a_spritelayers_free(SpriteLayers* const s);

extern void a_spritelayers_add(SpriteLayers* const s, Sprite* const sprite, const PixelBlend_t blend, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void a_spritelayers_blit(SpriteLayers* const s, const int x, const int y);

#endif // A2X_PACK_SPRITELAYERS_PH
