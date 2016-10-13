/*
    Copyright 2010, 2016 Alex Margarit

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

typedef struct ASpriteFrames ASpriteFrames;

#include "a2x_pack_list.p.h"
#include "a2x_pack_sprite.p.h"

extern ASpriteFrames* a_spriteframes_new(const ASprite* Sheet, int X, int Y, int CallsToNextFrame);
extern void a_spriteframes_free(ASpriteFrames* Frames, bool DoFreeSprites);

extern ASprite* a_spriteframes_next(ASpriteFrames* Frames);
extern ASprite* a_spriteframes_get(const ASpriteFrames* Frames);
extern ASprite* a_spriteframes_getByIndex(const ASpriteFrames* Frames, size_t Index);
extern ASprite* a_spriteframes_getRandom(const ASpriteFrames* Frames);
extern int a_spriteframes_num(const ASpriteFrames* Frames);

extern void a_spriteframes_setDirection(ASpriteFrames* Frames, bool Forward);
extern void a_spriteframes_flipDirection(ASpriteFrames* Frames);

extern int a_spriteframes_getSpeed(const ASpriteFrames* Frames);
extern void a_spriteframes_setSpeed(ASpriteFrames* Frames, int CallsToNextFrame);

extern void a_spriteframes_pause(ASpriteFrames* Frames);
extern void a_spriteframes_resume(ASpriteFrames* Frames);

extern void a_spriteframes_reset(ASpriteFrames* Frames);

extern ASpriteFrames* a_spriteframes_clone(const ASpriteFrames* Frames);
extern AList* a_spriteframes_sprites(const ASpriteFrames* Frames);

extern ASprite* a_spriteframes_pop(ASpriteFrames* Frames);
