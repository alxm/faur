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

#include "a2x_app_includes.h"

typedef struct SpriteFrames SpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern SpriteFrames* a_spriteframes_new(const Sprite* sheet, int x, int y, unsigned int callsToNextFrame);
extern void a_spriteframes_free(SpriteFrames* sf, bool freeSprites);

extern Sprite* a_spriteframes_next(SpriteFrames* sf);
extern Sprite* a_spriteframes_get(SpriteFrames* sf);
extern Sprite* a_spriteframes_geti(SpriteFrames* sf, int index);
extern bool a_spriteframes_last(const SpriteFrames* sf);

extern void a_spriteframes_setDir(SpriteFrames* sf, int dir);
extern void a_spriteframes_flipDir(SpriteFrames* sf);

extern void a_spriteframes_pause(SpriteFrames* sf);
extern void a_spriteframes_resume(SpriteFrames* sf);

extern void a_spriteframes_reset(SpriteFrames* sf);

extern SpriteFrames* a_spriteframes_clone(const SpriteFrames* src);
extern List* a_spriteframes_sprites(const SpriteFrames* sf);

extern Sprite* a_spriteframes_pop(SpriteFrames* sf);
