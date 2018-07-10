/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

typedef struct AList ASpriteLayers;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern ASpriteLayers* a_spritelayers_new(void);
extern void a_spritelayers_free(ASpriteLayers* Layers, bool FreeSprites);

extern void a_spritelayers_clear(ASpriteLayers* Layers, bool FreeSprites);
extern void a_spritelayers_add(ASpriteLayers* Layers, ASprite* Sprite, APixelBlend Blend, int Red, int Green, int Blue, int Alpha);

extern void a_spritelayers_blit(ASpriteLayers* Layers, int X, int Y);
