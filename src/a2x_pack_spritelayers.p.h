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

#include "a2x_pack_list.p.h"
#include "a2x_pack_pixel.p.h"
#include "a2x_pack_sprite.p.h"

typedef AList ASpriteLayers;

extern ASpriteLayers* a_spritelayers_new(void);
extern void a_spritelayers_free(ASpriteLayers* s);

extern void a_spritelayers_add(ASpriteLayers* s, const ASprite* sprite, APixelBlend blend, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
extern void a_spritelayers_blit(ASpriteLayers* s, int x, int y);
