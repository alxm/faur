/*
    Copyright 2010, 2016, 2018-2020 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_GRAPHICS_SPRITELAYERS_P_H
#define F_INC_GRAPHICS_SPRITELAYERS_P_H

#include "../general/f_system_includes.h"

#include "../data/f_listintr.p.h"
#include "../graphics/f_sprite.p.h"

typedef FListIntr FSpriteLayers;

extern FSpriteLayers* f_spritelayers_new(void);
extern void f_spritelayers_free(FSpriteLayers* Layers, bool FreeSprites);

extern void f_spritelayers_clear(FSpriteLayers* Layers, bool FreeSprites);
extern void f_spritelayers_add(FSpriteLayers* Layers, FSprite* Sprite, FColorBlend Blend, int Red, int Green, int Blue, int Alpha);

extern void f_spritelayers_blit(const FSpriteLayers* Layers, unsigned Frame, int X, int Y);

#endif // F_INC_GRAPHICS_SPRITELAYERS_P_H
