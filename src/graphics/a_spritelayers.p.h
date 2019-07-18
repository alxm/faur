/*
    Copyright 2010, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef A_INC_GRAPHICS_SPRITELAYERS_P_H
#define A_INC_GRAPHICS_SPRITELAYERS_P_H

#include "general/a_system_includes.h"

typedef struct AList ASpriteLayers;

#include "data/a_list.p.h"
#include "graphics/a_sprite.p.h"

extern ASpriteLayers* a_spritelayers_new(void);
extern void a_spritelayers_free(ASpriteLayers* Layers, bool FreeSprites);

extern void a_spritelayers_clear(ASpriteLayers* Layers, bool FreeSprites);
extern void a_spritelayers_add(ASpriteLayers* Layers, ASprite* Sprite, AColorBlend Blend, int Red, int Green, int Blue, int Alpha);

extern void a_spritelayers_blit(ASpriteLayers* Layers, unsigned Frame, int X, int Y);

#endif // A_INC_GRAPHICS_SPRITELAYERS_P_H
