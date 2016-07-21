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

typedef struct ASpriteFrames ASpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern ASpriteFrames* a_spriteframes_new(const ASprite* sheet, int x, int y, unsigned int callsToNextFrame);
extern void a_spriteframes_free(ASpriteFrames* sf, bool freeSprites);

extern ASprite* a_spriteframes_next(ASpriteFrames* sf);
extern ASprite* a_spriteframes_get(ASpriteFrames* sf);
extern ASprite* a_spriteframes_geti(ASpriteFrames* sf, int index);
extern bool a_spriteframes_last(const ASpriteFrames* sf);

extern void a_spriteframes_setDir(ASpriteFrames* sf, int dir);
extern void a_spriteframes_flipDir(ASpriteFrames* sf);

extern void a_spriteframes_setSpeed(ASpriteFrames* sf, unsigned int callsToNextFrame);

extern void a_spriteframes_pause(ASpriteFrames* sf);
extern void a_spriteframes_resume(ASpriteFrames* sf);

extern void a_spriteframes_reset(ASpriteFrames* sf);

extern ASpriteFrames* a_spriteframes_clone(const ASpriteFrames* src);
extern AList* a_spriteframes_sprites(const ASpriteFrames* sf);

extern ASprite* a_spriteframes_pop(ASpriteFrames* sf);
