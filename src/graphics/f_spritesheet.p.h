/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_SPRITESHEET_P_H
#define F_INC_GRAPHICS_SPRITESHEET_P_H

#include "../general/f_system_includes.h"

#include "../data/f_strhash.p.h"
#include "../graphics/f_sprite.p.h"

typedef FStrHash FSpriteSheet;

extern FSpriteSheet* f_spritesheet_new(const char* Path);
extern void f_spritesheet_free(FSpriteSheet* Sheet);

extern const FSprite* f_spritesheet_get(const FSpriteSheet* Sheet, const char* Id);

#endif // F_INC_GRAPHICS_SPRITESHEET_P_H
