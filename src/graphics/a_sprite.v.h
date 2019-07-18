/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
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

#ifndef A_INC_GRAPHICS_SPRITE_V_H
#define A_INC_GRAPHICS_SPRITE_V_H

#include "graphics/a_sprite.p.h"

#include "platform/a_platform.v.h"

extern APixels* a_sprite__pixelsGet(ASprite* Sprite);
extern APlatformTexture* a_sprite__textureGet(const ASprite* Sprite, unsigned Frame);
extern void a_sprite__textureCommit(ASprite* Sprite, unsigned Frame);

#endif // A_INC_GRAPHICS_SPRITE_V_H
